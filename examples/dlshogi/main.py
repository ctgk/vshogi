# flake8: noqa

import contextlib
from glob import glob
import os
import typing as tp

os.environ['TF_CPP_MIN_LOG_LEVEL']='2'

import joblib
from joblib import Parallel, delayed
import numpy as np
import pandas as pd
import tensorflow as tf
from tqdm import tqdm

import vshogi

from vshogi.animal_shogi import *
NUM_CHANNELS = 32
NUM_BACKBONE_LAYERS = 3
NUM_POLICY_LAYERS = 2
NUM_VALUE_LAYERS = 2
NUM_EXPLORATIONS = 1000
COEFF_PUCT = 4.
NUM_SELF_PLAY = 200
NUM_RANDOM_MOVES = 0
NUM_VALIDATION_PLAYS_PER_MODEL = 20
MAX_POLICY = 0.5
MINIBATCH = 32
EPOCHS = 20
NUM_SELF_PLAY_VALIDATE_TRAIN_CYCLE = 20
NUM_JOBS = 1
GET_SFEN = lambda: '{}l{}/1{}1/1{}1/{}L{} b - 1'.format(
    *np.random.choice(list('ceg'), 3, replace=False),
    *np.random.choice(list('CEG'), 3, replace=False))

# from vshogi.minishogi import *
# NUM_CHANNELS = 32
# NUM_BACKBONE_LAYERS = 6
# NUM_POLICY_LAYERS = 3
# NUM_VALUE_LAYERS = 3
# NUM_EXPLORATIONS = 1000
# COEFF_PUCT = 4.
# NUM_SELF_PLAY = 200
# NUM_RANDOM_MOVES = 0
# NUM_VALIDATION_PLAYS_PER_MODEL = 20
# MAX_POLICY = 1.0
# MINIBATCH = 32
# EPOCHS = 20
# NUM_SELF_PLAY_VALIDATE_TRAIN_CYCLE = 50
# NUM_JOBS = 4
# GET_SFEN = lambda: '{}{}{}{}k/4{}/5/{}4/K{}{}{}{} b - 1'.format(
#     *np.random.choice(list('rbsgp'), 5, replace=False),
#     *np.random.choice(list('RBSGP'), 5, replace=False))


def build_policy_value_network(
    input_size: tp.Tuple[int, int] = (Game.ranks, Game.files),  # (H, W)
    input_channels: int = Game.feature_channels,
    num_policy_per_square: int = Move._num_policy_per_square(),
    num_channels_in_hidden_layer: int = NUM_CHANNELS,
    num_backbone_layers: int = NUM_BACKBONE_LAYERS,
    num_policy_layers: int = NUM_POLICY_LAYERS,
    num_value_layers: int = NUM_VALUE_LAYERS,
    l2_coeff: float = 0.001,
):
    r = tf.keras.regularizers.L2(l2_coeff)
    x = tf.keras.Input(shape=(*input_size, input_channels))
    backbone_feature = x
    for _ in range(num_backbone_layers):
        backbone_feature = tf.keras.layers.Conv2D(
            num_channels_in_hidden_layer, 3,
            activation='relu',
            padding='same',
            kernel_regularizer=r,
        )(backbone_feature)

    h = backbone_feature
    for _ in range(num_policy_layers - 1):
        h = tf.keras.layers.Conv2D(
            num_channels_in_hidden_layer, 3,
            activation='relu',
            padding='same',
            kernel_regularizer=r,
        )(h)
    policy_logits = tf.keras.layers.Conv2D(
        num_policy_per_square, 1,
        kernel_regularizer=r,
    )(h)
    policy_logits = tf.keras.layers.Flatten(
        name='policy_logits',
    )(policy_logits)

    h = backbone_feature
    for _ in range(num_value_layers - 1):
        h = tf.keras.layers.Conv2D(
            num_channels_in_hidden_layer, 3,
            activation='relu',
            padding='valid',
            kernel_regularizer=r,
        )(h)
    h = tf.keras.layers.Flatten()(h)
    value = tf.keras.layers.Dense(
        1,
        activation='tanh',
        name='value',
        kernel_regularizer=r,
    )(h)

    model = tf.keras.Model(inputs=x, outputs=[policy_logits, value])
    return model


