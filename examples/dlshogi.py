"""Script to run DL-shogi training.

| index |        Self-play       |          Train NN         |
|-------|------------------------|---------------------------|
|   0   |          (skip)        | NN0 = Initialized         |
|   1   | DATASET1  = NN0 vs NN0 | NN1 = Trained by DATASET1 |
|   2   | DATASET2 += NN1 vs NN1 | NN2 = Trained by DATASET2 |
|  ...  |          ...           |             ...           |
"""

import contextlib
from glob import glob
import os
import subprocess
import sys
import typing as tp

os.environ['TF_CPP_MIN_LOG_LEVEL']='3'

from classopt import classopt, config
import joblib
from joblib.parallel import Parallel, delayed
import numpy as np
from tqdm import tqdm

import vshogi
from vshogi.dlshogi._cli_nn_trainer import _train_step


@classopt(default_long=True)
class Args:
    run: str = config(long=False, choices=['rl', 'self-play'])
    shogi_variant: str = config(
        long=False,
        choices=['shogi', 'judkins_shogi', 'minishogi'],
        help='Choose a variant of shogi to train!',
    )
    rl_cycle: int = config(type=int, default=10, help='# of Reinforcement Learning cycle. By default 10.')
    resume_rl_cycle_from: int = config(type=int, default=1, help='Resume Reinforcement Learning cycle if given. By default 0.')
    nn_hidden_channels: int = config(type=int, default=None, help='# of hidden channels in NN. Default value varies in shogi games.')
    nn_bottleneck_channels: int = config(type=int, default=None, help='# of bottleneck channels in NN. Default value varies in shogi games.')
    nn_backbone_blocks: int = config(type=int, default=None, help='# of backbone res-blocks in NN. Default value varies in shogi games.')
    nn_train_fraction: float = config(type=float, default=0.8, help='Fraction of game log by former models to use to train current one. By default 0.8')
    nn_epochs: int = config(type=int, default=5, help='# of epochs in NN training. By default 5.')
    nn_minibatch: int = config(type=int, default=32, help='Minibatch size in NN training. By default 32.')
    nn_grad_accum: int = config(type=int, default=1, help='Gradient accumulation steps. By default 1.')
    nn_learning_rate: float = config(type=float, default=1e-2, help='Learning rate of NN weight update')
    nn_coeff_policy_loss: float = config(type=float, default=0.1, help='Coefficient of policy loss, by default 0.1')
    nn_data_importance_decay: float = config(type=float, default=0.7)
    nn_entropy_regularization: float = config(type=float, default=1e-2)
    nn_load_previous_weights: int = config(type=int, default=1, help='Load previous weights if 1, else train network from scratch. By default 0.')
    nn_train_device: str = config(type=str, default='cpu', choices=['cpu', 'cuda', 'mps'])
    nn_workers: int = config(type=int, default=0, help='Number of workers for data loading. Default is 0.')
    nn_prefetch_factor: int = config(type=int, default=1, help='Prefetch factor for data loading. Default is 1.')
    nn_dataset_size: int = config(type=int, default=100000, help='Max size of NN training dataset. Default is 100000.')
    discount_factor: float = config(type=float, default=0.99, help='Discount factor of reward supervision. By default 0.99.')
    az_kldgain_threshold: float = config(type=float, default=1e-4, help='KL divergence threshold to stop Alpha Zero search')
    az_search: int = config(type=int, default=1000, help='# of searches in Alpha Zero, default=1000. Alpha Zero used 800 simulations in the paper.')
    az_random_rate: float = config(
        type=float, default=0.5,
        help=(
            'Select action by random sample from distribution by MCTS '
            'for first `r * average_moves_until_game_end` moves. '
            'The rest of the actions are obtained by selecting the mode of the distribution.'
        ),
    )
    az_temperature: float = config(type=float, default=1., help='Temperature parameter when selecting action by random.')
    az_coeff_puct: float = config(type=float, default=4., help='Coefficient of PUCT score in Alpha Zero, default=4.')
    az_q_greedy_depth: int = config(type=int, default=3, help='Number of depth to select node greedily when computing Q-value of a node, by default=3')
    dfpn_search_root: int = config(type=int, default=10000, help='Number of DFPN searches at root node of MCTS tree. By default 10000.')
    dfpn_search_leaf: int = config(type=int, default=100, help='Number of DFPN searches at leaf node of MCTS tree. By default 100.')
    self_play: int = config(type=int, default=100, help='# of self-play in one RL cycle, default=100')
    self_play_index_from: int = config(type=int, default=0, help='Index to start self-play from, default=0')
    another_player: list = config(type=int, nargs='*', default=[])
    validations: int = config(type=int, default=10, help='# of validation plays per model, default=10')
    win_ratio_threshold: float = config(type=float, default=0.55, help='Threshold of win ratio to adopt new model against previous one, default=0.55')
    jobs: int = config(short=False, type=int, default=1, help='# of jobs to run self-play in parallel, default=1')
    output: str = config(short=True, type=str, help='Output path of self-play datasets and trained NN models, default=`shogi`')


