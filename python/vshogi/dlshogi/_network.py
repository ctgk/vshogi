import typing as tp

import numpy as np
import tensorflow as tf


def _pconv(x, ch, use_bias=False):
    return tf.keras.layers.Conv2D(ch, 1, use_bias=use_bias)(x)


def _dconv(x, use_bias=False):
    return tf.keras.layers.DepthwiseConv2D(
        3, padding='same', use_bias=use_bias)(x)


def _bn(x):
    return tf.keras.layers.BatchNormalization(center=False, scale=False)(x)


def _act(x):
    return tf.keras.layers.LeakyReLU()(x)


def _act_pconv(x, ch):
    return _act(_pconv(x, ch, use_bias=True))


def _act_bn_pconv(x, ch):
    return _act(_bn(_pconv(x, ch, use_bias=False)))


class _Attention(tf.keras.layers.Layer):

    def __init__(self, attention_matrix: np.ndarray):
        super().__init__()
        self._attention_matrix = tf.constant(attention_matrix, tf.float32)

    def build(self, input_shape):
        n = input_shape[1] * input_shape[2]
        self._reshape_target = (-1, n, input_shape[3])
        self._input_shape = (-1, *input_shape[1:])
        assert n == self._attention_matrix.shape[0]

    def call(self, x):
        h = tf.reshape(x, self._reshape_target)
        h = tf.matmul(h, self._attention_matrix, transpose_a=True)
        h = tf.transpose(h, perm=[0, 2, 1])
        return tf.reshape(h, self._input_shape)


def _vertical_attention(x):
    return tf.math.reduce_sum(x, axis=1, keepdims=True) - x


def _horizontal_attention(x):
    return tf.math.reduce_sum(x, axis=2, keepdims=True) - x


def _resblock(x, ch, diagonal_attention_layer):
    h = _act_pconv(x, ch)
    h1, h2, h3, h4 = tf.split(h, [ch // 2, ch // 4, ch // 8, ch // 8], axis=-1)
    h1 = _dconv(h1)
    h2 = diagonal_attention_layer(h2)
    h3 = _vertical_attention(h3)
    h4 = _horizontal_attention(h4)
    h = tf.keras.layers.Concatenate()([h1, h2, h3, h4])
    h = _act(h)
    h = _pconv(h, x.shape[-1])
    return _act(_bn(x + h))


def _policy_head(x, num_policy_per_square, name='policy_logits'):
    h = _pconv(x, num_policy_per_square, use_bias=True)
    return tf.keras.layers.Flatten(name=name)(h)


def _value_head(x, name='value'):
    h = _act_bn_pconv(x, 1)
    h = tf.keras.layers.Flatten()(h)
    return tf.keras.layers.Dense(1, activation='tanh', name=name)(h)


def build_policy_value_network(
    input_size: tp.Tuple[int, int],  # (H, W)
    input_channels: int,
    num_policy_per_square: int,
    hidden_channels: int,
    bottleneck_channels: int,
    num_backbone_blocks: int,
    diagonal_attention_matrix: np.ndarray,
):
    """Return policy-value network.

    Parameters
    ----------
    input_size : tp.Tuple[int, int]
        Input height and width of the network. e.g. (9, 9) for Shogi.
    num_policy_per_square : int
        Number of policies per square. e.g. 27(= 2 * 10 + 7) for Shogi.
    hidden_channels : int
        Number of feature-channel of output of backbone network.
    bottleneck_channels : int
        Number of feature-channel in bottleneck block.
    num_backbone_blocks : int
        Number of backbone blocks.
    diagonal_attention_matrix : np.ndarray
        Attention matrix along diagonal directions.
    """
    diagonal_attention = _Attention(diagonal_attention_matrix)

    x = tf.keras.Input(shape=(*input_size, input_channels))
    h = _act_bn_pconv(x, hidden_channels)
    for _ in range(num_backbone_blocks):
        h = _resblock(h, bottleneck_channels, diagonal_attention)

    policy_logits = _policy_head(h, num_policy_per_square)
    value = _value_head(h)
    model = tf.keras.Model(inputs=x, outputs=[policy_logits, value])
    return model
