# flake8: noqa
import typing as tp

import numpy as np
import tensorflow as tf

from vshogi._game import Game


class MultiHeadAttention(tf.keras.layers.Layer):

    def __init__(self, ch: int, num_heads: int, hid_ch: int, mask: tf.Tensor):
        super().__init__()
        assert num_heads == 1
        self.num_heads = num_heads
        self.hid_ch = hid_ch
        self.mask = tf.constant(mask, tf.float32)  # (H*W, H*W)
        self.conv_in = tf.keras.layers.Conv1D(hid_ch, 1, activation='relu')
        self.dq = tf.keras.layers.Conv1D(num_heads * hid_ch, 1, use_bias=False)
        self.dk = tf.keras.layers.Conv1D(num_heads * hid_ch, 1, use_bias=False)
        self.dv = tf.keras.layers.Conv1D(num_heads * hid_ch, 1, use_bias=False)

        # https://arxiv.org/abs/2409.12272
        self.bq = self.add_weight(
            shape=(self.mask.shape[-1], 1),
            initializer='zeros',
            name=self.name + '_bias_q',
        )
        self.bk = self.add_weight(
            shape=(self.mask.shape[-1], 1),
            initializer='zeros',
            name=self.name + '_bias_k',
        )
        self.bv = self.add_weight(
            shape=(self.mask.shape[-1], 1),
            initializer='zeros',
            name=self.name + '_bias_v',
        )

        self.softmax = tf.keras.layers.Softmax()

        self.conv_out = tf.keras.layers.Conv1D(ch, 1, use_bias=False)

    def _split_heads(self, x):
        x = tf.reshape(x, [-1, x.shape[1], self.num_heads, self.hid_ch])
        x = tf.transpose(x, perm=[0, 2, 1, 3])  # (N, num_heads, H*W, hid_ch)
        return x

    def _merge_heads(self, x):
        x = tf.transpose(x, perm=[0, 2, 1, 3])  # (N, H*W, num_heads, hid_ch)
        x = tf.reshape(x, [-1, x.shape[1], self.num_heads * self.hid_ch])
        return x

    def call(self, x):  # (N, H*W, ch)
        # (N, num_heads, H*W, hid_ch)
        h = self.conv_in(x)  # (N, H*W, hid_ch)
        q = self._split_heads(self.dq(h) + self.bq)
        k = self._split_heads(self.dk(h) + self.bk)
        v = self._split_heads(self.dv(h) + self.bv)

        qk = tf.matmul(q, k, transpose_b=True)  # (N, num_heads, H*W, H*W)
        attention_logits = qk / tf.math.sqrt(tf.cast(tf.shape(k)[-1], x.dtype))
        attention = self.softmax(attention_logits, mask=self.mask)
        weighted_v = tf.matmul(attention, v)  # (N, num_heads, H*W, hid_ch)

        h = self._merge_heads(weighted_v)
        return self.conv_out(h)


class TransformerBlock(tf.keras.layers.Layer):

    def __init__(self, ch: int, num_heads: int, hid_ch: int, mask: tf.Tensor):
        super().__init__()
        self.mha = MultiHeadAttention(ch, num_heads, hid_ch, mask)
        self.norm1 = tf.keras.layers.BatchNormalization(
            center=False, scale=False)
        self.conv = tf.keras.Sequential([
            tf.keras.layers.Conv1D(num_heads * hid_ch, 1),
            tf.keras.layers.ReLU(),
            tf.keras.layers.Conv1D(ch, 1, use_bias=False),
        ])
        self.dropout = tf.keras.layers.Dropout(rate=0.1)
        self.norm2 = tf.keras.layers.BatchNormalization(
            center=False, scale=False)

    def call(self, x, training=None):
        h = self.mha(x)
        x = self.norm1(x + h)
        h = self.conv(x)
        h = self.dropout(h, training=training)
        return self.norm2(x + h)


class HorizontalSymmetry(tf.keras.constraints.Constraint):

    def __init__(self, shape: tuple):
        super().__init__()
        self._shape = (*shape, -1)

    def __call__(self, w):
        k = tf.reshape(w, self._shape)
        return tf.reshape(0.5 * (k + k[::-1]), w.shape)


