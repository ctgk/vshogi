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


class _DepthwiseAttention(tf.keras.layers.Layer):

    def __init__(self, attention_matrix: np.ndarray, use_bias: bool = True):
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
                shape=self._attention_matrix.shape[-1],
                initializer='zeros',
                name='bias',
            )

    def call(self, x):
        h = tf.reshape(x, self._reshape_target)
        h = tf.matmul(h, self._attention_matrix, transpose_a=True)
        if self._use_bias:
            h = h + self.bias
        h = tf.transpose(h, perm=[0, 2, 1])
        return tf.reshape(h, self._input_shape)


def _resblock(x, ch, attention_matrix):
    h = _act_bn_pconv(x, ch)
    h1 = _pconv(h, ch // 2)
    h2 = _DepthwiseAttention(attention_matrix)(_pconv(h, ch // 2))
    h = _act(tf.keras.layers.Concatenate()([h1, h2]))
    h = _bn(_pconv(h, x.shape[-1]))
    return _act(x + h)


def _build_backbone(
    input_shape: tp.Tuple[int, int, int],
    hidden_channels: int,
    bottleneck_channels: int,
    num_resblocks: int,
    attention_matrix: np.ndarray,
):
    x = tf.keras.Input(shape=input_shape)
    h = _act_bn_pconv(x, hidden_channels)
    for _ in range(num_resblocks):
        h = _resblock(h, bottleneck_channels, attention_matrix)
    return tf.keras.Model(inputs=x, outputs=h, name='backbone')


def _build_policy_head(num_policy_per_square, name='policy_head'):
    return tf.keras.Sequential([
        tf.keras.layers.Conv2D(num_policy_per_square, 1),
        tf.keras.layers.Flatten(name='policy_logits'),
    ], name=name)


def _build_value_head(name='value_head'):
    return tf.keras.Sequential([
        tf.keras.layers.Conv2D(1, 1),
        tf.keras.layers.BatchNormalization(center=False, scale=False),
        tf.keras.layers.LeakyReLU(),
        tf.keras.layers.Flatten(),
        tf.keras.layers.Dense(1, activation='tanh', name='value'),
    ], name=name)


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
    h = _build_backbone(
        (*input_size, input_channels),
        hidden_channels,
        bottleneck_channels,
        num_backbone_blocks,
        attention_matrix,
    )(x)
    policy_logits = _build_policy_head(num_policy_per_square)(h)
    value = _build_value_head()(h)
    model = tf.keras.Model(inputs=x, outputs=[policy_logits, value])
    return model
