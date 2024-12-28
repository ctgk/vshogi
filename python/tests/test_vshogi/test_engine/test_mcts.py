import numpy as np
import pytest

import vshogi.minishogi as shogi
from vshogi.engine import Mcts


def uniform_pv_func(game):
    return np.zeros(game.num_dlshogi_policy), 0.


def test_is_ready():
    game = shogi.Game()
    searcher = Mcts(uniform_pv_func)
    assert searcher.is_ready() is False
    searcher.set_game(game)
    assert searcher.is_ready()


def test_num_searched():
    game = shogi.Game()
    searcher = Mcts(uniform_pv_func)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    assert searcher.num_searched == 100 + 1


# def test_apply():
#     game = shogi.Game()
#     searcher = Mcts(uniform_pv_func)
#     searcher.set_game(game)
#     searcher.search(n_or_t=1000)
#     m = shogi.Move(shogi.B2, shogi.B3)
#     assert searcher.num_explored == 1000 + 1

#     expected = searcher._root.get_child(m).get_visit_count()

#     searcher.apply(m)

#     assert searcher.num_explored == expected


def test_clear():
    game = shogi.Game()
    searcher = Mcts(uniform_pv_func)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    searcher.clear()
    assert searcher.is_ready() is False
    assert searcher.num_searched == 0


def test_q_values_mate_in_one():
    game = shogi.Game("b2pk/3b1/4P/2gRR/4K b -")
    m = shogi.Move(shogi.SQ_1B, shogi.SQ_1C)

    searcher = Mcts(uniform_pv_func, random_depth=0)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    actual = searcher.get_q_values()
    print(actual)
    assert np.isclose(actual[m], 1, rtol=0, atol=1e-2)


def test_q_values_initial():
    game = shogi.Game()
    searcher = Mcts(uniform_pv_func)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    actual = searcher.get_q_values()
    print(actual)
    for a in actual.values():
        assert np.isclose(a, 0, rtol=0, atol=0.1)


def test_mate_in_three():
    game = shogi.Game('1r3/2k1G/5/2PG1/5 b -')
    m = shogi.Move(shogi.SQ_3C, shogi.SQ_2D)

    searcher = Mcts(uniform_pv_func, random_depth=0)
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
    m = shogi.Move(shogi.SQ_1B, shogi.SQ_1E)

    searcher = Mcts(
        lambda g: (np.arange(g.num_dlshogi_policy)[::-1], 0.), random_depth=0)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    visit_count = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2, breadth=-1))

    searcher = Mcts(
        lambda g: (np.arange(g.num_dlshogi_policy)[::-1], 0.),
        random_depth=1, non_random_ratio=0)
    searcher.set_game(game)
    searcher.search(n_or_t=100)
    visit_count_with_noise = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2, breadth=-1))
    assert visit_count > visit_count_with_noise + 10


def test_greedy_q_value():
    game = shogi.Game()
    searcher = Mcts(uniform_pv_func)
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
    searcher._tree(depth=2, breadth=-1)


if __name__ == '__main__':
    pytest.main([__file__])
