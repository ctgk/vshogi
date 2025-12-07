import numpy as np
import pytest

import vshogi.minishogi as shogi
from vshogi.engine import AlphaZero


def uniform_pv_func(game):
    return np.zeros(game.num_dlshogi_policy), 0.


def test_is_ready():
    game = shogi.Game()
    searcher = AlphaZero(uniform_pv_func)
    assert searcher.is_ready() is False
    searcher.set_game(game)
    assert searcher.is_ready()


def test_num_searched():
    game = shogi.Game()
    searcher = AlphaZero(uniform_pv_func)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    assert searcher.num_searched == 100


def test_clear():
    game = shogi.Game()
    searcher = AlphaZero(uniform_pv_func)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    searcher.clear()
    assert searcher.is_ready() is False
    assert searcher.num_searched == 0


def test_q_values_mate_in_one():
    game = shogi.Game("b2pk/3b1/4P/2gRR/4K b -")
    m = shogi.Move(shogi.SQ_1C, shogi.SQ_1B)

    searcher = AlphaZero(uniform_pv_func, random_rate=0.)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    actual = searcher.get_q_values()
    print(searcher._tree())
    print(actual)
    assert np.isclose(actual[m], 1, rtol=0, atol=1e-2)


def test_q_values_initial():
    game = shogi.Game()
    searcher = AlphaZero(uniform_pv_func)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    actual = searcher.get_q_values()
    print(actual)
    for a in actual.values():
        assert np.isclose(a, 0, rtol=0, atol=0.1)


def test_mate_in_three():
    game = shogi.Game('1r3/2k1G/5/2PG1/5 b -')
    m = shogi.Move(shogi.SQ_2D, shogi.SQ_3C)

    searcher = AlphaZero(uniform_pv_func, random_rate=0)
    searcher.set_game(game)
    searcher.search(n_or_t=100)

    actual = searcher.get_q_values()
    print(actual)
    print(searcher._tree(depth=3))
    assert np.isclose(actual[m], 1, rtol=0, atol=1e-2)

    visit_count = searcher.get_visit_counts()[m]
    searcher.search(n_or_t=100)
    # If there is a mate, all explorations go through the mate.
    assert searcher.get_visit_counts()[m] == visit_count + 100


def test_visit_count_by_random():
    game = shogi.Game()
    m = shogi.Move(shogi.SQ_1E, shogi.SQ_1B)

    searcher = AlphaZero(
        lambda g: (np.arange(g.num_dlshogi_policy)[::-1], 0.), random_rate=0)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    visit_count = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2, breadth=-1))

    searcher = AlphaZero(
        lambda g: (np.arange(g.num_dlshogi_policy)[::-1], 0.),
        random_rate=0.25)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    visit_count_with_noise = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2, breadth=-1))
    assert visit_count > visit_count_with_noise + 10


def test_greedy_q_value():
    game = shogi.Game()
    searcher = AlphaZero(uniform_pv_func)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    action = searcher.select()

    assert np.isclose(
        searcher.get_q_value(greedy_depth=1),
        searcher.get_q_values()[action],
    )
    assert np.isclose(
        searcher.get_q_value(greedy_depth=2),
        searcher.get_q_values(greedy_depth=1)[action],
    )

    searcher._tree(depth=2, breadth=3)
    searcher._tree(depth=2, breadth=5)
    searcher._tree(depth=2, breadth=-1, pv_line=["3e4d"])


def test_dfpn_root():
    mcts = AlphaZero(
        lambda g: (g.to_dlshogi_policy({}), 0.),
        random_rate=0,
        dfpn_search_root=10000,
    )

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
    mcts.set_game(game)
    mcts.search(n_or_t=1)
    assert shogi.Move("B*2b") == mcts.select()
    assert len(mcts.get_visit_counts()) > 10


def test_mating_net():
    # Turn: WHITE
    # White: KI
    #     5   4   3   2   1
    #   +---+---+---+---+---+
    # A |   |   |   |   |-OU|
    #   +---+---+---+---+---+
    # B |   |   |   |-KI|   |
    #   +---+---+---+---+---+
    # C |   |   |   |   |   |
    #   +---+---+---+---+---+
    # D |   |   |   |   |   |
    #   +---+---+---+---+---+
    # E |   |   |+OU|   |   |
    #   +---+---+---+---+---+
    # Black: -
    g = shogi.Game("4k/3g1/5/5/2K2 w g")
    mcts = AlphaZero(dfpn_search_root=0, dfpn_search_leaf=100)
    mcts.set_game(g)
    mcts.search(n_or_t=10000)
    assert mcts.proved_mate()
    assert shogi.Move("2b3c") == mcts.select()
    assert set(mcts.get_visit_counts().keys()) == set(g.get_legal_moves())


