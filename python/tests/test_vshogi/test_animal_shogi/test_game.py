from copy import copy, deepcopy

import numpy as np
import pytest

import vshogi.animal_shogi as shogi


def test_init():
    shogi.Game()


def test_copy():
    g1 = shogi.Game()
    g2 = g1.copy()
    g1.apply(shogi.Move(shogi.B2, shogi.B3))
    assert g1.board[shogi.B2] != g2.board[shogi.B2]


def test_shallow_copy():
    g1 = shogi.Game()
    g2 = copy(g1)
    g1.apply(shogi.Move(shogi.B2, shogi.B3))
    assert g1.board[shogi.B2] == g2.board[shogi.B2]


def test_deepcopy():
    g1 = shogi.Game()
    g2 = deepcopy(g1)
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
    actual = np.asarray(game)
    assert actual.dtype == np.float32
    assert actual.shape == (1, 4, 3, 16)
    assert np.allclose(actual[0, ..., 0], 0)
    assert np.allclose(actual[0, ..., 8], 1)
    assert np.allclose(actual[0, 2, 0, 11], 0)
    assert np.allclose(actual[0, 2, 1, 11], 1)


if __name__ == '__main__':
    pytest.main([__file__])
