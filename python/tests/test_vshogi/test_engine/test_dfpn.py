import pytest

import vshogi.minishogi as shogi
from vshogi.engine import DfpnSearcher


def test_proved_mate():
    g = shogi.Game("4k/5/4P/5/K4 b G")
    searcher = DfpnSearcher()
    searcher.set_game(g)
    searcher.search(100)
    print(searcher._tree())
    print(searcher._tree(pv_line=["G*1b"]))
    assert searcher.proved_mate() is True


def test_proved_mate_2():
    g = shogi.Game('2+B1k/3r1/3Sp/G1K2/3r1 w Gbsp')
    searcher = DfpnSearcher()
    searcher.set_game(g)
    searcher.search(10000)
    print(searcher._tree(depth=3, breadth=3))
    assert searcher.proved_mate() is True


def test_proved_no_mate():
    g = shogi.Game("2k2/5/1+P3/5/5 b 2S")
    searcher = DfpnSearcher()
    searcher.set_game(g)
    searcher.search(1100)
    assert searcher.proved()
    assert searcher.proved_no_mate()
    assert 700 < searcher.get_search_count() < 800


def test_error():
    with pytest.raises(ValueError):
        DfpnSearcher(tree_size=0)


if __name__ == '__main__':
    pytest.main([__file__])
