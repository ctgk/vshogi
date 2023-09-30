import numpy as np
import pytest

import vshogi.minishogi as shogi
from vshogi import play_game
from vshogi.engine import MonteCarloTreeSearcher


def test_play_game():
    def uniform_policy_zero_value_func(game):
        return np.zeros(game.num_dlshogi_policy), 0.

    game = shogi.Game()
    play_game(
        game,
        MonteCarloTreeSearcher(uniform_policy_zero_value_func),
        MonteCarloTreeSearcher(uniform_policy_zero_value_func),
    )
    assert game.result != shogi.ONGOING


if __name__ == '__main__':
    pytest.main([__file__])
