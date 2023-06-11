import numpy as np
import pytest

import vshogi.animal_shogi as shogi
from vshogi.engine import MonteCarloTreeSearcher


def uniform_pv_func(game):
    moves = game.get_legal_moves()
    n = len(moves)
    policy = {m: p for m, p in zip(moves, np.ones(n) / n)}
    value = {
        shogi.ONGOING: 0, shogi.DRAW: 0,
        shogi.BLACK_WIN: -1, shogi.WHITE_WIN: -1,
    }[game.result]
    return policy, value


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


def test_q_values():
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
    assert np.isclose(actual[m], 1)


def test_visit_count_by_dirichlet_noise():
    # Turn: BLACK
    # White: -
    #     A  B  C
    #   *--*--*--*
    # 1 |-G|-C|-E|
    #   *--*--*--*
    # 2 |  |-L|  |
    #   *--*--*--*
    # 3 |  |+L|  |
    #   *--*--*--*
    # 4 |+E|+C|+G|
    #   *--*--*--*
    # Black: -
    game = shogi.Game('gce/1l1/1L1/ECG b -')
    m = shogi.Move(shogi.B2, shogi.B3)

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


if __name__ == '__main__':
    pytest.main([__file__])
