import pytest

import vshogi.minishogi as shogi


@pytest.mark.parametrize(
    'p, expect',
    [
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
    ],
)
def test_to_jpn_piece(p, expect):
    actual = p.to_jpn()
    assert expect == actual


@pytest.mark.parametrize(
    'sq, expect',
    [
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
    ],
)
def test_to_jpn_square(sq, expect):
    actual = sq.to_jpn()
    assert expect == actual


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_1a():
    g = shogi.Game('5/2G2/G4/5/5 b -')
    assert "４二金上" == g.to_jpn("5c4b")
    assert "４二金寄" == g.to_jpn("3b4b")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_1b():
    g = shogi.Game("2G2/5/1G3/5/5 b -")
    assert "３二金上" == g.to_jpn("4c3b")
    assert "３二金引" == g.to_jpn("3a3b")

    g = shogi.Game('5/5/3g1/5/2g2 w -')
    assert "３四金上" == g.to_jpn('2c3d')
    assert "３四金引" == g.to_jpn('3e3d')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_1c():
    g = shogi.Game("1G3/G4/5/5/5 b -")
    assert "５一金上" == g.to_jpn("5b5a")
    assert "５一金寄" == g.to_jpn("4a5a")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_1d():
    g = shogi.Game("5/5/2S2/5/1S3 b -")
    assert "４四銀上" == g.to_jpn("4e4d")
    assert "４四銀引" == g.to_jpn("3c4d")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_1e():
    g = shogi.Game("5/5/3S1/5/1S3 b -")
    assert "３四銀上" == g.to_jpn("4e3d")
    assert "３四銀引" == g.to_jpn("2c3d")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_2a():
    g = shogi.Game("5/G1G2/5/5/5 b -")
    assert "４一金左" == g.to_jpn("5b4a")
    assert "４一金右" == g.to_jpn("3b4a")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_2b():
    g = shogi.Game("5/2G1G/5/5/5 b -")
    assert "２二金左" == g.to_jpn("3b2b")
    assert "２二金右" == g.to_jpn("1b2b")

    g = shogi.Game("5/2g1g/5/5/5 w -")
    assert "２二金左" == g.to_jpn('1b2b')
    assert "２二金右" == g.to_jpn('3b2b')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_2c():
    g = shogi.Game("5/5/1S1S1/5/5 b -")
    assert "３四銀左" == g.to_jpn("4c3d")
    assert "３四銀右" == g.to_jpn("2c3d")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_2d():
    g = shogi.Game("5/5/5/5/1GG2 b -")
    assert "３四金左" == g.to_jpn("4e3d")
    assert "３四金直" == g.to_jpn("3e3d")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_2e():
    g = shogi.Game("5/5/5/5/2SS1 b -")
    assert "３四銀直" == g.to_jpn("3e3d")
    assert "３四銀右" == g.to_jpn("2e3d")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_3a():
    g = shogi.Game("5/5/1GGG1/5/5 b -")
    assert "３二金左" == g.to_jpn('4c3b')
    assert "３二金直" == g.to_jpn('3c3b')
    assert "３二金右" == g.to_jpn('2c3b')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_3b():
    g = shogi.Game("5/5/1+P3/+P4/+P+P+P2 b -")
    assert "４四と右" == g.to_jpn('3e4d')
    assert "４四と直" == g.to_jpn('4e4d')
    assert "４四と左上" == g.to_jpn('5e4d')
    assert "４四と寄" == g.to_jpn('5d4d')
    assert "４四と引" == g.to_jpn('4c4d')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_3c():
    g = shogi.Game('5/5/2S1S/5/2SS1 b -')
    assert "２四銀直" == g.to_jpn('2e2d')
    assert "２四銀右" == g.to_jpn('1c2d')
    assert "２四銀左上" == g.to_jpn('3e2d')
    assert "２四銀左引" == g.to_jpn('3c2d')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_4a():
    g = shogi.Game('+R4/5/5/1+R3/5 b -')
    assert "４二龍引" == g.to_jpn("5a4b")
    assert "４二龍上" == g.to_jpn("4d4b")


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_4b():
    g = shogi.Game('5/+R4/3+R1/5/5 b -')
    assert "４三龍寄" == g.to_jpn('2c4c')
    assert "４三龍引" == g.to_jpn('5b4c')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_4c():
    g = shogi.Game('5/5/+R3+R/5/5 b -')
    assert "３三龍左" == g.to_jpn('5c3c')
    assert "３三龍右" == g.to_jpn('1c3c')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_4d():
    g = shogi.Game("5/5/5/5/+R+R3 b -")
    assert "４四龍左" == g.to_jpn('5e4d')
    assert "４四龍右" == g.to_jpn('4e4d')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_4e():
    g = shogi.Game('5/5/5/3+R1/4+R b -')
    assert "１三龍左" == g.to_jpn('2d1c')
    assert "１三龍右" == g.to_jpn('1e1c')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_5a():
    g = shogi.Game('+B+B3/5/5/5/5 b -')
    assert "４二馬左" == g.to_jpn('5a4b')
    assert "４二馬右" == g.to_jpn('4a4b')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_5b():
    g = shogi.Game('5/3+B1/5/+B4/5 b -')
    assert "４四馬寄" == g.to_jpn('5d4d')
    assert "４四馬引" == g.to_jpn('2b4d')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_5c():
    g = shogi.Game('4+B/5/5/2+B2/5 b -')
    assert "１二馬引" == g.to_jpn('1a1b')
    assert "１二馬上" == g.to_jpn('3d1b')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_5d():
    g = shogi.Game('5/5/5/5/+B3+B b -')
    assert "３三馬左" == g.to_jpn('5e3c')
    assert "３三馬右" == g.to_jpn('1e3c')


# https://www.shogi.or.jp/faq/kihuhyouki.html
def test_to_jpn_move_5e():
    g = shogi.Game('5/5/1+B3/4+B/5 b -')
    assert "２五馬左" == g.to_jpn('4c2e')
    assert "２五馬右" == g.to_jpn('1d2e')


if __name__ == '__main__':
    pytest.main([__file__])
