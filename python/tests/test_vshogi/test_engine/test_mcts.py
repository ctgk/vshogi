import numpy as np
import pytest

import vshogi.animal_shogi as shogi
from vshogi.engine import MonteCarloTreeSearcher


def uniform_pv_func(game):
    return np.zeros(game.num_dlshogi_policy, dtype=np.float32), 0.


def test_is_ready():
    # Turn: BLACK
    # White: -
    #     A  B  C
    #   *--*--*--*
    # 1 |-G|  |-E|
    #   *--*--*--*
    # 2 |  |-C|-L|
    #   *--*--*--*
    # 3 |  |+C|+L|
    #   *--*--*--*
    # 4 |+E|  |+G|
    #   *--*--*--*
    # Black: -
    game = shogi.Game('g1e/1cl/1CL/E1G b -')

    searcher = MonteCarloTreeSearcher(uniform_pv_func)
    assert searcher.is_ready() is False
    searcher.set_root(game)
    assert searcher.is_ready()


def test_num_explored():
    # Turn: BLACK
    # White: -
    #     A  B  C
    #   *--*--*--*
    # 1 |-G|  |-E|
    #   *--*--*--*
    # 2 |  |-C|-L|
    #   *--*--*--*
    # 3 |  |+C|+L|
    #   *--*--*--*
    # 4 |+E|  |+G|
    #   *--*--*--*
    # Black: -
    game = shogi.Game('g1e/1cl/1CL/E1G b -')

    searcher = MonteCarloTreeSearcher(uniform_pv_func)
    searcher.set_root(game)
    searcher.explore(n=100)
    assert searcher.num_explored == 100 + 1


def test_clear():
    # Turn: BLACK
    # White: -
    #     A  B  C
    #   *--*--*--*
    # 1 |-G|  |-E|
    #   *--*--*--*
    # 2 |  |-C|-L|
    #   *--*--*--*
    # 3 |  |+C|+L|
    #   *--*--*--*
    # 4 |+E|  |+G|
    #   *--*--*--*
    # Black: -
    game = shogi.Game('g1e/1cl/1CL/E1G b -')

    searcher = MonteCarloTreeSearcher(uniform_pv_func)
    searcher.set_root(game)
    searcher.explore(n=100)
    searcher.clear()
    assert searcher.is_ready() is False
    assert searcher.num_explored == 0


def test_q_values_mate_in_one():
    # Turn: BLACK
    # White: -
    #     A  B  C
    #   *--*--*--*
    # 1 |-G|  |-E|
    #   *--*--*--*
    # 2 |  |-C|-L|
    #   *--*--*--*
    # 3 |  |+C|+L|
    #   *--*--*--*
    # 4 |+E|  |+G|
    #   *--*--*--*
    # Black: -
    game = shogi.Game('g1e/1cl/1CL/E1G b -')
    m = shogi.Move(shogi.C2, shogi.C3)

    searcher = MonteCarloTreeSearcher(uniform_pv_func)
    searcher.set_root(game)
    searcher.explore(n=100, random_depth=0)
    actual = searcher.get_q_values()
    print(actual)
    assert np.isclose(actual[m], 1, rtol=0, atol=1e-2)


def test_mate_in_three():
    # Turn: BLACK
    # White: -
    #     A  B  C
    #   *--*--*--*
    # 1 |-G|  |-E|
    #   *--*--*--*
    # 2 |-L|-C|  |
    #   *--*--*--*
    # 3 |  |+C|+L|
    #   *--*--*--*
    # 4 |+E|  |+G|
    #   *--*--*--*
    # Black: -
    game = shogi.Game('l1e/gc1/1CL/E1G b -')
    m = shogi.Move(shogi.C2, shogi.C3)

    searcher = MonteCarloTreeSearcher(uniform_pv_func)
    searcher.set_root(game)
    searcher.explore(n=100, random_depth=0)

    actual = searcher.get_q_values()
    print(actual)
    print(searcher._tree(depth=3))
    assert np.isclose(actual[m], 1, rtol=0, atol=1e-2)

    visit_count = searcher.get_visit_counts()[m]
    searcher.explore(n=100, random_depth=0)
    # If there is a mate, all explorations go through the mate.
    assert searcher.get_visit_counts()[m] == visit_count + 100


def test_visit_count_by_random():
    # Turn: BLACK
    # White: -
    #     A  B  C
    #   *--*--*--*
    # 1 |-G|-C|  |
    #   *--*--*--*
    # 2 |-L|-E|  |
    #   *--*--*--*
    # 3 |  |  |+L|
    #   *--*--*--*
    # 4 |+E|+C|+G|
    #   *--*--*--*
    # Black: -
    game = shogi.Game('gc1/le1/2L/ECG b -')
    m = shogi.Move(shogi.C2, shogi.C3)

    searcher = MonteCarloTreeSearcher(uniform_pv_func)
    searcher.set_root(game)
    searcher.explore(n=100, random_depth=0)
    visit_count = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2))

    searcher.set_root(game)
    searcher.explore(n=100, random_depth=1)
    visit_count_with_noise = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2))

    assert visit_count > visit_count_with_noise


def test_tree():
    game = shogi.Game()
    searcher = MonteCarloTreeSearcher(uniform_pv_func)
    searcher.set_root(game)
    searcher.explore(n=100)

    searcher._tree(depth=2, breadth=3)
    searcher._tree(depth=2, breadth=5)
    searcher._tree(depth=2, breadth=-1)


if __name__ == '__main__':
    pytest.main([__file__])
