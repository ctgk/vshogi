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

os.environ['TF_CPP_MIN_LOG_LEVEL']='3'

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
):
    r = tf.keras.regularizers.L2(0.001)

    def pointwise_conv2d(x, ch):
        return tf.keras.layers.Conv2D(
            ch, 1, use_bias=False, kernel_regularizer=r)(x)

    def bn(x):
        return tf.keras.layers.BatchNormalization(center=False, scale=False)(x)

    def depthwise_multidilation_conv2d(x, dilations: tp.Iterable[int]):
        return tf.keras.layers.Concatenate()([
            tf.keras.layers.DepthwiseConv2D(
                3, dilation_rate=d, padding='same',
                use_bias=False, kernel_regularizer=r)(x)
            for d in dilations
        ])

    def bottleneck(x):
        h = pointwise_conv2d(x, num_channels_in_hidden_layer // 4)
        h = bn(h)
        h = tf.nn.relu6(h)

        h = depthwise_multidilation_conv2d(h, range(1, min(input_size)))
        h = bn(h)
        h = tf.nn.relu6(h)

        h = pointwise_conv2d(h, num_channels_in_hidden_layer)
        h = bn(h)
        return h

    def resblock(x):
        h = bottleneck(x)
        h = tf.keras.layers.Add()([x, h])
        return tf.nn.relu6(h)

    def backbone_network(x):
        h = tf.nn.relu6(bottleneck(x))
        for _ in range(num_backbone_layers - 1):
            h = resblock(h)
        return h

    def policy_network(x):
        h = pointwise_conv2d(x, num_policy_per_square)
        return tf.keras.layers.Flatten(name='policy_logits')(h)

    def value_network(x):
        h = pointwise_conv2d(x, num_channels_in_hidden_layer // 4)
        h = bn(h)
        h = tf.nn.relu6(h)

        h = tf.keras.layers.Flatten()(h)
        return tf.keras.layers.Dense(
            1, activation='tanh', name='value', kernel_regularizer=r)(h)

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
            lambda g, i: g.get_move_at(i).to_usi(),
            lambda g, _: g.result,
            lambda g, i: g.visit_counts_record[i],
        ),
        names=('state', 'move', 'result', 'visit_counts'), file_=file_,
    )


def _get_generator_from_df(df: pd.DataFrame):
    value_options = {
        (vshogi.DRAW, vshogi.BLACK): 0.,
        (vshogi.DRAW, vshogi.WHITE): 0.,
        (vshogi.ONGOING, vshogi.BLACK): 0.,
        (vshogi.ONGOING, vshogi.WHITE): 0.,
        (vshogi.BLACK_WIN, vshogi.BLACK): 1.,
        (vshogi.BLACK_WIN, vshogi.WHITE): -1.,
        (vshogi.WHITE_WIN, vshogi.BLACK): -1.,
        (vshogi.WHITE_WIN, vshogi.WHITE): 1.,
    }

    def _generator():
        for _, row in df.sample(n=len(df), replace=False).iterrows():
            game = args._shogi.Game(row['state'])
            visit_counts = {args._shogi.Move(k): v for k, v in eval(row['visit_counts']).items()}
            result = eval('vshogi.' + args.shogi_variant + '.' + row['result'])
            if np.random.uniform() > 0.5:
                game = game.hflip()
                visit_counts = {k.hflip(): v for k, v in visit_counts.items()}

            x = game.to_dlshogi_features()
            policy = game.to_dlshogi_policy(visit_counts, default_value=np.nan)
            value = value_options[(result, game.turn)]

            yield np.squeeze(x), (policy, value)

    return _generator


def get_dataset(df: pd.DataFrame):
    dataset = tf.data.Dataset.from_generator(
        _get_generator_from_df(df),
        output_types=(tf.float32, (tf.float32, tf.float32)),
    )
    dataset = dataset.batch(args.nn_minibatch)
    return dataset


