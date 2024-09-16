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

    def __init__(self, attention_matrix: np.ndarray, use_bias: bool = True):
        super().__init__()
        self._attention_matrix = tf.constant(attention_matrix, tf.float32)
        self._use_bias = use_bias

    def build(self, input_shape):
        n = input_shape[1] * input_shape[2]
        self._reshape_target = (-1, n, input_shape[3])
        self._input_shape = (-1, *input_shape[1:])
        assert n == self._attention_matrix.shape[0]
        self.kernel = self.add_weight(
            shape=self._attention_matrix.shape,
            constraint=lambda w: w * self._attention_matrix,
        )
        if self._use_bias:
            self.bias = self.add_weight(
                shape=self._attention_matrix.shape[-1],
                initializer='zeros',
            )

    def call(self, x):
        h = tf.reshape(x, self._reshape_target)
        h = tf.matmul(h, self.kernel, transpose_a=True)
        if self._use_bias:
            h = h + self.bias
        h = tf.transpose(h, perm=[0, 2, 1])
        return tf.reshape(h, self._input_shape)


def _resblock(x, ch, attention_matrix):
    h = _act_pconv(x, ch)
    h = _act(_Attention(attention_matrix, use_bias=True)(h))
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
    attention_matrix: np.ndarray,
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
    attention_matrix : np.ndarray
        Attention matrix.
    """
    x = tf.keras.Input(shape=(*input_size, input_channels))
    h = _act_bn_pconv(x, hidden_channels)
    for _ in range(num_backbone_blocks):
        h = _resblock(h, bottleneck_channels, attention_matrix)

    policy_logits = _policy_head(h, num_policy_per_square)
    value = _value_head(h)
    model = tf.keras.Model(inputs=x, outputs=[policy_logits, value])
    return model
