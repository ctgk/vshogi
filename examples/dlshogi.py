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
import random
import subprocess
import sys
import typing as tp

os.environ['TF_CPP_MIN_LOG_LEVEL']='3'
os.environ['TF_USE_LEGACY_KERAS']='1'

from classopt import classopt, config
import joblib
from joblib.parallel import Parallel, delayed
import numpy as np
import pandas as pd
import tensorflow as tf
from tqdm import tqdm

import vshogi


@classopt(default_long=True)
class Args:
    run: str = config(long=False, choices=['rl', 'self-play', 'train'])
    shogi_variant: str = config(
        long=False,
        choices=['shogi', 'animal_shogi', 'judkins_shogi', 'minishogi'],
        help='Choose a variant of shogi to train!',
    )
    rl_cycle: int = config(type=int, default=10, help='# of Reinforcement Learning cycle. By default 10.')
    resume_rl_cycle_from: int = config(type=int, default=1, help='Resume Reinforcement Learning cycle if given. By default 0.')
    nn_hidden_channels: int = config(type=int, default=None, help='# of hidden channels in NN. Default value varies in shogi games.')
    nn_bottleneck_channels: int = config(type=int, default=None, help='# of bottleneck channels in NN. Default value varies in shogi games.')
    nn_backbone_blocks: int = config(type=int, default=None, help='# of backbone res-blocks in NN. Default value varies in shogi games.')
    nn_train_fraction: float = config(type=float, default=0.5, help='Fraction of game record by former models to use to train current one. By default 0.5')
    nn_epochs: int = config(type=int, default=10, help='# of epochs in NN training. By default 10.')
    nn_minibatch: int = config(type=int, default=32, help='Minibatch size in NN training. By default 32.')
    nn_learning_rate: float = config(type=float, default=1e-3, help='Learning rate of NN weight update')
    mcts_kldgain_threshold: float = config(type=float, default=1e-4, help='KL divergence threshold to stop MCT-search')
    mcts_explorations: int = config(type=int, default=1000, help='# of explorations in MCTS, default=1000. Alpha Zero used 800 simulations.')
    mcts_random_rate: float = config(
        type=float, default=0.25,
        help=(
            'Select action by random sample from distribution by MCTS '
            'for first `r * average_moves_until_game_end` moves. '
            'The rest of the actions are obtained by selecting the mode of the distribution.'
        ),
    )
    mcts_temperature: float = config(type=float, default=1., help='Temperature parameter when selecting action by random.')
    mcts_coeff_puct: float = config(type=float, default=4., help='Coefficient of PUCT score in MCTS, default=4.')
    self_play: int = config(type=int, default=200, help='# of self-play in one RL cycle, default=200')
    self_play_index_from: int = config(type=int, default=0, help='Index to start self-play from, default=0')
    another_player: list = config(type=int, nargs='*', default=[])
    validations: int = config(type=int, default=10, help='# of validation plays per model, default=10')
    win_ratio_threshold: float = config(type=float, default=0.55, help='Threshold of win ratio to adopt new model against previous one, default=0.55')
    jobs: int = config(short=False, type=int, default=1, help='# of jobs to run self-play in parallel, default=1')
    output: str = config(short=True, type=str, help='Output path of self-play datasets and trained NN models, default=`shogi`')