def train_network(
    network: tf.keras.Model,
    dataset: tf.data.Dataset,
    learning_rate: float,
) -> tf.keras.Model:

    def masked_softmax_cross_entropy(y_true, y_pred):
        mask = tf.math.is_finite(y_true)
        y_true_masked = tf.where(mask, y_true, 0)
        y_pred = y_pred - tf.reduce_max(y_pred, axis=1, keepdims=True)
        logsumexp = tf.math.log(tf.reduce_sum(
            tf.where(mask, tf.math.exp(y_pred), 0), axis=1, keepdims=True))
        y_pred = y_pred - logsumexp
        return -tf.reduce_sum(y_true_masked * y_pred, axis=1)

    network.compile(
        loss=[
            # tf.keras.losses.CategoricalCrossentropy(from_logits=True),
            masked_softmax_cross_entropy,
            tf.keras.losses.MeanSquaredError(),
        ],
        optimizer=tf.keras.optimizers.Adam(learning_rate),
    )
    network.fit(
        dataset,
        epochs=args.nn_epochs,
    )
    return network


def read_kifu(tsv_path: str, fraction: float = None) -> pd.DataFrame:
    df = pd.read_csv(tsv_path, sep='\t')
    if fraction is None:
        return df
    n = int(len(df) * fraction)
    return df.tail(n)


