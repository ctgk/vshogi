"""Script to run DL-shogi training.

| index |        Self-play       |          Train NN         |        Validation       |
|-------|------------------------|---------------------------|-------------------------|
|   0   |          (skip)        | NN0 = Initialized         |          (skip)         |
|   1   | DATASET1  = NN0 vs NN0 | NN1 = Trained by DATASET1 | DATASET2 = NN1 vs NN0   |
|   2   | DATASET2 += NN1 vs NN1 | NN2 = Trained by DATASET2 | DATASET3 = NN2 vs NN1,0 |
|  ...  |          ...           |             ...           |           ...           |
"""

import contextlib
from glob import glob
import os
import sys
import typing as tp

os.environ['TF_CPP_MIN_LOG_LEVEL']='2'

from classopt import classopt, config
import numpy as np
import pandas as pd
import tensorflow as tf
from tqdm import tqdm

import vshogi


def build_policy_value_network(
    input_size: tp.Tuple[int, int],  # (H, W)
    input_channels: int,
    num_policy_per_square: int,
    num_channels_in_hidden_layer: int,
    num_backbone_layers: int,
    num_policy_layers: int,
    num_value_layers: int,
):
    r = tf.keras.regularizers.L2(0.001)

    def bottleneck_multidilation(x):
        h = tf.keras.layers.Conv2D(
            num_channels_in_hidden_layer // 4, 1, activation='relu6',
            kernel_regularizer=r)(x)
        h = tf.keras.layers.Concatenate()([
            tf.keras.layers.DepthwiseConv2D(
                3, dilation_rate=d, activation='relu6', padding='same',
                kernel_regularizer=r)(h)
            for d in range(1, min(input_size))
        ])
        h = tf.keras.layers.Conv2D(
            num_channels_in_hidden_layer, 1, activation='relu6',
            kernel_regularizer=r)(h)
        return h

    def bottleneck(x):
        h = tf.keras.layers.Conv2D(
            num_channels_in_hidden_layer // 4, 1, activation='relu6',
            kernel_regularizer=r)(x)
        h = tf.keras.layers.DepthwiseConv2D(
                3, activation='relu6', padding='same', kernel_regularizer=r)(h)
        h = tf.keras.layers.Conv2D(
            num_channels_in_hidden_layer, 1, activation='relu6',
            kernel_regularizer=r)(h)
        return h

    def resblock(x):
        h = bottleneck(x)
        return tf.keras.layers.Add()([x, h])

    def backbone_network(x):
        h = bottleneck_multidilation(x)
        for _ in range(num_backbone_layers - 1):
            h = resblock(h)
        return h

    def policy_network(h):
        for _ in range(num_policy_layers - 1):
            h = tf.keras.layers.Conv2D(
                num_channels_in_hidden_layer, 1,
                activation='relu6', kernel_regularizer=r,
            )(h)
        policy_logits = tf.keras.layers.Conv2D(
            num_policy_per_square, 1, kernel_regularizer=r,
        )(h)
        policy_logits = tf.keras.layers.Flatten(
            name='policy_logits',
        )(policy_logits)
        return policy_logits

    def value_network(h):
        for _ in range(num_value_layers - 1):
            h = tf.keras.layers.SeparableConv2D(
                num_channels_in_hidden_layer // 4, 3,
                activation='relu6', padding='valid', kernel_regularizer=r)(h)
        h = tf.keras.layers.Flatten()(h)
        value = tf.keras.layers.Dense(
            1, activation='tanh', name='value', kernel_regularizer=r)(h)
        return value

    x = tf.keras.Input(shape=(*input_size, input_channels))
    backbone_feature = backbone_network(x)
    policy_logits = policy_network(backbone_feature)
    value = value_network(backbone_feature)
    model = tf.keras.Model(inputs=x, outputs=[policy_logits, value])
    return model


