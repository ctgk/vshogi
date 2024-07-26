"""Command Line Interface to make two models play against each other."""

import itertools
import os
import typing as tp

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

from classopt import classopt, config
import numpy as np
import tensorflow as tf
from tqdm import tqdm

import vshogi


MESSAGE_TEMPLATE = '''\
+---------+-----------------------+-----------------------+-----------------------+
|  Result |          #Win         |         #Draw         |         #Loss         |
+---------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
|   Turn  | total | black | white | total | black | white | total | black | white |
+---------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
| player1 | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} |
+---------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
| player2 | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} | {:5d} |
+---------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
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
        nargs='*',
        required=True,
        help='Path to tflite model of first player',
    )
    player2: str = config(
        nargs='*',
        required=True,
        help='Path to tflite model of second player',
    )
    num_games_each: int = config(
        type=int, default=10,
        help=(
            'N games with each player alternating between black and white'
            ', default=10'
        ),
    )
    mcts_kldgain_threshold: float = config(
        type=float, default=None,
        help='KL divergence threshold to stop MCT-search',
    )
    mcts_explorations: int = config(
        type=int, default=100,
        help='# of explorations by players at each game status, default=100',
    )
    mcts_coeff_puct: float = config(
        type=float, default=4.,
        help='Coefficient to compute PUCT score in MCTS, default=4',
    )
    mcts_temperature: float = config(
        type=float, default=0.,
        help='Temperature parameter to select action to take, default=0.',
    )
    dfpn_search_root: int = config(type=int, default=10000)
    dfpn_search_vertex: int = config(type=int, default=100)
    show_pbar: bool = config(
        action='store_true', help='Print progress bar if passed',
    )


def _get_results_of_single_pair(
    shogi_variant: str,
    player1: str,
    player2: str,
    num_games_each: int,
    show_pbar: bool,
    mcts_init_args: dict,
    search_args: dict,
    select_args: dict,
) -> dict:
    shogi = getattr(vshogi, shogi_variant)
    player1 = vshogi.engine.DfpnMcts(
        vshogi.engine.DfpnSearcher(),
        vshogi.engine.Mcts(
            PolicyValueFunction(player1),
            **mcts_init_args,
        ),
    )
    player2 = vshogi.engine.DfpnMcts(
        vshogi.engine.DfpnSearcher(),
        vshogi.engine.Mcts(
            PolicyValueFunction(player2),
            **mcts_init_args,
        ),
    )
    results_of_p1 = {
        'bwin': 0,
        'bdraw': 0,
        'bloss': 0,
        'wwin': 0,
        'wdraw': 0,
        'wloss': 0,
    }
    iterator = range(num_games_each * 2)
    if show_pbar:
        iterator = tqdm(iterator, ncols=80)
        iterator.set_description(str({'p1': 0, 'draw': 0, 'p2': 0}))
    for i in iterator:
        if i % 2 == 0:
            result = vshogi.play_game(
                shogi.Game(), player1, player2,
                search_args=search_args,
                select_args=select_args,
            ).result
            if result == vshogi.BLACK_WIN:
                results_of_p1['bwin'] += 1
            elif result == vshogi.WHITE_WIN:
                results_of_p1['bloss'] += 1
            else:
                results_of_p1['bdraw'] += 1
        else:
            result = vshogi.play_game(
                shogi.Game(), player2, player1,
                search_args=search_args,
                select_args=select_args,
            ).result
            if result == vshogi.BLACK_WIN:
                results_of_p1['wloss'] += 1
            elif result == vshogi.WHITE_WIN:
                results_of_p1['wwin'] += 1
            else:
                results_of_p1['wdraw'] += 1
        if args.show_pbar:
            iterator.set_description(str({
                'p1': results_of_p1['bwin'] + results_of_p1['wwin'],
                'draw': results_of_p1['bdraw'] + results_of_p1['wdraw'],
                'p2': results_of_p1['bloss'] + results_of_p1['wloss'],
            }))
    return results_of_p1


def _print_results(results: dict):
    print(MESSAGE_TEMPLATE.format(
        results['bwin'] + results['wwin'],
        results['bwin'],
        results['wwin'],
        results['bdraw'] + results['wdraw'],
        results['bdraw'],
        results['wdraw'],
        results['bloss'] + results['wloss'],
        results['bloss'],
        results['wloss'],
        results['bloss'] + results['wloss'],
        results['wloss'],
        results['bloss'],
        results['bdraw'] + results['wdraw'],
        results['wdraw'],
        results['bdraw'],
        results['bwin'] + results['wwin'],
        results['wwin'],
        results['bwin'],
    ))


if __name__ == "__main__":
    args = Args.from_args()

    results_of_p1_accumulated = {}
    for p1, p2 in itertools.product(args.player1, args.player2):
        if args.show_pbar:
            print(f'player1: {p1}')
            print(f'player2: {p2}')
        results_of_p1 = _get_results_of_single_pair(
            args.shogi_variant,
            p1, p2, args.num_games_each, args.show_pbar,
            mcts_init_args = {
                'coeff_puct': args.mcts_coeff_puct,
            },
            search_args={
                'dfpn_searches_at_root': args.dfpn_search_root,
                'mcts_searches': args.mcts_explorations,
                'dfpn_searches_at_vertex': args.dfpn_search_vertex,
                'kldgain_threshold': args.mcts_kldgain_threshold,
            },
            select_args={
                'temperature': args.mcts_temperature,
            },
        )
        for key, value in results_of_p1.items():
            if key not in results_of_p1_accumulated:
                results_of_p1_accumulated[key] = value
            else:
                results_of_p1_accumulated[key] += value

        if args.show_pbar:
            _print_results(results_of_p1)

    if (not args.show_pbar) or (len(args.player1) > 1) or (len(args.player2) > 1):
        print(f'player1: {args.player1}')
        print(f'player2: {args.player2}')
        _print_results(results_of_p1_accumulated)
