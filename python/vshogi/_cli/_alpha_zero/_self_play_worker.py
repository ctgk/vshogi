import contextlib
import os
import sys
import typing as tp
from datetime import datetime
from glob import glob

import click as cl
import joblib
import numpy as np
from joblib.parallel import Parallel, delayed
from tqdm import tqdm

import vshogi as vs


def _self_play_parameters(prefix: str = "") -> callable:
    if prefix and (not prefix.endswith("-")):
        prefix = prefix + "-"
    wrappers = [
        cl.option(
            f"--{prefix}num-games",
            default=100,
            show_default=True,
            help="Number of self-play games to generate per iteration.",
        ),
        cl.option(
            f"--{prefix}dfpn-root",
            default=10000,
            show_default=True,
            help="DFPN search limit for root nodes",
        ),
        cl.option(
            f"--{prefix}dfpn-leaf",
            default=100,
            show_default=True,
            help="DFPN search limit for leaf nodes",
        ),
        cl.option(
            f"--{prefix}num-simulations",
            default=100,
            show_default=True,
            help=(
                "Number of search simulations per move; "
                "higher is stronger but slower."
            ),
        ),
        cl.option(f"--{prefix}coeff-puct", default=4.0, show_default=True),
        cl.option(
            f"--{prefix}kldgain-threshold", default=1e-4, show_default=True
        ),
        cl.option(f"--{prefix}temperature", default=1.0, show_default=True),
        cl.option(f"--{prefix}random-rate", default=0.5, show_default=True),
        cl.option(
            f"--{prefix}jobs",
            default=1,
            show_default=True,
            help=(
                "Number of parallel workers for self-play; "
                "1 runs sequentially."
            ),
        ),
        cl.option(
            f"--{prefix}job-size",
            default=5,
            show_default=True,
            help="Number of games per parallel job.",
        ),
    ]

    def decorator(func: callable) -> callable:
        for wrap in reversed(wrappers):
            func = wrap(func)
        return func

    return decorator


def _dump_game_log(file_, game: vs.Game) -> None:
    r = str(game.result)
    if 'ONGOING' in r:
        raise ValueError('The game is still ongoing')
    game.dump_log(
        (
            lambda g, i: g.get_sfen_at(i, include_move_count=False),
            lambda g, i: g.get_move_at(i).to_sfen(),
            lambda _, i: (
                0
                if 'DRAW' in r
                else (2 * (('BLACK' in r) is (i % 2 == 0)) - 1)
            ),
            lambda g, i: g.q_value_log[i],
            lambda g, i: g.policy_log[i],
        ),
        names=('sfen', 'move', 'result', 'q_value', 'policy'),
        file_=file_,
    )


def _play_game(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    black: vs.engine.Engine,
    white: vs.engine.Engine,
    num_simulations: int,
    temperature: float,
    main: tp.Optional[vs.engine.Engine] = None,
    max_random_moves: int = 320,
    max_moves: int = 320,
) -> vs.Game:
    shogi_module = getattr(vs, shogi_variant)
    game_class = getattr(shogi_module, 'Game')
    game: vs.Game = game_class()
    game.q_value_log = []
    game.policy_log = []

    num_random_moves = (
        np.random.choice(max_random_moves + 1)
        if np.isfinite(max_random_moves)
        else max_random_moves
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

        if player.proved_mate() and player.get_q_value() > 0:
            mate_moves = player.get_mate_moves()
            if mate_moves is not None:
                for i, m in enumerate(mate_moves):
                    game.apply(m)
                    game.q_value_log.append(1.0 if i % 2 == 0 else -1.0)
                    game.policy_log.append({})
                if game.result != vs.Result.ONGOING:
                    break
                else:
                    for _ in mate_moves:
                        game.undo()
        elif game.ply() < num_random_moves:
            move = player.select(temperature=temperature)
        else:
            move = player.select()
        if hash(move) == 0:
            raise ValueError(
                f"Invalid move ({move}) selected at the game, "
                f"{game.to_sfen()}.\n{player._tree(depth=2)}"
            )

        player_dump = main or player
        policy = player_dump.get_visit_counts(include_random=False)
        total = sum(policy.values())
        policy = {m.to_sfen(): v / total for m, v in policy.items()}
        game.policy_log.append(policy)
        game.q_value_log.append(player_dump.get_q_value())

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
    max_random_moves: int,
) -> None:
    game = _play_game(
        shogi_variant,
        player_black,
        player_white,
        num_simulations,
        temperature,
        main_player,
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
) -> vs.engine.Engine:
    return vs.engine.AlphaZero(
        (
            (
                lambda g: (
                    np.zeros(g.num_dlshogi_policy, dtype=np.float32),
                    vs.engine.piece_value_func(g),
                )
            )
            if tflite_path is None
            else vs.dlshogi.PolicyValueFunction(tflite_path)
        ),
        coeff_puct=coeff_puct,
        kldgain_threshold=kldgain_threshold,
        dfpn_search_root=dfpn_search_root,
        dfpn_search_leaf=dfpn_search_leaf,
        name=(
            'none'
            if tflite_path is None
            else tflite_path.split('/')[-1].split('.')[0]
        ),
    )


