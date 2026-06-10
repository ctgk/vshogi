from vshogi.minishogi import Game, Move
from vshogi.engine import Mcgs

import numpy as np
import pytest


def test_is_ready():
    mcgs = Mcgs()
    assert not mcgs.is_ready()
    mcgs.set_game(Game())
    assert mcgs.is_ready()


def test_get_search_count():
    game = Game()
    mcgs = Mcgs(epsilon_greedy=0.0)
    mcgs.set_game(game)
    mcgs.search(budget=100)
    assert mcgs.get_search_count() == 100

    mcgs = Mcgs(epsilon_greedy=0.2)
    mcgs.set_game(game)
    mcgs.search(budget=100)
    assert 50 < mcgs.get_search_count() < 100


def test_clear():
    game = Game()
    mcgs = Mcgs()
    mcgs.set_game(game)
    mcgs.search(budget=100)
    assert mcgs.get_search_count() > 0
    mcgs.clear()
    assert mcgs.is_ready() is False
    assert mcgs.get_search_count() == 0


def test_get_value_mate_in_one():
    game = Game("b2pk/3b1/4P/2gRR/4K b -")
    mcgs = Mcgs()
    mcgs.set_game(game)
    mcgs.search(budget=100)
    actual = mcgs.get_value()
    print(mcgs._tree())
    print(actual)
    assert np.isclose(actual, 1)


def test_get_value_initial_position():
    game = Game()
    mcgs = Mcgs()
    mcgs.set_game(game)
    mcgs.search(budget=100)
    actual = mcgs.get_value()
    assert np.isclose(actual, 0, rtol=0, atol=0.1)


def test_select_before_search():
    g = Game()
    mcgs = Mcgs()
    mcgs.set_game(g)
    with pytest.raises(ValueError):
        mcgs.select()
    with pytest.raises(ValueError):
        mcgs.select(temperature=1.0)


def test_select_after_one_search():
    g = Game()
    mcgs = Mcgs()
    mcgs.set_game(g)
    mcgs.search(budget=1)
    mcgs.select()
    mcgs.select(temperature=1.0)


def test_mate_in_three():
    game = Game("1r3/2k1G/5/2PG1/5 b -")
    m = Move("2d3c")
    mcgs = Mcgs(
        lambda g: (g.to_dlshogi_policy({}), -0.9 * g.in_check()),
        epsilon_greedy=0.0,
    )
    mcgs.set_game(game)
    mcgs.search(budget=100)
    actual = mcgs.get_q_values()
    assert np.isclose(actual[m], 1, rtol=0, atol=1e-3)
    print(mcgs._tree(depth=3))

    visits = mcgs.get_visit_counts()[m]
    mcgs.search(budget=100)
    assert mcgs.get_visit_counts()[m] == visits + 100


def test_visit_count_by_random():
    game = Game()
    m = Move("1e1b")
    mcgs = Mcgs(
        lambda g: (np.arange(g.num_dlshogi_policy)[::-1], 0.0),
        epsilon_greedy=0.0,
    )
    mcgs.set_game(game)
    mcgs.search(budget=100)
    visits = mcgs.get_visit_counts()[m]

    mcgs = Mcgs(
        lambda g: (np.arange(g.num_dlshogi_policy)[::-1], 0.0),
        epsilon_greedy=0.25,
    )
    mcgs.set_game(game)
    mcgs.search(100)
    visits_with_noise = mcgs.get_visit_counts()[m]
    assert visits > visits_with_noise + 10


def test_dfpn_root():
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
    game = Game("2rbk/2p1p/2P1P/3G1/3R1 b B")
    mcgs = Mcgs(dfpn_search_root=10000)
    mcgs.set_game(game)
    mcgs.search(budget=1)
    assert Move("B*2b") == mcgs.select()
    assert mcgs.proved_mate()
    assert len(mcgs.get_visit_counts()) > 10


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
    g = Game("4k/3g1/5/5/2K2 w g")
    mcgs = Mcgs(dfpn_search_root=0, dfpn_search_leaf=100)
    mcgs.set_game(g)
    mcgs.search(budget=10000)
    assert mcgs.proved_mate()
    assert Move("2b3c") == mcgs.select()
    assert set(mcgs.get_visit_counts().keys()) == set(g.get_legal_moves())
    mate_moves = mcgs.get_mate_moves()
    assert isinstance(mate_moves, list)
    assert len(mate_moves) == 3
    assert Move("2b3c") == mate_moves[0]


def test_dfpn_vertex():
    mcgs = Mcgs(
        epsilon_greedy=0.0,
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
    g = Game("2k2/5/2gp1/5/2K2 b -")
    mcgs.set_game(g)
    mcgs.search(budget=3)
    print(mcgs._tree(depth=5, breadth=5))
    assert Move("3e4e") == mcgs.select()

    mcgs.apply(Move("3e2e"))
    assert Move("3c2d") == mcgs.select()


def test_dfpn_vertex_2():
    mcgs = Mcgs(
        epsilon_greedy=0.0,
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
    g = Game("2k2/2rg1/2sp1/5/2K2 b -")
    mcgs.set_game(g)
    mcgs.search(budget=3)
    print(mcgs._tree(depth=2))
    assert Move("3e4e") == mcgs.select()
    mcgs.apply(Move("3e2e"))
    g.apply(Move("3e2e"))

    mcgs.search(budget=1)
    print(mcgs._tree(depth=2))
    assert Move("3c2d") == mcgs.select()
    mcgs.apply(Move("3c2d"))
    g.apply(Move("3c2d"))

    mcgs.search(budget=1)
    print(mcgs._tree(depth=2))
    assert mcgs.select() in g.get_legal_moves(), (
        mcgs.select(),
        g.get_legal_moves(),
    )


def test_dfpn_root_vertex():
    mcgs = Mcgs(
        epsilon_greedy=0.0,
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
    g = Game("rbs2/3k1/1Rg2/PGS2/K2B1 b P 9")
    mcgs.set_game(g)
    mcgs.search(100)
    assert mcgs.proved_mate()
    assert np.isclose(mcgs.get_value(), 1.0, rtol=0, atol=1e-3)

    assert Move("4d3c") == mcgs.select()
    g.apply(mcgs.select())
    mcgs.apply(mcgs.select())

    print(mcgs._tree())
    assert np.isclose(mcgs.get_value(), -1.0, rtol=0, atol=1e-3)
    mcgs.search(100)
    assert mcgs.proved_mate()
    assert np.isclose(mcgs.get_value(), -1.0, rtol=0, atol=1e-3)


def test_debug():
    mcgs = Mcgs(
        epsilon_greedy=0.0,
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
    g = Game("r4/2k2/2+b1P/PGB2/K1S2 w Rgs 22")
    mcgs.set_game(g)
    mcgs.search(3)
    print(mcgs._tree(depth=2))
    mcgs.search(1)
    print(mcgs._tree(depth=2))
    assert mcgs.proved_mate()
    assert np.isclose(mcgs.get_value(), 1.0)


def test_get_improved_policy():
    g = Game("4k/5/5/4g/4K b -")
    mcgs = Mcgs(epsilon_greedy=0.0)
    mcgs.set_game(g)

    mcgs.search(budget=1)
    with pytest.raises(ValueError):
        mcgs.get_improved_policy()

    mcgs.search(budget=1)
    actual = mcgs.get_improved_policy()
    assert len(actual) == 1
    assert np.isclose(actual[Move("1e1d")], 1.0)


if __name__ == "__main__":
    pytest.main([__file__])
