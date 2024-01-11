import numpy as np
import pytest

import vshogi.minishogi as shogi


def test_ranks():
    assert 5 == shogi.Game.ranks
    assert 5 == shogi.Game().ranks


def test_files():
    assert 5 == shogi.Game.files
    assert 5 == shogi.Game().files


def test_feature_channels():
    assert 2 * (10 + 5) == shogi.Game.feature_channels
    assert 2 * (10 + 5) == shogi.Game().feature_channels


def test_num_dlshogi_policy():
    assert 5 * 5 * (2 * 8 + 5) == shogi.Game.num_dlshogi_policy
    assert 5 * 5 * (2 * 8 + 5) == shogi.Game().num_dlshogi_policy


def test_array_black():
    # Turn: BLACK
    # White: KA
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |-KA|-GI|-OU|   |
    #   *---*---*---*---*---*
    # B |-HI|   |   |   |-FU|
    #   *---*---*---*---*---*
    # C |   |   |   |   |   |
    #   *---*---*---*---*---*
    # D |+FU|   |   |   |   |
    #   *---*---*---*---*---*
    # E |+OU|+KI|+GI|   |+HI|
    #   *---*---*---*---*---*
    # Black: KI
    game = shogi.Game().apply(shogi.C4, shogi.E2).apply(shogi.B5, shogi.A5)
    game.apply(shogi.A2, shogi.C4, True).apply(shogi.A2, shogi.A1)

    actual = game.to_dlshogi_features()
    assert np.allclose(actual[0, ..., 0], 0)  # white's captured pawn
    assert np.allclose(actual[0, ..., 1], 0)  # white's captured silver
    assert np.allclose(actual[0, ..., 2], 0)  # white's captured bishop
    assert np.allclose(actual[0, ..., 3], 0)  # white's captured rook
    assert np.allclose(actual[0, ..., 4], 1)  # white's captured gold


def test_array_white():
    game = shogi.Game()
    game.apply(shogi.Move(shogi.SQ_1B, shogi.SQ_1E))
    # Turn: WHITE
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |-HI|-KA|-GI|-KI|-OU|
    #   *---*---*---*---*---*
    # B |   |   |   |   |+HI|
    #   *---*---*---*---*---*
    # C |   |   |   |   |   |
    #   *---*---*---*---*---*
    # D |+FU|   |   |   |   |
    #   *---*---*---*---*---*
    # E |+OU|+KI|+GI|+KA|   |
    #   *---*---*---*---*---*
    # Black: FU

    actual = game.to_dlshogi_features()
    assert actual.dtype == np.float32
    assert actual.shape == (1, 5, 5, 30)
    assert np.allclose(actual[0, ..., 0], 0)  # white's captured pawn
    assert np.allclose(actual[0, ..., 1], 0)  # white's captured silver
    assert np.allclose(actual[0, ..., 2], 0)  # white's captured gold
    assert np.allclose(actual[0, ..., 3], 0)  # white's captured bishop
    assert np.allclose(actual[0, ..., 4], 0)  # white's captured rook
    assert np.allclose(actual[0, ..., 5], 0)  # white's board pawn
    assert np.allclose(actual[0, ..., 6], [
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 1, 0, 0],
    ])  # white's board silver
    assert np.allclose(actual[0, ..., 7], [
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 1, 0],
    ])  # white's board bishop
    assert np.allclose(actual[0, ..., 8], [
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 1],
    ])  # white's board rook
    assert np.allclose(actual[0, ..., 9], [
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0],
    ])  # white's board gold
    assert np.allclose(actual[0, ..., 15], 1)  # black's captured pawn
    assert np.allclose(actual[0, ..., 23], [
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [1, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
    ])  # black's rook


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


def test_to_dlshogi_policy():
    # Turn: WHITE
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |-KA|-GI|-KI|-OU|
    #   *---*---*---*---*---*
    # B |   |   |   |   |-FU|
    #   *---*---*---*---*---*
    # C |   |   |   |   |   |
    #   *---*---*---*---*---*
    # D |+FU|   |   |   |   |
    #   *---*---*---*---*---*
    # E |+OU|+KI|+GI|+KA|+HI|
    #   *---*---*---*---*---*
    # Black: -
    game = shogi.Game("1bsgk/4p/5/P4/KGSBR w - 1")
    a = shogi.Move(shogi.C1, shogi.B1)
    actual = game.to_dlshogi_policy(a, max_value=0.5)

    expected = np.zeros(5 * 5 * (2 * 8 + 5))
    expected[a.rotate()._to_dlshogi_policy_index()] = 0.5
    for m in game.get_legal_moves():
        if m == a:
            continue
        expected[m.rotate()._to_dlshogi_policy_index()] = 0.05
    assert np.allclose(expected, actual)


def test_get_sfen_at():
    actual = shogi.Game().apply(shogi.C5, shogi.D5).get_sfen_at(0)
    assert "rbsgk/4p/5/P4/KGSBR b - 1" == actual

    actual = shogi.Game().apply(shogi.C5, shogi.D5).get_sfen_at(
        0, include_move_count=False)
    assert "rbsgk/4p/5/P4/KGSBR b -" == actual

    game = shogi.Game('4k/5/5/5/K4 b RBGSPrbgsp')
    game.apply(game.get_legal_moves()[0])
    actual = game.get_sfen_at(-1)
    assert "4k/5/5/5/K4 b RBGSPrbgsp 1" == actual


if __name__ == '__main__':
    pytest.main([__file__])
