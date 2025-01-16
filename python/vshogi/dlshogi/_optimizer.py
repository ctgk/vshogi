import tensorflow as tf


class _AdamScheduleFree(tf.Module):
    # Note:
    # Schedule-Free verison of Adam optimizer unfortunately performed worse
    # than the original one.
    # But keeping the code just in case.

    def __init__(self, learning_rate=1e-3, beta_1=0.9, beta_2=0.999, ep=1e-7):
        # Initialize the Adam parameters
        self.beta_1 = beta_1
        self.beta_2 = beta_2
        self.learning_rate = learning_rate
        self.ep = ep
        self.t = tf.Variable(tf.constant(1, dtype=tf.float32))
        self.v_dvar, self.s_dvar, self.z_list = [], [], []
        self.title = f"Adam: learning rate={self.learning_rate}"
        self._built = False

    def _build(self, var_list):
        if not self._built:
            for var in var_list:
                self.v_dvar.append(tf.Variable(tf.zeros_like(var)))
                self.s_dvar.append(tf.Variable(tf.zeros_like(var)))
                self.z_list.append(tf.Variable(var))
        self._built = True

    def _update_single_var(self, grad, var, v, s, z):
        c = tf.divide(1, self.t)
        v.assign(self.beta_1 * v + (1 - self.beta_1) * grad)
        s.assign(self.beta_2 * s + (1 - self.beta_2) * tf.square(grad))

        v_bc = v / (1 - tf.pow(tf.cast(self.beta_1, var.dtype), self.t))
        s_bc = s / (1 - tf.pow(tf.cast(self.beta_2, var.dtype), self.t))

        update = self.learning_rate * (v_bc / (tf.sqrt(s_bc) + self.ep))
        var.assign(
            (1 - c) * var + c * z + (self.beta_1 * (1 - c) - 1) * update,
        )
        z.assign_sub(update)

    @tf.function
    def _update(self, grads, var_list):
        for i, (g, var) in enumerate(zip(grads, var_list)):
            self._update_single_var(
                g, var, self.v_dvar[i], self.s_dvar[i], self.z_list[i])

    def apply_gradients(self, grads_and_vars):
        grads, var_list = tuple(zip(*tuple(grads_and_vars)))
        self._build(var_list)
        self._update(grads, var_list)
        self.t.assign_add(1)

    def finalize_variable_values(self, var_list):
        for var, z in zip(var_list, self.z_list):
            var.assign((var - (1 - self.beta_1) * z) / self.beta_1)
