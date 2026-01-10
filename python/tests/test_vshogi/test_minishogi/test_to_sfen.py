import pytest

import vshogi.shogi as shogi


@pytest.mark.parametrize('p, expect', [
    (shogi.Piece.FU, "p"),
    (shogi.Piece.GI, "s"),
    (shogi.Piece.KA, "b"),
    (shogi.Piece.HI, "r"),
    (shogi.Piece.KI, "g"),
    (shogi.Piece.OU, "k"),
    (shogi.Piece.TO, "+p"),
    (shogi.Piece.NG, "+s"),
    (shogi.Piece.UM, "+b"),
    (shogi.Piece.RY, "+r"),
    (shogi.Piece.NA, "?"),
    (shogi.BoardPiece.B_FU, "P"),
    (shogi.BoardPiece.B_GI, "S"),
    (shogi.BoardPiece.B_KA, "B"),
    (shogi.BoardPiece.B_HI, "R"),
    (shogi.BoardPiece.B_KI, "G"),
    (shogi.BoardPiece.B_OU, "K"),
    (shogi.BoardPiece.B_TO, "+P"),
    (shogi.BoardPiece.B_NG, "+S"),
    (shogi.BoardPiece.B_UM, "+B"),
    (shogi.BoardPiece.B_RY, "+R"),
    (shogi.BoardPiece.W_FU, "p"),
    (shogi.BoardPiece.W_GI, "s"),
    (shogi.BoardPiece.W_KA, "b"),
    (shogi.BoardPiece.W_HI, "r"),
    (shogi.BoardPiece.W_KI, "g"),
    (shogi.BoardPiece.W_OU, "k"),
    (shogi.BoardPiece.W_TO, "+p"),
    (shogi.BoardPiece.W_NG, "+s"),
    (shogi.BoardPiece.W_UM, "+b"),
    (shogi.BoardPiece.W_RY, "+r"),
    (shogi.BoardPiece.VOID, "?"),
])
def test_to_sfen(p, expect):
    actual = p.to_sfen()
    assert actual == expect


if __name__ == '__main__':
    pytest.main([__file__])
