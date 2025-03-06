import pytest

import vshogi.minishogi as shogi


@pytest.mark.parametrize('p, expect', [
    (shogi.Piece.FU, "歩"),
    (shogi.Piece.GI, "銀"),
    (shogi.Piece.KA, "角"),
    (shogi.Piece.HI, "飛"),
    (shogi.Piece.KI, "金"),
    (shogi.Piece.OU, "玉"),
    (shogi.Piece.TO, "と"),
    (shogi.Piece.NG, "全"),
    (shogi.Piece.UM, "馬"),
    (shogi.Piece.RY, "龍"),
    (shogi.Piece.NA, "？"),
    (shogi.BoardPiece.B_FU, "歩"),
    (shogi.BoardPiece.B_GI, "銀"),
    (shogi.BoardPiece.B_KA, "角"),
    (shogi.BoardPiece.B_HI, "飛"),
    (shogi.BoardPiece.B_KI, "金"),
    (shogi.BoardPiece.B_OU, "玉"),
    (shogi.BoardPiece.B_TO, "と"),
    (shogi.BoardPiece.B_NG, "全"),
    (shogi.BoardPiece.B_UM, "馬"),
    (shogi.BoardPiece.B_RY, "龍"),
    (shogi.BoardPiece.W_FU, "歩"),
    (shogi.BoardPiece.W_GI, "銀"),
    (shogi.BoardPiece.W_KA, "角"),
    (shogi.BoardPiece.W_HI, "飛"),
    (shogi.BoardPiece.W_KI, "金"),
    (shogi.BoardPiece.W_OU, "玉"),
    (shogi.BoardPiece.W_TO, "と"),
    (shogi.BoardPiece.W_NG, "全"),
    (shogi.BoardPiece.W_UM, "馬"),
    (shogi.BoardPiece.W_RY, "龍"),
    (shogi.BoardPiece.VOID, "？"),
])
def test_to_jpn_piece(p, expect):
    actual = p.to_jpn()
    assert expect == actual


@pytest.mark.parametrize('sq, expect', [
    (shogi.Square.SQ_1A, "１一"),
    (shogi.Square.SQ_1B, "１二"),
    (shogi.Square.SQ_1C, "１三"),
    (shogi.Square.SQ_1D, "１四"),
    (shogi.Square.SQ_1E, "１五"),
    (shogi.Square.SQ_2A, "２一"),
    (shogi.Square.SQ_2B, "２二"),
    (shogi.Square.SQ_2C, "２三"),
    (shogi.Square.SQ_2D, "２四"),
    (shogi.Square.SQ_2E, "２五"),
    (shogi.Square.SQ_3A, "３一"),
    (shogi.Square.SQ_3B, "３二"),
    (shogi.Square.SQ_3C, "３三"),
    (shogi.Square.SQ_3D, "３四"),
    (shogi.Square.SQ_3E, "３五"),
    (shogi.Square.SQ_4A, "４一"),
    (shogi.Square.SQ_4B, "４二"),
    (shogi.Square.SQ_4C, "４三"),
    (shogi.Square.SQ_4D, "４四"),
    (shogi.Square.SQ_4E, "４五"),
    (shogi.Square.SQ_5A, "５一"),
    (shogi.Square.SQ_5B, "５二"),
    (shogi.Square.SQ_5C, "５三"),
    (shogi.Square.SQ_5D, "５四"),
    (shogi.Square.SQ_5E, "５五"),
])
def test_to_jpn_square(sq, expect):
    actual = sq.to_jpn()
    assert expect == actual


if __name__ == '__main__':
    pytest.main([__file__])
