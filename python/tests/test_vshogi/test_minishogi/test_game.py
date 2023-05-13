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


def test_get_valid_move_to():
    # Turn: BLACK
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |-KA|-GI|-KI|-OU|
    #   *---*---*---*---*---*
    # B |-HI|   |   |   |-FU|
    #   *---*---*---*---*---*
    # C |   |   |   |   |   |
    #   *---*---*---*---*---*
    # D |+FU|   |   |   |+HI|
    #   *---*---*---*---*---*
    # E |+OU|+KI|+GI|+KA|   |
    #   *---*---*---*---*---*
    # Black: -
    game = shogi.Game(sfen="1bsgk/r3p/5/P3R/KGSB1 b -")
    assert game.get_legal_move_to(shogi.SQ_1B, shogi.SOUTH) == shogi.Move(
        shogi.SQ_1B, shogi.SQ_1D)
    assert game.get_legal_move_to(shogi.SQ_1B, shogi.SOUTH, True) is None
    assert game.get_legal_move_to(shogi.SQ_5A, shogi.FU) is None


if __name__ == '__main__':
    pytest.main([__file__])
