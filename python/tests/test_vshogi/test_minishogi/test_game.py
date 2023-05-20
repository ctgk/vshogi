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


def test_stand():
    game = shogi.Game('2r2/1p1B+B/k1s2/G1R2/K1G2 w SP 2')
    # Turn: WHITE
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |   |-HI|   |   |
    #   *---*---*---*---*---*
    # B |   |-FU|   |+KA|+UM|
    #   *---*---*---*---*---*
    # C |-OU|   |-GI|   |   |
    #   *---*---*---*---*---*
    # D |+KI|   |+HI|   |   |
    #   *---*---*---*---*---*
    # E |+OU|   |+KI|   |   |
    #   *---*---*---*---*---*
    # Black: GI,FU
    assert game.stand(shogi.BLACK) == {
        shogi.HI: 0, shogi.KA: 0, shogi.KI: 0, shogi.GI: 1, shogi.FU: 1,
    }
    assert game.stand(shogi.WHITE) == {
        shogi.HI: 0, shogi.KA: 0, shogi.KI: 0, shogi.GI: 0, shogi.FU: 0,
    }


if __name__ == '__main__':
    pytest.main([__file__])