def build_policy_value_network(
    input_size: tp.Tuple[int, int],  # (H, W)
    input_channels: int,
    num_policy_per_square: int,
    num_channels_in_hidden_layer: int,
    num_channels_in_bottleneck: int,
    use_long_range_concat: bool,
    num_backbone_blocks: int,
):
    r = tf.keras.regularizers.L2(0.001)

    class Concat8Directions(tf.keras.layers.Layer):

        def __init__(self, max_dilation_rate: int):
            super().__init__()
            self._max_dilation_rate = max_dilation_rate

        def build(self, input_shape):
            assert input_shape[-1] == 8, input_shape[-1]
            k = np.concatenate((np.eye(9, 4), np.eye(9, 4, k=-5)), axis=-1).reshape(3, 3, 8, 1)
            self.kernel = tf.constant(k, dtype=tf.float32)

        def call(self, x):
            feature_maps = [
                tf.nn.depthwise_conv2d(
                    x, self.kernel, (1, 1, 1, 1), 'SAME', dilations=(d, d))
                for d in range(1, self._max_dilation_rate + 1)
            ]
            return tf.concat([x] + feature_maps, -1)

    def pointwise_conv2d(x, ch, use_bias=True):
        return tf.keras.layers.Conv2D(
            ch, 1, use_bias=use_bias, kernel_regularizer=r)(x)

    def depthwise_conv2d(x, dilation=1, use_bias=True):
        return tf.keras.layers.DepthwiseConv2D(
            3, dilation_rate=dilation, padding='same',
            use_bias=use_bias, depthwise_regularizer=r)(x)

    def bn(x):
        return tf.keras.layers.BatchNormalization(center=False, scale=False)(x)

    def relu6(x):
        return tf.keras.layers.ReLU(max_value=6)(x)

    def relu_pconv(x, ch):
        return relu6(pointwise_conv2d(x, ch))

    def multidilation_resblock(x):
        h = Concat8Directions(max(input_size) - 1)(relu_pconv(x, 8))
        h = pointwise_conv2d(h, num_channels_in_hidden_layer, use_bias=False)
        return relu6(bn(x + h))

    def resblock(x):
        h = relu_pconv(x, num_channels_in_bottleneck)
        h = relu6(depthwise_conv2d(h))
        h = pointwise_conv2d(h, num_channels_in_hidden_layer, use_bias=False)
        return relu6(bn(x + h))

    def backbone_network(x):
        h = relu_pconv(x, num_channels_in_hidden_layer)
        h = relu6(bn(depthwise_conv2d(h, use_bias=False)))

        for _ in range(num_backbone_blocks):
            if use_long_range_concat:
                h = multidilation_resblock(h)
            else:
                h = resblock(h)
        return h

    def policy_network(x):
        h = pointwise_conv2d(x, num_policy_per_square)
        return tf.keras.layers.Flatten(name='policy_logits')(h)

    def value_network(x):
        h = relu_pconv(x, 1)
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
            converter = tf.lite.TFLiteConverter.from_keras_model(model)
            self._model_content = converter.convert()
            self._interpreter = tf.lite.Interpreter(
                model_content=self._model_content, num_threads=num_threads)

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


def dump_game_records(file_, game: vshogi.Game, color_filter: vshogi.Color = None) -> None:
    game.dump_records(
        (
            lambda g, i: g.get_sfen_at(i, include_move_count=True),
            lambda g, i: g.get_move_at(i).to_usi(),
            lambda g, _: g.result,
            lambda g, i: g.v_value_record[i],
            lambda g, i: g.q_value_record[i],
            lambda g, i: g.visit_count_record[i],
        ),
        names=('state', 'move', 'result', 'v_value', 'q_value', 'visit_count'),
        file_=file_,
        color_filter=color_filter,
    )


def dump_game_records_and_convert_to_tfrecord(
    kifu_path: str,
    game: vshogi.Game,
    args: Args,
    color_filter: vshogi.Color = None,
) -> None:
    with open(kifu_path, 'w') as f:
        dump_game_records(f, game, color_filter)
    kifu_to_tfrecord(kifu_path.replace('.tsv', '.tfrecord'), kifu_path, args)


