import typing as tp

import click as cl
import numpy as np
from tqdm import tqdm

import vshogi as vs
from vshogi._game import Game
from vshogi._record import Record
from vshogi.engine._klent import Klent
from vshogi.engine._piece_value_func import piece_value_func
from vshogi.dlshogi._policy_value_function import PolicyValueFunction
from vshogi.dlshogi._alpha_zero._self_play_worker import (
    _SelfPlayWorker as _AlphaZeroSelfPlayWorker,
)


class _SelfPlayWorker(_AlphaZeroSelfPlayWorker):
    def __init__(
        self,
        shogi_variant: tp.Literal["minishogi", "judkin_shogi", "shogi"],
        num_games: int,
        coeff_kl: float,
        coeff_entropy: float,
        dfpn_search: int,
        temperature: float,
        n_jobs: int,
        job_size: int,
    ):
        super().__init__(
            shogi_variant=shogi_variant,
            num_games=num_games,
            coeff_puct=4.0,
            kldgain_threshold=1e-4,
            dfpn_search_root=dfpn_search,
            dfpn_search_leaf=0,
            simulations=100,
            temperature=temperature,
            n_jobs=n_jobs,
            job_size=job_size,
        )
        self._coeff_kl = coeff_kl
        self._coeff_entropy = coeff_entropy

    def _load_player(self, tflite_path: str | None) -> Klent:
        return Klent(
            (
                lambda g: (
                    np.zeros(g.num_dlshogi_policy, dtype=np.float32),
                    piece_value_func(g, return_action_values=True),
                )
            )
            if tflite_path is None
            else PolicyValueFunction(tflite_path),
            coeff_kl=self._coeff_kl,
            coeff_entropy=self._coeff_entropy,
            dfpn_search=self._dfpn_search_root,
            name=(
                'none'
                if tflite_path is None
                else tflite_path.split('/')[-1].split('.')[0]
            ),
        )

    def _play_game(
        self,
        black: vs.engine.Klent,
        white: vs.engine.Klent,
        main: vs.engine.Klent | None,
        max_random_moves: int = 320,
    ):
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
        else:
            game.declare_draw()
        black.clear()
        white.clear()
        return game

    def _set_game_and_search(self, player: Klent, game: Game) -> None:
        assert type(player) is Klent, type(player)
        player.set_game(game)
        player.search()

    def _log_policy_value(self, player: Klent, game: Game) -> None:
        policy = {m.to_sfen(): v for m, v in player.improved_policy().items()}
        game.policy_log.append(policy)
        game.q_value_log.append(player.get_q_value())

    def _validate(
        self,
        latest,
        previous,
        num_games=10,
        *,
        show_pbar=True,
    ) -> Record:
        game_class = getattr(getattr(vs, self._shogi_variant), 'Game')
        player_latest = super()._load_player(latest)
        player_prev = super()._load_player(previous)
        record = vs.Record()
        search_args = {"budget": 100}
        iterator = range(num_games)
        if show_pbar:
            iterator = tqdm(iterator, ncols=100)
        for n in iterator:
            if n % 2 == 0:
                result = vs.play_game(
                    game_class(),
                    player_latest,
                    player_prev,
                    search_args=search_args,
                    draw_on_max_moves=True,
                ).result
                record += vs.Record.from_black_result(result)
            else:
                result = vs.play_game(
                    game_class(),
                    player_prev,
                    player_latest,
                    search_args=search_args,
                    draw_on_max_moves=True,
                ).result
                record += vs.Record.from_white_result(result)
            if show_pbar:
                iterator.set_description(
                    f'{player_latest.name} vs {player_prev.name} '
                    f'= {record.wdl()}'
                )
        return record

    @staticmethod
    def _get_cli_options(prefix: str = "") -> dict[str, tp.Callable]:
        az_options = _AlphaZeroSelfPlayWorker._get_cli_options(prefix)
        return {
            "num-games": az_options["num-games"],
            "coeff-kl": cl.option(
                f"--{prefix}coeff-kl",
                type=float,
                default=0.1,
                show_default=True,
                help="Coefficient of KL divergence to previous policy.",
            ),
            "coeff-entropy": cl.option(
                f"--{prefix}coeff-entropy",
                type=float,
                default=0.03,
                show_default=True,
                help="Coefficient of policy entropy.",
            ),
            "dfpn": cl.option(
                f"--{prefix}dfpn",
                type=int,
                default=10000,
                show_default=True,
                help="DFPN search limit",
            ),
            "temperature": az_options["temperature"],
            "random-rate": az_options["random-rate"],
            "jobs": az_options["jobs"],
            "job-size": az_options["job-size"],
        }