def dump_game_log(file_, game: vshogi.Game, color_filter: vshogi.Color = None) -> None:
    r = str(game.result)
    if 'ONGOING' in r:
        raise ValueError('The game is still ongoing')
    game.dump_log(
        (
            lambda g, i: g.get_sfen_at(i, include_move_count=False),
            lambda g, i: g.get_move_at(i).to_sfen(),
            lambda g, i: 0 if 'DRAW' in r else (2 * (('BLACK' in r) is (i % 2 == 0)) - 1),
            lambda g, i: g.q_value_log[i],
            lambda g, i: g.visit_count_log[i],
            lambda g, i: g.z_weight_log[i],
        ),
        names=('sfen', 'move', 'result', 'q_value', 'policy', 'z_weight'),
        file_=file_,
        color_filter=color_filter,
    )


def play_game(
    player_black: vshogi.engine.AlphaZero,
    player_white: vshogi.engine.AlphaZero,
    args: Args,
    max_moves: int = 320,
    main_player: tp.Optional[vshogi.engine.AlphaZero] = None,
) -> vshogi.Game:
    """Make two players play the game until an end.

    Parameters
    ----------
    player_black : vshogi.engine.AlphaZero
        First player
    player_white : vshogi.engine.AlphaZero
        Second player
    max_moves : int
        Maximum number of moves to apply to the game.
        If it reaches the value, return the game even if it is ongoing.

    Returns
    -------
    Game
        The game the two players played.
    """
    game = args._shogi.Game()
    game.q_value_log = []
    game.visit_count_log = []
    game.z_weight_log = []
    num_random_moves = (
        np.random.choice(args._num_random_moves + 1)
        if np.isfinite(args._num_random_moves) else args._num_random_moves
    )
    for _ in range(max_moves):
        if game.result != vshogi.Result.ONGOING:
            break

        player = player_black if game.turn == vshogi.Color.BLACK else player_white
        if not player.is_ready():
            player.set_game(game)

        player.search(args.az_search - player.get_search_count())
        if (main_player is not None) and (main_player is not player):
            if not main_player.is_ready():
                main_player.set_game(game)
            main_player.search(args.az_search - main_player.get_search_count())

        if player.proved_mate():
            if player.get_q_value() > 0:
                mate_moves = game.get_mate_moves_if_any(args.dfpn_search_root)
                if mate_moves is not None:
                    for i, m in enumerate(mate_moves):
                        game.apply(m)
                        game.q_value_log.append(1. if i % 2 == 0 else -1.)
                        game.visit_count_log.append({})
                        game.z_weight_log.append(0.)
                    if game.result != vshogi.Result.ONGOING:
                        break
                    else:
                        for _ in mate_moves:
                            game.undo()
            move = player.select()
            # Setting z_weight = 0, because the result can be independent of
            # this proof when the player fails to prove a checkmate in the
            # following game position.
            game.z_weight_log.append(0.)
        elif game.ply() < num_random_moves:
            move = player.select(temperature=args.az_temperature)
            game.z_weight_log.append(0.)
        else:
            move = player.select()
            game.z_weight_log.append(0.5 if (main_player is None) else 0.)
        if (move == args._shogi.Move("1a1a")):
            raise ValueError(
                f"Invalid move ({move}) selected at the game, "
                f"{game.to_sfen()}.\n{player._tree(depth=2)}")

        player_dump = main_player or player
        visit_count = {
            m.to_sfen(): v + 1  # +1 for smoothing
            for m, v in
            player_dump.get_visit_counts(include_random=False).items()
        }
        game.q_value_log.append(
            player_dump.get_q_value(greedy_depth=args.az_q_greedy_depth))
        game.visit_count_log.append(visit_count)

        game.apply(move)
        player_black.apply(move)
        if player_white is not player_black:
            player_white.apply(move)

    if game.result == vshogi.Result.ONGOING:
        game.declare_draw()

    player_black.clear()
    player_white.clear()
    return game


