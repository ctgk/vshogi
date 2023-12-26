import numpy as np
import pytest

import vshogi.animal_shogi as shogi
from vshogi.engine import MonteCarloTreeSearcher


def uniform_pv_func(game):
    return np.zeros(game.num_dlshogi_policy), 0.


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


# def test_apply():
#     game = shogi.Game()
#     searcher = MonteCarloTreeSearcher(uniform_pv_func)
#     searcher.set_root(game)
#     searcher.explore(n=1000)
#     m = shogi.Move(shogi.B2, shogi.B3)
#     assert searcher.num_explored == 1000 + 1

#     expected = searcher._root.get_child(m).get_visit_count()

#     searcher.apply(m)

#     assert searcher.num_explored == expected


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
    game = shogi.Game('l2/3/3/2L b -')
    m = shogi.Move(shogi.B3, shogi.C4)

    searcher = MonteCarloTreeSearcher(
        lambda g: (
            np.zeros(g.num_dlshogi_policy),
            -0.99 if g.to_sfen(False) == 'l2/3/1L1/3 w -' else 0.,
        ),
    )
    searcher.set_root(game)
    searcher.explore(n=50, random_depth=0)
    visit_count = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2, breadth=-1))

    searcher.set_root(game)
    searcher.explore(n=50, random_depth=1, non_random_ratio=0)
    visit_count_with_noise = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2, breadth=-1))

    assert visit_count > visit_count_with_noise


def test_visit_count_by_random_2():
    game = shogi.Game()
    m = shogi.Move(shogi.C3, shogi.C4)

    searcher = MonteCarloTreeSearcher(
        lambda g: (np.arange(g.num_dlshogi_policy), 0.))
    searcher.set_root(game)
    searcher.explore(n=100, random_depth=0)
    visit_count = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2, breadth=-1))

    searcher.set_root(game)
    searcher.explore(n=100, random_depth=1, non_random_ratio=0)
    visit_count_with_noise = searcher.get_visit_counts()[m]
    print(searcher._tree(depth=2, breadth=-1))
    assert visit_count > visit_count_with_noise + 10


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
