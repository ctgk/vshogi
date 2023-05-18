import numpy as np
import pytest

import vshogi.minishogi as shogi
from vshogi import play_game
from vshogi.engine import MonteCarloTreeSearcher


def test_play_game():
    def uniform_policy_zero_value_func(game):
        legal_moves = game.get_legal_moves()
        if len(legal_moves) == 0:
            return {}, (0. if game.result == shogi.DRAW else -1.)
        return (
            {
                m: p for m, p in zip(
                    legal_moves,
                    np.ones(len(legal_moves)) / len(legal_moves),
                )
            },
            0.,  # value
        )

    game = shogi.Game()
    play_game(
        game,
        MonteCarloTreeSearcher(uniform_policy_zero_value_func),
        MonteCarloTreeSearcher(uniform_policy_zero_value_func),
    )
    assert game.result != shogi.ONGOING


if __name__ == '__main__':
    pytest.main([__file__])
