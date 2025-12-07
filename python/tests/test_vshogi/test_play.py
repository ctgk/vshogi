import pytest

import vshogi.minishogi as shogi
from vshogi import play_game
from vshogi.engine import AlphaZero


def test_play_game():
    game = shogi.Game()
    play_game(
        game,
        AlphaZero(),
        AlphaZero(),
    )
    assert game.result != shogi.ONGOING


if __name__ == '__main__':
    pytest.main([__file__])
