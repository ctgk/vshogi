import pytest

from vshogi.shogi import Game, Move
from vshogi import solve_tsumeshogi


@pytest.mark.parametrize(
    ("sfen", "expect"),
    [
        ("8k/8b/6SS1/6K2/9/9/9/9/9 b -", ["3c2b+"]),
        ("7pk/+R8/+R8/9/9/9/9/9/9 b p", ["9c1c"]),
        ("6gk1/7nb/5S3/6R2/9/9/9/9/9 b -", ["3d3a+, 2a3a, G*3b"]),
        ('8k/9/8+R/9/9/9/9/9/9 w Gr2g3s2l2n2p', ["1a2a, G*2b", "P*1b, G*2b"]),
        ("7bk/8p/9/9/9/9/B8/9/9 b N9p2l2n2s2g", ["9g8h, P*2b, N*2c"]),
        (
            "9/2k3p1R/1pp1B3p/9/9/9/9/9/9 b 2G6Pr2g3s2lnp",
            ["1b3b+, 7b6a, 5c6b+", "1b3b+, 7b6c, G*6d", "1b3b+, 7b8a, G*8b"],
        ),
        (  # simple case
            "6+B1k/7+b1/9/8P/9/9/9/9/9 b RN10p4l3n4s4gr",
            ["R*2a, 2b2a, N*2c, 1a1b, 1d1c+"],
        ),
        (
            "9/6l2/6+P2/8n/5RBk1/8+p/6P2/9/9 b S9p2l2n2s2gbr",
            ["S*3d, 2e1e, 3e1g, 1e2d, 3d2c+", "S*3d, 2e1e, 3e1g, P*2e, 4e2e"],
        ),
        (
            "6bkn/9/9/6Pr+B/9/9/9/9/9 b GSN2p2l2n2s2gr",
            [
                'N*3c, 2a1b, G*2b, 1b2b, S*1c',
                'N*3c, 2a1b, G*2b, 3a2b, S*2a',
            ],
        ),
        (  # mate involving ranging attack at 3rd move
            "6n1+P/4s2r1/7k1/5+rpB1/7P1/9/9/9/9 b BGS9p2l2n2s2g",
            [
                "S*1d, 2c1d, G*1e, 1d2c, B*1d",
                "S*1d, 2c3b, B*5d, 4d5d, G*4b",
                "S*1d, 2c3b, B*5d, 3b4a, G*5a",
                "S*1d, 2c3b, B*5d, 3a4c, G*4b",
            ],
        ),
        (  # mate involving ranging attack at 1st move
            "9/9/6n2/6n+Pp/6+b2/8k/7R1/8S/9 b RG9p2l2n2s2gb",
            ["R*3f, 3e3f, 1h1g, 1f2g, G*2h", "R*3f, 3e3f, 1h1g, 1f1e, G*1f"],
        ),
        (
            "1ns2+N3/1r2k4/2ppp4/9/9/9/9/1B7/9 b B2Gr2g3s2n4l15p",
            [
                "G*4b, 5b6a, 4a5a, 6a6b, 4b5b, 6b7b, B*6a",
                "G*4b, 5b6a, 4a5a, 6a7b, B*6a, 7b6b, 4b5b",
            ],
        ),
    ],
)
def test_solve_tsumeshogi(sfen, expect):
    g = Game(sfen)
    print(g)
    actual = solve_tsumeshogi(g, max_ply=len(expect[0].split(", ")))
    assert set(expect) == set(
        [", ".join(m.to_sfen() for m in a) for a in actual]
    )

    expect = [
        ", ".join(Move(m).rotate().to_sfen() for m in mate.split(", "))
        for mate in expect
    ]
    g = g.rotate()
    actual = solve_tsumeshogi(g, max_ply=len(expect[0].split(", ")))
    assert set(expect) == set(
        [", ".join(m.to_sfen() for m in a) for a in actual]
    )


if __name__ == "__main__":
    pytest.main([__file__])
