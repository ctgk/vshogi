"""Command Line Interface to make two models play against each other."""

import itertools
import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

from classopt import classopt, config
import numpy as np
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


@classopt(default_long=True)
class Args:
    shogi_variant: str = config(
        long=False,
        choices=['shogi', 'judkins_shogi', 'minishogi'],
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
    mcts_search_count: int = config(
        type=int, default=None,
        help='Number of searches at each game status, default=None',
    )
    mcts_search_second: float = config(
        type=float, default=None,
        help='Duration of searches at each game status, default=None',
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
    dfpn_search_leaf: int = config(type=int, default=100)
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
) -> vshogi.Record:
    shogi = getattr(vshogi, shogi_variant)
    player1 = vshogi.engine.DfpnMcts(
        vshogi.engine.DfpnSearcher(),
        vshogi.engine.Mcts(
            vshogi.dlshogi.PolicyValueFunction(player1),
            **mcts_init_args,
        ),
    )
    player2 = vshogi.engine.DfpnMcts(
        vshogi.engine.DfpnSearcher(),
        vshogi.engine.Mcts(
            vshogi.dlshogi.PolicyValueFunction(player2),
            **mcts_init_args,
        ),
    )
    record_of_p1 = vshogi.Record(0, 0, 0, 0, 0, 0)
    iterator = range(num_games_each * 2)
    show_mcts_search: bool = isinstance(search_args['mcts_search'], float)
    p1_search_total = 0
    p2_search_total = 0
    if show_pbar:
        iterator = tqdm(iterator, ncols=80)
        iterator.set_description(str({'p1': 0, 'draw': 0, 'p2': 0}))
    for i in iterator:
        if i % 2 == 0:
            out = vshogi.play_game(
                shogi.Game(), player1, player2,
                search_args=search_args,
                select_args=select_args,
                _return_num_searched=isinstance(
                    search_args['mcts_search'], float),
            )
            if isinstance(search_args['mcts_search'], float):
                result = out[0].result
                p1_search_total += np.nanmean(np.asarray(out[1][::2], float))
                p2_search_total += np.nanmean(np.asarray(out[1][1::2], float))
            else:
                result = out.result
            record_of_p1 += vshogi.Record.from_black_result(result)
        else:
            out = vshogi.play_game(
                shogi.Game(), player2, player1,
                search_args=search_args,
                select_args=select_args,
                _return_num_searched=isinstance(
                    search_args['mcts_search'], float),
            )
            if isinstance(search_args['mcts_search'], float):
                result = out[0].result
                p2_search_total += np.nanmean(np.asarray(out[1][::2], float))
                p1_search_total += np.nanmean(np.asarray(out[1][1::2], float))
            else:
                result = out.result
            record_of_p1 += vshogi.Record.from_white_result(result)
        if args.show_pbar:
            iterator.set_description(str({
                'p1': record_of_p1.wins_total,
                'draw': record_of_p1.draws_total,
                'p2': record_of_p1.losses_total,
            }))
    if args.show_pbar and show_mcts_search:
        print(
            'Initial search counts:',
            f'p1={p1_search_total / (num_games_each * 2):.2f},',
            f'p2={p2_search_total / (num_games_each * 2):.2f}',
        )
    return record_of_p1


def _print_results(record: vshogi.Record):
    print(MESSAGE_TEMPLATE.format(
        record.wins_total,
        record.wins_black,
        record.wins_white,
        record.draws_total,
        record.draws_black,
        record.draws_white,
        record.losses_total,
        record.losses_black,
        record.losses_white,
        record.losses_total,
        record.losses_black,
        record.losses_white,
        record.draws_total,
        record.draws_black,
        record.draws_white,
        record.wins_total,
        record.wins_black,
        record.wins_white,
    ))


if __name__ == "__main__":
    args = Args.from_args()

    record_of_p1_group = vshogi.Record(0, 0, 0, 0, 0, 0)
    for p1, p2 in itertools.product(args.player1, args.player2):
        if args.show_pbar:
            print(f'player1: {p1}')
            print(f'player2: {p2}')
        record_of_p1 = _get_results_of_single_pair(
            args.shogi_variant,
            p1, p2, args.num_games_each, args.show_pbar,
            mcts_init_args = {
                'coeff_puct': args.mcts_coeff_puct,
            },
            search_args={
                'dfpn_search_root': args.dfpn_search_root,
                'mcts_search': (
                    args.mcts_search_count or args.mcts_search_second
                ),
                'dfpn_search_leaf': args.dfpn_search_leaf,
                'kldgain_threshold': args.mcts_kldgain_threshold,
            },
            select_args={
                'temperature': args.mcts_temperature,
            },
        )
        record_of_p1_group += record_of_p1

        if args.show_pbar:
            _print_results(record_of_p1)

    if (not args.show_pbar) or (len(args.player1) > 1) or (len(args.player2) > 1):
        print(f'player1: {args.player1}')
        print(f'player2: {args.player2}')
        _print_results(record_of_p1_group)
