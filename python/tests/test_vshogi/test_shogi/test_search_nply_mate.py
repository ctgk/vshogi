import pytest

import vshogi.shogi as shogi
from vshogi.engine import search_nply_mate


@pytest.mark.parametrize('sfen, expect', [
    (
        '7bk/8p/9/9/9/9/B8/9/9 b N10p',
        [(shogi.Move('9g8h'), shogi.Move('P*2b'), shogi.Move('N*2c'))],
    ),
])
def test_search_3ply_mate(sfen, expect):
    g = shogi.Game(sfen)
    actual = search_nply_mate(g, num_ply=3)
    assert set(expect) == set(actual)


if __name__ == '__main__':
    pytest.main([__file__])
