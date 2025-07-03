import torch as th


class PolicyHead(th.nn.Module):
    """Policy head class.

    Input (B, C_in, H*W) -> Output (B, H*W * C_out)
    """

    def __init__(self, in_ch: int, out_ch: int):
        """Initialize policy head.

        Parameters
        ----------
        in_ch : int
            # of channels of input.
        out_ch : int
            # of policy per square.
        """
        super().__init__()
        self.conv = th.nn.Conv1d(in_ch, out_ch, kernel_size=1)

    def forward(self, x: th.Tensor):
        """Forward propagate an input.

        Parameters
        ----------
        x : th.Tensor (B, C_in, H*W)
            Input

        Returns
        -------
        th.Tensor (B, H*W * C_out)
            Logit of policies.
        """
        # x: (B, C_in, H*W)
        x = self.conv(x)
        return x.transpose(-1, -2).flatten(1, -1)