def load_player_of(index: int) -> vshogi.engine.AlphaZero:
    return vshogi.engine.AlphaZero(
        (
            vshogi.dlshogi.PolicyValueFunction(f'models/model_{index:04d}.tflite')
            if index != 0 else lambda g: (np.zeros(g.num_dlshogi_policy, dtype=np.float32), vshogi.engine.piece_value_func(g))
        ),
        coeff_puct=args.az_coeff_puct,
        kldgain_threshold=args.az_kldgain_threshold,
        dfpn_search_root=args.dfpn_search_root,
        dfpn_search_leaf=args.dfpn_search_leaf,
        name=str(index),
    )


def play_game_and_dump_log(
    black,
    white,
    args: Args,
    index: int,
    suffix: str,
    main_player = None,
) -> vshogi.Result:
    while True:
        game = play_game(black, white, args, main_player=main_player)
        if game.result != vshogi.ONGOING:
            break
    if (index is not None) and (suffix is not None):
        path = f'datasets/dataset_{index:04d}/kifu_{suffix}.tsv'
        with open(path, 'w') as f:
            dump_game_log(f, game)
    return game.result


@contextlib.contextmanager
def tqdm_joblib(tqdm_object):
    """Context manager to patch joblib to report into tqdm progress, args bar given as argument"""
    class TqdmBatchCompletionCallback(joblib.parallel.BatchCompletionCallBack):
        def __call__(self, *args, **kwargs):
            tqdm_object.update(n=self.batch_size)
            return super().__call__(*args, **kwargs)

    old_batch_callback = joblib.parallel.BatchCompletionCallBack
    joblib.parallel.BatchCompletionCallBack = TqdmBatchCompletionCallback
    try:
        yield tqdm_object
    finally:
        joblib.parallel.BatchCompletionCallBack = old_batch_callback
        tqdm_object.close()


