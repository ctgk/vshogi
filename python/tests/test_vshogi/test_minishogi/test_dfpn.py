import pytest

import vshogi.minishogi as shogi
from vshogi.engine import DfpnSearcher


def test_mate():
    sfen_searches_pair_list = [
        ('4+P/1gk2/1p3/3+R1/5 b G2s2br', 9),
        ('2+B1k/3r1/3Sp/G1K2/3r1 w Gbsp', 1552),
    ]

    actual_total_searches = 0
    expect_total_searches = 0
    searcher = DfpnSearcher()
    for sfen, searches in sfen_searches_pair_list:
        game = shogi.Game(sfen)
        searcher.set_game(game)
        searcher.search(n=100000)
        if not searcher.found_mate():
            print(game)
            print(searcher._tree(depth=5, breadth=5))
            assert False
        if searcher.get_search_count() != searches:
            print(
                sfen,
                f'actual={searcher.get_search_count()}',
                f'expect={searches}',
            )
        for m in searcher.get_mate_moves():
            assert game.result == shogi.ONGOING
            game.apply(m)
        assert game.result == (
            shogi.BLACK_WIN if sfen.split()[1] == 'b' else shogi.WHITE_WIN)
        actual_total_searches += searcher.get_search_count()
        expect_total_searches += searches
    print(
        f'#searches_total: actual={actual_total_searches}, '
        f'expected={expect_total_searches}',
    )
    assert actual_total_searches == expect_total_searches


if __name__ == '__main__':
    pytest.main([__file__])
