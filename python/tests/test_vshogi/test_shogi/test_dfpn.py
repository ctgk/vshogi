import pytest

import vshogi.shogi as shogi
from vshogi.engine import DfpnSearcher


def test_mate():
    sfen_searches_pair_list = [
        ('9/9/9/6l1+P/5n3/7ks/6R2/5B1Np/7P1 b R10p4l2n3s4gb', 124),
        ('8l/5+R3/4B1sNk/6G1p/9/9/9/9/9 b G10p3l3n3s2gbr', 1025),
        ('9/5+R3/6n2/6k+b1/8+B/5P3/9/9/9 b 2S10p4l3n2s4gr', 913),
        ('5p+r1S/9/8k/5+B2p/7PN/9/9/9/9 b B10p4l3n3s4gr', 4162),
        ('7bk/8p/9/9/9/9/B8/9/9 b N10p4l3n4s4gb2r', 6700),
        ('3R2l2/4r2+B1/5+Bnn1/8k/6G2/8P/7P1/9/9 b 10p3l2n4s4g', 5964),
        ('5+Pnk1/8P/4p1+rsS/9/9/9/9/9/9 b BGS10p4l3ns3gbr', 22468),
        ('4k4/5s+r2/3R1Ls2/3+B5/9/9/9/9/9 b B10p3l4n2s4g', 21829),
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
