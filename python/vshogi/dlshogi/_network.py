# flake8: noqa
import typing as tp

import numpy as np
import torch as th

from vshogi._game import Game
from vshogi.dlshogi._depthwise_attention import _DepthwiseAttention
from vshogi.dlshogi._policy_head import PolicyHead
from vshogi.dlshogi._residual_block import _ResidualBlock
from vshogi.dlshogi._value_head import _ValueHead


GameClass = tp.TypeVar('Game', bound=Game)


class PolicyValueNetwork(th.nn.Module):
    """Policy-value network.

    Input (B, C_in, H, W) -> [Output1 (B, H*W*P), Output2 (B, 1)]
    """

    def __init__(
        self,
        game_class: GameClass,
        hidden_channels: int,
        bottleneck_channels: int,
        num_backbone_blocks: int,
    ):
        """Initialize policy-value network.

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
        super().__init__()
        in_ch: int = game_class.feature_channels
        num_squares = game_class.files * game_class.files
        num_policy_per_square = (
            game_class._get_move_class()._num_policy_per_square())
        attentions = np.concatenate((
            np.eye(num_squares).reshape(
                1, game_class.files, game_class.ranks,
                game_class.files, game_class.ranks,
            ),
            game_class.get_local_attentions(),
            game_class.get_adjacent_attention()[None, ...],
            game_class.get_diagonal_attention()[None, ...],
        ), axis=0)
        self._backbone = th.nn.Sequential(
            th.nn.Flatten(2, -1),
            th.nn.Conv1d(in_ch, hidden_channels, kernel_size=1, bias=False),
            th.nn.BatchNorm1d(hidden_channels, affine=False),
            th.nn.LeakyReLU(inplace=True),
            *[
                _ResidualBlock(
                    hidden_channels, bottleneck_channels, attentions,
                    attention_groups=8,
                )
                for _ in range(num_backbone_blocks)
            ],
        )
        self._policy_head = PolicyHead(hidden_channels, num_policy_per_square)
        self._value_head = _ValueHead(hidden_channels, num_squares)

    def forward(self, x: th.Tensor) -> tp.Tuple[th.Tensor, th.Tensor]:
        # x: (B, H, W, C_in)
        x = x.moveaxis(-1, 1)  # (B, C_in, H, W)
        x = self._backbone(x)  # (B, C_hid, H*W)
        p = self._policy_head(x)
        v = self._value_head(x)
        return p, v
