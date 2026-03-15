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


class _SelfPlayWorker:
    def __init__(
        self,
        shogi_variant: tp.Literal["minishogi", "judkin_shogi", "shogi"],
        num_games: int,
        coeff_puct: float,
        kldgain_threshold: float,
        dfpn_search_root: int,
        dfpn_search_leaf: int,
        simulations: int,
        temperature: float,
        n_jobs: int,
        job_size: int,
    ):
        self._shogi_variant = shogi_variant
        self._num_games = num_games
        self._coeff_puct = coeff_puct
        self._kldgain_threshold = kldgain_threshold
        self._dfpn_search_root = dfpn_search_root
        self._dfpn_search_leaf = dfpn_search_leaf
        self._simulations = simulations
        self._temperature = temperature
        self._n_jobs = n_jobs
        self._job_size = job_size

    def __call__(
        self,
        tflite_path: str | None,
        tflite_path_others: tp.List[str],
        kifu_dir: str,
        max_random_moves: int,
    ) -> None:
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
        if self._n_jobs <= 1:
            self._run_self_play_single(
                tflite_path=tflite_path,
                tflite_path_others=tflite_path_others,
                kifu_dir=kifu_dir,
                kifu_index_range=range(
                    index_start, index_start + self._num_games
                ),
                max_random_moves=max_random_moves,
            )
        else:
            self._run_self_play_parallel(
                tflite_path=tflite_path,
                tflite_path_others=tflite_path_others,
                kifu_dir=kifu_dir,
                kifu_index_groups=[
                    range(i, i + self._job_size)
                    for i in range(
                        index_start,
                        index_start + self._num_games,
                        self._job_size,
                    )
                ],
                max_random_moves=max_random_moves,
                n_jobs=self._n_jobs,
            )

    def validate(self, latest: str, num_games: int = 10) -> list[str]:
        previous = self._get_previous_models(latest, max_models=10)
        if (self._n_jobs == 1) or (len(previous) < 1):
            scores = [
                self._validate(latest, prev, num_games).score()
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
                    n_jobs=min(self._n_jobs, len(previous)),
                    timeout=timeout_second,
                )(
                    delayed(self._validate)(
                        latest,
                        prev,
                        num_games,
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

    @staticmethod
    def _get_previous_models(latest: str, max_models: int) -> list[str]:
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
        return previous

    def _validate(
        self,
        latest: str,
        previous: str,
        num_games: int = 10,
        *,
        show_pbar: bool = True,
    ) -> vs.Record:
        game_class = getattr(getattr(vs, self._shogi_variant), 'Game')
        player_latest = _load_player(
            latest,
            coeff_puct=self._coeff_puct,
            kldgain_threshold=None,
            dfpn_search_root=0,
            dfpn_search_leaf=0,
        )
        player_prev = _load_player(
            previous,
            coeff_puct=self._coeff_puct,
            kldgain_threshold=None,
            dfpn_search_root=0,
            dfpn_search_leaf=0,
        )
        record = vs.Record()
        iterator = range(num_games)
        if show_pbar:
            iterator = tqdm(iterator, ncols=100)
        for n in iterator:
            if n % 2 == 0:
                result = vs.play_game(
                    game_class(),
                    player_latest,
                    player_prev,
                    draw_on_max_moves=True,
                ).result
                record += vs.Record.from_black_result(result)
            else:
                result = vs.play_game(
                    game_class(),
                    player_prev,
                    player_latest,
                    draw_on_max_moves=True,
                ).result
                record += vs.Record.from_white_result(result)
            if show_pbar:
                iterator.set_description(
                    f'{player_latest.name} vs {player_prev.name} '
                    f'= {record.wdl()}'
                )
        return record

    def _run_self_play_single(
        self,
        tflite_path: str | None,
        tflite_path_others: list[str],
        kifu_dir: str,
        kifu_index_range: tp.Iterable[int],
        max_random_moves: int,
        show_pbar: bool = True,
    ):
        player = self._load_player(tflite_path)
        player_others = [
            player for _ in range(10 - len(tflite_path_others))
        ] + [self._load_player(path) for path in tflite_path_others]
        iterator = kifu_index_range
        if show_pbar:
            iterator = tqdm(
                iterator,
                ncols=100,
                desc=f"{player.name} vs {player.name}",
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
            self._play_game_and_dump_log(
                player_black,
                player_white,
                file_path=kifu_dir + '/' + f'kifu_{i:05d}{suffix}.tsv',
                main=None if player_black is player_white else player,
                max_random_moves=max_random_moves,
            )

    def _run_self_play_parallel(
        self,
        tflite_path: str | None,
        tflite_path_others: list[str],
        kifu_dir: str,
        kifu_index_groups: tp.Iterable[tp.Iterable[int]],
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
                    delayed(self._run_self_play_single)(
                        tflite_path,
                        tflite_path_others,
                        kifu_dir,
                        indices,
                        max_random_moves,
                        show_pbar=False,
                    )
                    for indices in kifu_index_groups
                )
            except Exception as e:
                print(f"Self-play task timed out: {e}")

    def _load_player(self, tflite_path: str | None) -> vs.engine.AlphaZero:
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
            coeff_puct=self._coeff_puct,
            kldgain_threshold=self._kldgain_threshold,
            dfpn_search_root=self._dfpn_search_root,
            dfpn_search_leaf=self._dfpn_search_leaf,
            name=(
                'none'
                if tflite_path is None
                else tflite_path.split('/')[-1].split('.')[0]
            ),
        )

    def _play_game_and_dump_log(
        self,
        black: vs.engine.AlphaZero,
        white: vs.engine.AlphaZero,
        file_path: str,
        main: vs.engine.AlphaZero | None,
        max_random_moves: int,
    ) -> None:
        game = self._play_game(black, white, main, max_random_moves)
        with open(file_path, "w") as f:
            self._dump_game_log(f, game)

    def _play_game(
        self,
        black: vs.engine.Engine,
        white: vs.engine.Engine,
        main: vs.engine.Engine | None,
        max_random_moves: int = 320,
    ) -> vs.Game:
        max_moves = 320
        game_class = getattr(getattr(vs, self._shogi_variant), "Game")
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
            self._search(player, main, game)
            if self._found_mate(player, game):
                break
            self._log_policy_value(main or player, game)
            move = self._select(player, game, num_random_moves)
            game.apply(move)
            black.apply(move)
            if white is not black:
                white.apply(move)
        else:
            game.declare_draw()
        black.clear()
        white.clear()
        return game

    def _search(self, player, main, game):
        self._set_game_and_search(player, game)
        if (main is not None) and (main is not player):
            self._set_game_and_search(main, game)

    def _set_game_and_search(self, player: vs.engine.Engine, game: vs.Game):
        if not player.is_ready():
            player.set_game(game)
        player.search(self._simulations - player.get_search_count())

    def _found_mate(self, player: vs.engine.Engine, game: vs.Game) -> bool:
        if (
            player.proved_mate()
            and player.get_q_value() > 0
            and ((mate_moves := player.get_mate_moves()) is not None)
        ):
            for i, m in enumerate(mate_moves):
                game.apply(m)
                game.q_value_log.append(1.0 if i % 2 == 0 else -1.0)
                game.policy_log.append({})
            if game.result != vs.Result.ONGOING:
                return True
            else:
                for _ in mate_moves:
                    game.undo()
        return False

    def _select(
        self,
        player: vs.engine.Engine,
        game: vs.Game,
        num_random_moves: int,
    ) -> vs.Move:
        if game.ply() < num_random_moves:
            move = player.select(temperature=self._temperature)
        else:
            move = player.select()
        if hash(move) == 0:
            raise ValueError(
                f"Invalid move ({move}) selected at the game, "
                f"{game.to_sfen()}.\n{player._tree(depth=2)}"
            )
        return move

    def _log_policy_value(
        self,
        player: vs.engine.AlphaZero,
        game: vs.Game,
    ) -> None:
        policy = player.get_visit_counts(include_random=False)
        total = sum(policy.values())
        policy = {m.to_sfen(): v / total for m, v in policy.items()}
        game.policy_log.append(policy)
        game.q_value_log.append(player.get_q_value())

    @staticmethod
    def _dump_game_log(file_: tp.TextIO, game: vs.Game) -> None:
        r = str(game.result)
        if "ONGOING" in r:
            raise ValueError("The game is still ongoing")
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

    @classmethod
    def wrap_options(cls, prefix: str = "") -> tp.Callable:
        if prefix and (not prefix.endswith("-")):
            prefix = prefix + "-"
        wrappers = cls._get_cli_options(prefix)

        def decorator(func: tp.Callable) -> tp.Callable:
            for wrap in reversed(wrappers):
                func = wrap(func)
            return func

        return decorator

    @staticmethod
    def _get_cli_options(prefix: str = "") -> list[cl.Option]:
        return [
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
            cl.option(
                f"--{prefix}coeff-puct",
                default=4.0,
                show_default=True,
                help=(
                    "PUCT exploration constant; larger values increase "
                    "search exploration."
                ),
            ),
            cl.option(
                f"--{prefix}kldgain-threshold",
                default=1e-4,
                show_default=True,
                help=(
                    "Early-stop threshold based on KL-divergence gain during "
                    "search; smaller values search longer."
                ),
            ),
            cl.option(
                f"--{prefix}temperature",
                default=1.0,
                show_default=True,
                help=(
                    "Sampling temperature for move selection during random "
                    "opening moves; lower is greedier."
                ),
            ),
            cl.option(
                f"--{prefix}random-rate",
                default=0.5,
                show_default=True,
                help=(
                    "Fraction of recent average game length used to set "
                    "random opening moves."
                ),
            ),
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
    worker = _SelfPlayWorker(
        shogi_variant=shogi_variant,
        coeff_puct=coeff_puct,
        kldgain_threshold=kldgain_threshold,
        dfpn_search_root=dfpn_search_root,
        dfpn_search_leaf=dfpn_search_leaf,
        simulations=num_simulations,
        temperature=temperature,
    )
    if n_jobs <= 1:
        worker._run_self_play_single(
            tflite_path=tflite_path,
            tflite_path_others=tflite_path_others,
            kifu_dir=kifu_dir,
            kifu_index_range=range(index_start, index_start + num_selfplay),
            max_random_moves=max_random_moves,
        )
    else:
        worker._run_self_play_parallel(
            tflite_path=tflite_path,
            tflite_path_others=tflite_path_others,
            kifu_dir=kifu_dir,
            kifu_index_groups=[
                range(i, i + job_size)
                for i in range(
                    index_start, index_start + num_selfplay, job_size
                )
            ],
            max_random_moves=max_random_moves,
            n_jobs=n_jobs,
        )


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
@_SelfPlayWorker.wrap_options()
def _selfplay_worker(**kwargs):
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    worker = _SelfPlayWorker(
        shogi_variant=kwargs['shogi'],
        num_games=kwargs["play_num_games"],
        coeff_puct=kwargs['coeff_puct'],
        kldgain_threshold=kwargs['kldgain_threshold'],
        dfpn_search_root=kwargs['dfpn_root'],
        dfpn_search_leaf=kwargs['dfpn_leaf'],
        simulations=kwargs['num_simulations'],
        temperature=kwargs['temperature'],
        n_jobs=kwargs['play_jobs'],
        job_size=kwargs["job_size"],
    )

    tflite_path = 'models/model_{:04d}.tflite'
    for ii in range(10000):
        if os.path.exists(tflite_path.format(ii)) and os.path.exists(
            tflite_path.format(ii + 1)
        ):
            continue
        max_random_moves = _compute_random_moves(
            kwargs['random_rate'], f'datasets/dataset_{ii - 1:04d}'
        )
        others = worker.validate(tflite_path.format(ii))
        while True:
            worker(
                tflite_path=tflite_path.format(ii) if ii > 0 else None,
                tflite_path_others=others,
                kifu_dir=f'datasets/dataset_{ii:04d}',
                max_random_moves=max_random_moves,
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
