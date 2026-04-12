import os
import typing as tp
import warnings
from glob import glob
from time import time

import click as cl
import numpy as np
import pandas as pd
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
        if self._loss["value_target"] == "aggregate":
            for sfen, a in aggregated.items():
                self._buffer.add(
                    Data(
                        sfen=sfen,
                        policy=a["policy"],
                        value01={
                            m: np.mean(v) for m, v in a["value01"].items()
                        },
                        weight=a["weight"],
                    )
                )
        else:
            for d in new_data:
                self._buffer.add(d)
        print(f"Dataset Length = {len(self._buffer)}")
        df_summary = pd.DataFrame(
            [
                {
                    "sfen": sfen,
                    "count": len(sum((v for v in a["value01"].values()), [])),
                    "value": 2
                    * np.mean(sum((v for v in a["value01"].values()), []))
                    - 1,
                    "stddev": 2
                    * np.std(sum((v for v in a["value01"].values()), [])),
                }
                for sfen, a in aggregated.items()
            ]
        )
        print(df_summary.sort_values(by="count", ascending=False).head(n=10))

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
    ) -> dict[str, dict[str, dict]]:
        aggregated = {}
        for d in data_list:
            if d.sfen not in aggregated:
                aggregated[d.sfen] = {
                    "policy": d.policy,
                    "value01": {},
                    "weight": 0,
                }
            aggregated[d.sfen]["weight"] += d.weight
            for m, v in d.value01.items():
                if m not in aggregated[d.sfen]["value01"]:
                    aggregated[d.sfen]["value01"][m] = []
                aggregated[d.sfen]["value01"][m].append(v)
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
            "loss-value-target": cl.option(
                f"--{prefix}loss-value-target",
                default="as-is",
                type=cl.Choice(["as-is", "aggregate"]),
                show_default=True,
                help=(
                    "Strategy for constructing value targets. 'as-is' uses "
                    "each sample's original value target, while "
                    "'aggregate' replaces it with the aggregated values of "
                    "all samples sharing the same position."
                ),
            ),
            "validation-threshold": az_options["validation-threshold"],
            "max-dataset-size": az_options["max-dataset-size"],
            "min-dataset-size": az_options["min-dataset-size"],
        }
