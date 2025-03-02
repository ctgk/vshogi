import pytest

import vshogi.judkins_shogi as shogi


@pytest.mark.parametrize('p, expect', [
    (shogi.Piece.FU, "歩"),
    (shogi.Piece.KE, "桂"),
    (shogi.Piece.GI, "銀"),
    (shogi.Piece.KA, "角"),
    (shogi.Piece.HI, "飛"),
    (shogi.Piece.KI, "金"),
    (shogi.Piece.OU, "玉"),
    (shogi.Piece.TO, "と"),
    (shogi.Piece.NK, "圭"),
    (shogi.Piece.NG, "全"),
    (shogi.Piece.UM, "馬"),
    (shogi.Piece.RY, "龍"),
    (shogi.Piece.NA, "？"),
    (shogi.BoardPiece.B_FU, "歩"),
    (shogi.BoardPiece.B_KE, "桂"),
    (shogi.BoardPiece.B_GI, "銀"),
    (shogi.BoardPiece.B_KA, "角"),
    (shogi.BoardPiece.B_HI, "飛"),
    (shogi.BoardPiece.B_KI, "金"),
    (shogi.BoardPiece.B_OU, "玉"),
    (shogi.BoardPiece.B_TO, "と"),
    (shogi.BoardPiece.B_NK, "圭"),
    (shogi.BoardPiece.B_NG, "全"),
    (shogi.BoardPiece.B_UM, "馬"),
    (shogi.BoardPiece.B_RY, "龍"),
    (shogi.BoardPiece.W_FU, "歩"),
    (shogi.BoardPiece.W_KE, "桂"),
    (shogi.BoardPiece.W_GI, "銀"),
    (shogi.BoardPiece.W_KA, "角"),
    (shogi.BoardPiece.W_HI, "飛"),
    (shogi.BoardPiece.W_KI, "金"),
    (shogi.BoardPiece.W_OU, "玉"),
    (shogi.BoardPiece.W_TO, "と"),
    (shogi.BoardPiece.W_NK, "圭"),
    (shogi.BoardPiece.W_NG, "全"),
    (shogi.BoardPiece.W_UM, "馬"),
    (shogi.BoardPiece.W_RY, "龍"),
    (shogi.BoardPiece.VOID, "？"),
])
def test_to_jpn(p, expect):
    actual = p.to_jpn()
    assert expect == actual


if __name__ == '__main__':
    pytest.main([__file__])
