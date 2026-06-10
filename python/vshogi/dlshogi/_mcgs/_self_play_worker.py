from typing import Callable

import numpy as np

from vshogi.dlshogi._alpha_zero._self_play_worker import (
    _SelfPlayWorker as _AlphaZeroWorker,
)
from vshogi.dlshogi._policy_value_function import PolicyValueFunction
from vshogi.engine._mcgs import Mcgs
from vshogi.engine._piece_value_func import piece_value_func


class _SelfPlayWorker(_AlphaZeroWorker):
    def __init__(self, **kwargs) -> None:
        super().__init__(coeff_puct=4.0, kldgain_threshold=1e-5, **kwargs)

    def _load_player(self, tflite_path: str | None) -> Mcgs:
        player = Mcgs(
            policy_value_func=(
                (
                    lambda g: (
                        np.zeros(g.num_dlshogi_policy, dtype=np.float32),
                        piece_value_func(g),
                    )
                )
                if tflite_path is None
                else PolicyValueFunction(tflite_path)
            ),
            epsilon_greedy=self._epsilon_greedy,
            dfpn_search_root=self._dfpn_search_root,
            dfpn_search_leaf=self._dfpn_search_leaf,
            name=(
                "none"
                if tflite_path is None
                else tflite_path.split("/")[-1].split(".")[0]
            ),
        )
        player.get_q_value = player.get_value
        return player

    @staticmethod
    def _get_cli_options(prefix="") -> dict[str, Callable]:
        options = _AlphaZeroWorker._get_cli_options(prefix)
        options = {
            k: v
            for k, v in options.items()
            if k not in ("coeff-puct", "kldgain-threshold")
        }
        return options
