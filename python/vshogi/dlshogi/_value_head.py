import typing as tp

import torch as th


class _ValueHead(th.nn.Module):

    def __init__(self, in_ch: int, shape: tp.Tuple[int, int]):
        super().__init__()
        self.layers = th.nn.Sequential(
            th.nn.Conv2d(
                in_ch, in_ch,
                kernel_size=shape, groups=in_ch, bias=False,
            ),
            th.nn.BatchNorm2d(in_ch, affine=False),
            th.nn.LeakyReLU(inplace=True),
            th.nn.Flatten(),
            th.nn.Linear(in_ch, 1, bias=False),
        )

    def forward(self, x: th.Tensor):
        # x: (B, C_in, H, W)
        x = self.layers(x)
        if self.training:
            return x
        return th.tanh(x)
