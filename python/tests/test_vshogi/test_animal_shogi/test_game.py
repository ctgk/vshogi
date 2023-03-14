import pytest

import vshogi.animal_shogi as shogi


def test_init():
    shogi.Game()


def test_hash():
    game = shogi.Game().apply(shogi.Move(shogi.B2, shogi.B3))

    actual = shogi.Game(game.hash_current_state())
    assert shogi.B_CH == actual.board[shogi.B2]
    assert shogi.VOID == actual.board[shogi.B3]
    assert {shogi.CH: 1, shogi.EL: 0, shogi.GI: 0} == actual.black_stand
    assert {shogi.CH: 0, shogi.EL: 0, shogi.GI: 0} == actual.white_stand


if __name__ == '__main__':
    pytest.main([__file__])
