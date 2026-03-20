import typing as tp

import click as cl
import numpy as np
from tqdm import tqdm

import vshogi as vs
from vshogi.engine import GumbelAlphaZero
from vshogi.dlshogi._alpha_zero._self_play_worker import (
    _SelfPlayWorker as _AlphaZeroSelfPlayWorker,
)


class _SelfPlayWorker(_AlphaZeroSelfPlayWorker):
    def __init__(
        self,
        shogi_variant: tp.Literal["minishogi", "judkin_shogi", "shogi"],
        num_games: int,
        num_actions: int,
        dfpn_search_root: int,
        dfpn_search_leaf: int,
        simulations: int,
        temperature: float,
        n_jobs: int,
        job_size: int,
    ):
        self._shogi_variant = shogi_variant
        self._num_games = num_games
        self._num_actions = num_actions
        self._dfpn_search_root = dfpn_search_root
        self._dfpn_search_leaf = dfpn_search_leaf
        self._simulations = simulations
        self._temperature = temperature
        self._n_jobs = n_jobs
        self._job_size = job_size

    def _load_player(self, tflite_path: str | None) -> GumbelAlphaZero:
        return GumbelAlphaZero(
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
            dfpn_search_root=self._dfpn_search_root,
            dfpn_search_leaf=self._dfpn_search_leaf,
            name=(
                'none'
                if tflite_path is None
                else tflite_path.split('/')[-1].split('.')[0]
            ),
        )

    def _set_game_and_search(self, player: GumbelAlphaZero, game: vs.Game):
        player.set_game(game)
        player.search(self._simulations, self._num_actions)

    def _log_policy_value(self, player: GumbelAlphaZero, game: vs.Game):
        # action = player.select()
        # policy = {
        #     m.to_sfen(): float(m == action) for m in game.get_legal_moves()
        # } # simple policy loss
        policy = {
            m.to_sfen(): v for m, v in player.improved_policy().items()
        }  # policy with completed Q-values
        policy = dict(sorted(policy.items(), key=lambda t: t[1], reverse=True))
        game.policy_log.append(policy)
        game.q_value_log.append(player.get_q_value())

    def _validate(
        self,
        latest: str,
        previous: str,
        num_games: int = 10,
        *,
        show_pbar: bool = True,
    ) -> vs.Record:
        game_class = getattr(getattr(vs, self._shogi_variant), 'Game')
        player_latest = self._load_player(latest)
        player_prev = self._load_player(previous)
        record = vs.Record()
        search_args = {"budget": 100, "num_actions": self._num_actions}
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
            "dfpn-root": az_options["dfpn-root"],
            "dfpn-leaf": az_options["dfpn-leaf"],
            "num-simulations": az_options["num-simulations"],
            "num-actions": cl.option(
                f"--{prefix}num-actions",
                default=16,
                show_default=True,
                help="Number of actions to sample",
            ),
            "temperature": az_options["temperature"],
            "random-rate": az_options["random-rate"],
            "jobs": az_options["jobs"],
            "job-size": az_options["job-size"],
        }
