import numpy as np
import pytest
import tensorflow as tf

from vshogi.dlshogi._train import masked_softmax_cross_entropy


@pytest.mark.parametrize('t, x, dx_expect', [
    (np.zeros(4) + 0.25, np.zeros(4), np.zeros(4)),
    (
        np.array([1, 0]),
        np.array([0, 0]),  # p=[0.5, 0.5]
        np.array([-0.5, 0.5]),
    ),
    (
        np.array([1, -1, 0, -1]),
        np.array([0, 10, 0, 10]),  # p=[0.5, nan, 0.5, nan]
        np.array([-0.5, 0, 0.5, 0]),
    ),
    (
        np.array([[0, 0, 0, 1], [1, -1, 0, -1]]),
        np.array([[0, 0, 0, 0], [0, 10, 0, 10]]),
        np.array([
            [0.25, 0.25, 0.25, -0.75],
            [-0.5, 0, 0.5, 0],
        ]),
    ),
])
def test_masked_softmax_cross_entropy(t, x, dx_expect):
    t = tf.constant(t, dtype=tf.float32)
    x = tf.constant(x, dtype=tf.float32)
    with tf.GradientTape() as tape:
        tape.watch(x)
        loss = tf.reduce_sum(masked_softmax_cross_entropy(t, x))
    dx = tape.gradient(loss, x)
    print(dx)
    assert np.allclose(dx.numpy(), dx_expect, rtol=0, atol=1e-2)


if __name__ == '__main__':
    pytest.main([__file__])
