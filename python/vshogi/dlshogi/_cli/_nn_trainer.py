import os
import sys
import tempfile
import typing as tp
import warnings
from collections.abc import Callable
from datetime import datetime
from glob import glob
from time import time

import click as cl
import numpy as np
import pandas as pd
import torch as th
from tqdm import tqdm

import vshogi as vs
from vshogi.dlshogi import ReplayBuffer


def _trainer_parameters(prefix: str = "") -> callable:
    if prefix and (not prefix.endswith("-")):
        prefix = prefix + "-"
    wrappers = [
        cl.option(
            f"--{prefix}hidden-channels",
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
                "Number of hidden channels in the neural network backbone. "
                "Variant defaults: minishogi=64, judkins_shogi=64, shogi=128."
            ),
        ),
        cl.option(
            f"--{prefix}bottleneck-channels",
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
                "Variant defaults: minishogi=32, judkins_shogi=32, shogi=64."
            ),
        ),
        cl.option(
            f"--{prefix}backbone-blocks",
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
                "Number of residual blocks in the neural network backbone. "
                "Variant defaults: minishogi=3, judkins_shogi=4, shogi=8."
            ),
        ),
        cl.option(
            f"--{prefix}buffer-size",
            default=100000,
            show_default=True,
            help=(
                "Maximum size of the replay buffer for storing training "
                "samples."
            ),
        ),
        cl.option(
            f"--{prefix}buffer-decay",
            default=0.0,
            show_default=True,
            help="Decay factor for the replay buffer.",
        ),
        cl.option(
            f"--{prefix}per/--{prefix}no-per",
            default=False,
            show_default=True,
            help="Enable/Disable prioritized experience replay.",
        ),
        cl.option(
            f"--{prefix}kifu-fraction",
            default=1.0,
            show_default=True,
            help=(
                "Fraction of samples to use from each kifu file (0.0 to 1.0). "
                "Use values less than 1.0 to subsample the dataset."
            ),
        ),
        cl.option(
            f"--{prefix}discount-factor",
            default=0.99,
            show_default=True,
        ),
        cl.option(
            f"--{prefix}importance-decay",
            default=0.7,
            show_default=True,
        ),
        cl.option(
            f"--{prefix}backup-result",
            type=cl.Choice(["always", "best"]),
            default="always",
            show_default=True,
            help=(
                "Specify when to backup the result. "
                "Options are 'always' to backup every time or "
                "'best' to backup only if following actions are all best. "
                "The original AlphaZero trains on 'always' setting."
            ),
        ),
        cl.option(
            f"--{prefix}result-backup-rate",
            default=0.1,
            show_default=True,
        ),
        cl.option(f"--{prefix}minibatch-size", default=32, show_default=True),
        cl.option(f"--{prefix}learning-rate", default=1e-2, show_default=True),
        cl.option(f"--{prefix}epochs", default=5, show_default=True),
        cl.option(
            f"--{prefix}beta2",
            type=float,
            callback=lambda ctx, param, value: (
                value
                if value is not None
                else 0.999
                ** (
                    ctx.params.get(f"{prefix}minibatch-size".replace("-", "_"))
                    / 1024
                )
            ),
            help=(
                "2nd-moment decay (`beta2`) of Adam optimizer. "
                "Omit to auto-tune by batch size: "
                "`beta2 = 0.999 ** (minibatch_size / 1024)`. "
                "This keeps the effective averaging window comparable "
                "across different minibatch sizes."
            ),
        ),
        cl.option(
            f"--{prefix}resume-optimizer/--{prefix}reset-optimizer",
            default=True,
            show_default=True,
            help="Resume/Reset optimizer state.",
        ),
        cl.option(
            f"--{prefix}coeff-policy-loss", default=0.1, show_default=True
        ),
        cl.option(
            f"--{prefix}coeff-policy-entropy",
            default=0.1,
            show_default=True,
            help="Coefficient for policy entropy regularization.",
        ),
        cl.option(
            f"--{prefix}win-ratio-threshold", default=0.55, show_default=True
        ),
        cl.option(
            f"--{prefix}device",
            default='cpu',
            type=cl.Choice(['cpu', 'cuda', 'mps']),
            show_default=True,
        ),
    ]

    def decorator(func: callable) -> callable:
        for wrap in reversed(wrappers):
            func = wrap(func)
        return func

    return decorator


