import torch as th


class _ActionValueHead(th.nn.Module):
    """Action value head class.

    Input (B, C_in, H, W) -> Output (B, H, W, C_out)
    """

    def __init__(self, in_ch: int, out_ch: int):
        super().__init__()
        self.layers = th.nn.Sequential(
            th.nn.Conv2d(
                in_ch,
                in_ch,
                kernel_size=1,
                bias=False,
            ),
            th.nn.BatchNorm2d(in_ch, affine=False),
            th.nn.ReLU(inplace=True),
            th.nn.Conv2d(in_ch, out_ch, kernel_size=1),
        )

    def forward(self, x: th.Tensor):
        # x: (B, C_in, H, W)
        x = self.layers(x)
        x = x.moveaxis(1, -1)
        if self.training:
            return x
        return th.tanh(x)