class PolicyValueFunction:

    def __init__(
        self,
        model: tp.Union[tf.keras.Model, str],
        num_threads: int = 1,
    ) -> None:
        if isinstance(model, str):
            self._model_content = None
            self._interpreter = tf.lite.Interpreter(model_path=model, num_threads=num_threads)
        else:
            self._model_content = tf.lite.TFLiteConverter.from_keras_model(model).convert()
            self._interpreter = tf.lite.Interpreter(model_content=self._model_content, num_threads=num_threads)

        self._interpreter.allocate_tensors()
        input_details = self._interpreter.get_input_details()[0]
        self._input_placeholder = np.empty(input_details['shape'], dtype=np.float32)
        self._input_index = input_details['index']
        output_details = self._interpreter.get_output_details()
        self._value_index = output_details[0]['index']
        self._policy_index = output_details[1]['index']

    def __call__(self, game: vshogi.Game) -> tp.Tuple[np.ndarray, float]:
        game.to_dlshogi_features(out=self._input_placeholder)
        self._interpreter.set_tensor(self._input_index, self._input_placeholder)
        self._interpreter.invoke()
        value = self._interpreter.get_tensor(self._value_index).item()
        policy_logits = self._interpreter.get_tensor(self._policy_index)
        return policy_logits, value

    def save_model_as_tflite(self, output_path: str):
        if self._model_content is None:
            raise ValueError('Cannot save tflite binary.')
        with open(output_path, 'wb') as f:
            f.write(self._model_content)


def dump_game_records(file_, game: vshogi.Game) -> None:
    game.dump_records(
        (
            lambda g, i: g.get_sfen_at(i, include_move_count=True),
            lambda g, i: g.get_move_at(i),
            lambda g, _: g.result,
        ),
        names=('state', 'move', 'result'), file_=file_,
    )


def _get_generator_from_df(df: pd.DataFrame):
    value_options = {
        (vshogi.DRAW, vshogi.BLACK): 0.,
        (vshogi.DRAW, vshogi.WHITE): 0.,
        (vshogi.BLACK_WIN, vshogi.BLACK): 1.,
        (vshogi.BLACK_WIN, vshogi.WHITE): -1.,
        (vshogi.WHITE_WIN, vshogi.BLACK): -1.,
        (vshogi.WHITE_WIN, vshogi.WHITE): 1.,
    }

    def _generator():
        for _, row in df.sample(n=len(df), replace=False).iterrows():
            game = args._shogi.Game(row['state'])
            move = eval(
                row['move'].replace('Move', f'vshogi.{args.shogi_variant}.Move',
                    ).replace('dst=', f'dst=vshogi.{args.shogi_variant}.',
                    ).replace('src=', f'src=vshogi.{args.shogi_variant}.'))
            result = eval('vshogi.' + args.shogi_variant + '.' + row['result'])
            if np.random.uniform() > 0.5:
                game = game.hflip()
                move = move.hflip()

            x = game.to_dlshogi_features()
            policy = game.to_dlshogi_policy(move, args.nn_max_policy)
            value = value_options[(result, game.turn)]

            yield np.squeeze(x), (policy, value)

    return _generator


def get_dataset(tsv_list: tp.List[str]):
    df = pd.concat([pd.read_csv(p, sep='\t') for p in tsv_list], ignore_index=True)
    dataset = tf.data.Dataset.from_generator(
        _get_generator_from_df(df),
        output_types=(tf.float32, (tf.float32, tf.float32)),
    )
    dataset = dataset.batch(args.nn_minibatch)
    return dataset


def train_network(
    network: tf.keras.Model,
    dataset: tf.data.Dataset,
) -> tf.keras.Model:
    network.compile(
        loss=[
            tf.keras.losses.CategoricalCrossentropy(from_logits=True),
            tf.keras.losses.MeanSquaredError(),
        ],
        optimizer=tf.keras.optimizers.Adam(args.nn_learning_rate),
    )
    network.fit(
        dataset,
        epochs=args.nn_epochs,
    )
    return network


def load_data_and_train_network(network, index: int):
    dataset = get_dataset(glob(f'datasets/dataset_{index:04d}/*.tsv'))
    return train_network(network, dataset)


