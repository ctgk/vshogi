import os
import typing as tp
import warnings

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
        self._print_dataset()
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

    def _print_dataset(self) -> None:
        print(f"Dataset Length = {len(self._buffer)}")
        data = {}
        for b in self._buffer._buffer:
            if b.sfen not in data:
                data[b.sfen] = []
            for _ in range(b.count):
                data[b.sfen].append(
                    2 * np.nanmean(list(b.value01.values())) - 1
                )
        df_summary = pd.DataFrame(
            [
                {
                    "sfen": sfen,
                    "count": len(a),
                    "value": np.nanmean(a),
                    "stddev": np.nanstd(a),
                }
                for sfen, a in data.items()
            ]
        )
        print(df_summary.sort_values(by="count", ascending=False).head(n=10))

    def _read_kifu(self, path: str) -> list[Data]:
        df = read_kifu(
            path,
            result_backup_rate=1.0 - self._loss["q_ratio"],
            lambda_=self._loss["lambda"],
            discount_factor=self._loss["discount_factor"],
            follow_any_path=(self._loss["backup_path"] == "any"),
        )
        return [
            Data(
                sfen=row["sfen"],
                policy=row["policy"],
                value01={row["move"]: row["value01"]},
                weight=row["weight"],
                malignancy=row["malignancy"],
            )
            for _, row in df.iterrows()
        ]

    @staticmethod
    def _get_cli_options(prefix: str = "") -> list[tp.Callable]:
        az_options = _AlphaZeroNetworkTrainer._get_cli_options(prefix)
        return {
            k: v
            for k, v in az_options.items()
            if k not in ("loss-coeff-policy", "loss-coeff-entropy")
        }
