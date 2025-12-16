import numpy as np
import pytest

import vshogi.minishogi as shogi
from vshogi.engine import GumbelAlphaZero


def test_is_ready():
    game = shogi.Game()
    searcher = GumbelAlphaZero()
    assert searcher.is_ready() is False
    searcher.set_game(game)
    assert searcher.is_ready()


def test_get_search_count():
    game = shogi.Game()
    searcher = GumbelAlphaZero()
    searcher.set_game(game)
    assert searcher.get_search_count() == 0
    searcher.search(num_sims=20, num_actions=4)
    assert searcher.get_search_count() == 20


def test_set_game():
    game = shogi.Game()
    searcher = GumbelAlphaZero()
    searcher.set_game(game)
    searcher.search(num_sims=10, num_actions=2)
    action = searcher.select()
    game.apply(action)
    assert searcher.get_search_count() == 10

    searcher.set_game(game)
    assert searcher.get_search_count() == 0


def test_search_without_sequential_halving():
    game = shogi.Game()
    searcher = GumbelAlphaZero()
    searcher.set_game(game)
    searcher.search(num_sims=500)
    action = searcher.select()
    assert game.is_legal(action)


def test_dfpn_root():
    game = shogi.Game("3k1/5/4G/4K/5 b G")
    searcher = GumbelAlphaZero(dfpn_search_root=10000, dfpn_search_leaf=100)
    searcher.set_game(game)
    searcher.search(num_sims=10, num_actions=2)
    assert np.isclose(searcher.get_q_value(), 1.)
    assert searcher.proved_mate()
    assert shogi.Move("G*2b") == searcher.select()


def test_apply():
    game = shogi.Game("p4/5/5/5/4P b -")
    searcher = GumbelAlphaZero()
    searcher.set_game(game)
    searcher.search(10)
    actual = searcher.select()
    assert shogi.Move("1e1d") == actual
    assert searcher.get_search_count() == 10
    searcher.apply(actual)
    assert shogi.Move("5a5b") == searcher.select()
    assert searcher.get_search_count() == 9


def test_select_action():
    game = shogi.Game()
    searcher = GumbelAlphaZero()
    searcher.set_game(game)
    searcher.search(32, num_actions=4)
    print(searcher._tree(depth=1, breadth=-1))
    with pytest.raises(ValueError):
        searcher.select(10.)
    searcher.clear()
    searcher.set_game(game)
    searcher.search(32)
    assert (
        len(game.get_legal_moves())
        == len(set([searcher.select(10.) for _ in range(1000)]))
    )


if __name__ == '__main__':
    pytest.main([__file__])
