import typing as tp

import numpy as np
import torch as th
from executorch.runtime import Runtime

from vshogi._game import Game


class PolicyValueFunction:
    """Policy-value function class."""

    def __init__(self, model_path: str) -> None:
        """Construct policy-value function.

        Parameters
        ----------
        model_path : str
            Path to pte model.
        """
        runtime = Runtime.get()
        self._forward = runtime.load_program(model_path).load_method('forward')
        self._input_placeholder = th.empty(
            self._forward.metadata.input_tensor_meta(0).sizes(),
            dtype=th.float32,
        )

    def __call__(self, game: Game) -> tp.Tuple[np.ndarray, float]:
        """Return logits of policy and value of the current game state.

        Parameters
        ----------
        game : Game
            Current game state to evaluate.

        Returns
        -------
        tp.Tuple[np.ndarray, float]
            Tuple of logits of policy and value.
        """
        game.to_dlshogi_features(out=self._input_placeholder)
        policy_logits, value = self._forward.execute([self._input_placeholder])
        return policy_logits, value