def _network_and_optimizer(
    game_class: type,
    hidden_channels: int,
    bottleneck_channels: int,
    num_backbone_blocks: int,
    learning_rate: float,
    beta2: float,
    candidate_path: list = [],
    load_optimizer_state: bool = True,
):
    network = vs.dlshogi.PolicyValueNetwork(
        game_class=game_class,
        hidden_channels=hidden_channels,
        bottleneck_channels=bottleneck_channels,
        num_backbone_blocks=num_backbone_blocks,
    )
    optimizer = th.optim.AdamW(
        network.parameters(),
        lr=learning_rate,
        betas=(0.9, beta2),
    )
    print(f"{candidate_path=}")
    for path in candidate_path:
        if path is None:
            continue
        try:
            checkpoint = th.load(path)
            network.load_state_dict(checkpoint["state_dict"])
            if load_optimizer_state:
                optimizer.load_state_dict(checkpoint["optimizer"])
        except Exception:
            if path == candidate_path[0]:
                if os.path.exists(path):
                    warnings.warn(f'Failed loading: {path}')
            else:
                if not os.path.exists(path):
                    warnings.warn(f'File not found: {path}')
        else:
            print(f"Loaded file: {path}")
            break
    return network, optimizer


def _dataset(
    buffer: ReplayBuffer,
    kifu_path_pattern: str,
    *,
    kifu_fraction: float = 1.0,
    discount_factor: float = 1.0,
    importance_decay: float = 1.0,
    always_backup_result: bool = False,
    result_backup_rate: float = 1.0,
    value_func: Callable[[str], float] | None = None,
) -> th.utils.data.Dataset:
    buffer._first = None
    if not hasattr(buffer, "_last"):
        buffer._last = None
    count: dict[str, int] = {}
    kifu_dir_list = sorted(
        glob('/'.join(kifu_path_pattern.split('/')[:-1])),
        reverse=True,
    )
    start = time()
    for kifu_dir, fr in zip(
        kifu_dir_list,
        (0.8**i for i in range(len(kifu_dir_list))),
    ):
        if fr < 0.01 or (time() - start) > 60:
            break
        kifu_list = sorted(
            glob(kifu_dir + '/' + kifu_path_pattern.split('/')[-1]),
            reverse=True,
        )
        for kifu_path in kifu_list:
            if buffer._first is None:
                buffer._first = kifu_path
            if buffer._last == kifu_path:
                buffer._last = buffer._first
                break
            df = vs.dlshogi.read_kifu(
                kifu_path,
                discount_factor=discount_factor,
                importance_decay=importance_decay,
                result_backup_rate=result_backup_rate,
                tail_fraction=fr,
                always_backup_result=always_backup_result,
            )
            if len(df) == 0:
                continue
            if value_func is None:
                df["priority"] = 1.0
            else:
                df["priority"] = df.apply(
                    lambda row: max(
                        np.abs(row["q_value"] - value_func(row["sfen"])),
                        1e-4,
                    ),
                    axis=1,
                )
            df = df.sample(
                **(
                    {"n": 1}
                    if len(df) * kifu_fraction < 1
                    else {"frac": kifu_fraction}
                ),
                weights="priority",
            )
            for _, row in df.iterrows():
                data = vs.dlshogi.Data(
                    sfen=row['sfen'],
                    policy=row['policy'],
                    value01=row['value01'],
                    weight=row['weight'],
                )
                buffer.add(data)
                if data.sfen not in count:
                    count[data.sfen] = 0
                count[data.sfen] += 1
            if (time() - start) > 60:
                break
        if (buffer._last == buffer._first) or (time() - start) > 60:
            break
    buffer._last = buffer._first
    df_summary = pd.DataFrame(
        [
            {
                'sfen': sfen,
                'count': v,
                'value': (
                    2 * getattr(buffer.get_ema_of(sfen), "value01", np.nan) - 1
                ),
            }
            for sfen, v in count.items()
        ],
        columns=['sfen', 'count', 'value'],
    )
    print(f"Dataset size = {len(buffer)}")
    print(
        df_summary.sort_values(by='count', ascending=False).head(n=10)[
            ['sfen', 'value', 'count']
        ]
    )
    return buffer