def play_game(
    game: vshogi.Game,
    player_black: vshogi.engine.MonteCarloTreeSearcher,
    player_white: vshogi.engine.MonteCarloTreeSearcher,
    max_moves: int = 400,
) -> vshogi.Game:
    """Make two players play the game until an end.

    Parameters
    ----------
    game : Game
        Game to make two players play
    player_black : MonteCarloTreeSearcher
        First player
    player_white : MonteCarloTreeSearcher
        Second player
    max_moves : int
        Maximum number of moves to apply to the game.
        If it reaches the value, return the game even if it is ongoing.

    Returns
    -------
    Game
        The game the two players played.
    """
    for _ in range(max_moves):
        if game.result != vshogi.Result.ONGOING:
            break

        mate_moves = game.get_mate_moves_if_any()
        if mate_moves is not None:
            for move in mate_moves:
                game.apply(move)
            break

        player = player_black if game.turn == vshogi.Color.BLACK else player_white
        player.set_root(game)
        player.explore(n=args.mcts_explorations - player.num_explored)
        move = player.select(
            temperature=(
                args.mcts_temperature(game) if callable(args.mcts_temperature) else args.mcts_temperature
            ),
        )
        game.apply(move)
    return game


def load_player_of(index_path_or_network, num_threads=1) -> vshogi.engine.MonteCarloTreeSearcher:
    if isinstance(index_path_or_network, int):
        i = index_path_or_network
        return vshogi.engine.MonteCarloTreeSearcher(
            PolicyValueFunction(f'models/model_{i:04d}.tflite', num_threads),
            coeff_puct=args.mcts_coeff_puct,
        )
    return vshogi.engine.MonteCarloTreeSearcher(
        PolicyValueFunction(index_path_or_network, num_threads),
        coeff_puct=args.mcts_coeff_puct,
    )


def _self_play_and_dump_record(player, index, nth_game: int) -> vshogi.Game:
    while True:
        game = play_game(args._game_getter(), player, player)
        if game.result != vshogi.ONGOING:
            break
    with open(f'datasets/dataset_{index:04d}/record_{nth_game:04d}.tsv', mode='w') as f:
        dump_game_records(f, game)


