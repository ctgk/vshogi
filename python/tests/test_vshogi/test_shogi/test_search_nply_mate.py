import pytest

import vshogi.shogi as shogi
from vshogi.engine import search_nply_mate


@pytest.mark.parametrize('sfen, expect', [
    (
        '8k/8b/6SS1/6K2/9/9/9/9/9 b -',
        [(shogi.Move('3c2b+'),)],
    ),
])
def test_search_1ply_mate(sfen, expect):
    g = shogi.Game(sfen)
    actual = search_nply_mate(g, num_ply=1)
    assert set(expect) == set(actual)


@pytest.mark.parametrize('sfen, expect', [
    (
        '7bk/8p/9/9/9/9/B8/9/9 b N10p',
        [(shogi.Move('9g8h'), shogi.Move('P*2b'), shogi.Move('N*2c'))],
    ),
    (
        '6gk1/7nb/5S3/6R2/9/9/9/9/9 b -',
        [(shogi.Move('3d3a+'), shogi.Move('2a3a'), shogi.Move('G*3b'))],
    ),
    (
        '9/2k3p1R/1pp1B3p/9/9/9/9/9/9 b 2G6Pr2g3s2lnp',
        [
            (shogi.Move('1b3b+'), shogi.Move('7b6a'), shogi.Move('G*6b')),
            (shogi.Move('1b3b+'), shogi.Move('7b6a'), shogi.Move('5c6b+')),
            (shogi.Move('1b3b+'), shogi.Move('7b6a'), shogi.Move('3b6b')),
            (shogi.Move('1b3b+'), shogi.Move('7b6c'), shogi.Move('G*6d')),
            (shogi.Move('1b3b+'), shogi.Move('7b8a'), shogi.Move('G*8b')),
            (shogi.Move('1b3b+'), shogi.Move('7b8a'), shogi.Move('G*9b')),
        ],
    ),
    (
        '4R1+B2/9/6kg1/6pp1/5P3/9/9/9/9 b rgs',
        [],
    ),
])
def test_search_3ply_mate(sfen, expect):
    g = shogi.Game(sfen)
    actual = search_nply_mate(g, num_ply=3)
    assert set(expect) == set(actual)


if __name__ == '__main__':
    pytest.main([__file__])