def _engine(tflite_path: str, name: str) -> vs.engine.Engine:
    if os.path.exists(tflite_path):
        return vs.engine.AlphaZero(
            vs.dlshogi.PolicyValueFunction(tflite_path),
            name=name,
        )
    else:
        warnings.warn(f"tflite model, {tflite_path}, not found")
        return vs.engine.AlphaZero(name=f'{name}_not_found')


def _train(
    model: th.nn.Module,
    dataset: th.utils.data.Dataset,
    optimizer: th.optim.Optimizer,
    minibatch_size: int,
    epochs: int,
    coeff_policy_loss: float = 0.1,
    coeff_entropy_regularization: float = 0.01,
):
    dataloader = th.utils.data.DataLoader(
        dataset,
        batch_size=minibatch_size,
        shuffle=True,
        drop_last=True,
    )
    vs.dlshogi.train(
        model,
        dataloader,
        optimizer,
        epochs=epochs,
        coeff_policy_loss=coeff_policy_loss,
        coeff_entropy_regularization=coeff_entropy_regularization,
    )


def _get_best_player_index(
    game_class: type,
    player_curr: vs.engine.Engine,
    player_best: vs.engine.Engine,
    search_args: dict,
    select_args: dict,
    win_ratio_threshold: float = 0.55,
):
    record_curr = vs.Record()
    num_play = 40
    win_threshold = num_play * win_ratio_threshold
    loss_threshold = num_play * (1 - win_ratio_threshold)
    pbar = tqdm(range(num_play), ncols=100)
    for n in pbar:
        if (record_curr.score() >= win_threshold) or (
            (~record_curr).score() > loss_threshold
        ):
            break
        if n % 2 == 0:
            result = vs.play_game(
                game_class(),
                player_curr,
                player_best,
                search_args=search_args,
                select_args=select_args,
                draw_on_max_moves=True,
            ).result
            record_curr += vs.Record.from_black_result(result)
        else:
            result = vs.play_game(
                game_class(),
                player_best,
                player_curr,
                search_args=search_args,
                select_args=select_args,
                draw_on_max_moves=True,
            ).result
            record_curr += vs.Record.from_white_result(result)
        pbar.set_description(
            f'{player_curr.name} vs {player_best.name} = {record_curr.wdl()}'
        )
    return (
        player_curr.name
        if record_curr.score() >= win_threshold
        else player_best.name
    )


