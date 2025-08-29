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
    logit_masked = th.where(mask, logit, -100000.)
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
    return cross_entropy + coeff_entropy_regularization * entropy


NumpyIterable = tp.TypeVar('NumpyIterable')


def train(
    model: th.nn.Module,
    dataset: NumpyIterable,
    optimizer: th.optim.Optimizer,
    epochs: int,
    coeff_policy_loss: float = 0.1,
    coeff_entropy_regularization: tp.Optional[float] = None,
    gradient_accumulation_steps: int = 1,
) -> None:
    """Train a model given dataset.

    Parameters
    ----------
    model : th.nn.Module
        Pytorch model to train
    dataset : NumpyIterable
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
    gradient_accumulation_steps : int, optional
        Steps to accumulate gradient computation, by default 1
    """
    model.train()
    device = next(model.parameters()).device

    def compute_losses_and_backward(x, y_policy, y_value, w):
        p_logits, v_logits = model(x)
        loss_policy = th.mean(
            w * masked_softmax_cross_entropy(
                y_policy, p_logits, coeff_entropy_regularization))
        loss_policy = (
            coeff_policy_loss * loss_policy
            + (1 - coeff_policy_loss) * loss_policy.detach()
        )
        loss_value = th.nn.functional.binary_cross_entropy_with_logits(
            v_logits, y_value, w, reduction='mean')
        loss = loss_policy + loss_value
        loss.backward()

        loss_policy = loss_policy.item()
        loss_value = loss_value.item()
        loss = loss.item()
        return loss, loss_policy, loss_value

    counter = 0
    for e in range(1, epochs + 1):
        pbar = tqdm(enumerate(dataset, start=1), ncols=80)
        loss_policy_mean = 0.
        loss_value_mean = 0.
        loss_mean = 0.
        for i, (x_mb, (p_mb, v_mb), w_mb) in pbar:
            x_mb = th.tensor(x_mb, device=device)
            p_mb = th.tensor(p_mb, device=device)
            v_mb = th.tensor(v_mb, device=device)
            w_mb = th.tensor(w_mb, device=device)
            if counter == 0:
                optimizer.zero_grad()
            counter += 1
            loss, loss_policy, loss_value = compute_losses_and_backward(
                x_mb, p_mb, v_mb, w_mb)
            if counter == gradient_accumulation_steps:
                for p in model.parameters():
                    p.grad /= gradient_accumulation_steps
                optimizer.step()
                counter = 0

            loss_policy_mean = ((i - 1) * loss_policy_mean + loss_policy) / i
            loss_value_mean = ((i - 1) * loss_value_mean + loss_value) / i
            loss_mean = ((i - 1) * loss_mean + loss) / i
            pbar.set_description(
                f'Epoch {e:2}/{epochs}, loss={loss_mean:f}, '
                f'loss_policy={loss_policy_mean:f}, '
                f'loss_value={loss_value_mean:f}',
            )
