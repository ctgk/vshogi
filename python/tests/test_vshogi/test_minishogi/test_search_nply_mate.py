import pytest

import vshogi.minishogi as shogi
from vshogi.engine import search_nply_mate


@pytest.mark.parametrize('sfen, expect', [
    ('2k2/5/2P2/5/5 b G', [(shogi.Move('G*3b'),)]),
    ('3pk/5/3S1/5/R4 b psgbr', [(shogi.Move('5e1e'),)]),
    ('3gk/3p1/3P1/1+B3/+B4 b psgr', []),
])
def test_search_1ply_mate(sfen, expect):
    g = shogi.Game(sfen)
    actual = search_nply_mate(g, num_ply=1)
    assert set(expect) == set(actual)


@pytest.mark.parametrize('sfen, expect', [
    (
        '2pp1/3k1/5/3P1/R4 b Gsgbr',
        [(shogi.Move('G*2c'), shogi.Move('2b1a'), shogi.Move('5e1e'))],
    ),
    (
        '3sk/5/B3S/5/5 b Gpgbr',
        [(shogi.Move('5c4d'), shogi.Move('2a2b'), shogi.Move('G*1b'))],
    ),
    ('3sk/2p2/B3P/5/5 b Gsgbr', []),
])
def test_search_3ply_mate(sfen, expect):
    g = shogi.Game(sfen)
    actual = search_nply_mate(g, num_ply=3)
    assert set(expect) == set(actual)


if __name__ == '__main__':
    pytest.main([__file__])
