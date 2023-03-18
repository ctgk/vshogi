from copy import copy, deepcopy

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


def test_hash():
    game = shogi.Game().apply(shogi.Move(shogi.B2, shogi.B3))

    actual = shogi.Game(game.hash_current_state())
    assert shogi.B_CH == actual.board[shogi.B2]
    assert shogi.VOID == actual.board[shogi.B3]
    assert {shogi.CH: 1, shogi.EL: 0, shogi.GI: 0} == actual.black_stand
    assert {shogi.CH: 0, shogi.EL: 0, shogi.GI: 0} == actual.white_stand


if __name__ == '__main__':
    pytest.main([__file__])
