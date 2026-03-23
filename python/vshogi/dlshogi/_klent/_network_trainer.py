import os
import typing as tp
import warnings
from time import time

import click as cl
import numpy as np
import torch as th

from vshogi.dlshogi import (
    read_kifu,
    train_klent,
    PolicyValueNetwork,
    Data,
)
from vshogi.dlshogi._alpha_zero._network_trainer import (
    _NetworkTrainer as _AlphaZeroNetworkTrainer,
)


class _NetworkTrainer(_AlphaZeroNetworkTrainer):
    def _network_and_optimizer(
        self, candidate_path: list = []
    ) -> tp.Tuple[th.nn.Module, th.optim.Optimizer]:
        network = PolicyValueNetwork(
            game_class=self._game_class,
            hidden_channels=self._network["hiddens"],
            bottleneck_channels=self._network["bottlenecks"],
            num_backbone_blocks=self._network["blocks"],
            action_value_head=True,
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
        train_klent(
            network,
            dataloader,
            optimizer,
            epochs=self._optimization["epochs"],
        )
        network.to(th.device("cpu"))
        print(f"Saving trained parameters: {path}")
        state = {
            "state_dict": network.state_dict(),
            "optimizer": optimizer.state_dict(),
        }
        th.save(state, path)

    def _read_kifu_list(
        self,
        kifu_list: list[str],
        duration_sec: int = 60,
    ) -> list[Data]:
        move_class = self._game_class._get_move_class()
        new_data: list[Data] = []
        start = time()
        for kifu_path in kifu_list:
            if ((time() - start) > duration_sec) or (
                self._last_read_kifu == kifu_path
            ):
                break
            df = read_kifu(
                kifu_path,
                lambda_=self._loss["lambda"],
                result_backup_rate=0.0,
            )
            if len(df) == 0:
                continue
            for _, row in df.iterrows():
                new_data.append(
                    Data(
                        sfen=row["sfen"],
                        policy=row["policy"],
                        value01={move_class(row["move"]): row["value01"]},
                        weight=row["weight"],
                        malignancy=row["malignancy"],
                    )
                )
        self._last_read_kifu = kifu_list[0]
        return new_data

    @staticmethod
    def _aggregate(
        data_list: list[Data],
    ) -> dict[str, list[float]]:
        aggregated = {}
        for d in data_list:
            if d.sfen not in aggregated:
                aggregated[d.sfen] = []
            aggregated[d.sfen].append(
                float(2 * np.nansum(list(d.value01.values())) - 1)
            )
        return aggregated

    @staticmethod
    def _get_cli_options(prefix: str = "") -> list[tp.Callable]:
        az_options = _AlphaZeroNetworkTrainer._get_cli_options(prefix)
        return {
            "device": az_options["device"],
            "network-hiddens": az_options["network-hiddens"],
            "network-bottlenecks": az_options["network-bottlenecks"],
            "network-blocks": az_options["network-blocks"],
            "optimization-epochs": az_options["optimization-epochs"],
            "optimization-minibatch": az_options["optimization-minibatch"],
            "optimization-learning-rate": az_options[
                "optimization-learning-rate"
            ],
            "loss-lambda": cl.option(
                f"--{prefix}loss-lambda",
                default=0.9,
                show_default=True,
                help=(
                    "Hyperparameter used to blend all possible n-step returns."
                ),
            ),
            "validation-threshold": az_options["validation-threshold"],
            "buffer-size": az_options["buffer-size"],
        }
