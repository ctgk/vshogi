import pytest

import vshogi.minishogi as shogi
from vshogi.engine import DfpnSearcher


def test_found_mate():
    g = shogi.Game("4k/5/4P/5/K4 b G")
    searcher = DfpnSearcher()
    searcher.set_game(g)
    searcher.search()
    assert searcher.found_mate() is True


def test_found_no_mate():
    g = shogi.Game("2k2/5/1+P3/5/5 b 2S")
    searcher = DfpnSearcher()
    searcher.set_game(g)
    searcher.search(600)
    assert searcher.found_conclusion() is False

    searcher = DfpnSearcher(thnc_def=5)
    searcher.set_game(g)
    searcher.search(500)
    assert searcher.found_conclusion()
    assert searcher.found_no_mate()


if __name__ == '__main__':
    pytest.main([__file__])
