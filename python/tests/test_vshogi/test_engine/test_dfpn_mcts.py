import pytest

import vshogi.minishogi as shogi
from vshogi.engine import DfpnMcts, DfpnSearcher, Mcts


def test_dfpn_root():
    dfpn = DfpnSearcher()
    mcts = Mcts(
        lambda g: (g.to_dlshogi_policy({}), 0.), random_depth=0)
    engine = DfpnMcts(dfpn, mcts)

    # Turn: BLACK
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |   |-HI|-KA|-OU|
    #   *---*---*---*---*---*
    # B |   |   |-FU|   |-FU|
    #   *---*---*---*---*---*
    # C |   |   |+FU|   |+FU|
    #   *---*---*---*---*---*
    # D |   |   |   |+KI|   |
    #   *---*---*---*---*---*
    # E |   |   |   |+HI|   |
    #   *---*---*---*---*---*
    # Black: KA
    game = shogi.Game("2rbk/2p1p/2P1P/3G1/3R1 b B")
    engine.set_game(game)
    engine.search(
        dfpn_searches_at_root=10000,
        mcts_searches=0,
        dfpn_searches_at_vertex=0,
    )
    assert shogi.Move("B*2b") == engine.select()


def test_dfpn_vertex():
    dfpn = DfpnSearcher()
    mcts = Mcts(
        lambda g: (g.to_dlshogi_policy({}), 0.), random_depth=0)
    engine = DfpnMcts(dfpn, mcts)

    # Turn: BLACK
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |   |-OU|   |   |
    #   *---*---*---*---*---*
    # B |   |   |   |   |   |
    #   *---*---*---*---*---*
    # C |   |   |-KI|-FU|   |
    #   *---*---*---*---*---*
    # D |   |   |   |   |   |
    #   *---*---*---*---*---*
    # E |   |   |+OU|   |   |
    #   *---*---*---*---*---*
    # Black: -
    g = shogi.Game("2k2/5/2gp1/5/2K2 b -")
    engine.set_game(g)
    engine.search(
        dfpn_searches_at_root=0,
        mcts_searches=2,
        dfpn_searches_at_vertex=100,
    )
    print(engine._tree(depth=5, breadth=5))
    assert shogi.Move("3e4e") == engine.select()


if __name__ == '__main__':
    pytest.main([__file__])
