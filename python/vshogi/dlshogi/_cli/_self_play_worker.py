import contextlib
import os
import typing as tp
from glob import glob

import click as cl
import joblib
import numpy as np
from joblib.parallel import Parallel, delayed
from tqdm import tqdm

import vshogi as vs


def _dump_game_log(file_, game: vs.Game) -> None:
    r = str(game.result)
    if 'ONGOING' in r:
        raise ValueError('The game is still ongoing')
    game.dump_log(
        (
            lambda g, i: g.get_sfen_at(i, include_move_count=False),
            lambda g, i: g.get_move_at(i).to_sfen(),
            lambda _, i: (
                0 if 'DRAW' in r
                else (2 * (('BLACK' in r) is (i % 2 == 0)) - 1)
            ),
            lambda g, i: g.q_value_log[i],
            lambda g, i: g.visit_count_log[i],
            lambda g, i: g.z_weight_log[i],
        ),
        names=('sfen', 'move', 'result', 'q_value', 'policy', 'z_weight'),
        file_=file_,
    )


def _play_game(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    black: vs.engine.AlphaZero,
    white: vs.engine.AlphaZero,
    num_simulations: int,
    temperature: float,
    main: tp.Optional[vs.engine.AlphaZero] = None,
    q_greedy_depth: int = 1,
    max_random_moves: int = 320,
    max_moves: int = 320,
) -> vs.Game:
    shogi_module = getattr(vs, shogi_variant)
    game_class = getattr(shogi_module, 'Game')
    game = game_class()
    game.q_value_log = []
    game.visit_count_log = []
    game.z_weight_log = []

    num_random_moves = (
        np.random.choice(max_random_moves + 1)
        if np.isfinite(max_random_moves) else max_random_moves
    )
    for _ in range(max_moves):
        if game.result != vs.Result.ONGOING:
            break

        player = black if game.turn == vs.Color.BLACK else white
        if not player.is_ready():
            player.set_game(game)

        player.search(num_simulations - player.get_search_count())
        if (main is not None) and (main is not player):
            if not main.is_ready():
                main.set_game(game)
            main.search(num_simulations - main.get_search_count())

        if player.proved_mate():
            if player.get_q_value() > 0:
                mate_moves = game.get_mate_moves_if_any(
                    player._dfpn_search_root)
                if mate_moves is not None:
                    for i, m in enumerate(mate_moves):
                        game.apply(m)
                        game.q_value_log.append(1. if i % 2 == 0 else -1.)
                        game.visit_count_log.append({})
                        game.z_weight_log.append(0.)
                    if game.result != vs.Result.ONGOING:
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
            move = player.select(temperature=temperature)
            game.z_weight_log.append(0.)
        else:
            move = player.select()
            game.z_weight_log.append(0.5 if (main is None) else 0.)
        if (hash(move) == 0):
            raise ValueError(
                f"Invalid move ({move}) selected at the game, "
                f"{game.to_sfen()}.\n{player._tree(depth=2)}")

        player_dump = main or player
        visit_count = {
            m.to_sfen(): v + 1  # +1 for smoothing
            for m, v in
            player_dump.get_visit_counts(include_random=False).items()
        }
        game.q_value_log.append(
            player_dump.get_q_value(greedy_depth=q_greedy_depth))
        game.visit_count_log.append(visit_count)

        game.apply(move)
        black.apply(move)
        if white is not black:
            white.apply(move)

    if game.result == vs.Result.ONGOING:
        game.declare_draw()

    black.clear()
    white.clear()
    return game


def _play_game_and_dump_log(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    player_black: vs.engine.Engine,
    player_white: vs.engine.Engine,
    filepath: str,
    main_player: vs.engine.Engine | None,
    num_simulations: int,
    temperature: float,
    q_greedy_depth: int,
    max_random_moves: int,
) -> None:
    game = _play_game(
        shogi_variant,
        player_black,
        player_white,
        num_simulations,
        temperature,
        main_player,
        q_greedy_depth=q_greedy_depth,
        max_random_moves=max_random_moves,
    )
    with open(filepath, 'w') as f:
        _dump_game_log(f, game)


