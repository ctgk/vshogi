import numpy as np
import pytest

import vshogi.minishogi as shogi


def test_array():
    game = shogi.Game()
    game.apply(shogi.Move(shogi.SQ_1B, shogi.SQ_1E))
    actual = np.asarray(game)
    assert actual.dtype == np.float32
    assert actual.shape == (1, 5, 5, 30)
    assert np.allclose(actual[0, ..., 0], 0)  # white's captured pawn
    assert np.allclose(actual[0, ..., 15], 1)  # black's captured pawn
    assert np.allclose(actual[0, 3, 0, 23], 1)  # black's rook
    assert np.allclose(actual[0, 2, 0, 25], 0)  # not black's rook
    assert np.allclose(actual[0, 4, 4, 8], 1)  # white's rook
    assert np.allclose(actual[0, 3, 3, 8], 0)  # not white's rook


if __name__ == '__main__':
    pytest.main([__file__])
