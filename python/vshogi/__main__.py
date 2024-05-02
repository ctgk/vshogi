"""Command Line Interface to make two models play against each other."""

import os
import typing as tp

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

from classopt import classopt, config
import numpy as np
import tensorflow as tf

import vshogi


MESSAGE_TEMPLATE = '''\
+---------+-------+-------+-------+
|  #ofWin | total | black | white |
+---------+-------+-------+-------+
| player1 | {:5d} | {:5d} | {:5d} |
+---------+-------+-------+-------+
| player2 | {:5d} | {:5d} | {:5d} |
+---------+-------+-------+-------+
'''

class PolicyValueFunction:

    def __init__(
        self,
        model: str,
        num_threads: int = 1,
    ) -> None:
        self._interpreter = tf.lite.Interpreter(
            model_path=model, num_threads=num_threads)
        self._interpreter.allocate_tensors()
        input_details = self._interpreter.get_input_details()[0]
        self._input_placeholder = np.empty(
            input_details['shape'], dtype=np.float32)
        self._input_index = input_details['index']
        output_details = self._interpreter.get_output_details()
        self._value_index = output_details[0]['index']
        self._policy_index = output_details[1]['index']

    def __call__(self, game: vshogi.Game) -> tp.Tuple[np.ndarray, float]:
        game.to_dlshogi_features(out=self._input_placeholder)
        self._interpreter.set_tensor(
            self._input_index, self._input_placeholder)
        self._interpreter.invoke()
        value = self._interpreter.get_tensor(self._value_index).item()
        policy_logits = self._interpreter.get_tensor(self._policy_index)
        return policy_logits, value


@classopt(default_long=True)
class Args:
    shogi_variant: str = config(
        long=False,
        choices=['shogi', 'animal_shogi', 'judkins_shogi', 'minishogi'],
        help='Choose a variant of shogi to play',
    )
    player1: str = config(
        long=False,
        help='Path to tflite model of first player',
    )
    player2: str = config(
        long=False,
        help='Path to tflite model of second player',
    )
    num_games_each: int = config(
        type=int, default=10,
        help=(
            'N games with each player alternating between black and white'
            ', default=10'
        ),
    )
    mcts_explorations: int = config(
        type=int, default=100,
        help='# of explorations by players at each game status, default=100',
    )
    mcts_coeff_puct: float = config(
        type=float, default=4.,
        help='Coefficient to compute PUCT score in MCTS, default=4',
    )



if __name__ == "__main__":
    args = Args.from_args()
    shogi = getattr(vshogi, args.shogi_variant)

    player1 = vshogi.engine.MonteCarloTreeSearcher(
        PolicyValueFunction(args.player1),
        num_explorations=args.mcts_explorations,
        coeff_puct=args.mcts_coeff_puct,
    )
    player2 = vshogi.engine.MonteCarloTreeSearcher(
        PolicyValueFunction(args.player2),
        num_explorations=args.mcts_explorations,
        coeff_puct=args.mcts_coeff_puct,
    )

    results = [
        vshogi.play_game(shogi.Game(), player1, player2).result
        for _ in range(args.num_games_each)
    ] + [
        vshogi.play_game(shogi.Game(), player2, player1).result
        for _ in range(args.num_games_each)
    ]

    num_black_win_1st_half = np.sum([
        r == vshogi.BLACK_WIN for r in results[:args.num_games_each]])
    num_black_win_2nd_half = np.sum([
        r == vshogi.BLACK_WIN for r in results[args.num_games_each:]])

    print(MESSAGE_TEMPLATE.format(
        num_black_win_1st_half + args.num_games_each - num_black_win_2nd_half,
        num_black_win_1st_half,
        args.num_games_each - num_black_win_2nd_half,
        args.num_games_each - num_black_win_1st_half + num_black_win_2nd_half,
        args.num_games_each - num_black_win_1st_half,
        num_black_win_2nd_half,
    ))
