from math import isclose

import pytest
import tensorflow as tf

import vshogi.minishogi as shogi
from vshogi.dlshogi._network import ValueHead


def test_value_head_symmetry():
    value_head = ValueHead((shogi.Game.files, shogi.Game.ranks))
    optimizer = tf.keras.optimizers.SGD(1.)

    x_train = shogi.Game().to_dlshogi_features().reshape(1, 5 * 5, -1)
    y_train = [[1.]]

    for _ in range(1000):
        with tf.GradientTape() as tape:
            logit = value_head(x_train, training=True)
            loss = tf.nn.sigmoid_cross_entropy_with_logits(y_train, logit)
        grads = tape.gradient(loss, value_head.trainable_weights)
        optimizer.apply_gradients(zip(grads, value_head.trainable_weights))

    g_test = shogi.Game('4k/5/5/5/2K2 b -')
    y1 = float(value_head(g_test.to_dlshogi_features().reshape(1, 5 * 5, -1)))
    y2 = float(
        value_head(g_test.hflip().to_dlshogi_features().reshape(1, 5 * 5, -1)))
    assert isclose(y1, y2, rel_tol=0, abs_tol=1e-8)


if __name__ == '__main__':
    pytest.main([__file__])