def play_game(
    player_black: vshogi.engine.DfpnMcts,
    player_white: vshogi.engine.DfpnMcts,
    args: Args,
    max_moves: int = 400,
) -> vshogi.Game:
    """Make two players play the game until an end.

    Parameters
    ----------
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
    game = args._shogi.Game()
    game.v_value_record = []
    game.q_value_record = []
    game.visit_count_record = []
    for _ in range(max_moves):
        if game.result != vshogi.Result.ONGOING:
            break

        player = player_black if game.turn == vshogi.Color.BLACK else player_white
        if not player.is_ready():
            player.set_game(game)

        player.search(
            dfpn_searches_at_root=10000,
            mcts_searches=args.mcts_explorations - player.mcts_num_searched,
            dfpn_searches_at_vertex=100,  # approximately worth three-move mate
            kldgain_threshold=args.mcts_kldgain_threshold,
        )
        if player.dfpn_found_mate:
            mate_moves = player.get_mate_moves()
            for i, move in enumerate(mate_moves):
                player.search(dfpn_searches_at_root=0, mcts_searches=1, dfpn_searches_at_vertex=0)
                game.v_value_record.append(player.get_value())
                q_value = int(i % 2 == 0) * 2 - 1
                game.q_value_record.append(q_value)
                legal_moves = game.get_legal_moves()
                if i % 2 == 0:
                    game.visit_count_record.append({
                        m.to_usi(): 10 * len(legal_moves) if m == move else 1
                        for m in legal_moves
                    })
                else:
                    game.visit_count_record.append({
                        m.to_usi(): 1 for m in legal_moves
                    })
                game.apply(move)
                player.apply(move)
            break

        if game.record_length < args._num_random_moves:
            move = player.select(temperature=args.mcts_temperature)
        else:
            move = player.select()

        visit_count = {
            m.to_usi(): v + 1  # +1 for smoothing
            for m, v in player.get_visit_counts(include_random=False).items()
        }
        game.v_value_record.append(player.get_value())
        game.q_value_record.append(max(player.get_q_values().values()))
        game.visit_count_record.append(visit_count)

        game.apply(move)
        player_black.apply(move)
        if player_white is not player_black:
            player_white.apply(move)

    player_black.clear()
    player_white.clear()
    return game


def load_player_of(index_path_or_network, num_threads=1) -> vshogi.engine.DfpnMcts:
    if isinstance(index_path_or_network, int):
        i = index_path_or_network
        mcts = vshogi.engine.MonteCarloTreeSearcher(
            PolicyValueFunction(f'models/model_{i:04d}.tflite', num_threads),
            coeff_puct=args.mcts_coeff_puct,
        )
    else:
        mcts = vshogi.engine.MonteCarloTreeSearcher(
            PolicyValueFunction(index_path_or_network, num_threads),
            coeff_puct=args.mcts_coeff_puct,
        )
    return vshogi.engine.DfpnMcts(vshogi.engine.DfpnSearcher(), mcts)


def play_game_and_dump_record(
    black,
    white,
    args: Args,
    index: int,
    suffix: str,
    color_filter: vshogi.Color = None,
) -> vshogi.Result:
    while True:
        game = play_game(black, white, args)
        if game.result != vshogi.ONGOING:
            break
    if (index is not None) and (suffix is not None):
        path = f'datasets/dataset_{index:04d}/record_{suffix}.tsv'
        dump_game_records_and_convert_to_tfrecord(path, game, args, color_filter)
    return game.result


def read_kifu(tsv_path: str, fraction: float = None) -> pd.DataFrame:
    df = pd.read_csv(
        tsv_path, sep='\t',
        usecols=['state', 'result', 'q_value', 'visit_count'],
        dtype={'state': str, 'result': str, 'q_value': float, 'visit_count': str},
    )
    if fraction is None:
        return df
    n = int(len(df) * fraction)
    return df.tail(n)


def kifu_to_tfrecord(
    tfrecord_path: str,
    kifu_path: str,
    args: Args,
    fraction: float = None,
):
    with tf.io.TFRecordWriter(tfrecord_path) as writer:
        df = read_kifu(kifu_path, fraction=fraction)
        for i in range(len(df)):
            row = df.iloc[i]
            state = args._shogi.State(row.state)
            visit_count = {args._shogi.Move(k): v for k, v in eval(row.visit_count).items()}
            s = sum(visit_count.values())
            visit_proba = {m: v / s for m, v in visit_count.items()}
            z_value = 0 if ('DRAW' in row.result) else 2 * int(('BLACK' in row.result) == (' b ' in row.state)) - 1
            value = 0.5 * (z_value + row.q_value)

            x = state.to_dlshogi_features()
            policy = state.to_dlshogi_policy(visit_proba, default_value=-100000.)
            record_bytes = tf.train.Example(features=tf.train.Features(feature={
                'x': tf.train.Feature(float_list=tf.train.FloatList(value=x.ravel())),
                'policy': tf.train.Feature(float_list=tf.train.FloatList(value=policy.ravel())),
                'value': tf.train.Feature(float_list=tf.train.FloatList(value=[value])),
            })).SerializeToString()
            writer.write(record_bytes)

            state = state.hflip()
            visit_proba = {m.hflip(): p for m, p in visit_proba.items()}
            x = state.to_dlshogi_features()
            policy = state.to_dlshogi_policy(visit_proba, default_value=-100000.)
            record_bytes = tf.train.Example(features=tf.train.Features(feature={
                'x': tf.train.Feature(float_list=tf.train.FloatList(value=x.ravel())),
                'policy': tf.train.Feature(float_list=tf.train.FloatList(value=policy.ravel())),
                'value': tf.train.Feature(float_list=tf.train.FloatList(value=[value])),
            })).SerializeToString()
            writer.write(record_bytes)


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

    def _self_play_and_dump_record(player, index, nth_game: int):
        play_game_and_dump_record(player, player, args, index, f'{nth_game:05d}')

    def _play_game_and_dump_record(player, player_another, index, index_another, nth_game: int):
        if (nth_game // 10) % 2 == 0:
            play_game_and_dump_record(player, player_another, args, index, f'{nth_game:05d}_B{index-1:02d}vsW{index_another:02d}', vshogi.BLACK)
        else:
            play_game_and_dump_record(player_another, player, args, index, f'{nth_game:05d}_B{index_another:02d}vsW{index-1:02d}', vshogi.WHITE)

    def self_play_and_dump_records_in_parallel(index: int, index_another: int, n_jobs: int):

        def _self_play_and_dump_record_n_times(index, index_another, nth_game: list):
            player = load_player_of(index - 1)
            for i in nth_game:
                if index_another[i % len(index_another)] is not None:
                    player_another = load_player_of(index_another[i % len(index_another)])
                    _play_game_and_dump_record(player, player_another, index, index_another[i % len(index_another)], i)
                else:
                    _self_play_and_dump_record(player, index, i)

        group_size = 5
        with tqdm_joblib(tqdm(total=args.self_play // group_size, ncols=100)):
            Parallel(n_jobs=n_jobs)(
                delayed(_self_play_and_dump_record_n_times)(
                    index, index_another, list(range(i, i + group_size)),
                )
                for i in range(args.self_play_index_from, args.self_play_index_from + args.self_play, group_size)
            )


    def self_play_and_dump_records(index: int, index_another: tp.List[int]):
        if args.jobs == 1:
            player = load_player_of(index - 1)
            player_another = [None if i is None else load_player_of(i) for i in index_another]
            for i in tqdm(range(args.self_play_index_from, args.self_play_index_from + args.self_play), ncols=100):
                if index_another[i % len(index_another)] is not None:
                    _play_game_and_dump_record(
                        player, player_another[i % len(index_another)],
                        index, index_another[i % len(index_another)], i)
                else:
                    _self_play_and_dump_record(player, index, i)
        else:
            self_play_and_dump_records_in_parallel(index, index_another, args.jobs)

    i = args.resume_rl_cycle_from
    if not os.path.isdir(f'datasets/dataset_{i:04d}'):
        os.makedirs(f'datasets/dataset_{i:04d}')

    args.another_player = [None] * (10 - len(args.another_player)) + args.another_player
    self_play_and_dump_records(i, args.another_player)


def run_train(args: Args):

    def get_dataset_from_tfrecord(path_list: tp.List[str]):

        def parse_example(example):
            x_shape = [args._shogi.Game.ranks, args._shogi.Game.files, args._shogi.Game.feature_channels]
            p_shape = [args._shogi.Game.num_dlshogi_policy]
            features = tf.io.parse_example(example, {
                'x': tf.io.FixedLenFeature(x_shape, dtype=tf.float32),
                'policy': tf.io.FixedLenFeature(p_shape, dtype=tf.float32),
                'value': tf.io.FixedLenFeature([1], dtype=tf.float32),
            })
            return features['x'], (features['policy'], features['value'])

        dataset = tf.data.Dataset.from_tensor_slices(path_list).shuffle(
            buffer_size=10000,
            reshuffle_each_iteration=True,
        )
        dataset = dataset.interleave(lambda filename: tf.data.TFRecordDataset(filename))
        dataset = dataset.shuffle(
            buffer_size=10000,
            reshuffle_each_iteration=True,
        )
        dataset = dataset.batch(args.nn_minibatch)
        dataset = dataset.map(parse_example)
        dataset = dataset.prefetch(2)
        return dataset

    def train_network(
        network: tf.keras.Model,
        dataset: tf.data.Dataset,
        learning_rate: float,
    ) -> tf.keras.Model:

        def masked_softmax_cross_entropy(y_true, logit):
            # https://github.com/tensorflow/tensorflow/issues/24476
            # In order to make this function work in CPU,
            # the following consists without using `tf.where()`

            y_true_masked = tf.clip_by_value(y_true, 0., 1.)
            logit_masked = logit + tf.clip_by_value(y_true, -100000., 0.)  # masked out values should be -100000 here.

            logit_max = tf.stop_gradient(tf.reduce_max(logit_masked, axis=1, keepdims=True))
            # tf.debugging.assert_all_finite(logit_max, message="max(logit) should be finite")
            logit_subtracted = logit_masked - logit_max  # masked out values should be -100000 here.
            # tf.debugging.assert_near(tf.reduce_max(logit_subtracted, axis=1), 0., rtol=0., atol=0.1, message="`max(logit - max(logit))` should be near 0")
            logsumexp = tf.reduce_logsumexp(logit_subtracted, axis=1, keepdims=True)
            log_softmax = logit_subtracted - logsumexp
            return -tf.reduce_sum(y_true_masked * log_softmax, axis=1)

        def lr_scheduler(epoch):
            # relative learning schedule from 1.0 to 0.1
            schedule = np.cos(np.linspace(0, np.pi, args.nn_epochs)) * 0.45 + 0.55
            return args.nn_learning_rate * schedule[epoch]

        network.compile(
            loss=[
                masked_softmax_cross_entropy,
                tf.keras.losses.MeanSquaredError(),
            ],
            optimizer=tf.keras.optimizers.Adam(learning_rate),
        )
        network.fit(
            dataset,
            epochs=args.nn_epochs,
            callbacks=[tf.keras.callbacks.LearningRateScheduler(lr_scheduler)],
        )
        return network

    def load_data_and_train_network(network, index: int, learning_rate: float):
        num_tfrecord_max = 100000
        tfrecord_list = []
        for i in range(index, 0, -1):
            list_ = [p for p in glob(f'datasets/dataset_{i:04d}/*.tfrecord') if os.path.getsize(p) != 0]
            random.shuffle(list_)
            tfrecord_list.extend(list_)
            if len(tfrecord_list) > num_tfrecord_max:
                break
        if len(tfrecord_list) > num_tfrecord_max:
            tfrecord_list = tfrecord_list[:num_tfrecord_max]
        random.shuffle(tfrecord_list)
        print(f'#tfrecord = {len(tfrecord_list):,}')
        dataset = get_dataset_from_tfrecord(tfrecord_list)
        return train_network(network, dataset, learning_rate)

    shogi = args._shogi
    network = build_policy_value_network(
        input_size=(shogi.Game.ranks, shogi.Game.files),
        input_channels=shogi.Game.feature_channels,
        num_policy_per_square=shogi.Move._num_policy_per_square(),
        num_channels_in_hidden_layer=args.nn_hidden_channels,
        num_channels_in_bottleneck=args.nn_bottleneck_channels,
        use_long_range_concat=(args.shogi_variant != "animal_shogi"),
        num_backbone_blocks=args.nn_backbone_blocks,
    )
    i = args.resume_rl_cycle_from
    if i > 1:
        if os.path.exists(f'models/checkpoint_{i:04d}'):
            print(f"Loading checkpoint_{i:04d}")
            network.load_weights(f'models/checkpoint_{i:04d}/checkpoint_{i:04d}').expect_partial()
        else:
            print(f"Loading checkpoint_{i-1:04d}")
            network.load_weights(f'models/checkpoint_{i-1:04d}/checkpoint_{i-1:04d}').expect_partial()
    if i > 0:
        load_data_and_train_network(network, i, args.nn_learning_rate)
        network.save_weights(f'models/checkpoint_{i:04d}/checkpoint_{i:04d}')
    PolicyValueFunction(network).save_model_as_tflite(f'models/model_{i:04d}.tflite')


def run_rl_cycle(args: Args):

    def get_best_past_player_against_latest(args: Args, index: int):
        player = load_player_of(index, args.jobs)
        indices_prev = list(range(index - 1, -1, -1))
        n = 10
        if len(indices_prev) > n:
            p = np.array(indices_prev) + 1
            p = p / np.sum(p)
            indices_prev = np.random.choice(indices_prev, size=n, replace=False, p=p)
            indices_prev = np.sort(indices_prev)[::-1]
        validation_result_list = []
        for i_prev in indices_prev:
            player_prev = load_player_of(int(i_prev), args.jobs)
            validation_results = {'win': 0, 'loss': 0, 'draw': 0}
            pbar = tqdm(range(args.validations), ncols=100)
            for n in pbar:
                if n % 2 == 0:
                    result = play_game_and_dump_record(player, player_prev, args, None, None)
                    validation_results[{
                        vshogi.BLACK_WIN: 'win',
                        vshogi.WHITE_WIN: 'loss',
                        vshogi.DRAW: 'draw',
                        vshogi.ONGOING: 'draw',
                    }[result]] += 1
                else:
                    result = play_game_and_dump_record(player_prev, player, args, None, None)
                    validation_results[{
                        vshogi.BLACK_WIN: 'loss',
                        vshogi.WHITE_WIN: 'win',
                        vshogi.DRAW: 'draw',
                        vshogi.ONGOING: 'draw',
                    }[result]] += 1
                pbar.set_description(f'{index} vs {i_prev}: {validation_results}')
            validation_result_list.append(validation_results)
        win_point_list = [r['win'] - r['loss'] for r in validation_result_list]
        indices_for_sort = np.argsort(win_point_list)
        indices_prev = np.asarray(indices_prev)[indices_for_sort]
        win_point_list = np.asarray(win_point_list)[indices_for_sort]
        print(f'Players in weak to strong order: {indices_prev}')
        return (
            indices_prev[:2].tolist()
            + indices_prev[2:][win_point_list[2:] <= 3].tolist()[:3]
        )

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
                game = play_game(player_curr, player_best, args)
                results[{
                    vshogi.BLACK_WIN: 'win',
                    vshogi.WHITE_WIN: 'loss',
                    vshogi.DRAW: 'draw',
                    vshogi.ONGOING: 'draw',
                }[game.result]] += 1
            else:
                game = play_game(player_best, player_curr, args)
                results[{
                    vshogi.BLACK_WIN: 'loss',
                    vshogi.WHITE_WIN: 'win',
                    vshogi.DRAW: 'draw',
                    vshogi.ONGOING: 'draw',
                }[game.result]] += 1
            pbar.set_description(f'{current} vs {best}: {results}')
        return current if results['win'] > win_threshold else best

    def keep_only_end_games_in_previous_tfrecord(index: int, args: Args):
        for i, f in zip(range(index, 0, -1), (args.nn_train_fraction ** i for i in range(index))):
            if i == index:
                continue
            kifu_list = glob(f'datasets/dataset_{i:04d}/*.tsv')
            if args.jobs == 1:
                for kifu_path in tqdm(kifu_list, desc=f'Dataset_{i:04d}', ncols=100):
                    kifu_to_tfrecord(kifu_path.replace('.tsv', '.tfrecord'), kifu_path, args, f)
            else:
                with tqdm_joblib(tqdm(total=len(kifu_list), desc=f'Dataset_{i:04d}', ncols=100)):
                    Parallel(n_jobs=args.jobs)(
                        delayed(kifu_to_tfrecord)(p.replace('.tsv', '.tfrecord'), p, args, f)
                        for p in kifu_list
                    )

    with open('command.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')
    os.system(f"cp {__file__} ./")

    if args.resume_rl_cycle_from == 1:
        subprocess.call([
            sys.executable, "dlshogi.py", "train", args.shogi_variant,
            "--resume_rl_cycle_from", str(0),
        ] + ' '.join([
            f'--{k} {v}' for k, v in args.to_dict().items()
            if (
                (k not in ('run', 'shogi_variant', 'resume_rl_cycle_from', 'another_player'))
                and (v is not None)
            )
        ]).split())

    for i in range(args.resume_rl_cycle_from, args.rl_cycle + 1):
        if i == 1:
            args._num_random_moves = np.inf
        else:
            line_length_list = []
            for path in glob(f'datasets/dataset_{i-1:04d}/record_*.tsv'):
                if 'vs' in path:
                    continue
                with open(path, 'rb') as f:
                    line_length_list.append(sum(1 for _ in f) - 1)
            n = np.mean(line_length_list) * args.mcts_random_rate
            args._num_random_moves = int(np.ceil(n / 2)) * 2

        if i > 1:
            keep_only_end_games_in_previous_tfrecord(i, args)

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
            ]).split())

            # Train NN!
            subprocess.call([
                sys.executable, "dlshogi.py", "train", args.shogi_variant,
                "--resume_rl_cycle_from", str(i),
            ] + ' '.join([
                f'--{k} {v}' for k, v in args.to_dict().items()
                if (
                    (k not in ('run', 'shogi_variant', 'resume_rl_cycle_from', 'another_player'))
                    and (v is not None)
                )
            ]).split())

            if (i == 1) or (get_best_player_index(i, i - 1) == i):
                break


def parse_args() -> Args:

    args = Args.from_args()
    args._shogi = getattr(vshogi, args.shogi_variant)
    default_configs = {
        'animal_shogi':  {'nn_hidden_channels':  32, 'nn_bottleneck_channels':  8, 'nn_backbone_blocks': 3},
        'minishogi':     {'nn_hidden_channels':  64, 'nn_bottleneck_channels': 16, 'nn_backbone_blocks': 3},
        'judkins_shogi': {'nn_hidden_channels':  64, 'nn_bottleneck_channels': 16, 'nn_backbone_blocks': 4},
        'shogi':         {'nn_hidden_channels': 128, 'nn_bottleneck_channels': 32, 'nn_backbone_blocks': 6},
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
        for path in glob(f'datasets/dataset_{i-1:04d}/record_*.tsv'):
            if 'vs' in path:
                continue
            with open(path, 'rb') as f:
                line_length_list.append(sum(1 for _ in f) - 1)
        n = np.mean(line_length_list) * args.mcts_random_rate
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
    elif args.run == 'train':
        run_train(args)
