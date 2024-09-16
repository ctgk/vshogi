from copy import copy

import numpy as np
import pytest

import vshogi.animal_shogi as shogi


def test_ranks():
    assert 4 == shogi.Game.ranks


def test_files():
    assert 3 == shogi.Game.files


def test_feature_channels():
    assert 2 * (5 + 3) == shogi.Game.feature_channels


def test_num_dlshogi_policy():
    assert 3 * 4 * (8 + 3) == shogi.Game.num_dlshogi_policy
    assert 3 * 4 * (8 + 3) == shogi.Game().num_dlshogi_policy


def test_get_attention():
    a = shogi.Game.get_attention()
    assert a.shape == (12, 12)
    assert np.allclose(a.T, a)
    expect = np.array([
        [0, 1, 0],
        [0, 1, 1],
        [0, 0, 0],
        [0, 0, 0],
    ])
    assert np.allclose(a[2].reshape(4, 3), expect)


def test_get_adjacent_attention():
    a = shogi.Game.get_adjacent_attention()
    assert a.shape == (12, 12)
    assert np.allclose(a.T, a)
    expect = np.array([
        [0, 1, 0],
        [0, 0, 1],
        [0, 0, 0],
        [0, 0, 0],
    ])
    assert np.allclose(a[2].reshape(4, 3), expect)


def test_get_diagonal_attention():
    a = shogi.Game.get_diagonal_attention()
    assert a.shape == (12, 12)
    assert np.allclose(a.T, a)
    expect = np.array([
        [0, 0, 0],
        [0, 1, 0],
        [0, 0, 0],
        [0, 0, 0],
    ])
    assert np.allclose(a[2].reshape(4, 3), expect)


def test_init():
    shogi.Game()


def test_shallow_copy():
    g1 = shogi.Game()
    g2 = copy(g1)
    g1.apply(shogi.Move(shogi.B2, shogi.B3))
    assert g1.board[shogi.B2] == g2.board[shogi.B2]


def test_copy():
    g1 = shogi.Game()
    g2 = g1.copy()
    g1.apply(shogi.Move(shogi.B2, shogi.B3))
    assert g1.board[shogi.B2] != g2.board[shogi.B2]


def test_to_sfen():
    game = shogi.Game().apply(shogi.Move(shogi.B2, shogi.B3))

    actual = shogi.Game(game.to_sfen())
    assert shogi.B_CH == actual.board[shogi.B2]
    assert shogi.VOID == actual.board[shogi.B3]
    assert {shogi.CH: 1, shogi.EL: 0, shogi.GI: 0} == actual.stand(shogi.BLACK)
    assert {shogi.CH: 0, shogi.EL: 0, shogi.GI: 0} == actual.stand(shogi.WHITE)


def test_board():
    game: shogi.Game = shogi.Game()
    actual = np.asarray(game.board)

    expected = np.array([
        [shogi.W_GI, shogi.W_LI, shogi.W_EL],
        [shogi.VOID, shogi.W_CH, shogi.VOID],
        [shogi.VOID, shogi.B_CH, shogi.VOID],
        [shogi.B_EL, shogi.B_LI, shogi.B_GI],
    ])
    assert (actual == expected).all()


def test_init_from_sfen():
    game = shogi.Game('Gce/2E/l1G/L2 w c')
    assert game.result == shogi.ONGOING
    print(game)

    game.apply(shogi.Move(shogi.C4, shogi.CH))
    print(game.result)
    print(game)
    assert game.result == shogi.ONGOING


def test_play():
    game = shogi.Game()
    game.apply(shogi.Move(shogi.B2, shogi.B3))
    game.apply(shogi.Move(shogi.C2, shogi.B1))
    game.apply(shogi.Move(shogi.A2, shogi.CH))
    game.apply(shogi.Move(shogi.B3, shogi.C2))
    game.apply(shogi.Move(shogi.A1, shogi.A2))
    game.apply(shogi.Move(shogi.A2, shogi.B3))
    game.apply(shogi.Move(shogi.C3, shogi.C4))
    game.apply(shogi.Move(shogi.A1, shogi.A2))
    game.apply(shogi.Move(shogi.B3, shogi.A4))
    game.apply(shogi.Move(shogi.B1, shogi.CH))
    game.apply(shogi.Move(shogi.C2, shogi.B3))
    game.apply(shogi.Move(shogi.B2, shogi.A1))
    game.apply(shogi.Move(shogi.A4, shogi.B4))
    game.apply(shogi.Move(shogi.A3, shogi.B2))
    game.apply(shogi.Move(shogi.A1, shogi.GI))
    game.apply(shogi.Move(shogi.C4, shogi.CH))
    print(game)
    assert game.result == shogi.ONGOING


def test_get_legal_moves():
    game = shogi.Game()
    assert len(game.get_legal_moves()) == 4
    game.apply(shogi.Move(shogi.A3, shogi.B4))
    game.apply(shogi.Move(shogi.A2, shogi.B1))
    game.apply(shogi.Move(shogi.A2, shogi.A3))
    print(game)
    assert len(game.get_legal_moves()) == 0


def test_array():
    game = shogi.Game()
    game.apply(shogi.Move(shogi.B2, shogi.B3))
    actual = game.to_dlshogi_features()
    assert actual.dtype == np.float32
    assert actual.shape == (1, 4, 3, 16)
    assert np.allclose(actual[0, ..., 0], 0)
    assert np.allclose(actual[0, ..., 8], 1)
    assert np.allclose(actual[0, 2, 0, 11], 0)
    assert np.allclose(actual[0, 2, 1, 11], 1)


def test_to_dlshogi_policy():
    game = shogi.Game()
    a = shogi.Move(shogi.B2, shogi.B3)
    actual = game.to_dlshogi_policy({
        m: 0.7 if m == a else 0.1 for m in game.get_legal_moves()
    })

    expected = np.zeros(12 * 11)
    expected[a._to_dlshogi_policy_index()] = 0.7
    for m in game.get_legal_moves():
        if m == a:
            continue
        expected[m._to_dlshogi_policy_index()] = 0.1
    assert np.allclose(expected, actual)


def test_get_sfen_at():
    actual = shogi.Game().apply(shogi.B2, shogi.B3).get_sfen_at(0)
    assert "gle/1c1/1C1/ELG b - 1" == actual

    actual = shogi.Game().apply(shogi.B2, shogi.B3).get_sfen_at(
        0, include_move_count=False)
    assert "gle/1c1/1C1/ELG b -" == actual


if __name__ == '__main__':
    pytest.main([__file__])
