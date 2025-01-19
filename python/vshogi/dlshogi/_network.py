# flake8: noqa
import typing as tp

import numpy as np
import tensorflow as tf


def _pconv(x, ch, use_bias=False):
    return tf.keras.layers.Conv2D(ch, 1, use_bias=use_bias)(x)


def _bn(x):
    return tf.keras.layers.BatchNormalization(center=False, scale=False)(x)


def _act(x):
    return tf.keras.layers.LeakyReLU()(x)


def _act_bn_pconv(x, ch):
    return _act(_bn(_pconv(x, ch, use_bias=False)))


class DepthwiseAttention(tf.keras.layers.Layer):

    def __init__(self, attention_matrix: tf.Tensor, use_bias: bool = True):
        super().__init__()
        self._attention_matrix = tf.constant(attention_matrix, tf.float32)
        self._use_bias = use_bias

    def build(self, input_shape):
        n = input_shape[1] * input_shape[2]
        self._reshape_target = (-1, n, input_shape[3])
        self._input_shape = (-1, *input_shape[1:])
        assert n == self._attention_matrix.shape[0]
        if self._use_bias:
            self.bias = self.add_weight(
                shape=(self._attention_matrix.shape[-1],),
                initializer='zeros',
                name=self.name + '_bias',
            )

    def call(self, x):
        h = tf.reshape(x, self._reshape_target)
        h = tf.matmul(h, self._attention_matrix, transpose_a=True)
        if self._use_bias:
            h = h + self.bias
        h = tf.transpose(h, perm=[0, 2, 1])
        return tf.reshape(h, self._input_shape)


class ResBlock(tf.keras.layers.Layer):

    def __init__(self, in_ch: int, hid_ch: int, attention_matrix: np.ndarray):
        super().__init__()
        self._act_bn_conv = tf.keras.Sequential([
            tf.keras.layers.Conv2D(1, 1, use_bias=False),
            tf.keras.layers.BatchNormalization(center=False, scale=False),
            tf.keras.layers.LeakyReLU(),
        ])
        self._conv1 = tf.keras.layers.Conv2D(hid_ch // 2, 1)
        self._conv2 = tf.keras.Sequential([
            tf.keras.layers.Conv2D(hid_ch // 2, 1),
            DepthwiseAttention(attention_matrix),
        ])
        self._bn_conv = tf.keras.Sequential([
            tf.keras.layers.Conv2D(in_ch, 1, use_bias=False),
            tf.keras.layers.BatchNormalization(center=False, scale=False),
        ])

    def call(self, x):
        h = self._act_bn_conv(x)
        h = tf.nn.leaky_relu(
            tf.concat([self._conv1(h), self._conv2(h)], axis=-1))
        h = self._bn_conv(h)
        return tf.nn.leaky_relu(x + h)


class PolicyHead(tf.keras.layers.Layer):

    def __init__(self, num_policy_per_square: int):
        super().__init__()
        self._conv = tf.keras.layers.Conv2D(
            num_policy_per_square, 1, use_bias=True)
        self._flat = tf.keras.layers.Flatten()

    def call(self, x):
        return self._flat(self._conv(x))


class ValueHead(tf.keras.layers.Layer):

    def __init__(self):
        super().__init__()
        self._act_bn_conv = tf.keras.Sequential([
            tf.keras.layers.Conv2D(1, 1, use_bias=False),
            tf.keras.layers.BatchNormalization(center=False, scale=False),
            tf.keras.layers.LeakyReLU(),
        ])
        self._flat = tf.keras.layers.Flatten()
        self._dense = tf.keras.layers.Dense(1, activation='tanh')

    def call(self, x, training=None):
        h = self._act_bn_conv(x, training=training)
        return self._dense(self._flat(h))


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
        h = ResBlock(hidden_channels, bottleneck_channels, attention_matrix)(h)

    policy_logits = PolicyHead(num_policy_per_square)(h)
    value = ValueHead()(h)
    model = tf.keras.Model(inputs=x, outputs=[policy_logits, value])
    return model
