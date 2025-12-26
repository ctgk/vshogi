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
            lambda g, i: g.policy_log[i],
            lambda g, i: g.z_weight_log[i],
        ),
        names=('sfen', 'move', 'result', 'q_value', 'policy', 'z_weight'),
        file_=file_,
    )


def _play_game(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    black: vs.engine.Engine,
    white: vs.engine.Engine,
    num_simulations: int,
    temperature: float,
    main: vs.engine.Engine = None,
    q_greedy_depth: int = 1,
    max_random_moves: int = 320,
    max_moves: int = 320,
    gumbel_actions: int | None = None,
) -> vs.Game:
    shogi_module = getattr(vs, shogi_variant)
    game_class = getattr(shogi_module, 'Game')
    engine = black.__class__.__name__
    game = game_class()
    game.q_value_log = []
    game.policy_log = []
    game.z_weight_log = []

    num_random_moves = (
        np.random.choice(max_random_moves + 1)
        if np.isfinite(max_random_moves) else max_random_moves
    )
    for _ in range(max_moves):
        if game.result != vs.Result.ONGOING:
            break

        player = black if game.turn == vs.Color.BLACK else white
        if (not player.is_ready()) or (engine == 'GumbelAlphaZero'):
            player.set_game(game)

        if engine == 'AlphaZero':
            player.search(num_simulations - player.get_search_count())
        elif engine == 'GumbelAlphaZero':
            player.search(num_simulations, num_actions=gumbel_actions)

        if (main is not None) and (main is not player):
            if (not main.is_ready()) or (engine == 'GumbelAlphaZero'):
                main.set_game(game)
            if engine == 'AlphaZero':
                main.search(num_simulations - main.get_search_count())
            elif engine == 'GumbelAlphaZero':
                main.search(num_simulations, num_actions=gumbel_actions)

        if player.proved_mate():
            if player.get_q_value() > 0:
                mate_moves = game.get_mate_moves_if_any(
                    player._dfpn_search_root)
                if mate_moves is not None:
                    for i, m in enumerate(mate_moves):
                        game.apply(m)
                        game.q_value_log.append(1. if i % 2 == 0 else -1.)
                        game.policy_log.append({})
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
        elif (engine == 'AlphaZero') and (game.ply() < num_random_moves):
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
        if engine == 'AlphaZero':
            visit_count = {
                m.to_sfen(): v + 1  # +1 for smoothing
                for m, v in
                player_dump.get_visit_counts(include_random=False).items()
            }
            game.policy_log.append(visit_count)
        elif engine == 'GumbelAlphaZero':
            game.policy_log.append(player_dump.select().to_sfen())
        game.q_value_log.append(
            player_dump.get_q_value(greedy_depth=q_greedy_depth))

        game.apply(move)
        if engine == 'AlphaZero':
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
    gumbel_actions: int | None,
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
        gumbel_actions=gumbel_actions,
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
    engine: tp.Literal['AlphaZero', 'GumbelAlphaZero'],
) -> vs.engine.Engine:
    engine_class = getattr(vs.engine, engine)
    kwargs = {} if engine == 'GumbelAlphaZero' else {
        'coeff_puct': coeff_puct,
        'kldgain_threshold': kldgain_threshold,
    }
    return engine_class(
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
        **kwargs,
        dfpn_search_root=dfpn_search_root,
        dfpn_search_leaf=dfpn_search_leaf,
        name=(
            'none' if tflite_path is None else
            tflite_path.split('/')[-1].split('.')[0]
        ),
    )


def _run_self_play_single(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    engine: tp.Literal['AlphaZero', 'GumbelAlphaZero'],
    tflite_path: str | None,
    tflite_path_others: list[str],
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
    gumbel_actions: int | None,
    show_pbar: bool = True,
):
    player = _load_player(
        tflite_path=tflite_path,
        coeff_puct=coeff_puct,
        kldgain_threshold=kldgain_threshold,
        dfpn_search_root=dfpn_search_root,
        dfpn_search_leaf=dfpn_search_leaf,
        engine=engine,
    )
    player_others = [player for _ in range(10 - len(tflite_path_others))] + [
        _load_player(
            tflite_path=path,
            coeff_puct=coeff_puct,
            kldgain_threshold=kldgain_threshold,
            dfpn_search_root=dfpn_search_root,
            dfpn_search_leaf=dfpn_search_leaf,
            engine=engine,
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
            gumbel_actions=gumbel_actions,
        )


def _run_self_play_parallel(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    engine: tp.Literal['AlphaZero'],
    tflite_path: str | None,
    tflite_path_others: list[str],
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
    gumbel_actions: int | None,
    n_jobs: int,
):
    name = (
        'none' if tflite_path is None
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
                    engine,
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
                    gumbel_actions=gumbel_actions,
                    show_pbar=False,
                )
                for indices in kifu_index_groups
            )
        except Exception as e:
            print(f"Self-play task timed out: {e}")


