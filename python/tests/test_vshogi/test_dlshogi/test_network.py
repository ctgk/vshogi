import numpy as np
import pytest
import tensorflow as tf

import vshogi.minishogi as shogi
from vshogi.dlshogi._network import DepthwiseAttention, ResBlock, ValueHead


def test_depthwise_attention_symmetry():
    layer = DepthwiseAttention(shogi.Game.get_attention().reshape(5, 5, 5, 5))
    optimizer = tf.keras.optimizers.Adam()

    x_train = shogi.Game().to_dlshogi_features().reshape(1, 5 * 5, -1)
    y_train = np.random.normal(size=x_train.shape)
    for _ in range(1000):
        with tf.GradientTape() as tape:
            h = layer(x_train, training=True)
            loss = tf.reduce_sum(tf.square(h - y_train))
        grads = tape.gradient(loss, layer.trainable_weights)
        optimizer.apply_gradients(zip(grads, layer.trainable_weights))

    g = shogi.Game('4k/5/5/5/2K2 b -')
    x1 = g.to_dlshogi_features().reshape(1, 5 * 5, -1)
    x2 = g.hflip().to_dlshogi_features().reshape(1, 5 * 5, -1)
    h1 = layer(x1).numpy().reshape(1, 5, 5, -1).sum(axis=-1)[0]
    h2 = layer(x2).numpy().reshape(1, 5, 5, -1).sum(axis=-1)[0]
    print(h1)
    print(h2)
    assert np.allclose(h1, h2[::-1], rtol=0, atol=1e-8)


def test_resblock_symmetry():
    layer = ResBlock(
        in_ch=shogi.Game.feature_channels,
        hid_ch=8,
        attention_matrix=shogi.Game.get_attention().reshape(5, 5, 5, 5),
    )
    optimizer = tf.keras.optimizers.Adam()

    x_train = shogi.Game().to_dlshogi_features().reshape(1, 5 * 5, -1)
    y_train = np.random.normal(size=x_train.shape)
    for _ in range(1000):
        with tf.GradientTape() as tape:
            h = layer(x_train, training=True)
            loss = tf.reduce_sum(tf.square(h - y_train))
        grads = tape.gradient(loss, layer.trainable_weights)
        optimizer.apply_gradients(zip(grads, layer.trainable_weights))

    g = shogi.Game('4k/5/5/5/P1K1S b -')
    x1 = g.to_dlshogi_features().reshape(1, 5 * 5, -1)
    x2 = g.hflip().to_dlshogi_features().reshape(1, 5 * 5, -1)
    h1 = layer(x1).numpy().reshape(1, 5, 5, -1).sum(axis=-1)[0]
    h2 = layer(x2).numpy().reshape(1, 5, 5, -1).sum(axis=-1)[0]
    print(h1)
    print(h2[::-1])
    print(np.isclose(h1, h2[::-1], rtol=0, atol=1e-8))
    assert np.allclose(h1, h2[::-1], rtol=0, atol=1e-8)


def test_value_head_symmetry():
    layer = ValueHead((shogi.Game.files, shogi.Game.ranks))
    optimizer = tf.keras.optimizers.SGD(1.)

    x_train = shogi.Game().to_dlshogi_features().reshape(1, 5 * 5, -1)
    y_train = [[1.]]

    for _ in range(1000):
        with tf.GradientTape() as tape:
            logit = layer(x_train, training=True)
            loss = tf.nn.sigmoid_cross_entropy_with_logits(y_train, logit)
        grads = tape.gradient(loss, layer.trainable_weights)
        optimizer.apply_gradients(zip(grads, layer.trainable_weights))

    g_test = shogi.Game('4k/5/5/5/2K2 b -')
    y1 = float(layer(g_test.to_dlshogi_features().reshape(1, 5 * 5, -1)))
    y2 = float(
        layer(g_test.hflip().to_dlshogi_features().reshape(1, 5 * 5, -1)))
    assert np.isclose(y1, y2, rtol=0, atol=1e-8)


if __name__ == '__main__':
    pytest.main([__file__])
