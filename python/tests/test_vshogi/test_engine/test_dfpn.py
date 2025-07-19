import pytest

import vshogi.minishogi as shogi
from vshogi.engine import DfpnSearcher


def test_proved_mate():
    g = shogi.Game("4k/5/4P/5/K4 b G")
    searcher = DfpnSearcher()
    searcher.set_game(g)
    searcher.search()
    assert searcher.proved_mate() is True


def test_proved_no_mate():
    g = shogi.Game("2k2/5/1+P3/5/5 b 2S")
    searcher = DfpnSearcher()
    searcher.set_game(g)
    searcher.search(1100)
    assert searcher.proved()
    assert searcher.proved_no_mate()
    assert 700 < searcher.get_search_count() < 800


if __name__ == '__main__':
    pytest.main([__file__])
