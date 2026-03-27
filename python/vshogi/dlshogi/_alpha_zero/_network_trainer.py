import os
import tempfile
import typing as tp
import warnings
from collections.abc import Callable
from glob import glob
from time import time

import ai_edge_torch
import click as cl
import numpy as np
import pandas as pd
import torch as th
from tqdm import tqdm

import vshogi as vs
from vshogi.dlshogi import (
    read_kifu,
    Data,
    PolicyValueFunction,
    PolicyValueNetwork,
    ReplayBuffer,
)


class _NetworkTrainer:
    def __init__(self, **kwargs) -> None:
        shogi_module = getattr(vs, kwargs["shogi"])
        self._game_class = getattr(shogi_module, "Game")
        self._device: tp.Literal["cpu", "cuda", "mps"] = kwargs["device"]
        self._network: dict = {
            k.removeprefix("network_"): v
            for k, v in kwargs.items()
            if k.startswith("network_")
        }
        self._optimization = {
            k.removeprefix("optimization_"): v
            for k, v in kwargs.items()
            if k.startswith("optimization_")
        }
        self._loss = {
            k.removeprefix("loss_"): v
            for k, v in kwargs.items()
            if k.startswith("loss_")
        }
        self._validation = {
            k.removeprefix("validation_"): v
            for k, v in kwargs.items()
            if k.startswith("validation_")
        }

        if "beta2" not in self._optimization:
            self._optimization["beta2"] = 0.999 ** (
                self._optimization["minibatch"] / 1024
            )  # https://arxiv.org/abs/2507.07101

        self._buffer = ReplayBuffer(buffer_size=kwargs["buffer_size"])
        self._last_read_kifu: str | None = None

    def __call__(
        self,
        model_path: str,
        prev_model_path: str | None,
        kifu_path_pattern: str = "datasets/dataset_*/kifu_*.tsv",
    ) -> None:
        if not os.path.isdir(os.path.dirname(model_path)):
            os.makedirs(os.path.dirname(model_path))
        network, optimizer = self._network_and_optimizer(
            candidate_path=[model_path, prev_model_path]
        )
        if prev_model_path is None:
            edge_model = self._to_edge_model(network)
            edge_model.export(model_path.replace(".pth", ".tflite"))
            return
        self._add_data_from_kifu(kifu_path_pattern)
        if len(self._buffer) == 0:
            return

        self._train(network, optimizer, model_path)
        edge_model = self._to_edge_model(network)
        with tempfile.TemporaryDirectory(delete=True) as temp_dir:
            file_path = os.path.join(temp_dir, model_path.split("/")[-1])
            edge_model.export(file_path)
            player_curr = _NetworkTrainer._engine(file_path)
        player_prev = _NetworkTrainer._engine(
            prev_model_path.replace('.pth', '.tflite'),
        )
        winner = self._play_games(player_curr, player_prev)
        if player_curr.name == winner:
            edge_model.export(model_path.replace('.pth', '.tflite'))

    def _network_and_optimizer(
        self, candidate_path: list = []
    ) -> tp.Tuple[th.nn.Module, th.optim.Optimizer]:
        network = PolicyValueNetwork(
            game_class=self._game_class,
            hidden_channels=self._network["hiddens"],
            bottleneck_channels=self._network["bottlenecks"],
            num_backbone_blocks=self._network["blocks"],
        )
        optimizer = th.optim.AdamW(
            network.parameters(),
            lr=self._optimization["learning_rate"],
            betas=(0.9, self._optimization["beta2"]),
        )
        for path in candidate_path:
            if path is None:
                continue
            try:
                checkpoint = th.load(path)
                network.load_state_dict(checkpoint["state_dict"])
                optimizer.load_state_dict(checkpoint["optimizer"])
            except Exception:
                if path == candidate_path[0]:
                    if os.path.exists(path):
                        warnings.warn(f"Failed to load: {path}")
                else:
                    if not os.path.exists(path):
                        warnings.warn(f"FileNotFound: {path}")
            else:
                print(f"Loaded file: {path}")
                break
        return network, optimizer

    def _read_kifu_list(
        self,
        kifu_list: list[str],
        duration_sec: int = 60,
    ) -> list[Data]:
        new_data: list[Data] = []
        start = time()
        for kifu_path in kifu_list:
            if ((time() - start) > duration_sec) or (
                self._last_read_kifu == kifu_path
            ):
                break
            df = read_kifu(
                kifu_path,
                result_backup_rate=1.0 - self._loss["q_ratio"],
                lambda_=self._loss["lambda"],
            )
            if len(df) == 0:
                continue
            for _, row in df.iterrows():
                new_data.append(
                    Data(
                        sfen=row["sfen"],
                        policy=row["policy"],
                        value01=row["value01"],
                        weight=row["weight"],
                        malignancy=row["malignancy"],
                    )
                )
        self._last_read_kifu = kifu_list[0]
        return new_data

    def _add_data_from_kifu(self, kifu_path_pattern: str) -> None:
        kifu_dir = sorted(
            glob("/".join(kifu_path_pattern.split("/")[:-1])), reverse=True
        )[0]
        if (self._last_read_kifu is not None) and (
            kifu_dir != os.path.dirname(self._last_read_kifu)
        ):
            print("Removing data from previous policy")
            self._buffer._buffer = []
        kifu_list = sorted(
            glob(kifu_dir + "/" + kifu_path_pattern.split("/")[-1]),
            reverse=True,
        )
        new_data = self._read_kifu_list(kifu_list)
        aggregated = self._aggregate(new_data)
        for d in new_data:
            self._buffer.add(d)
        print(f"Dataset Length = {len(self._buffer)}")
        df_summary = pd.DataFrame(
            [
                {
                    "sfen": sfen,
                    "count": len(a),
                    "value": np.mean(a),
                    "stddev": np.std(a),
                }
                for sfen, a in aggregated.items()
            ]
        )
        print(df_summary.sort_values(by="count", ascending=False).head(n=10))

    @staticmethod
    def _aggregate(
        data_list: list[Data],
    ) -> dict[str, list[float]]:
        aggregated = {}
        for d in data_list:
            if d.sfen not in aggregated:
                aggregated[d.sfen] = []
            aggregated[d.sfen].append(2 * d.value01 - 1)
        return aggregated

    def _train(
        self,
        network: th.nn.Module,
        optimizer: th.optim.Optimizer,
        path: str,
    ) -> None:
        print(f"Start training: {path}")
        device = th.device(self._device)
        network.to(device)
        for state in optimizer.state.values():
            for k, v in state.items():
                if isinstance(v, th.Tensor) and v.device != device:
                    state[k] = v.to(device)
        dataloader = th.utils.data.DataLoader(
            self._buffer,
            batch_size=self._optimization["minibatch"],
            shuffle=True,
            drop_last=True,
        )
        vs.dlshogi.train(
            network,
            dataloader,
            optimizer,
            epochs=self._optimization["epochs"],
            coeff_policy_loss=self._loss["coeff_policy"],
            coeff_entropy_regularization=self._loss["coeff_entropy"],
        )
        network.to(th.device("cpu"))
        print(f"Saving trained parameters: {path}")
        state = {
            "state_dict": network.state_dict(),
            "optimizer": optimizer.state_dict(),
        }
        th.save(state, path)

    def _to_edge_model(self, network: th.nn.Module) -> tp.Any:
        sample_inputs = th.randn(
            1,
            self._game_class.files,
            self._game_class.ranks,
            self._game_class.feature_channels,
        )
        edge_model = ai_edge_torch.convert(network.eval(), (sample_inputs,))
        return edge_model

    @staticmethod
    def _engine(tflite_path: str) -> vs.engine.Engine:
        name: str = tflite_path.split("/")[-1].split(".")[0]
        if os.path.exists(tflite_path):
            return vs.engine.AlphaZero(
                PolicyValueFunction(tflite_path),
                name=name,
            )
        else:
            msg = f"FileNotFound: {tflite_path}"
            warnings.warn(msg)
            return vs.engine.AlphaZero(name=f"{name}_not_found")

    def _play_games(
        self,
        player_curr: vs.engine.Engine,
        player_prev: vs.engine.Engine,
    ) -> str:
        record_curr = vs.Record()
        num_play = 40
        win_threshold = num_play * self._validation["threshold"]
        loss_threshold = num_play * (1 - self._validation["threshold"])
        pbar = tqdm(range(num_play), ncols=100)
        for n in pbar:
            if (record_curr.score() >= win_threshold) or (
                (~record_curr).score() > loss_threshold
            ):
                break
            if n % 2 == 0:
                result = vs.play_game(
                    self._game_class(),
                    player_curr,
                    player_prev,
                    draw_on_max_moves=True,
                ).result
                record_curr += vs.Record.from_black_result(result)
            else:
                result = vs.play_game(
                    self._game_class(),
                    player_prev,
                    player_curr,
                    draw_on_max_moves=True,
                ).result
                record_curr += vs.Record.from_white_result(result)
            pbar.set_description(
                f'{player_curr.name} vs {player_prev.name} '
                f'= {record_curr.wdl()}'
            )
        winner = (
            player_curr.name
            if record_curr.score() >= win_threshold
            else player_prev.name
        )
        print(f"{player_curr.name} vs {player_prev.name}: Winner {winner}")
        return winner

    @classmethod
    def wrap_options(cls, prefix: str = "") -> Callable:
        if prefix and (not prefix.endswith("-")):
            prefix = prefix + "-"
        wrappers = list(cls._get_cli_options(prefix).values())

        def decorator(func: Callable) -> Callable:
            for wrap in reversed(wrappers):
                func = wrap(func)
            return func

        return decorator

    @staticmethod
    def _get_cli_options(prefix: str = "") -> dict[str, tp.Callable]:
        options = {
            "device": cl.option(
                f"--{prefix}device",
                default='cpu',
                type=cl.Choice(['cpu', 'cuda', 'mps']),
                show_default=True,
            ),
            "network-hiddens": cl.option(
                f"--{prefix}network-hiddens",
                type=int,
                callback=lambda ctx, param, value: (
                    value
                    if value is not None
                    else {
                        "minishogi": 64,
                        "judkins_shogi": 64,
                        "shogi": 128,
                    }.get(ctx.params.get("shogi"), 128)
                ),
                help=(
                    "Number of hidden channels in the backbone network. "
                    "Defaults: minishogi=64, judkins_shogi=64, shogi=128."
                ),
            ),
            "network-bottlenecks": cl.option(
                f"--{prefix}network-bottlenecks",
                type=int,
                callback=lambda ctx, param, value: (
                    value
                    if value is not None
                    else {
                        "minishogi": 32,
                        "judkins_shogi": 32,
                        "shogi": 64,
                    }.get(ctx.params.get("shogi"), 64)
                ),
                help=(
                    "Number of bottleneck channels in the backbone network. "
                    "Defaults: minishogi=32, judkins_shogi=32, shogi=64."
                ),
            ),
            "network-blocks": cl.option(
                f"--{prefix}network-blocks",
                type=int,
                callback=lambda ctx, param, value: (
                    value
                    if value is not None
                    else {
                        "minishogi": 3,
                        "judkins_shogi": 4,
                        "shogi": 8,
                    }.get(ctx.params.get("shogi"), 8)
                ),
                help=(
                    "Number of residual blocks in the backbone network. "
                    "Defaults: minishogi=3, judkins_shogi=4, shogi=8."
                ),
            ),
            "optimization-epochs": cl.option(
                f"--{prefix}optimization-epochs", default=5, show_default=True
            ),
            "optimization-minibatch": cl.option(
                f"--{prefix}optimization-minibatch",
                default=32,
                show_default=True,
            ),
            "optimization-learning-rate": cl.option(
                f"--{prefix}optimization-learning-rate",
                default=1e-2,
                show_default=True,
            ),
            "loss-q-ratio": cl.option(
                f"--{prefix}loss-q-ratio",
                default=1.0,
                show_default=True,
                help=(
                    "Blends game outcome z with search evaluation q for the "
                    "value target: q_ratio * q + (1 - q_ratio) * z. "
                    "Set to 0.0 for the original AlphaZero, or increase "
                    "it to train against search estimates (LC0 style)."
                ),
            ),
            "loss-lambda": cl.option(
                f"--{prefix}loss-lambda",
                default=0.6,
                show_default=True,
                help=(
                    "Lambda for computing lambda-returns from MCTS Q-values. "
                    "0.0 uses one-step bootstrapping (raw Q), while larger "
                    "values mix longer-horizon alternating backups. "
                    "Use with non-zero q_ratio."
                ),
            ),
            "loss-coeff-policy": cl.option(
                f"--{prefix}loss-coeff-policy", default=0.1, show_default=True
            ),
            "loss-coeff-entropy": cl.option(
                f"--{prefix}loss-coeff-entropy",
                default=0.1,
                show_default=True,
                help="Coefficient for policy entropy regularization.",
            ),
            "validation-threshold": cl.option(
                f"--{prefix}validation-threshold",
                default=0.55,
                show_default=True,
            ),
            "buffer-size": cl.option(
                f"--{prefix}buffer-size",
                default=100000,
                show_default=True,
                help=(
                    "Maximum size of the replay buffer for storing training "
                    "samples."
                ),
            ),
        }
        return options
