import typing as tp

import click as cl

from vshogi.dlshogi._alpha_zero._network_trainer import (
    _NetworkTrainer as _AlphaZeroNetworkTrainer,
)


class _NetworkTrainer(_AlphaZeroNetworkTrainer):
    r"""Network Trainer class for Gumbel-Alpha-Zero algorithm.

    Training Target
    ---------------
    The original [Gumbel-AlphaZero paper](https://openreview.net/pdf?id=bERaNdoegnO)
    does not explicitly specify the training target for the value network.
    We use MCTS value as the training target because the value network should
    approximate v_pi according to the paper:

    > We will construct an approximation of v_pi.
    > The exact v_pi is defined by v_pi = \sum_{a}pi(a)q(a).
    > We have an approximate \hat{v}_pi from a value network,
    > we know pi, and we have q(a) for the visited actions.
    """

    @staticmethod
    def _get_cli_options(prefix: str = "") -> dict[str, tp.Callable]:
        options = _AlphaZeroNetworkTrainer._get_cli_options(prefix)
        options["loss-q-ratio"] = cl.option(
            f"--{prefix}loss-q-ratio",
            default=1.0,
            show_default=True,
            help=(
                "Blends game outcome z with search evaluation q for the "
                "value target: q_ratio * q + (1 - q_ratio) * z. "
                "Set to 1.0 for the original GumbelAlphaZero, or decrease "
                "it to train against game outcomes."
            ),
        )
        return options