class DepthwiseAttention(tf.keras.layers.Layer):

    def __init__(
        self,
        attention_maps: tf.Tensor,
        num_groups: int,
        use_bias: bool = True,
    ):
        super().__init__()
        shape = attention_maps.shape
        self._num_groups = num_groups

        # (H*W, H*W, D)
        self._attention_maps = tf.transpose(tf.reshape(
            tf.constant(attention_maps, tf.float32),
            (shape[0], shape[1] * shape[2], shape[3] * shape[4]),
        ), perm=[1, 2, 0])

        if use_bias:
            self.bias = self.add_weight(
                shape=(shape[3] * shape[4],),
                initializer='zeros',
                name=self.name + '_bias',
                constraint=HorizontalSymmetry(shape[1:3]),
                trainable=True,
            )

    def build(self, input_shape):
        assert input_shape[-1] % self._num_groups == 0, (
            input_shape, self._num_groups)
        self._ch_in_group = input_shape[-1] // self._num_groups
        self._hw = input_shape[1]

        # (D, G)
        self._kernel = self.add_weight(
            shape=(self._attention_maps.shape[2], self._num_groups),
            initializer='glorot_uniform',
            name=self.name + '_kernel',
            trainable=True,
        )

    def call(self, x):
        # (B, H*W, C) -> (B, G, C/G, H*W)
        h = tf.reshape(
            tf.transpose(x, perm=[0, 2, 1]),
            (-1, self._num_groups, self._ch_in_group, self._hw),
        )

        w = tf.matmul(self._attention_maps, self._kernel)  # (H*W, H*W, G)
        w = tf.transpose(w, perm=[2, 0, 1])  # (G, H*W, H*W)

        h = tf.matmul(h, w)  # (B, G, C/G, H*W)
        h = tf.reshape(
            h,
            (-1, self._num_groups * self._ch_in_group, self._hw),
        )  # (B, C, H*W)

        if hasattr(self, 'bias'):
            h = h + self.bias
        return tf.transpose(h, perm=[0, 2, 1])


class ResBlock(tf.keras.layers.Layer):

    def __init__(self, in_ch: int, hid_ch: int, attention_matrix: np.ndarray):
        super().__init__()
        self._attention = tf.keras.Sequential([
            tf.keras.layers.Conv1D(hid_ch, 1),
            tf.keras.layers.LeakyReLU(),
            DepthwiseAttention(attention_matrix, 8),
            tf.keras.layers.LeakyReLU(),
        ])
        self._bn_conv = tf.keras.Sequential([
            tf.keras.layers.Conv1D(in_ch, 1, use_bias=False),
            tf.keras.layers.Dropout(0.1),
            tf.keras.layers.BatchNormalization(center=False, scale=False),
        ])

    def call(self, x, training=None):
        h = self._attention(x, training=training)
        h = self._bn_conv(h, training=training)
        return tf.nn.leaky_relu(x + h)


class PolicyHead(tf.keras.layers.Layer):

    def __init__(self, num_policy_per_square: int):
        super().__init__()
        self.layers = tf.keras.Sequential([
            tf.keras.layers.Conv1D(num_policy_per_square, 1),
            tf.keras.layers.Flatten(),
        ])

    def call(self, x):
        return self.layers(x)


class ValueHead(tf.keras.layers.Layer):

    def __init__(self, shape: tp.Tuple[int, int]):
        super().__init__()
        self.layers = tf.keras.Sequential([
            tf.keras.layers.DepthwiseConv1D(
                shape[0] * shape[1],
                use_bias=False,
                depthwise_constraint=HorizontalSymmetry(shape),
            ),
            tf.keras.layers.BatchNormalization(center=False, scale=False),
            tf.keras.layers.LeakyReLU(),
            tf.keras.layers.Flatten(),
            tf.keras.layers.Dense(1, use_bias=False),
        ])

    def call(self, x, training=None):
        x = self.layers(x, training=training)
        if training:
            return x
        return tf.tanh(x)


GameClass = tp.TypeVar('Game', bound=Game)


def build_policy_value_network(
    game_class: GameClass,
    hidden_channels: int,
    bottleneck_channels: int,
    num_backbone_blocks: int,
):
    """Return policy-value network.

    Parameters
    ----------
    game_class : GameClass
        A subclass of `vshogi.Game` class.
    hidden_channels : int
        Number of feature-channel of output of backbone network.
    bottleneck_channels : int
        Number of feature-channel in bottleneck block.
    num_backbone_blocks : int
        Number of backbone blocks.
    """
    input_size = (game_class.files, game_class.ranks)
    input_channels = game_class.feature_channels
    num_policy_per_square = (
        game_class._get_move_class()._num_policy_per_square())
    attention_maps = np.concatenate((
        np.eye(game_class.ranks * game_class.files).reshape(
            1, game_class.files, game_class.ranks,
            game_class.files, game_class.ranks,
        ),
        game_class.get_local_attentions(),
        game_class.get_adjacent_attention()[None, ...],
        game_class.get_diagonal_attention()[None, ...],
    ), axis=0)

    x = tf.keras.Input(shape=(*input_size, input_channels))
    h = tf.keras.layers.Reshape(
        [input_size[0] * input_size[1], input_channels])(x)

    h = tf.keras.layers.Conv1D(hidden_channels, 1)(h)
    h = tf.keras.layers.BatchNormalization(center=False, scale=False)(h)
    h = tf.keras.layers.ReLU()(h)
    for _ in range(num_backbone_blocks):
        h = ResBlock(hidden_channels, bottleneck_channels, attention_maps)(h)

    policy_logits = PolicyHead(num_policy_per_square)(h)
    value = ValueHead(input_size)(h)
    model = tf.keras.Model(inputs=x, outputs=[policy_logits, value])
    return model