class PolicyValueFunction:

    def __init__(
        self,
        model: tp.Union[tf.keras.Model, str],
    ) -> None:
        if isinstance(model, str):
            model = tf.lite.TFLiteConverter.from_saved_model(model).convert()
        else:
            model = tf.lite.TFLiteConverter.from_keras_model(model).convert()

        self._model = tf.lite.Interpreter(model_content=model)
        self._model.allocate_tensors()
        self._input_details = self._model.get_input_details()
        self._output_details = self._model.get_output_details()

    def __call__(self, game: vshogi.Game) -> tp.Tuple[dict, float]:
        if game.result != vshogi.Result.ONGOING:
            return {}, (0. if game.result == vshogi.Result.DRAW else -1.)

        x = np.asarray(game)
        self._model.set_tensor(self._input_details[0]['index'], x)
        self._model.invoke()
        value = float(self._model.get_tensor(self._output_details[0]['index']))
        value = np.clip(value, -0.99, 0.99)
        policy_logits = self._model.get_tensor(self._output_details[1]['index'])
        policy = game.to_policy_probas(policy_logits)
        return policy, value


def dump_game_records(file_, game: vshogi.Game) -> None:
    print("state\tmove\tresult", file=file_)
    for i in range(game.record_length):
        print(
            game.get_sfen_at(i, include_move_count=True),
            game.get_move_at(i),
            game.result,
            file=file_, sep='\t',
        )


def _df_to_xy(df: pd.DataFrame, max_policy: float = MAX_POLICY):
    x_list = []
    policy_list = []
    value_list = []
    num_policies = Game.ranks * Game.files * Move._num_policy_per_square()
    non_max_policy = (1 - max_policy) / (num_policies - 1)
    value_options = {
        (vshogi.DRAW, vshogi.BLACK): 0.,
        (vshogi.DRAW, vshogi.WHITE): 0.,
        (vshogi.BLACK_WIN, vshogi.BLACK): 1.,
        (vshogi.BLACK_WIN, vshogi.WHITE): -1.,
        (vshogi.WHITE_WIN, vshogi.BLACK): -1.,
        (vshogi.WHITE_WIN, vshogi.WHITE): 1.,
    }
    for _, row in df.iterrows():
        if int(row['state'].split(' ')[-1]) <= 2 * NUM_RANDOM_MOVES:
            continue
        game = Game(row['state'])
        move = eval(row['move'])
        result = eval(row['result'])

        x = np.asarray(game)
        policy = game.to_dlshogi_policy(move, max_policy)
        value = value_options[(result, game.turn)]

        x_list.append(x)
        policy_list.append(policy)
        value_list.append(value)
    return (
        np.concatenate(x_list, axis=0),
        np.asarray(policy_list, dtype=np.float32),
        np.asarray(value_list, dtype=np.float32),
    )


def tsv_to_xy(tsv_list: tp.List[str]):
    return _df_to_xy(pd.concat(
        [pd.read_csv(p, sep='\t') for p in tsv_list], ignore_index=True))


def train_network(
    network: tf.keras.Model,
    x_train: np.ndarray,
    y_policy_train: np.ndarray,
    y_value_train: np.ndarray,
    minibatch: int = MINIBATCH,
    epochs: int = EPOCHS,
    output_path: tp.Optional[str] = None,
) -> tf.keras.Model:
    network.compile(
        loss=[
            tf.keras.losses.CategoricalCrossentropy(from_logits=True),
            tf.keras.losses.MeanSquaredError(),
        ],
        optimizer=tf.keras.optimizers.Adam(),
    )
    network.fit(
        x_train,
        [y_policy_train, y_value_train],
        batch_size=minibatch,
        epochs=epochs,
    )
    if output_path is not None:
        network.save(output_path)
    return network


def load_data_and_train_network(network, index: int):
    x, y_policy, y_value = tsv_to_xy(
        glob(f'datasets/dataset_{index-1:04d}/*.tsv'))
    return train_network(
        network, x, y_policy, y_value, output_path=f'models/model_{index:04d}')


def play_game(
    game: Game,
    player_black: vshogi.engine.MonteCarloTreeSearcher,
    player_white: vshogi.engine.MonteCarloTreeSearcher,
    temperature: tp.Union[tp.Callable[[Game], float], float, str] = 'max',
    max_moves: int = 400,
) -> Game:
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
    player_black.clear()
    player_white.clear()
    for _ in range(max_moves):
        if game.result != Result.ONGOING:
            break
        player = player_black if game.turn == Color.BLACK else player_white
        if not player.is_ready():
            player.set_root(game)
        player.explore(n=NUM_EXPLORATIONS - player.num_explored)
        move = player.select(
            temperature=(
                temperature(game) if callable(temperature) else temperature
            ),
        )
        game.apply(move)
        player_black.apply(move)
        if player_white is not player_black:
            player_white.apply(move)
    player_black.clear()
    player_white.clear()
    return game


