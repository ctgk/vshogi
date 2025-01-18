# flake8: noqa
import tensorflow as tf


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