@contextlib.contextmanager
def _tqdm_joblib(tqdm_object):

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


def _load_player(
    tflite_path: str | None,
    coeff_puct: float,
    kldgain_threshold: float,
    dfpn_search_root: int,
    dfpn_search_leaf: int,
) -> vs.engine.AlphaZero:
    return vs.engine.AlphaZero(
        (
            (
                lambda g: (
                    np.zeros(g.num_dlshogi_policy, dtype=np.float32),
                    vs.engine.piece_value_func(g),
                )
            )
            if tflite_path is None else
            vs.dlshogi.PolicyValueFunction(tflite_path)
        ),
        coeff_puct=coeff_puct,
        kldgain_threshold=kldgain_threshold,
        dfpn_search_root=dfpn_search_root,
        dfpn_search_leaf=dfpn_search_leaf,
        name=(
            'none' if tflite_path is None else
            tflite_path.split('/')[-1].split('.')[0]
        ),
    )


def _run_self_play_single(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    tflite_path: str,
    tflite_path_others: tp.List[str],
    kifu_dir: str,
    kifu_index_range: tp.Iterable[int],
    coeff_puct: float,
    kldgain_threshold: float,
    dfpn_search_root: int,
    dfpn_search_leaf: int,
    num_simulations: int,
    temperature: float,
    q_greedy_depth: int,
    max_random_moves: int,
    show_pbar: bool = True,
):
    player = _load_player(
        tflite_path=tflite_path,
        coeff_puct=coeff_puct,
        kldgain_threshold=kldgain_threshold,
        dfpn_search_root=dfpn_search_root,
        dfpn_search_leaf=dfpn_search_leaf,
    )
    player_others = [player for _ in range(10 - len(tflite_path_others))] + [
        _load_player(
            tflite_path=path,
            coeff_puct=coeff_puct,
            kldgain_threshold=kldgain_threshold,
            dfpn_search_root=dfpn_search_root,
            dfpn_search_leaf=dfpn_search_leaf,
        )
        for path in tflite_path_others
    ]
    iterator = kifu_index_range
    if show_pbar:
        iterator = tqdm(
            iterator,
            ncols=100,
            desc=f'{player.name} vs {player.name}',
        )
    for i in iterator:
        suffix = ''
        player_black = player
        player_white = player
        if (i // 10) % 2 == 0:
            player_white = player_others[i % len(player_others)]
            if (player_white is not player):
                suffix = f'_W{player_white.name}'
        else:
            player_black = player_others[i % len(player_others)]
            if (player_black is not player):
                suffix = f'_B{player_black.name}'
        _play_game_and_dump_log(
            shogi_variant=shogi_variant,
            player_black=player_black,
            player_white=player_white,
            filepath=kifu_dir + '/' + f'kifu_{i:05d}{suffix}.tsv',
            main_player=player,
            num_simulations=num_simulations,
            temperature=temperature,
            q_greedy_depth=q_greedy_depth,
            max_random_moves=max_random_moves,
        )


def _run_self_play_parallel(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    tflite_path: str,
    tflite_path_others: tp.List[str],
    kifu_dir: str,
    kifu_index_groups: tp.Iterable[tp.Iterable[int]],
    coeff_puct: float,
    kldgain_threshold: float,
    dfpn_search_root: int,
    dfpn_search_leaf: int,
    num_simulations: int,
    temperature: float,
    q_greedy_depth: int,
    max_random_moves: int,
    n_jobs: int,
):
    name = tflite_path.split('/')[-1].split('.')[0]
    with _tqdm_joblib(
        tqdm(
            total=len(kifu_index_groups),
            ncols=100,
            desc=f'{name} vs {name}',
        ),
    ):
        Parallel(n_jobs=n_jobs)(
            delayed(_run_self_play_single)(
                shogi_variant,
                tflite_path,
                tflite_path_others,
                kifu_dir,
                indices,
                coeff_puct,
                kldgain_threshold,
                dfpn_search_root,
                dfpn_search_leaf,
                num_simulations,
                temperature,
                q_greedy_depth,
                max_random_moves,
                show_pbar=False,
            )
            for indices in kifu_index_groups
        )


def _run_self_play(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    tflite_path: str,
    tflite_path_others: tp.List[str],
    kifu_dir: str,
    num_selfplay: int,
    coeff_puct: float,
    kldgain_threshold: float,
    dfpn_search_root: int,
    dfpn_search_leaf: int,
    num_simulations: int,
    temperature: float,
    q_greedy_depth: int,
    max_random_moves: int,
    n_jobs: int,
    job_size: int = 5,
):
    print(
        f'Self-play ({tflite_path.split("/")[-1].split(".")[0]}) '
        + 'with others: '
        + ', '.join(
            p.split('/')[-1].split('.')[0]
            for p in tflite_path_others
        ),
    )
    if not os.path.isdir(kifu_dir):
        os.makedirs(kifu_dir)
    index_start = len(glob(os.path.join(kifu_dir, 'kifu_*.tsv')))
    if n_jobs <= 1:
        _run_self_play_single(
            shogi_variant=shogi_variant,
            tflite_path=tflite_path,
            tflite_path_others=tflite_path_others,
            kifu_dir=kifu_dir,
            kifu_index_range=range(index_start, index_start + num_selfplay),
            coeff_puct=coeff_puct,
            kldgain_threshold=kldgain_threshold,
            dfpn_search_root=dfpn_search_root,
            dfpn_search_leaf=dfpn_search_leaf,
            num_simulations=num_simulations,
            temperature=temperature,
            q_greedy_depth=q_greedy_depth,
            max_random_moves=max_random_moves,
        )
    else:
        _run_self_play_parallel(
            shogi_variant=shogi_variant,
            tflite_path=tflite_path,
            tflite_path_others=tflite_path_others,
            kifu_dir=kifu_dir,
            kifu_index_groups=[
                range(i, i + job_size) for i in
                range(index_start, index_start + num_selfplay, job_size)
            ],
            coeff_puct=coeff_puct,
            kldgain_threshold=kldgain_threshold,
            dfpn_search_root=dfpn_search_root,
            dfpn_search_leaf=dfpn_search_leaf,
            num_simulations=num_simulations,
            temperature=temperature,
            q_greedy_depth=q_greedy_depth,
            max_random_moves=max_random_moves,
            n_jobs=n_jobs,
        )


def _validate(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    latest: str,
    previous: str,
    num_games: int,
    coeff_puct: float,
) -> float:
    game_class = getattr(getattr(vs, shogi_variant), 'Game')
    player_latest = _load_player(
        latest,
        coeff_puct=coeff_puct,
        kldgain_threshold=None,
        dfpn_search_root=0,
        dfpn_search_leaf=0,
    )
    player_prev = _load_player(
        previous,
        coeff_puct=coeff_puct,
        kldgain_threshold=None,
        dfpn_search_root=0,
        dfpn_search_leaf=0,
    )
    record = vs.Record(0, 0, 0, 0, 0, 0)
    pbar = tqdm(range(num_games), ncols=100)
    for n in pbar:
        if n % 2 == 0:
            result = vs.play_game(
                game_class(),
                player_latest,
                player_prev,
                search_args={'n_or_t': 100},
                select_args={'temperature': None},
                draw_on_max_moves=True,
            ).result
            record += vs.Record.from_black_result(result)
        else:
            result = vs.play_game(
                game_class(),
                player_prev,
                player_latest,
                search_args={'n_or_t': 100},
                select_args={'temperature': None},
                draw_on_max_moves=True,
            ).result
            record += vs.Record.from_white_result(result)
        pbar.set_description(
            f'{player_latest.name} vs {player_prev.name} = {record.wdl()}')
    return record.score()


def _get_previous_models_superior_to_latest(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    latest: str,
    previous: list[str],
    num_games: int = 10,
    coeff_puct: float = 4.,
) -> list[str]:
    return [
        prev for prev in previous
        if _validate(
            shogi_variant,
            latest,
            prev,
            num_games,
            coeff_puct,
        ) < num_games * 0.5
    ]


def _average_kifu_length(kifu_dir: str) -> float:
    line_length_list = []
    for path in glob(os.path.join(kifu_dir, 'kifu_*.tsv')):
        if ('B' in path) or ('W' in path):
            continue
        with open(path, 'rb') as f:
            line_length_list.append(sum(1 for _ in f) - 1)
    if line_length_list:
        return np.mean(line_length_list)
    return np.inf


def _compute_random_moves(random_rate: float, kifu_dir: str):
    max_random_moves = random_rate * _average_kifu_length(kifu_dir=kifu_dir)
    if max_random_moves != float('inf'):
        max_random_moves = int(np.ceil(max_random_moves / 2)) * 2
    print(f'max_random_moves = {max_random_moves}')
    return max_random_moves


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@cl.option("--selfplay", default=100)
@cl.option("--coeff-puct", default=4.)
@cl.option("--kldgain-threshold", default=1e-4)
@cl.option("--dfpn-search-root", default=10000)
@cl.option("--dfpn-search-leaf", default=100)
@cl.option("--num-simulations", default=100)
@cl.option("--temperature", default=1.)
@cl.option("--q-greedy-depth", default=1)
@cl.option("--random-rate", default=0.5)
@cl.option("--jobs", default=1)
@cl.option("--job-size", default=5)
def _selfplay_worker(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    num_selfplay: int,
    coeff_puct: float,
    kldgain_threshold: float,
    dfpn_search_root: int,
    dfpn_search_leaf: int,
    num_simulations: int,
    temperature: float,
    q_greedy_depth: int,
    random_rate: float,
    n_jobs: int,
    job_size: int = 5,
):
    tflite_path = 'models/model_{:04d}.tflite'
    for ii in range(10000):
        if (
            os.path.exists(tflite_path.format(ii))
            and os.path.exists(tflite_path.format(ii + 1))
        ):
            continue
        max_random_moves = random_rate * _average_kifu_length(
            f'datasets/dataset_{ii - 1:04d}')
        if max_random_moves != np.inf:
            max_random_moves = int(np.ceil(max_random_moves / 2)) * 2
        others = [
            f'models/model_{jj:04d}.tflite'
            for jj in list(range(ii - 1, -1, -1))[:10]
            if _validate(
                shogi_variant,
                latest=f'models/model_{ii:04d}.tflite',
                previous=f'models/model_{jj:04d}.tflite',
                num_games=10,
                coeff_puct=coeff_puct,
            ) < 10 * 0.5
        ]
        while True:
            _run_self_play(
                shogi_variant=shogi_variant,
                tflite_path=tflite_path.format(ii),
                tflite_path_others=others,
                kifu_dir=f'datasets/dataset_{ii:04d}',
                num_selfplay=num_selfplay,
                coeff_puct=coeff_puct,
                kldgain_threshold=kldgain_threshold,
                dfpn_search_root=dfpn_search_root,
                dfpn_search_leaf=dfpn_search_leaf,
                num_simulations=num_simulations,
                temperature=temperature,
                q_greedy_depth=q_greedy_depth,
                max_random_moves=max_random_moves,
                n_jobs=n_jobs,
                job_size=job_size,
            )
            if os.path.exists(tflite_path.format(ii + 1)):
                break
