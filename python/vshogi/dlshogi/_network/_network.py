# flake8: noqa
import typing as tp

import numpy as np
import torch as th

from vshogi._game import Game
from vshogi.dlshogi._network._policy_head import _PolicyHead
from vshogi.dlshogi._network._residual_block import _ResidualBlock
from vshogi.dlshogi._network._value_head import _ValueHead
from vshogi.dlshogi._network._action_value_head import _ActionValueHead


GameClass = tp.TypeVar('Game', bound=Game)


class PolicyValueNetwork(th.nn.Module):
    """Policy-value network.

    State value head:
        Input (B, C_in, H, W) -> [Output1 (B, H*W*P), Output2 (B, 1)]
    Action value head:
        Input (B, C_in, H, W) -> [Output1 (B, H*W*P), Output2 (B, H*W*P)]
    """

    def __init__(
        self,
        game_class: GameClass,
        hidden_channels: int,
        bottleneck_channels: int,
        num_backbone_blocks: int,
        *,
        action_value_head: bool = False,
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
        action_value_head : bool
            Use action value head if true, default is false.
        """
        super().__init__()
        in_ch: int = game_class.feature_channels
        shape = (game_class.files, game_class.ranks)
        num_policy_per_square = (
            game_class._get_move_class()._num_policy_per_square()
        )
        attentions = np.concatenate(
            (
                np.eye(shape[0] * shape[1]).reshape(1, *shape, *shape),
                game_class.get_local_attentions(),
                game_class.get_adjacent_attention()[None, ...],
                game_class.get_diagonal_attention()[None, ...],
            ),
            axis=0,
        )
        self._backbone = th.nn.Sequential(
            th.nn.Conv2d(in_ch, hidden_channels, kernel_size=1, bias=False),
            th.nn.BatchNorm2d(hidden_channels),
            th.nn.ReLU(inplace=True),
            *[
                _ResidualBlock(
                    hidden_channels,
                    bottleneck_channels,
                    attentions,
                    attention_groups=8,
                )
                for _ in range(num_backbone_blocks)
            ],
        )
        self._policy_head = _PolicyHead(hidden_channels, num_policy_per_square)
        if action_value_head:
            self._value_head = _ActionValueHead(
                hidden_channels, num_policy_per_square
            )
        else:
            self._value_head = _ValueHead(hidden_channels, shape)

    def forward(self, x: th.Tensor) -> tp.Tuple[th.Tensor, th.Tensor]:
        # x: (B, C_in, H, W)
        x = self._backbone(x)  # (B, C_hid, H, W)
        p = self._policy_head(x)
        v = self._value_head(x)
        return p, v