def _train_step(
    buffer: ReplayBuffer,
    model_path: str,
    prev_model_path: str | None,
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    network_hidden_channels: int,
    network_bottleneck_channels: int,
    network_backbone_blocks: int,
    kifu_path_pattern: str,
    prioritized_experience_replay: bool,
    kifu_fraction: float,
    discount_factor: float,
    importance_decay: float,
    always_backup_result: bool,
    result_backup_rate: float,
    minibatch_size: int,
    learning_rate: float,
    epochs: int,
    beta2: float,
    load_optimizer_state: bool,
    coeff_policy_loss: float,
    coeff_entropy_regularization: float,
    win_ratio_threshold: float,
    device: tp.Literal['cpu', 'cuda', 'mps'],
):
    import ai_edge_torch

    if not os.path.isdir(os.path.dirname(model_path)):
        os.makedirs(os.path.dirname(model_path))
    shogi_module = getattr(vs, shogi_variant)
    game_class = getattr(shogi_module, 'Game')
    network, optimizer = _network_and_optimizer(
        game_class,
        network_hidden_channels,
        network_bottleneck_channels,
        network_backbone_blocks,
        learning_rate=learning_rate,
        beta2=beta2,
        candidate_path=[model_path, prev_model_path],
        load_optimizer_state=load_optimizer_state,
    )
    if epochs == 0:
        sample_inputs = (
            th.randn(
                1,
                game_class.files,
                game_class.ranks,
                game_class.feature_channels,
            ),
        )
        edge_model = ai_edge_torch.convert(network.eval(), sample_inputs)
        edge_model.export(model_path.replace('.pth', '.tflite'))
        return

    if prioritized_experience_replay and (prev_model_path is not None):
        pv_func = vs.dlshogi.PolicyValueFunction(
            prev_model_path.replace(".pth", ".tflite")
        )

        def value_func(sfen: str) -> float:
            return pv_func(game_class(sfen))[1]
    else:
        value_func = None

    buffer = _dataset(
        buffer=buffer,
        kifu_path_pattern=kifu_path_pattern,
        kifu_fraction=kifu_fraction,
        discount_factor=discount_factor,
        importance_decay=importance_decay,
        always_backup_result=always_backup_result,
        result_backup_rate=result_backup_rate,
        value_func=value_func,
    )
    if len(buffer) != 0:
        print(f"Start training: {model_path}")
        network.to(th.device(device))
        for state in optimizer.state.values():
            for k, v in state.items():
                if isinstance(v, th.Tensor) and v.device != th.device(device):
                    state[k] = v.to(th.device(device))
        _train(
            network,
            buffer,
            optimizer,
            minibatch_size=minibatch_size,
            epochs=epochs,
            coeff_policy_loss=coeff_policy_loss,
            coeff_entropy_regularization=coeff_entropy_regularization,
        )
        network.to(th.device('cpu'))
        print(f"Saving trained parameters: {model_path}")
        state = {
            "state_dict": network.state_dict(),
            "optimizer": optimizer.state_dict(),
        }
        th.save(state, model_path)

    sample_inputs = (
        th.randn(
            1,
            game_class.files,
            game_class.ranks,
            game_class.feature_channels,
        ),
    )
    edge_model = ai_edge_torch.convert(network.eval(), sample_inputs)

    with tempfile.NamedTemporaryFile(delete=True) as t:
        edge_model.export(t.name)
        player_curr = _engine(
            t.name,
            name=model_path.split('/')[-1].split('.')[0],
        )
    player_prev = _engine(
        prev_model_path.replace('.pth', '.tflite'),
        name=prev_model_path.split('/')[-1].split('.')[0],
    )
    name_better = _get_best_player_index(
        game_class,
        player_curr,
        player_prev,
        {'budget': 100},
        {'temperature': None},
        win_ratio_threshold=win_ratio_threshold,
    )
    print(f"{player_curr.name} vs {player_prev.name}: Winner {name_better}")
    if player_curr.name == name_better:
        edge_model.export(model_path.replace('.pth', '.tflite'))


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@_trainer_parameters(prefix="")
def _nn_trainer(**kwargs):
    print(f"{kwargs=}")
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    def _resume_from() -> int:
        tflite_list = sorted(glob('models/model_*.tflite'))
        if not tflite_list:
            return 0
        return int(tflite_list[-1].split('_')[-1].split('.')[0]) + 1

    ii = _resume_from()
    buffer = ReplayBuffer(
        buffer_size=kwargs["buffer_size"],
        alpha=kwargs["buffer_decay"],
    )
    model_path = 'models/model_{:04d}.pth'
    while ii < 10000:
        _train_step(
            buffer=buffer,
            model_path=model_path.format(ii),
            prev_model_path=None if ii == 0 else model_path.format(ii - 1),
            shogi_variant=kwargs['shogi'],
            network_hidden_channels=kwargs['hidden_channels'],
            network_bottleneck_channels=kwargs['bottleneck_channels'],
            network_backbone_blocks=kwargs['backbone_blocks'],
            kifu_path_pattern="datasets/dataset_*/kifu_*.tsv",
            prioritized_experience_replay=kwargs["per"],
            kifu_fraction=kwargs['kifu_fraction'],
            discount_factor=kwargs['discount_factor'],
            importance_decay=kwargs['importance_decay'],
            always_backup_result=kwargs["backup_result"] == "always",
            result_backup_rate=kwargs['result_backup_rate'],
            minibatch_size=kwargs['minibatch_size'],
            learning_rate=kwargs['learning_rate'],
            epochs=0 if ii == 0 else kwargs['epochs'],
            beta2=kwargs['beta2'],
            load_optimizer_state=kwargs["resume_optimizer"],
            coeff_policy_loss=kwargs['coeff_policy_loss'],
            coeff_entropy_regularization=kwargs['coeff_policy_entropy'],
            win_ratio_threshold=kwargs['win_ratio_threshold'],
            device=kwargs['device'],
        )
        if os.path.exists(model_path.format(ii).replace('.pth', '.tflite')):
            ii += 1


if __name__ == '__main__':
    _nn_trainer()
