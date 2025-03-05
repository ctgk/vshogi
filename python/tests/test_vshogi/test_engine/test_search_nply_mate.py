import pytest

import vshogi.minishogi as shogi
from vshogi.engine import search_nply_mate


@pytest.mark.parametrize('sfen, expect', [
    ('2k2/5/2P2/5/5 b G', [(shogi.Move('G*3b'),)]),
    ('3pk/5/3S1/5/R4 b psgbr', [(shogi.Move('5e1e'),)]),
])
def test_search_1ply_mate(sfen, expect):
    actual = search_nply_mate(shogi.Game(sfen), num_ply=1)
    assert set(expect) == set(actual)