def load_player_of(index_or_network) -> vshogi.engine.MonteCarloTreeSearcher:
    if isinstance(index_or_network, int):
        network = tf.keras.models.load_model(f'models/model_{index_or_network:04d}', compile=False)
    else:
        network = index_or_network
    return vshogi.engine.MonteCarloTreeSearcher(PolicyValueFunction(network), coeff_puct=COEFF_PUCT)


def _self_play_and_dump_record(network_or_player, index, nth_game: int) -> vshogi.Game:
    if isinstance(network_or_player, vshogi.engine.MonteCarloTreeSearcher):
        player = network_or_player
    else:
        player = load_player_of(network)
    while True:
        sfen = GET_SFEN()
        game = play_game(
            Game(sfen), player, player,
            temperature=lambda g: 0.01 if g.record_length >= NUM_RANDOM_MOVES else 100,
        )
        if game.result != vshogi.ONGOING:
            break
    with open(f'datasets/dataset_{index:04d}/record_{nth_game:04d}.tsv', mode='w') as f:
        dump_game_records(f, game)


def self_play_and_dump_records(player, index: int, n: int = NUM_SELF_PLAY):
    for i in tqdm(range(n)):
        _self_play_and_dump_record(player, index, i)


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


def self_play_and_dump_records_in_parallel(
    network,
    index: int,
    num_self_play: int = NUM_SELF_PLAY,
    n_jobs: int = NUM_JOBS,
):

    def _self_play_and_dump_record_n_times(network, index, nth_game: list):
        player = load_player_of(network)
        for i in nth_game:
            _self_play_and_dump_record(player, index, i)

    group_size = 10
    with tqdm_joblib(tqdm(total=num_self_play // group_size)):
        Parallel(n_jobs=n_jobs)(
            delayed(_self_play_and_dump_record_n_times)(
                network, index, list(range(i, i + group_size)),
            )
            for i in range(0, num_self_play, group_size)
        )


def play_against_past_players(player, index: int, dump_records: bool = False):
    for i_prev in range(index - 1, -1, -1):
        player_prev = load_player_of(i_prev)
        validation_results = {'win': 0, 'loss': 0, 'draw': 0}
        pbar = tqdm(range(NUM_VALIDATION_PLAYS_PER_MODEL))
        for n in pbar:
            if n % 2 == 0:
                while True:
                    sfen = GET_SFEN()
                    game = play_game(
                        Game(sfen), player, player_prev,
                        temperature=lambda g: 0.01 if g.record_length >= NUM_RANDOM_MOVES else 100,
                    )
                    if game.result != vshogi.ONGOING:
                        break
                validation_results[{
                    vshogi.BLACK_WIN: 'win',
                    vshogi.WHITE_WIN: 'loss',
                    vshogi.DRAW: 'draw',
                }[game.result]] += 1
                if dump_records:
                    path = f'datasets/dataset_{index:04d}/record_B{index:02d}vsW{i_prev:02d}_{n:04d}.tsv'
                    with open(path, mode='w') as f:
                        dump_game_records(f, game)
            else:
                while True:
                    sfen = GET_SFEN()
                    game = play_game(
                        Game(sfen), player_prev, player,
                        temperature=lambda g: 0.01 if g.record_length >= NUM_RANDOM_MOVES else 100,
                    )
                    if game.result != vshogi.ONGOING:
                        break
                validation_results[{
                    vshogi.BLACK_WIN: 'loss',
                    vshogi.WHITE_WIN: 'win',
                    vshogi.DRAW: 'draw',
                }[game.result]] += 1
                if dump_records:
                    path = f'datasets/dataset_{index:04d}/record_B{i_prev:02d}vsW{index:02d}_{n:04d}.tsv'
                    with open(path, mode='w') as f:
                        dump_game_records(f, game)
            pbar.set_description(f'{index} vs {i_prev}: {validation_results}')


if __name__ == '__main__':
    for i in range(NUM_SELF_PLAY_VALIDATE_TRAIN_CYCLE):
        # Initialize network or train!
        if i == 0:
            network = build_policy_value_network()
            network.save('models/model_0000')
            player = load_player_of(network)
        elif 'network' not in locals():
            network = tf.keras.models.load_model(f'models/model_{i-1:04d}')
            network = load_data_and_train_network(network, i)
            player = load_player_of(network)
        else:
            network = load_data_and_train_network(network, i)
            player = load_player_of(network)

        # Self-play!
        if not os.path.isdir(f'datasets/dataset_{i:04d}'):
            os.makedirs(f'datasets/dataset_{i:04d}')
        if NUM_JOBS == 1:
            self_play_and_dump_records(player, i)
        else:
            self_play_and_dump_records_in_parallel(network, i, n_jobs=NUM_JOBS)

        # Validate!
        play_against_past_players(player, i, dump_records=True)
