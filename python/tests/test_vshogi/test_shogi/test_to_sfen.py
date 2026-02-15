import pytest

import vshogi.shogi as shogi


@pytest.mark.parametrize(
    'p, expect',
    [
        ("FU", "p"),
        ("KY", "l"),
        ("KE", "n"),
        ("GI", "s"),
        ("KA", "b"),
        ("HI", "r"),
        ("KI", "g"),
        ("OU", "k"),
        ("TO", "+p"),
        ("NY", "+l"),
        ("NK", "+n"),
        ("NG", "+s"),
        ("UM", "+b"),
        ("RY", "+r"),
        ("NA", "?"),
        ("B_FU", "P"),
        ("B_KY", "L"),
        ("B_KE", "N"),
        ("B_GI", "S"),
        ("B_KA", "B"),
        ("B_HI", "R"),
        ("B_KI", "G"),
        ("B_OU", "K"),
        ("B_TO", "+P"),
        ("B_NY", "+L"),
        ("B_NK", "+N"),
        ("B_NG", "+S"),
        ("B_UM", "+B"),
        ("B_RY", "+R"),
        ("W_FU", "p"),
        ("W_KY", "l"),
        ("W_KE", "n"),
        ("W_GI", "s"),
        ("W_KA", "b"),
        ("W_HI", "r"),
        ("W_KI", "g"),
        ("W_OU", "k"),
        ("W_TO", "+p"),
        ("W_NY", "+l"),
        ("W_NK", "+n"),
        ("W_NG", "+s"),
        ("W_UM", "+b"),
        ("W_RY", "+r"),
        ("VOID", "?"),
    ],
)
def test_to_sfen(p, expect):
    p = getattr(shogi.Piece if len(p) == 2 else shogi.BoardPiece, p)
    actual = p.to_sfen()
    assert actual == expect


if __name__ == '__main__':
    pytest.main([__file__])