def _run_self_play(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    engine: tp.Literal['AlphaZero', 'GumbelAlphaZero'],
    tflite_path: str | None,
    tflite_path_others: list[str],
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
    gumbel_actions: int | None,
    n_jobs: int,
    job_size: int = 5,
):
    name = (
        "none" if tflite_path is None
        else tflite_path.split("/")[-1].split(".")[0]
    )
    print(
        f'Self-play ({name}) with others: '
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
            engine=engine,
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
            gumbel_actions=gumbel_actions,
        )
    else:
        _run_self_play_parallel(
            shogi_variant=shogi_variant,
            engine=engine,
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
            gumbel_actions=gumbel_actions,
            n_jobs=n_jobs,
        )


def _validate(
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    engine: tp.Literal['AlphaZero', 'GumbelAlphaZero'],
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
        engine=engine,
    )
    player_prev = _load_player(
        previous,
        coeff_puct=coeff_puct,
        kldgain_threshold=None,
        dfpn_search_root=0,
        dfpn_search_leaf=0,
        engine=engine,
    )
    record = vs.Record(0, 0, 0, 0, 0, 0)
    pbar = tqdm(range(num_games), ncols=100)
    for n in pbar:
        if n % 2 == 0:
            result = vs.play_game(
                game_class(),
                player_latest,
                player_prev,
                search_args=(
                    {'n_or_t': 100} if engine == 'AlphaZero'
                    else {'num_sims': 100, 'num_actions': 16}
                ),
                select_args={'temperature': None},
                draw_on_max_moves=True,
            ).result
            record += vs.Record.from_black_result(result)
        else:
            result = vs.play_game(
                game_class(),
                player_prev,
                player_latest,
                search_args=(
                    {'n_or_t': 100} if engine == 'AlphaZero'
                    else {'num_sims': 100, 'num_actions': 16}
                ),
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
    engine: tp.Literal['AlphaZero'],
    num_games: int = 10,
    coeff_puct: float = 4.,
) -> list[str]:
    return [
        prev for prev in previous
        if _validate(
            shogi_variant,
            engine,
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
@cl.option("--num-games", default=100, show_default=True)
@cl.option("--coeff-puct", default=4., show_default=True)
@cl.option("--kldgain-threshold", default=1e-4, show_default=True)
@cl.option("--dfpn-root", default=10000, show_default=True)
@cl.option("--dfpn-leaf", default=100, show_default=True)
@cl.option("--num-simulations", default=100, show_default=True)
@cl.option("--temperature", default=1., show_default=True)
@cl.option("--q-greedy-depth", default=1, show_default=True)
@cl.option("--random-rate", default=0.5, show_default=True)
@cl.option(
    "--engine",
    default='AlphaZero',
    type=cl.Choice(['AlphaZero', 'GumbelAlphaZero']),
    show_default=True,
)
@cl.option("--gumbel-actions", default=16, show_default=True)
@cl.option("--jobs", default=1, show_default=True)
@cl.option("--job-size", default=5, show_default=True)
def _selfplay_worker(**kwargs):
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    tflite_path = 'models/model_{:04d}.tflite'
    for ii in range(10000):
        if (
            os.path.exists(tflite_path.format(ii))
            and os.path.exists(tflite_path.format(ii + 1))
        ):
            continue
        max_random_moves = _compute_random_moves(
            kwargs['random_rate'], f'datasets/dataset_{ii - 1:04d}')
        others = _get_previous_models_superior_to_latest(
            shogi_variant=kwargs['shogi'],
            latest=tflite_path.format(ii),
            previous=[
                tflite_path.format(j)
                for j in list(range(ii - 1, -1, -1))[:10]
            ],
            engine=kwargs['engine'],
            num_games=10,
            coeff_puct=kwargs['coeff_puct'],
        )
        while True:
            _run_self_play(
                shogi_variant=kwargs['shogi'],
                engine=kwargs['engine'],
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
                q_greedy_depth=kwargs['q_greedy_depth'],
                max_random_moves=max_random_moves,
                gumbel_actions=kwargs['gumbel_actions'],
                n_jobs=kwargs['jobs'],
                job_size=kwargs['job_size'],
            )
            if os.path.exists(tflite_path.format(ii + 1)):
                break
