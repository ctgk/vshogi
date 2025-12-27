import numpy as np
import torch as th

from vshogi.dlshogi._network._depthwise_attention import _DepthwiseAttention


class _ResidualBlock(th.nn.Module):

    def __init__(
        self,
        in_ch: int,
        hid_ch: int,
        attentions: np.ndarray,
        attention_groups: int,
    ) -> None:
        super().__init__()
        self.layers = th.nn.Sequential(
            # (B, C_in, H, W) -> (B, C_hid, H, W)
            th.nn.Conv2d(in_ch, hid_ch, kernel_size=1, bias=False),
            th.nn.BatchNorm2d(hid_ch),
            th.nn.ReLU(inplace=True),

            # (B, C_hid, H, W) -> (B, C_hid, H, W)
            _DepthwiseAttention(attentions, attention_groups),
            th.nn.ReLU(inplace=True),

            # (B, C_hid, H, W) -> (B, C_in, H, W)
            th.nn.Conv2d(hid_ch, in_ch, kernel_size=1, bias=False),
            th.nn.BatchNorm2d(in_ch),
            th.nn.Dropout(p=0.1, inplace=True),
        )

    def forward(self, x: th.Tensor) -> th.Tensor:
        # x: (B, C_in, H*W)
        x = x + self.layers(x)
        return th.nn.functional.relu(x, inplace=True)