def run_self_play(args: Args):

    def _self_play_and_dump_log(player, index, nth_game: int):
        play_game_and_dump_log(player, player, args, index, f'{nth_game:05d}')

    def _play_game_and_dump_log(player, player_another, index, index_another, nth_game: int):
        if (nth_game // 10) % 2 == 0:
            play_game_and_dump_log(player, player_another, args, index, f'{nth_game:05d}_B{index-1:02d}vsW{index_another:02d}', main_player=player)
        else:
            play_game_and_dump_log(player_another, player, args, index, f'{nth_game:05d}_B{index_another:02d}vsW{index-1:02d}', main_player=player)

    def self_play_and_dump_logs_in_parallel(index: int, index_another: int, n_jobs: int):

        def _self_play_and_dump_log_n_times(index, index_another, nth_game: list):
            player = load_player_of(index - 1)
            for i in nth_game:
                if index_another[i % len(index_another)] is not None:
                    player_another = load_player_of(index_another[i % len(index_another)])
                    _play_game_and_dump_log(player, player_another, index, index_another[i % len(index_another)], i)
                else:
                    _self_play_and_dump_log(player, index, i)

        group_size = 5
        with tqdm_joblib(tqdm(total=args.self_play // group_size, ncols=100, desc=f'{index-1} vs {index-1}', file=sys.stdout)):
            Parallel(n_jobs=n_jobs)(
                delayed(_self_play_and_dump_log_n_times)(
                    index, index_another, list(range(i, i + group_size)),
                )
                for i in range(args.self_play_index_from, args.self_play_index_from + args.self_play, group_size)
            )


    def self_play_and_dump_logs(index: int, index_another: tp.List[int]):
        if args.jobs == 1:
            player = load_player_of(index - 1)
            player_another = [None if i is None else load_player_of(i) for i in index_another]
            for i in tqdm(
                range(args.self_play_index_from, args.self_play_index_from + args.self_play),
                ncols=100, desc=f'{index-1} vs {index-1}',
                file=sys.stdout,
            ):
                if index_another[i % len(index_another)] is not None:
                    _play_game_and_dump_log(
                        player, player_another[i % len(index_another)],
                        index, index_another[i % len(index_another)], i)
                else:
                    _self_play_and_dump_log(player, index, i)
        else:
            self_play_and_dump_logs_in_parallel(index, index_another, args.jobs)

    i = args.resume_rl_cycle_from
    if not os.path.isdir(f'datasets/dataset_{i:04d}'):
        os.makedirs(f'datasets/dataset_{i:04d}')

    args.another_player = [None] * (10 - len(args.another_player)) + args.another_player
    self_play_and_dump_logs(i, args.another_player)


def run_rl_cycle(args: Args):

    def get_best_past_player_against_latest(args: Args, index: int):
        player = load_player_of(index)
        indices_prev = list(range(index - 1, -1, -1))
        n = 10
        if len(indices_prev) > n:
            p = np.array(indices_prev) + 1
            p = p / np.sum(p)
            indices_prev = np.random.choice(indices_prev, size=n, replace=False, p=p)
            indices_prev = np.sort(indices_prev)[::-1]
        players_prev = [load_player_of(int(i)) for i in indices_prev]
        validation_result_list = []
        for p_prev in players_prev:
            record = vshogi.Record(0, 0, 0, 0, 0, 0)
            pbar = tqdm(range(args.validations), ncols=100)
            for n in pbar:
                if n % 2 == 0:
                    result = vshogi.play_game(
                        args._shogi.Game(),
                        player,
                        p_prev,
                        search_args={'n_or_t': args.az_search},
                        select_args={'temperature': None},
                        draw_on_max_moves=True,
                    ).result
                    record += vshogi.Record.from_black_result(result)
                else:
                    result = vshogi.play_game(
                        args._shogi.Game(),
                        p_prev,
                        player,
                        search_args={'n_or_t': args.az_search},
                        select_args={'temperature': None},
                        draw_on_max_moves=True,
                    ).result
                    record += vshogi.Record.from_white_result(result)
                pbar.set_description(f'{player.name} vs {p_prev.name} = {record.wdl()}')
            validation_result_list.append(record)
        win_point_list = [r.wins_total - r.losses_total for r in validation_result_list]
        indices_for_sort = np.argsort(win_point_list)
        indices_prev = np.asarray(indices_prev)[indices_for_sort]
        win_point_list = np.asarray(win_point_list)[indices_for_sort]
        print(f'Players in weak to strong order: {indices_prev}')
        return (
            indices_prev[:2].tolist()
            + indices_prev[2:][win_point_list[2:] <= 3].tolist()[:3]
        )

    with open('command.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')
    os.system(f"cp {__file__} ./")

    if args.resume_rl_cycle_from == 1:
        i = 0
        weight_path = 'models/model_{:04d}.pth'
        _train_step(
            model_path=weight_path.format(i),
            prev_model_path=None if i == 0 else weight_path.format(i - 1),
            shogi_variant=args.shogi_variant,
            network_hidden_channels=args.nn_hidden_channels,
            network_bottleneck_channels=args.nn_bottleneck_channels,
            network_backbone_blocks=args.nn_backbone_blocks,
            max_dataset_size=args.nn_dataset_size,
            kifu_path_pattern='datasets/dataset_*/kifu_*.tsv',
            kifu_fraction=args.nn_train_fraction,
            discount_factor=args.discount_factor,
            importance_decay=args.nn_data_importance_decay,
            minibatch_size=args.nn_minibatch,
            learning_rate=args.nn_learning_rate,
            epochs=args.nn_epochs,
            coeff_policy_loss=args.nn_coeff_policy_loss,
            coeff_entropy_regularization=args.nn_entropy_regularization,
            grad_accumulations=args.nn_grad_accum,
            win_ratio_threshold=args.win_ratio_threshold,
            device=args.nn_train_device,
            engine='AlphaZero',
        )

    for i in range(args.resume_rl_cycle_from, args.rl_cycle + 1):
        if i == 1:
            args._num_random_moves = np.inf
        else:
            line_length_list = []
            for path in glob(f'datasets/dataset_{i-1:04d}/kifu_*.tsv'):
                if 'vs' in path:
                    continue
                with open(path, 'rb') as f:
                    line_length_list.append(sum(1 for _ in f) - 1)
            n = np.mean(line_length_list) * args.az_random_rate
            args._num_random_moves = int(np.ceil(n / 2)) * 2

        if i >= 3:
            best_past_players = get_best_past_player_against_latest(args, i - 1)
        elif i == 2:
            best_past_players = [0]
        else:
            best_past_players = []
        print(f"Players to generate training data: {best_past_players}")

        while True:
            pattern = f'datasets/dataset_{i:04d}/*.tsv'
            self_play_index_from = len(glob(pattern))
            # Self-play!
            with open('errors.txt', 'a') as f:
                subprocess.call([
                    sys.executable, "dlshogi.py", "self-play", args.shogi_variant,
                    "--resume_rl_cycle_from", str(i),
                    "--self_play_index_from", str(self_play_index_from),
                    "--another_player", *[str(a) for a in best_past_players],
                ] + ' '.join([
                    f'--{k} {v}' for k, v in args.to_dict().items()
                    if (k not in (
                        'run', 'shogi_variant', "another_player",
                        'resume_rl_cycle_from', 'self_play_index_from',
                    ) and (v is not None))
                ]).split(), stderr=f)


            # Train NN!
            weight_path = 'models/model_{:04d}.pth'
            _train_step(
                model_path=weight_path.format(i),
                prev_model_path=None if i == 0 else weight_path.format(i - 1),
                shogi_variant=args.shogi_variant,
                network_hidden_channels=args.nn_hidden_channels,
                network_bottleneck_channels=args.nn_bottleneck_channels,
                network_backbone_blocks=args.nn_backbone_blocks,
                max_dataset_size=args.nn_dataset_size,
                kifu_path_pattern='datasets/dataset_*/kifu_*.tsv',
                kifu_fraction=args.nn_train_fraction,
                discount_factor=args.discount_factor,
                importance_decay=args.nn_data_importance_decay,
                minibatch_size=args.nn_minibatch,
                learning_rate=args.nn_learning_rate,
                epochs=args.nn_epochs,
                coeff_policy_loss=args.nn_coeff_policy_loss,
                coeff_entropy_regularization=args.nn_entropy_regularization,
                grad_accumulations=args.nn_grad_accum,
                win_ratio_threshold=args.win_ratio_threshold,
                device=args.nn_train_device,
                engine='AlphaZero',
            )
            if os.path.exists(f'models/model_{i:04d}.tflite'):
                break


def parse_args() -> Args:

    args = Args.from_args()
    args._shogi = getattr(vshogi, args.shogi_variant)
    default_configs = {
        'minishogi':     {'nn_hidden_channels':  64, 'nn_bottleneck_channels': 32, 'nn_backbone_blocks': 3},
        'judkins_shogi': {'nn_hidden_channels':  64, 'nn_bottleneck_channels': 32, 'nn_backbone_blocks': 4},
        'shogi':         {'nn_hidden_channels': 128, 'nn_bottleneck_channels': 64, 'nn_backbone_blocks': 6},
    }
    if args.shogi_variant in default_configs:
        for key, value in default_configs[args.shogi_variant].items():
            if getattr(args, key) is None:
                setattr(args, key, value)
    if args.output is None:
        args.output = args.shogi_variant

    if os.path.basename(os.getcwd()) != args.output:
        if not os.path.isdir(args.output):
            os.makedirs(args.output)
        os.chdir(args.output)
    if not os.path.isdir('models'):
        os.makedirs('models')
    if not os.path.isdir('datasets'):
        os.makedirs('datasets')

    i = args.resume_rl_cycle_from
    if i in (0, 1):
        args._num_random_moves = np.inf
    else:
        line_length_list = []
        for path in glob(f'datasets/dataset_{i-1:04d}/kifu_*.tsv'):
            if 'vs' in path:
                continue
            with open(path, 'rb') as f:
                line_length_list.append(sum(1 for _ in f) - 1)
        n = np.mean(line_length_list) * args.az_random_rate
        args._num_random_moves = int(np.ceil(n / 2)) * 2

    print(args)
    print(f"#random_moves={args._num_random_moves}")
    return args


if __name__ == '__main__':
    args = parse_args()

    if args.run == 'rl':
        run_rl_cycle(args)
    elif args.run == 'self-play':
        run_self_play(args)