def test_dfpn_vertex():
    mcts = AlphaZero(
        lambda g: (g.to_dlshogi_policy({}), 0.),
        random_rate=0,
        dfpn_search_root=0,
        dfpn_search_leaf=100,
    )

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
    mcts.set_game(g)
    mcts.search(n_or_t=3)
    print(mcts._tree(depth=5, breadth=5))
    assert shogi.Move("3e4e") == mcts.select()

    mcts.apply(shogi.Move("3e2e"))
    assert shogi.Move("3c2d") == mcts.select()


def test_dfpn_vertex_2():
    mcts = AlphaZero(
        lambda g: (g.to_dlshogi_policy({}), 0.),
        random_rate=0,
        dfpn_search_root=0,
        dfpn_search_leaf=100,
    )
    # Turn: BLACK
    # White: -
    #     5   4   3   2   1
    #   +---+---+---+---+---+
    # A |   |   |-OU|   |   |
    #   +---+---+---+---+---+
    # B |   |   |-HI|-KI|   |
    #   +---+---+---+---+---+
    # C |   |   |-GI|-FU|   |
    #   +---+---+---+---+---+
    # D |   |   |   |   |   |
    #   +---+---+---+---+---+
    # E |   |   |+OU|   |   |
    #   +---+---+---+---+---+
    # Black: -
    g = shogi.Game("2k2/2rg1/2sp1/5/2K2 b -")
    mcts.set_game(g)
    mcts.search(n_or_t=3)
    print(mcts._tree(depth=2))
    assert shogi.Move("3e4e") == mcts.select()
    mcts.apply(shogi.Move("3e2e"))
    g.apply(shogi.Move("3e2e"))

    mcts.search(n_or_t=1)
    print(mcts._tree(depth=2))
    assert shogi.Move("3c2d") == mcts.select()
    mcts.apply(shogi.Move("3c2d"))
    g.apply(shogi.Move("3c2d"))

    mcts.search(n_or_t=1)
    print(mcts._tree(depth=2))
    assert mcts.select() in g.get_legal_moves(), (
        mcts.select(), g.get_legal_moves())


def test_dfpn_root_vertex():
    mcts = AlphaZero(
        lambda g: (g.to_dlshogi_policy({}), 0.),
        random_rate=0,
        dfpn_search_root=10000,
        dfpn_search_leaf=100,
    )
    # Turn: BLACK
    # White: -
    #     5   4   3   2   1
    #   +---+---+---+---+---+
    # A |-HI|-KA|-GI|   |   |
    #   +---+---+---+---+---+
    # B |   |   |   |-OU|   |
    #   +---+---+---+---+---+
    # C |   |+HI|-KI|   |   |
    #   +---+---+---+---+---+
    # D |+FU|+KI|+GI|   |   |
    #   +---+---+---+---+---+
    # E |+OU|   |   |+KA|   |
    #   +---+---+---+---+---+
    # Black: FU
    g = shogi.Game("rbs2/3k1/1Rg2/PGS2/K2B1 b P 9")
    mcts.set_game(g)
    mcts.search(100)
    assert mcts.proved_mate()
    assert np.isclose(mcts.get_q_value(), 1., rtol=0, atol=1e-3)

    assert shogi.Move("4d3c") == mcts.select()
    g.apply(mcts.select())
    mcts.apply(mcts.select())

    print(mcts._tree())
    assert np.isclose(mcts.get_q_value(), -1., rtol=0, atol=1e-3)
    mcts.search(100)
    assert mcts.proved_mate()
    assert np.isclose(mcts.get_q_value(), -1., rtol=0, atol=1e-3)


def test_debug():
    mcts = AlphaZero(
        lambda g: (g.to_dlshogi_policy({}), 0.),
        random_rate=0,
        dfpn_search_root=10000,
        dfpn_search_leaf=100,
    )
    # Turn: WHITE
    # White: GI,KI
    #     5   4   3   2   1
    #   +---+---+---+---+---+
    # A |-HI|   |   |   |   |
    #   +---+---+---+---+---+
    # B |   |   |-OU|   |   |
    #   +---+---+---+---+---+
    # C |   |   |-UM|   |+FU|
    #   +---+---+---+---+---+
    # D |+FU|+KI|+KA|   |   |
    #   +---+---+---+---+---+
    # E |+OU|   |+GI|   |   |
    #   +---+---+---+---+---+
    # Black: HI
    g = shogi.Game("r4/2k2/2+b1P/PGB2/K1S2 w Rgs 22")
    mcts.set_game(g)
    mcts.search(3)
    print(mcts._tree(depth=2))
    mcts.search(1)
    print(mcts._tree(depth=2))
    assert mcts.proved_mate()
    assert np.isclose(mcts.get_q_value(), 1.)


if __name__ == '__main__':
    pytest.main([__file__])
