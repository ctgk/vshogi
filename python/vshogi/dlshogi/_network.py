import typing as tp

import numpy as np
import tensorflow as tf


class _ConcatDiagonalDirections(tf.keras.layers.Layer):

    def __init__(self, max_dilation_rate: int):
        super().__init__()
        self._max_dilation_rate = max_dilation_rate

    def build(self, input_shape):
        assert input_shape[-1] == 4, input_shape[-1]
        k = np.concatenate(
            (np.eye(9, 4)[:, :-1:2], np.eye(9, 4, k=-5)[:, 1::2]), axis=-1,
        ).reshape(3, 3, 4, 1)
        self.kernel = tf.constant(k, dtype=tf.float32)

    def call(self, x):
        feature_maps = [
            tf.nn.depthwise_conv2d(
                x, self.kernel, (1, 1, 1, 1), 'SAME', dilations=(d, d),
            )
            for d in range(1, self._max_dilation_rate + 1)
        ]
        return tf.concat(feature_maps, -1)


def _pointwise_conv2d(x, ch, use_bias=True):
    return tf.keras.layers.Conv2D(ch, 1, use_bias=use_bias)(x)


def _depthwise_conv2d(x, dilation=1, use_bias=True):
    return tf.keras.layers.DepthwiseConv2D(
        3, dilation_rate=dilation, padding='same', use_bias=use_bias)(x)


def _depthwise_hconv1d(x):
    return tf.keras.layers.DepthwiseConv2D((1, x.shape[2]), use_bias=False)(x)


def _depthwise_vconv1d(x):
    return tf.keras.layers.DepthwiseConv2D((x.shape[1], 1), use_bias=False)(x)


def _bn(x):
    return tf.keras.layers.BatchNormalization(center=False, scale=False)(x)


def _act(x):
    return tf.keras.layers.LeakyReLU()(x)


def _act_pconv(x, ch):
    return _act(_pointwise_conv2d(x, ch))


def build_policy_value_network(
    input_size: tp.Tuple[int, int],  # (H, W)
    input_channels: int,
    num_policy_per_square: int,
    num_channels_in_hidden_layer: int,
    num_channels_in_bottleneck: int,
    use_long_range_concat: bool,
    num_backbone_blocks: int,
):
    """Return policy-value network.

    Parameters
    ----------
    input_size : tp.Tuple[int, int]
        Input height and width of the network. e.g. (9, 9) for Shogi.
    num_policy_per_square : int
        Number of policies per square. e.g. 27(= 2 * 10 + 7) for Shogi.
    num_channels_in_hidden_layer : int
        Number of feature-channel of output of backbone network.
    num_channels_in_bottleneck : int
        Number of feature-channel in bottleneck block.
    use_long_range_concat : bool
        Use long range concatenation block if true, otherwise false.
    num_backbone_blocks : int
        Number of backbone blocks.
    """

    def multidilation_resblock(x):
        h1 = _act_pconv(x, num_channels_in_bottleneck)
        h1 = _act(_depthwise_conv2d(h1))

        h2 = _act_pconv(x, 4)
        h2 = _ConcatDiagonalDirections(max(input_size) - 1)(h2)

        h3 = _act_pconv(x, num_channels_in_bottleneck // 4)
        h3 = _depthwise_hconv1d(h3)
        h4 = _act_pconv(x, num_channels_in_bottleneck // 4)
        h4 = _depthwise_vconv1d(h4)

        h = tf.keras.layers.Concatenate()([h1, h2, _act(h3 + h4)])
        h = _pointwise_conv2d(h, x.shape[-1], use_bias=False)
        return _act(_bn(x + h))

    def resblock(x):
        h = _act_pconv(x, num_channels_in_bottleneck)
        h = _act(_depthwise_conv2d(h))
        h = _pointwise_conv2d(h, x.shape[-1], use_bias=False)
        return _act(_bn(x + h))

    def backbone_network(x):
        h = _act_pconv(x, num_channels_in_hidden_layer)
        h = _act(_bn(_depthwise_conv2d(h, use_bias=False)))

        for _ in range(num_backbone_blocks):
            if use_long_range_concat:
                h = multidilation_resblock(h)
            else:
                h = resblock(h)
        return h

    def policy_network(x):
        h = _pointwise_conv2d(x, num_policy_per_square)
        return tf.keras.layers.Flatten(name='policy_logits')(h)

    def value_network(x):
        h = _bn(_pointwise_conv2d(x, 1))
        h = _act(h)
        h = tf.keras.layers.Flatten()(h)
        return tf.keras.layers.Dense(1, activation='tanh', name='value')(h)

    x = tf.keras.Input(shape=(*input_size, input_channels))
    backbone_feature = backbone_network(x)
    policy_logits = policy_network(backbone_feature)
    value = value_network(backbone_feature)
    model = tf.keras.Model(inputs=x, outputs=[policy_logits, value])
    return model
