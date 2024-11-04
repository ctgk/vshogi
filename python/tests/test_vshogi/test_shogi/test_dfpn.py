import pytest

import vshogi.shogi as shogi
from vshogi.engine import DfpnSearcher


@pytest.mark.parametrize('sfen', [
    '7bk/8p/9/9/9/9/B8/9/9 b N10p4l3n4s4gb2r',
    '3R2l2/4r2+B1/5+Bnn1/8k/6G2/8P/7P1/9/9 b 10p3l2n4s4g',
])
def test_mate_in_three(sfen: str):
    game = shogi.Game(sfen)
    print(game)
    searcher = DfpnSearcher()
    searcher.set_game(game)
    searcher.search(n=50000)
    print(searcher._tree(depth=5, breadth=3))
    assert searcher.found_conclusion()
    assert searcher.found_mate()
    print([m.to_usi() for m in searcher.get_mate_moves()])
    # assert False


if __name__ == '__main__':
    pytest.main([__file__])