def _run_self_play_single(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    tflite_path: str | None,
    tflite_path_others: tp.List[str],
    kifu_dir: str,
    kifu_index_range: tp.Iterable[int],
    coeff_puct: float,
    kldgain_threshold: float,
    dfpn_search_root: int,
    dfpn_search_leaf: int,
    num_simulations: int,
    temperature: float,
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
            if player_white is not player:
                suffix = f'_W{player_white.name}'
        else:
            player_black = player_others[i % len(player_others)]
            if player_black is not player:
                suffix = f'_B{player_black.name}'
        _play_game_and_dump_log(
            shogi_variant=shogi_variant,
            player_black=player_black,
            player_white=player_white,
            filepath=kifu_dir + '/' + f'kifu_{i:05d}{suffix}.tsv',
            main_player=None if player_black is player_white else player,
            num_simulations=num_simulations,
            temperature=temperature,
            max_random_moves=max_random_moves,
        )


def _run_self_play_parallel(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    tflite_path: str | None,
    tflite_path_others: tp.List[str],
    kifu_dir: str,
    kifu_index_groups: tp.Iterable[tp.Iterable[int]],
    coeff_puct: float,
    kldgain_threshold: float,
    dfpn_search_root: int,
    dfpn_search_leaf: int,
    num_simulations: int,
    temperature: float,
    max_random_moves: int,
    n_jobs: int,
):
    name = (
        'none'
        if tflite_path is None
        else tflite_path.split('/')[-1].split('.')[0]
    )
    timeout_second = 10 * 60  # 10 minutes
    with _tqdm_joblib(
        tqdm(
            total=len(kifu_index_groups),
            ncols=100,
            desc=f'{name} vs {name}',
        ),
    ):
        try:
            Parallel(n_jobs=n_jobs, timeout=timeout_second)(
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
                    max_random_moves,
                    show_pbar=False,
                )
                for indices in kifu_index_groups
            )
        except Exception as e:
            print(f"Self-play task timed out: {e}")


