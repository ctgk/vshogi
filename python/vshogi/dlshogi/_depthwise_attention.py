import numpy as np
import torch as th


class _DepthwiseAttention(th.nn.Module):

    def __init__(
        self,
        attentions: np.ndarray,
        groups: int,
        bias: bool = True,
    ):
        super().__init__()
        self._groups = groups
        shape = attentions.shape
        attentions = attentions.reshape(
            shape[0], shape[1] * shape[2], shape[3] * shape[4])
        attentions = attentions.transpose(1, 2, 0)  # (H*W, H*W, D)
        self.register_buffer(
            '_attentions', th.tensor(attentions, dtype=th.float32))
        self._kernel = th.nn.Parameter(
            th.randn(attentions.shape[-1], groups), requires_grad=True)
        if bias:
            self._bias = th.nn.Parameter(
                th.zeros((groups, 1, attentions.shape[0])), requires_grad=True)

    def forward(self, x: th.Tensor):
        # x: (B, C_in, H*W)
        assert x.shape[1] % self._groups == 0, (
            f"x.shape[1]({x.shape[1]}) % groups({self._groups}) != 0")
        ch_in_group = x.shape[1] // self._groups

        # (B, G, C/G, H*W)
        h = x.reshape(-1, self._groups, ch_in_group, x.shape[-1])
        if self.training:
            w = th.matmul(self._attentions, self._kernel)  # (H*W, H*W, G)
            w = w.moveaxis(2, 0)  # (G, H*W, H*W)
        else:
            w = self._w
        h = th.matmul(h, w)  # (B, G, C/G, H*W)
        if hasattr(self, '_bias'):
            h = h + self._bias
        h = h.reshape(-1, x.shape[1], x.shape[-1])  # (B, C, H*W)
        return h

    def train(self, mode: bool = True):
        if mode is False:
            self.register_buffer(
                '_w',
                th.matmul(
                    self._attentions, self._kernel.detach()).moveaxis(2, 0),
            )
        return super().train(mode)
