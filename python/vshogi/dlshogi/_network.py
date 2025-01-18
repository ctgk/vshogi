import typing as tp

import numpy as np
import tensorflow as tf

from vshogi.dlshogi._depthwise_attention import DepthwiseAttention


def _pconv(x, ch, use_bias=False):
    return tf.keras.layers.Conv2D(ch, 1, use_bias=use_bias)(x)


def _bn(x):
    return tf.keras.layers.BatchNormalization(center=False, scale=False)(x)


def _act(x):
    return tf.keras.layers.LeakyReLU()(x)


def _act_bn_pconv(x, ch):
    return _act(_bn(_pconv(x, ch, use_bias=False)))


def _resblock(x, ch, attention_matrix):
    h = _act_bn_pconv(x, ch)
    h1 = _pconv(h, ch // 2)
    h2 = DepthwiseAttention(attention_matrix)(_pconv(h, ch // 2))
    h = _act(tf.keras.layers.Concatenate()([h1, h2]))
    h = _bn(_pconv(h, x.shape[-1]))
    return _act(x + h)


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
