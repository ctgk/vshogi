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


def test_num_searched():
    game = shogi.Game()
    searcher = GumbelAlphaZero()
    searcher.set_game(game)
    assert searcher.num_searched == 0
    searcher.search(num_sims=20, num_actions=4)
    assert searcher.num_searched == 20


def test_set_game():
    game = shogi.Game()
    searcher = GumbelAlphaZero()
    searcher.set_game(game)
    searcher.search(num_sims=10, num_actions=2)
    action = searcher.select()
    game.apply(action)
    assert searcher.num_searched == 10

    searcher.set_game(game)
    assert searcher.num_searched == 0


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


if __name__ == '__main__':
    pytest.main([__file__])