def _run_self_play(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    tflite_path: str | None,
    tflite_path_others: tp.List[str],
    kifu_dir: str,
    num_selfplay: int,
    coeff_puct: float,
    kldgain_threshold: float,
    dfpn_search_root: int,
    dfpn_search_leaf: int,
    num_simulations: int,
    temperature: float,
    max_random_moves: int,
    n_jobs: int,
    job_size: int = 5,
):
    name = (
        "none"
        if tflite_path is None
        else tflite_path.split("/")[-1].split(".")[0]
    )
    print(
        f'Self-play ({name}) with others: '
        + ', '.join(
            p.split('/')[-1].split('.')[0] for p in tflite_path_others
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
            max_random_moves=max_random_moves,
        )
    else:
        _run_self_play_parallel(
            shogi_variant=shogi_variant,
            tflite_path=tflite_path,
            tflite_path_others=tflite_path_others,
            kifu_dir=kifu_dir,
            kifu_index_groups=[
                range(i, i + job_size)
                for i in range(
                    index_start, index_start + num_selfplay, job_size
                )
            ],
            coeff_puct=coeff_puct,
            kldgain_threshold=kldgain_threshold,
            dfpn_search_root=dfpn_search_root,
            dfpn_search_leaf=dfpn_search_leaf,
            num_simulations=num_simulations,
            temperature=temperature,
            max_random_moves=max_random_moves,
            n_jobs=n_jobs,
        )


def _validate(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    latest: str,
    previous: str,
    num_games: int,
    coeff_puct: float,
    *,
    show_pbar: bool = True,
) -> vs.Record:
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
    iterator = range(num_games)
    if show_pbar:
        iterator = tqdm(iterator, ncols=100)
    search_args = {'budget': 100}
    for n in iterator:
        if n % 2 == 0:
            result = vs.play_game(
                game_class(),
                player_latest,
                player_prev,
                search_args=search_args,
                select_args={'temperature': None},
                draw_on_max_moves=True,
            ).result
            record += vs.Record.from_black_result(result)
        else:
            result = vs.play_game(
                game_class(),
                player_prev,
                player_latest,
                search_args=search_args,
                select_args={'temperature': None},
                draw_on_max_moves=True,
            ).result
            record += vs.Record.from_white_result(result)
        if show_pbar:
            iterator.set_description(
                f'{player_latest.name} vs {player_prev.name} = {record.wdl()}'
            )
    return record


def _get_previous_models_superior_to_latest(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    latest: str,
    num_games: int = 10,
    coeff_puct: float = 4.0,
    n_jobs: int = 1,
) -> list[str]:
    index = int(latest.split("_")[-1].split(".")[0])
    path_template = "_".join(latest.split("_")[:-1] + ["{:04d}.tflite"])
    if index == 0:
        return []
    p = np.arange(1, index + 1).astype(float) ** 2
    p /= p.sum()
    previous = sorted(
        [
            path_template.format(i)
            for i in np.random.choice(
                index, size=min(10, index), replace=False, p=p
            )
        ],
        reverse=True,
    )
    if (n_jobs == 1) or (len(previous) == 1):
        scores = [
            _validate(
                shogi_variant, latest, prev, num_games, coeff_puct
            ).score()
            for prev in previous
        ]
    else:
        timeout_second = 5 * 60  # 5 minutes
        with _tqdm_joblib(
            tqdm(
                total=len(previous),
                desc="Play against previous models",
                ncols=80,
            ),
        ):
            records: list[vs.Record] = Parallel(
                n_jobs=min(n_jobs, len(previous)),
                timeout=timeout_second,
            )(
                delayed(_validate)(
                    shogi_variant,
                    latest,
                    prev,
                    num_games,
                    coeff_puct,
                    show_pbar=False,
                )
                for prev in previous
            )
        for prev, record in zip(previous, records):
            msg = (
                f"{latest.split('/')[-1].split('.')[0]} vs "
                f"{prev.split('/')[-1].split('.')[0]} = {record.wdl()}"
            )
            print(msg)
        scores = [r.score() for r in records]
    indices = np.argsort(scores)[:9]  # low -> high
    return [previous[i] for i in indices if scores[i] < num_games * 0.5]


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
@_self_play_parameters(prefix="")
def _selfplay_worker(**kwargs):
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    tflite_path = 'models/model_{:04d}.tflite'
    for ii in range(10000):
        if os.path.exists(tflite_path.format(ii)) and os.path.exists(
            tflite_path.format(ii + 1)
        ):
            continue
        max_random_moves = _compute_random_moves(
            kwargs['random_rate'], f'datasets/dataset_{ii - 1:04d}'
        )
        others = _get_previous_models_superior_to_latest(
            shogi_variant=kwargs['shogi'],
            latest=tflite_path.format(ii),
            engine=kwargs['engine'],
            num_games=10,
            coeff_puct=kwargs['coeff_puct'],
            n_jobs=kwargs['jobs'],
        )
        while True:
            _run_self_play(
                shogi_variant=kwargs['shogi'],
                tflite_path=tflite_path.format(ii) if ii > 0 else None,
                tflite_path_others=others,
                kifu_dir=f'datasets/dataset_{ii:04d}',
                num_selfplay=kwargs['num_games'],
                coeff_puct=kwargs['coeff_puct'],
                kldgain_threshold=kwargs['kldgain_threshold'],
                dfpn_search_root=kwargs['dfpn_root'],
                dfpn_search_leaf=kwargs['dfpn_leaf'],
                num_simulations=kwargs['num_simulations'],
                temperature=kwargs['temperature'],
                max_random_moves=max_random_moves,
                n_jobs=kwargs['jobs'],
                job_size=kwargs['job_size'],
            )
            if os.path.exists(tflite_path.format(ii + 1)):
                print(f"Found new model: {tflite_path.format(ii + 1)}")
                break
            else:
                msg = (
                    f"New model ({tflite_path.format(ii + 1)}) not found. "
                    f"Continue self-play with {tflite_path.format(ii)}"
                )
                print(msg)
