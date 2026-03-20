import sys
import typing as tp

import torch as th
from tqdm import tqdm


def masked_log_softmax(
    logit: th.Tensor,
    mask: th.Tensor,
    axis: int = -1,
) -> th.Tensor:
    """Compute log softmax of logits given masks.

    Parameters
    ----------
    logit : Tensor [..., C]
        Input logits.
    mask : Tensor [..., C]
        Binary tensor, where true denotes valid logit.
    axis : int, optional
        Axis to compute softmax along, by default -1.

    Returns
    -------
    Tensor [..., C]
        Masked log softmax.
    """
    logit_masked = th.where(mask, logit, -100000.0)
    logit_max = th.max(logit_masked.detach(), dim=axis, keepdim=True).values
    logit_subtracted = logit_masked - logit_max
    logsumexp = th.logsumexp(logit_subtracted, dim=axis, keepdim=True)
    log_softmax = logit_subtracted - logsumexp
    return log_softmax


def masked_softmax_cross_entropy(
    target: th.Tensor,
    logit: th.Tensor,
    coeff_entropy_regularization: tp.Optional[float] = None,
) -> th.Tensor:
    """Return masked softmax cross entropy loss.

    Parameters
    ----------
    target : Tensor [..., C]
        Ground truth. Negative values indicate masks.
    logit : Tensor [..., C]
        Output logit
    coeff_entropy_regularization : tp.Optional[float]
        Coefficient of entropy regularization, by default None.

    Returns
    -------
    Tensor [..., 1]
        Masked softmax cross entropy loss.
    """
    t_masked = th.clamp(target, 0, 1)
    lnp = masked_log_softmax(logit, th.greater_equal(target, 0), axis=-1)
    cross_entropy = th.sum(-t_masked * lnp, axis=-1, keepdims=True)
    if coeff_entropy_regularization is None:
        return cross_entropy
    entropy = th.sum(-th.exp(lnp) * lnp, axis=-1, keepdims=True)
    return cross_entropy - coeff_entropy_regularization * (
        entropy - entropy.detach()
    )


def masked_binary_cross_entropy(
    target: th.Tensor,
    logit: th.Tensor,
) -> th.Tensor:
    """Return masked binary cross entropy.

    Parameters
    ----------
    target : th.Tensor
        Target values in [0, 1], whose shape is (N, ...).
        `Nan` values work as mask.
    logit : th.Tensor
        Logit values, whose shape is (N, ...).

    Returns
    -------
    th.Tensor
        Mean of binary cross entropy excluding masks.
        The shape is (N,).
    """
    mask = ~th.isnan(target)
    target_safe = th.where(mask, target, 0.0)
    loss = th.nn.functional.binary_cross_entropy_with_logits(
        logit, target_safe, reduction="none"
    )
    loss_summed = th.sum(loss * mask, dim=tuple(range(1, target.ndim)))
    return loss_summed / th.sum(mask, dim=tuple(range(1, target.ndim)))


def _scale_grad(x: th.Tensor, s):
    return x * s + (1 - s) * x.detach()


def _training_loop(model, dataset, optimizer, epochs, loss_fn_and_backprop):
    model.train()
    device = next(model.parameters()).device
    for e in range(1, epochs + 1):
        pbar = tqdm(enumerate(dataset, start=1), ncols=80, file=sys.stdout)
        loss_policy_mean = 0.0
        loss_value_mean = 0.0
        loss_mean = 0.0
        for i, minibatch in pbar:
            minibatch = tuple(mb.to(device) for mb in minibatch)
            optimizer.zero_grad()
            loss, loss_policy, loss_value = loss_fn_and_backprop(*minibatch)
            optimizer.step()

            loss_policy_mean = ((i - 1) * loss_policy_mean + loss_policy) / i
            loss_value_mean = ((i - 1) * loss_value_mean + loss_value) / i
            loss_mean = ((i - 1) * loss_mean + loss) / i
            pbar.set_description(
                f'Epoch {e:2}/{epochs}, loss={loss_mean:f}, '
                f'loss_policy={loss_policy_mean:f}, '
                f'loss_value={loss_value_mean:f}',
            )


def train(
    model: th.nn.Module,
    dataset: th.utils.data.DataLoader,
    optimizer: th.optim.Optimizer,
    epochs: int,
    coeff_policy_loss: float = 0.1,
    coeff_entropy_regularization: tp.Optional[float] = None,
) -> None:
    """Train a model given dataset.

    Parameters
    ----------
    model : th.nn.Module
        Pytorch model to train
    dataset : th.utils.data.DataLoader
        Training dataset
    optimizer : th.nn.Optimizer
        Optimizer to update parameters in the model.
    epochs : int
        Number of epochs to train
    coeff_policy_loss : float, optional
        Coefficient of policy loss, by default 0.1.
        `total_loss = value_loss + coeff_policy_loss * policy_loss`.
    coeff_entropy_regularization : float, optional
        Coefficient of entropy regularization
    """

    def compute_losses_and_backward(x, y_policy, y_value, w):
        p_logits, v_logits = model(x)
        loss_policy_each = _scale_grad(
            masked_softmax_cross_entropy(
                y_policy,
                p_logits,
                coeff_entropy_regularization,
            ),
            w,
        )
        loss_value_each = _scale_grad(
            th.nn.functional.binary_cross_entropy_with_logits(
                v_logits,
                y_value,
                reduction="none",
            ),
            w,
        )

        loss_policy = _scale_grad(th.mean(loss_policy_each), coeff_policy_loss)
        loss_value = th.mean(loss_value_each)
        loss = loss_policy + loss_value
        loss.backward()

        loss_policy = loss_policy.item()
        loss_value = loss_value.item()
        loss = loss.item()
        return loss, loss_policy, loss_value

    _training_loop(
        model,
        dataset,
        optimizer,
        epochs,
        compute_losses_and_backward,
    )


def train_klent(
    model: th.nn.Module,
    dataset: th.utils.data.DataLoader,
    optimizer: th.optim.Optimizer,
    epochs: int,
) -> None:
    """Train a model given dataset using KLENT.

    Notes
    -----
    KLENT : https://arxiv.org/pdf/2602.10894

    Parameters
    ----------
    model : th.nn.Module
        Pytorch model to train
    dataset : th.utils.data.DataLoader
        Training dataset
    optimizer : th.nn.Optimizer
        Optimizer to update parameters in the model.
    epochs : int
        Number of epochs to train
    """

    def loss_fn_and_backprop(x, y_policy, y_value, w):
        p_logits, v_logits = model(x)
        loss_policy_each = masked_softmax_cross_entropy(y_policy, p_logits)
        loss_policy_each = _scale_grad(loss_policy_each, w)
        loss_value_each = masked_binary_cross_entropy(y_value, v_logits)
        loss_value_each = _scale_grad(loss_value_each, w)

        loss_policy = th.mean(loss_policy_each)
        loss_value = th.mean(loss_value_each)
        loss = loss_policy + loss_value
        loss.backward()
        return loss.item(), loss_policy.item(), loss_value.item()

    assert isinstance(dataset, th.utils.data.DataLoader), type(dataset)
    _training_loop(model, dataset, optimizer, epochs, loss_fn_and_backprop)
