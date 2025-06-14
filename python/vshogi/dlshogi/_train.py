import tensorflow as tf
from tqdm import tqdm


@tf.function
def masked_log_softmax(logit, mask, axis: int = -1):
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
    Tensor
        Masked log softmax.
    """
    # https://github.com/tensorflow/tensorflow/issues/24476
    # In order to make this function work in CPU, remove `tf.where()`.
    # masked out values should be -100000 here.
    logit_masked = tf.where(mask, logit, -100000.)
    # logit_masked = logit + tf.cast(~mask, dtype=tf.float32) * -100000.

    logit_max = tf.reduce_max(
        tf.stop_gradient(logit_masked), axis=axis, keepdims=True)
    logit_subtracted = logit_masked - logit_max
    logsumexp = tf.reduce_logsumexp(logit_subtracted, axis=axis, keepdims=True)
    log_softmax = logit_subtracted - logsumexp
    return log_softmax


@tf.function
def masked_softmax_cross_entropy(
    target,
    logit,
    coeff_entropy_regularization: float = 0.,
):
    """Return masked softmax cross entropy loss.

    Parameters
    ----------
    target : Tensor
        Ground truth. Negative values indicate masks.
    logit : Tensor
        Output logit
    coeff_entropy_regularization : float
        Coefficient of entropy regularization

    Returns
    -------
    Tensor
        Masked softmax cross entropy loss.
    """
    t_masked = tf.clip_by_value(target, 0., 1.)
    lnp = masked_log_softmax(logit, tf.greater_equal(target, 0), axis=-1)
    cross_entropy = tf.reduce_sum(-t_masked * lnp, axis=-1, keepdims=True)
    entropy = tf.reduce_sum(-tf.exp(lnp) * lnp, axis=-1, keepdims=True)
    return cross_entropy + coeff_entropy_regularization * entropy


def train(
    model: tf.keras.Model,
    dataset: tf.data.Dataset,
    optimizer,
    epochs: int,
    coeff_entropy_regularization: float,
    gradient_accumulation_steps: int = 1,
) -> None:
    """Train a model given dataset.

    Parameters
    ----------
    model : tf.keras.Model
        Model to train
    dataset : tf.data.Dataset
        Training dataset
    epochs : int
        Number of epochs to train
    learning_rate : float
        Learning rate of weight updates
    coeff_entropy_regularization : float
        Coefficient of entropy regularization
    gradient_accumulation_steps : int, optional
        Steps to accumulate gradient computation, by default 1
    """
    model.compile()

    @tf.function
    def compute_losses(x, y_policy, y_value, w):
        p_logits, v_logits = model(x, training=True)

        loss_policy = tf.reduce_mean(
            w * masked_softmax_cross_entropy(
                y_policy, p_logits, coeff_entropy_regularization))
        loss_value = tf.reduce_mean(
            tf.nn.sigmoid_cross_entropy_with_logits(y_value, v_logits) * w)

        loss = loss_policy + loss_value
        return loss, loss_policy, loss_value

    @tf.function
    def compute_losses_grads(x, y_policy, y_value, w):
        with tf.GradientTape() as tape:
            loss, loss_policy, loss_value = compute_losses(
                x, y_policy, y_value, w,
            )
        grads = tape.gradient(loss, model.trainable_weights)
        return float(loss), float(loss_policy), float(loss_value), grads

    @tf.function
    def apply_gradients(grads):
        optimizer.apply_gradients(zip(grads, model.trainable_weights))

    accumulated_grads = None
    counter = 0
    for e in range(1, epochs + 1):
        pbar = tqdm(enumerate(dataset, start=1), ncols=80)
        loss_policy_mean = 0.
        loss_value_mean = 0.
        loss_mean = 0.
        for i, (x_mb, (p_mb, v_mb), w_mb) in pbar:
            counter += 1
            loss, loss_policy, loss_value, grads = compute_losses_grads(
                x_mb, p_mb, v_mb, w_mb)
            if accumulated_grads is None:
                accumulated_grads = grads
            else:
                accumulated_grads = [
                    g1 + g2 for g1, g2 in zip(accumulated_grads, grads)
                ]
            if counter % gradient_accumulation_steps == 0:
                apply_gradients([
                    g / gradient_accumulation_steps for g in accumulated_grads
                ])
                accumulated_grads = None

            loss_policy_mean = ((i - 1) * loss_policy_mean + loss_policy) / i
            loss_value_mean = ((i - 1) * loss_value_mean + loss_value) / i
            loss_mean = ((i - 1) * loss_mean + loss) / i
            pbar.set_description(
                f'Epoch {e:2}/{epochs}, loss={loss_mean:f}, '
                f'loss_policy={loss_policy_mean:f}, '
                f'loss_value={loss_value_mean:f}',
            )
