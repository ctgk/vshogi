import numpy as np
import pytest

import vshogi.animal_shogi as shogi
from vshogi.engine import MonteCarloTreeSearcher


def pv_func(game):
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

    searcher = MonteCarloTreeSearcher(pv_func)
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

    searcher = MonteCarloTreeSearcher(pv_func)
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

    searcher = MonteCarloTreeSearcher(pv_func)
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

    searcher = MonteCarloTreeSearcher(pv_func)
    searcher.set_root(game)
    searcher.explore(n=100, dirichlet_noise_depth=0)
    actual = searcher.get_q_values()
    print(actual)
    assert np.isclose(actual[m], 1)


def test_q_values_by_dirichlet_noise():
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
    game = shogi.Game('g1e/lc1/1CL/E1G b -')
    m = shogi.Move(shogi.C2, shogi.C3)

    searcher = MonteCarloTreeSearcher(pv_func)
    searcher.set_root(game)
    searcher.explore(n=100, dirichlet_noise_depth=0)
    actual = searcher.get_q_values()
    print(actual)
    print(searcher._root._children[m])

    searcher = MonteCarloTreeSearcher(pv_func)
    searcher.set_root(game)
    searcher.explore(n=100, dirichlet_noise_depth=3)
    actual_with_noise = searcher.get_q_values()
    print(actual_with_noise)
    print(searcher._root._children[m])

    assert actual[m] > actual_with_noise[m]


def test_q_value_near_game_end():
    # Turn: WHITE
    # White: -
    #     A  B  C
    #   *--*--*--*
    # 1 |-G|-L|-E|
    #   *--*--*--*
    # 2 |  |+C|  |
    #   *--*--*--*
    # 3 |  |  |  |
    #   *--*--*--*
    # 4 |+E|+L|+G|
    #   *--*--*--*
    # Black: C
    game = shogi.Game('gle/1C1/3/ELG w C')

    def _pv_func(game: shogi.Game):
        m = shogi.Move(shogi.A2, shogi.A1)
        if m in game.get_legal_moves():
            return {m: 1.}, 0.
        return pv_func(game)

    searcher = MonteCarloTreeSearcher(_pv_func)
    searcher.set_root(game)
    searcher.explore(n=1000, c_puct=0.1, dirichlet_noise_depth=1)
    actual = searcher.get_q_values()
    print(actual)
    print(searcher._root._children)
    assert np.isclose(
        actual[shogi.Move(shogi.A2, shogi.A1)], -1.,
        rtol=0, atol=1e-2,
    )


def test_visit_counts():
    game = shogi.Game()
    seacher = MonteCarloTreeSearcher(pv_func)
    seacher.set_root(game)
    seacher.explore(n=10, dirichlet_noise_depth=0)
    assert sum(seacher.get_visit_counts().values()) == 10


def test_visit_counts_by_dirichlet_noise():
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

    searcher = MonteCarloTreeSearcher(pv_func)
    searcher.set_root(game)
    searcher.explore(n=100, dirichlet_noise_depth=0)
    actual = searcher.get_visit_counts()
    print(actual)

    np.random.seed(0)
    searcher = MonteCarloTreeSearcher(pv_func)
    searcher.set_root(game)
    searcher.explore(n=100, dirichlet_noise_depth=1)
    actual_with_noise = searcher.get_visit_counts()
    print(actual_with_noise)
    assert actual[m] > actual_with_noise[m]


if __name__ == '__main__':
    pytest.main([__file__])
