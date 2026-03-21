import numpy as np
import pytest

import vshogi.minishogi as shogi
from vshogi.engine import Klent


def test_is_ready():
    game = shogi.Game()
    engine = Klent()
    assert engine.is_ready() is False
    engine.set_game(game)
    assert engine.is_ready()


def test_clear():
    game = shogi.Game()
    engine = Klent()
    engine.set_game(game)
    assert engine.is_ready()
    engine.clear()
    assert engine.is_ready() is False


def test_mate():
    game = shogi.Game("b2pk/3b1/4P/2gRR/4K b -")
    move = shogi.Move(shogi.SQ_1C, shogi.SQ_1B)
    engine = Klent(dfpn_search=100)
    engine.set_game(game)
    engine.search()
    assert engine.proved_mate()
    assert engine.select() == move
    assert engine.get_mate_moves() == [move]


def test_get_value_policy():
    game = shogi.Game("3rk/5/5/4K/5 b -")
    moves = [shogi.Move("1d1c"), shogi.Move("1d1e")]
    policy = game.to_dlshogi_policy({}, default_value=0.5)
    policy_logits = np.log(policy)
    q_values = np.zeros_like(policy)
    q_values[moves[0]._to_dlshogi_policy_index(game.turn)] = -0.1
    q_values[moves[1]._to_dlshogi_policy_index(game.turn)] = 0.1
    engine = Klent(lambda g: (policy_logits, q_values))
    engine.set_game(game)
    engine.search()
    value = engine.get_q_value()
    print(value)
    assert value > 0
    policy = engine.get_improved_policy()
    print(policy)
    assert policy[moves[0]] < 0.5
    assert policy[moves[1]] > 0.5


def test_get_value_policy_dict():
    game = shogi.Game("3rk/5/5/4K/5 b -")
    moves = [shogi.Move("1d1c"), shogi.Move("1d1e")]
    policy = game.to_dlshogi_policy({}, default_value=0.5)
    policy_logits = np.log(policy)
    engine = Klent(lambda g: (policy_logits, {moves[0]: -0.1, moves[1]: 0.1}))
    engine.set_game(game)
    engine.search()
    value = engine.get_q_value()
    print(value)
    assert value > 0
    policy = engine.get_improved_policy()
    print(policy)
    assert policy[moves[0]] < 0.5
    assert policy[moves[1]] > 0.5


if __name__ == "__main__":
    pytest.main([__file__])
