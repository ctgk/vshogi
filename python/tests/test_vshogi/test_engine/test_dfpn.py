import pytest

import vshogi.minishogi as shogi
from vshogi.engine import DfpnSearcher


def test_found_mate():
    g = shogi.Game("4k/5/4P/5/K4 b G")
    searcher = DfpnSearcher()
    searcher.set_game(g)
    searcher.search()
    assert searcher.found_mate() is True


if __name__ == '__main__':
    pytest.main([__file__])