def load_data_and_train_network(network, index: int, learning_rate: float):
    df = pd.concat([
        pd.concat([read_kifu(p, f) for p in glob(f'datasets/dataset_{i:04d}/*.tsv')], ignore_index=True)
        for i, f in zip(range(index, 0, -1), (args.nn_train_fraction ** i for i in range(index)))
    ], ignore_index=True)
    dataset = get_dataset(df)
    return train_network(network, dataset, learning_rate)


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
    game.visit_counts_record = []
    for _ in range(max_moves):
        if game.result != vshogi.Result.ONGOING:
            break

        mate_moves = game.get_mate_moves_if_any(num_dfpn_nodes=10000)
        if mate_moves is not None:
            for move in mate_moves:
                game.visit_counts_record.append({
                    m.to_usi(): int(m == move) for m in game.get_legal_moves()
                })
                game.apply(move)
            break

        player = player_black if game.turn == vshogi.Color.BLACK else player_white
        if not player.is_ready():
            player.set_root(game)
        player.explore(
            n=args.mcts_explorations - player.num_explored,
            num_dfpn_nodes=100, # approximately worth three-move mate
        )
        visit_counts = player.get_visit_counts()
        move = player.select(temperature='max') # off-policy
        game.apply(move)
        player_black.apply(move)
        if player_white is not player_black:
            player_white.apply(move)
        game.visit_counts_record.append({k.to_usi(): v for k, v in visit_counts.items()})
    player_black.clear()
    player_white.clear()
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
    with open(f'datasets/dataset_{index:04d}/record_{nth_game:05d}.tsv', mode='w') as f:
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

    group_size = 5
    with tqdm_joblib(tqdm(total=args.self_play // group_size, ncols=100)):
        Parallel(n_jobs=n_jobs)(
            delayed(_self_play_and_dump_record_n_times)(
                index, list(range(i, i + group_size)),
            )
            for i in range(args.self_play_index_from, args.self_play_index_from + args.self_play, group_size)
        )


def self_play_and_dump_records(index: int):
    if args.jobs == 1:
        player = load_player_of(index - 1)
        for i in tqdm(range(args.self_play_index_from, args.self_play_index_from + args.self_play), ncols=100):
            _self_play_and_dump_record(player, index, i)
    else:
        self_play_and_dump_records_in_parallel(index, args.jobs)


def get_best_player_index(current: int, best: int):
    results = {'win': 0, 'loss': 0, 'draw': 0}
    player_curr = load_player_of(current, args.jobs)
    player_best = load_player_of(best, args.jobs)
    num_play = 40
    win_threshold = num_play * args.win_ratio_threshold
    loss_threshold = num_play * (1 - args.win_ratio_threshold)
    pbar = tqdm(range(num_play), ncols=100)
    for n in pbar:
        if (results['win'] > win_threshold) or (results['loss'] > loss_threshold):
            break
        if n % 2 == 0:
            game_init = args._game_getter()
            game = play_game(game_init.copy(), player_curr, player_best)
            results[{
                vshogi.BLACK_WIN: 'win',
                vshogi.WHITE_WIN: 'loss',
                vshogi.DRAW: 'draw',
                vshogi.ONGOING: 'draw',
            }[game.result]] += 1
        else:
            game = play_game(game_init.copy(), player_best, player_curr)
            results[{
                vshogi.BLACK_WIN: 'loss',
                vshogi.WHITE_WIN: 'win',
                vshogi.DRAW: 'draw',
                vshogi.ONGOING: 'draw',
            }[game.result]] += 1
        pbar.set_description(f'{current} vs {best}: {results}')
    return current if results['win'] > win_threshold else best


def play_against_past_players(index: int, dump_records: bool = False):
    player = load_player_of(index, args.jobs)
    indices_prev = list(range(index - 1, -1, -1))
    n = 10
    if len(indices_prev) > n:
        p = np.array(indices_prev) + 1
        p = p / np.sum(p)
        indices_prev = np.random.choice(indices_prev, size=n, replace=False, p=p)
        indices_prev = np.sort(indices_prev)[::-1]
    for i_prev in indices_prev:
        player_prev = load_player_of(int(i_prev), args.jobs)
        validation_results = {'win': 0, 'loss': 0, 'draw': 0}
        pbar = tqdm(range(args.validations), ncols=100)
        for n in pbar:
            if n % 2 == 0:
                game_init = args._game_getter()
                game = play_game(game_init.copy(), player, player_prev)
                validation_results[{
                    vshogi.BLACK_WIN: 'win',
                    vshogi.WHITE_WIN: 'loss',
                    vshogi.DRAW: 'draw',
                    vshogi.ONGOING: 'draw',
                }[game.result]] += 1
                if dump_records:
                    path = f'datasets/dataset_{index + 1:04d}/record_B{index:02d}vsW{i_prev:02d}_{n:04d}.tsv'
                    with open(path, mode='w') as f:
                        dump_game_records(f, game)
                    os.system(f'cat {path} | grep -e " b " -e "state" > tmp.tsv; mv tmp.tsv {path}')
            else:
                game = play_game(game_init.copy(), player_prev, player)
                validation_results[{
                    vshogi.BLACK_WIN: 'loss',
                    vshogi.WHITE_WIN: 'win',
                    vshogi.DRAW: 'draw',
                    vshogi.ONGOING: 'draw',
                }[game.result]] += 1
                if dump_records:
                    path = f'datasets/dataset_{index + 1:04d}/record_B{i_prev:02d}vsW{index:02d}_{n:04d}.tsv'
                    with open(path, mode='w') as f:
                        dump_game_records(f, game)
                    os.system(f'cat {path} | grep -e " w " -e "state" > tmp.tsv; mv tmp.tsv {path}')
            pbar.set_description(f'{index} vs {i_prev}: {validation_results}')


def parse_args():

    @classopt(default_long=True)
    class Args:
        run: str = config(long=False, choices=['rl', 'self-play', 'train', 'validation'])
        shogi_variant: str = config(
            long=False,
            choices=['shogi', 'animal_shogi', 'judkins_shogi', 'minishogi'],
            help='Choose a variant of shogi to train!',
        )
        rl_cycle: int = config(type=int, default=10, help='# of Reinforcement Learning cycle. By default 10.')
        resume_rl_cycle_from: int = config(type=int, default=1, help='Resume Reinforcement Learning cycle if given. By default 0.')
        nn_channels: int = config(type=int, default=None, help='# of hidden channels in NN. Default value varies in shogi games.')
        nn_backbones: int = config(type=int, default=None, help='# of backbone layers in NN. Default value varies in shogi games.')
        nn_train_fraction: float = config(type=float, default=0.5, help='Fraction of game record by former models to use to train current one. By default 0.5')
        nn_epochs: int = config(type=int, default=20, help='# of epochs in NN training. By default 20.')
        nn_minibatch: int = config(type=int, default=32, help='Minibatch size in NN training. By default 32.')
        nn_learning_rate: float = config(type=float, default=1e-3, help='Learning rate of NN weight update')
        mcts_explorations: int = config(type=int, default=1000, help='# of explorations in MCTS, default=1000. Alpha Zero used 800 simulations.')
        mcts_coeff_puct: float = config(type=float, default=4., help='Coefficient of PUCT score in MCTS, default=4.')
        self_play: int = config(type=int, default=200, help='# of self-play in one RL cycle, default=200')
        self_play_index_from: int = config(type=int, default=0, help='Index to start self-play from, default=0')
        win_ratio_threshold: float = config(type=float, default=0.55, help='Threshold of win ratio to adopt new model against previous one, default=0.55')
        validations: int = config(type=int, default=10, help='# of validation plays per model, default=10')
        jobs: int = config(short=False, type=int, default=1, help='# of jobs to run self-play in parallel, default=1')
        output: str = config(short=True, type=str, help='Output path of self-play datasets and trained NN models, default=`shogi`')


    def shogi_game_getter():
        g = vshogi.shogi.Game()
        first_moves = g.get_legal_moves()
        g.apply(first_moves[0])
        second_moves = g.get_legal_moves()

        def get_random_game():
            if np.random.uniform() < 0.1:
                return vshogi.shogi.Game()
            m1 = np.random.choice(first_moves)
            m2 = np.random.choice(second_moves)
            sfen = vshogi.shogi.Game().apply(m1).apply(m2).to_sfen(include_move_count=False)
            return vshogi.shogi.Game(sfen=sfen)

        return get_random_game


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
        'shogi': shogi_game_getter(),
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
    elif args.run == 'train':
        shogi = args._shogi
        network = build_policy_value_network(
            input_size=(shogi.Game.ranks, shogi.Game.files),
            input_channels=shogi.Game.feature_channels,
            num_policy_per_square=shogi.Move._num_policy_per_square(),
            num_channels_in_hidden_layer=args.nn_channels,
            num_backbone_layers=args.nn_backbones,
        )
        i = args.resume_rl_cycle_from
        if i > 1:
            network.load_weights(f'models/checkpoint_{i-1:04d}/checkpoint_{i-1:04d}').expect_partial()
        if i > 0:
            load_data_and_train_network(network, i, args.nn_learning_rate)
            network.save_weights(f'models/checkpoint_{i:04d}/checkpoint_{i:04d}')
        PolicyValueFunction(network).save_model_as_tflite(f'models/model_{i:04d}.tflite')
        sys.exit(0)
    elif args.run == 'validation':
        i = args.resume_rl_cycle_from
        if not os.path.isdir(f'datasets/dataset_{i + 1:04d}'):
            os.makedirs(f'datasets/dataset_{i + 1:04d}')
        play_against_past_players(i, dump_records=True)
        sys.exit(0)

    if args.resume_rl_cycle_from == 1:
        shogi = args._shogi
        subprocess.call([
            sys.executable, "dlshogi.py", "train", args.shogi_variant,
            "--resume_rl_cycle_from", str(0),
            "--nn_channels", str(args.nn_channels),
            "--nn_backbones", str(args.nn_backbones),
            "--output", args.output,
        ])

    for i in range(args.resume_rl_cycle_from, args.rl_cycle + 1):
        learning_rate = args.nn_learning_rate
        while True:
            pattern = f'datasets/dataset_{i:04d}/*.tsv'
            self_play_index_from = len([p for p in glob(pattern) if 'vs' not in p])
            # Self-play!
            subprocess.call([
                sys.executable, "dlshogi.py", "self-play", args.shogi_variant,
                "--resume_rl_cycle_from", str(i),
                "--mcts_explorations", str(args.mcts_explorations),
                "--mcts_coeff_puct", str(args.mcts_coeff_puct),
                "--self_play", str(args.self_play),
                "--self_play_index_from", str(self_play_index_from),
                "--jobs", str(args.jobs),
                "--output", args.output,
            ])

            # Train NN!
            subprocess.call([
                sys.executable, "dlshogi.py", "train", args.shogi_variant,
                "--resume_rl_cycle_from", str(i),
                "--nn_channels", str(args.nn_channels),
                "--nn_backbones", str(args.nn_backbones),
                "--nn_train_fraction", str(args.nn_train_fraction),
                "--nn_epochs", str(args.nn_epochs),
                "--nn_minibatch", str(args.nn_minibatch),
                "--nn_learning_rate", str(learning_rate),
                "--output", args.output,
            ])

            if (i == 1) or (get_best_player_index(i, i - 1) == i):
                break
            else:
                self_play_index_from += args.self_play

        # Validate!
        subprocess.call([
            sys.executable, "dlshogi.py", "validation", args.shogi_variant,
            "--resume_rl_cycle_from", str(i),
            "--mcts_explorations", str(args.mcts_explorations),
            "--mcts_coeff_puct", str(args.mcts_coeff_puct),
            "--validations", str(args.validations),
            "--jobs", str(args.jobs),
            "--output", args.output,
        ])