def self_play_and_dump_records_in_parallel(index: int, n_jobs: int):
    import joblib
    from joblib.parallel import Parallel, delayed

    @contextlib.contextmanager
    def tqdm_joblib(tqdm_object):
        """Context manager to patch joblib to report into tqdm progress bar given as argument"""
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

    def _self_play_and_dump_record_n_times(index, nth_game: list):
        player = load_player_of(index - 1)
        for i in nth_game:
            _self_play_and_dump_record(player, index, i)

    group_size = 10
    with tqdm_joblib(tqdm(total=args.self_play // group_size)):
        Parallel(n_jobs=n_jobs)(
            delayed(_self_play_and_dump_record_n_times)(
                index, list(range(i, i + group_size)),
            )
            for i in range(0, args.self_play, group_size)
        )


def self_play_and_dump_records(index: int):
    if args.jobs == 1:
        player = load_player_of(index - 1)
        for i in tqdm(range(args.self_play)):
            _self_play_and_dump_record(player, index, i)
    else:
        self_play_and_dump_records_in_parallel(index, args.jobs)


def play_against_past_players(index: int, dump_records: bool = False):
    player = load_player_of(index, args.jobs)
    for i_prev in range(index - 1, -1, -1):
        player_prev = load_player_of(i_prev, args.jobs)
        validation_results = {'win': 0, 'loss': 0, 'draw': 0}
        pbar = tqdm(range(args.validations))
        for n in pbar:
            if n % 2 == 0:
                while True:
                    game = play_game(args._game_getter(), player, player_prev)
                    if game.result != vshogi.ONGOING:
                        break
                validation_results[{
                    vshogi.BLACK_WIN: 'win',
                    vshogi.WHITE_WIN: 'loss',
                    vshogi.DRAW: 'draw',
                }[game.result]] += 1
                if dump_records:
                    path = f'datasets/dataset_{index + 1:04d}/record_B{index:02d}vsW{i_prev:02d}_{n:04d}.tsv'
                    with open(path, mode='w') as f:
                        dump_game_records(f, game)
            else:
                while True:
                    game = play_game(args._game_getter(), player_prev, player)
                    if game.result != vshogi.ONGOING:
                        break
                validation_results[{
                    vshogi.BLACK_WIN: 'loss',
                    vshogi.WHITE_WIN: 'win',
                    vshogi.DRAW: 'draw',
                }[game.result]] += 1
                if dump_records:
                    path = f'datasets/dataset_{index + 1:04d}/record_B{i_prev:02d}vsW{index:02d}_{n:04d}.tsv'
                    with open(path, mode='w') as f:
                        dump_game_records(f, game)
            pbar.set_description(f'{index} vs {i_prev}: {validation_results}')


def parse_args():

    @classopt(default_long=True)
    class Args:
        run: str = config(long=False, choices=['rl', 'self-play', 'validation'])
        shogi_variant: str = config(
            long=False,
            choices=['shogi', 'animal_shogi', 'judkins_shogi', 'minishogi'],
            help='Choose a variant of shogi to train!',
        )
        rl_cycle: int = config(type=int, default=10, help='# of Reinforcement Learning cycle. By default 10.')
        resume_rl_cycle_from: int = config(type=int, default=1, help='Resume Reinforcement Learning cycle if given. By default 0.')
        nn_channels: int = config(type=int, default=None, help='# of hidden channels in NN. Default value varies in shogi games.')
        nn_backbones: int = config(type=int, default=None, help='# of backbone layers in NN. Default value varies in shogi games.')
        nn_policy: int = config(type=int, default=1, help='# of layers for policy head in NN. By default 1.')
        nn_value: int = config(type=int, default=2, help='# of layers for value head in NN. By default 2.')
        nn_epochs: int = config(type=int, default=20, help='# of epochs in NN training. By default 20.')
        nn_minibatch: int = config(type=int, default=32, help='Minibatch size in NN training. By default 32.')
        nn_learning_rate: float = config(type=float, default=1e-4)
        nn_max_policy: float = config(type=float, default=0.8, help='Maximum value of supervised signal of policy in NN training, default=0.8')
        mcts_explorations: int = config(type=int, default=1000, help='# of explorations in MCTS, default=1000')
        mcts_coeff_puct: float = config(type=float, default=4., help='Coefficient of PUCT score in MCTS, default=4.')
        mcts_temperature: float = config(type=float, default=0.1, help='Temperature for selecting action in MCTS, default=0.1')
        self_play: int = config(type=int, default=200, help='# of self-play in one RL cycle, default=200')
        validations: int = config(type=int, default=10, help='# of validation plays per model, default=10')
        jobs: int = config(short=False, type=int, default=1, help='# of jobs to run self-play in parallel, default=1')
        output: str = config(short=True, type=str, help='Output path of self-play datasets and trained NN models, default=`shogi`')


    args = Args.from_args()
    args._game_getter = {
        'animal_shogi': lambda: vshogi.animal_shogi.Game(
            '{}l{}/1{}1/1{}1/{}L{} b - 1'.format(
                *np.random.choice(list('ceg'), 3, replace=False),
                *np.random.choice(list('CEG'), 3, replace=False),
            ),
        ),
        'minishogi': lambda: vshogi.minishogi.Game(
            '{}{}{}{}k/4{}/5/{}4/K{}{}{}{} b - 1'.format(
                *np.random.choice(list('rbsgp'), 5, replace=False),
                *np.random.choice(list('RBSGP'), 5, replace=False),
            ),
        ),
        'judkins_shogi': lambda: vshogi.judkins_shogi.Game(
            '{}{}{}{}{}k/5{}/6/6/{}5/K{}{}{}{}{} b - 1'.format(
                *np.random.choice(list('rbnsgp'), 6, replace=False),
                *np.random.choice(list('RBNSGP'), 6, replace=False),
            ),
        ),
        'shogi': lambda: vshogi.shogi.Game(
            '{}{}{}{}k{}{}{}{}/1{}5{}1/ppppppppp/9/9/9/'
            'PPPPPPPPP/1{}5{}1/{}{}{}{}K{}{}{}{} b - 1'.format(
                *np.random.choice(list('lnsggsnlrb'), 10, replace=False),
                *np.random.choice(list('BRLNSGGSNL'), 10, replace=False),
            ),
        ),
    }[args.shogi_variant]
    args._shogi = getattr(vshogi, args.shogi_variant)
    default_configs = {
        'animal_shogi':  {'nn_channels':  32, 'nn_backbones': 3},
        'minishogi':     {'nn_channels':  64, 'nn_backbones': 3},
        'judkins_shogi': {'nn_channels':  64, 'nn_backbones': 3},
        'shogi':         {'nn_channels': 128, 'nn_backbones': 4},
    }
    if args.shogi_variant in default_configs:
        for key, value in default_configs[args.shogi_variant].items():
            if getattr(args, key) is None:
                setattr(args, key, value)
    if args.output is None:
        args.output = args.shogi_variant

    print(args)
    return args


if __name__ == '__main__':
    import subprocess
    import sys

    args = parse_args()

    if os.path.basename(os.getcwd()) != args.output:
        if not os.path.isdir(args.output):
            os.makedirs(args.output)
        os.chdir(args.output)
    if not os.path.isdir('models'):
        os.makedirs('models')
    if not os.path.isdir('datasets'):
        os.makedirs('datasets')

    if args.run == 'rl':
        with open('command.txt', 'w') as f:
            f.write(f'python {" ".join(sys.argv)}')
        os.system(f"cp {__file__} ./")

    if args.run == 'self-play':
        i = args.resume_rl_cycle_from
        if not os.path.isdir(f'datasets/dataset_{i:04d}'):
            os.makedirs(f'datasets/dataset_{i:04d}')
        self_play_and_dump_records(i)
        sys.exit(0)
    elif args.run == 'validation':
        i = args.resume_rl_cycle_from
        if not os.path.isdir(f'datasets/dataset_{i + 1:04d}'):
            os.makedirs(f'datasets/dataset_{i + 1:04d}')
        play_against_past_players(i, dump_records=True)
        sys.exit(0)

    shogi = args._shogi

    network = build_policy_value_network(
        input_size=(shogi.Game.ranks, shogi.Game.files),
        input_channels=shogi.Game.feature_channels,
        num_policy_per_square=shogi.Move._num_policy_per_square(),
        num_channels_in_hidden_layer=args.nn_channels,
        num_backbone_layers=args.nn_backbones,
        num_policy_layers=args.nn_policy,
        num_value_layers=args.nn_value,
    )
    if args.resume_rl_cycle_from == 1:
        PolicyValueFunction(network).save_model_as_tflite(f'models/model_{0:04d}.tflite')

    for i in range(args.resume_rl_cycle_from, args.rl_cycle + 1):
        # Self-play!
        subprocess.call([
            sys.executable, "dlshogi.py", "self-play", args.shogi_variant,
            "--resume_rl_cycle_from", str(i),
            "--mcts_explorations", str(args.mcts_explorations),
            "--mcts_coeff_puct", str(args.mcts_coeff_puct),
            "--mcts_temperature", str(args.mcts_temperature),
            "--self_play", str(args.self_play),
            "--jobs", str(args.jobs),
            "--output", args.output,
        ])

        # Train NN!
        load_data_and_train_network(network, i)
        PolicyValueFunction(network).save_model_as_tflite(f'models/model_{i:04d}.tflite')

        # Validate!
        subprocess.call([
            sys.executable, "dlshogi.py", "validation", args.shogi_variant,
            "--resume_rl_cycle_from", str(i),
            "--mcts_explorations", str(args.mcts_explorations),
            "--mcts_coeff_puct", str(args.mcts_coeff_puct),
            "--mcts_temperature", str(args.mcts_temperature),
            "--validations", str(args.validations),
            "--jobs", str(args.jobs),
            "--output", args.output,
        ])
