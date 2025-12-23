import os
import sys
import tempfile
import typing as tp
import warnings
from datetime import datetime
from glob import glob

import click as cl
import pandas as pd
import torch as th
from executorch.backends.xnnpack.partition.xnnpack_partitioner import (
    XnnpackPartitioner,
)
from executorch.exir import to_edge_transform_and_lower
from tqdm import tqdm

import vshogi as vs


def _network(
    game_class: type,
    hidden_channels: int,
    bottleneck_channels: int,
    num_backbone_blocks: int,
    weight_candidate_path: list = [],
):
    network = vs.dlshogi.PolicyValueNetwork(
        game_class=game_class,
        hidden_channels=hidden_channels,
        bottleneck_channels=bottleneck_channels,
        num_backbone_blocks=num_backbone_blocks,
    )
    for path in weight_candidate_path:
        if path is None:
            continue
        try:
            network.load_state_dict(th.load(path, weights_only=True))
        except Exception:
            if path == weight_candidate_path[0]:
                if os.path.exists(path):
                    warnings.warn(f'Failed loading weight: {path}')
            else:
                if not os.path.exists(path):
                    warnings.warn(f'Weight file not found: {path}')
        else:
            break
    return network


def _dataset(
    max_dataset_size: int,
    kifu_path_pattern: str,
    *,
    kifu_fraction: float = 1.0,
    discount_factor: float = 1.0,
    importance_decay: float = 1.0,
    default_result_rate: float = 1.0,
) -> th.utils.data.Dataset:
    buffer = vs.dlshogi.ReplayBuffer(buffer_size=max_dataset_size)
    kifu_dir_list = sorted(
        glob('/'.join(kifu_path_pattern.split('/')[:-1])),
        reverse=True,
    )
    for kifu_dir, fr in zip(
        kifu_dir_list,
        (kifu_fraction**i for i in range(len(kifu_dir_list))),
    ):
        if fr < 0.01:
            break
        kifu_list = sorted(
            glob(kifu_dir + '/' + kifu_path_pattern.split('/')[-1]),
            reverse=True,
        )
        for kifu_path in kifu_list:
            df = vs.dlshogi.read_kifu(
                kifu_path,
                discount_factor=discount_factor,
                importance_decay=importance_decay,
                default_result_rate=default_result_rate,
            )
            df = df.tail(int(len(df) * fr))
            for _, row in df.iterrows():
                buffer.add(
                    vs.dlshogi.Data(
                        sfen=row['sfen'],
                        policy={m: v for m, v in row['policy'].items()},
                        value01=row['value01'],
                        weight=row['weight'],
                    )
                )
            if buffer.is_full():
                break
        if buffer.is_full():
            break
    summary = buffer.deduplicate()
    df_summary = pd.DataFrame(
        [
            {
                'sfen': s,
                'count': data['count'],
                'value': 2 * data['value01'] - 1,
            }
            for s, data in summary.items()
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


def _engine(engine: str, pte_path: str, name: str) -> vs.engine.Engine:
    engine_class = {'AlphaZero': vs.engine.AlphaZero}[engine]
    if os.path.exists(pte_path):
        return engine_class(
            vs.dlshogi.PolicyValueFunction(pte_path),
            name=name,
        )
    else:
        warnings.warn(f"pte model, {pte_path}, not found")
        return engine_class(name=f'{name}_not_found')


def _train(
    model: th.nn.Module,
    dataset: th.utils.data.Dataset,
    minibatch_size: int,
    learning_rate: float,
    epochs: int,
    coeff_policy_loss: float = 0.1,
    coeff_entropy_regularization: float = 0.01,
    grad_accumulations: int = 1,
):
    dataloader = th.utils.data.DataLoader(
        dataset,
        batch_size=minibatch_size,
        shuffle=True,
        drop_last=True,
    )
    optimizer = th.optim.AdamW(model.parameters(), learning_rate)
    vs.dlshogi.train(
        model,
        dataloader,
        optimizer,
        epochs=epochs,
        coeff_policy_loss=coeff_policy_loss,
        coeff_entropy_regularization=coeff_entropy_regularization,
        gradient_accumulation_steps=grad_accumulations,
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
    model_path: str,
    prev_model_path: str | None,
    shogi_variant: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    network_hidden_channels: int,
    network_bottleneck_channels: int,
    network_backbone_blocks: int,
    max_dataset_size: int,
    kifu_path_pattern: str,
    kifu_fraction: float,
    discount_factor: float,
    importance_decay: float,
    default_result_rate: float,
    minibatch_size: int,
    learning_rate: float,
    epochs: int,
    coeff_policy_loss: float,
    coeff_entropy_regularization: float,
    grad_accumulations: int,
    win_ratio_threshold: float,
    device: tp.Literal['cpu', 'cuda', 'mps'],
    engine: tp.Literal['AlphaZero'] = 'AlphaZero',
):
    if not os.path.isdir(os.path.dirname(model_path)):
        os.makedirs(os.path.dirname(model_path))
    shogi_module = getattr(vs, shogi_variant)
    game_class = getattr(shogi_module, 'Game')
    network = _network(
        game_class,
        network_hidden_channels,
        network_bottleneck_channels,
        network_backbone_blocks,
        weight_candidate_path=[model_path, prev_model_path],
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
        exported = th.export.export(network.eval(), sample_inputs, strict=True)
        edge_model = to_edge_transform_and_lower(
            exported, partitioner=[XnnpackPartitioner()]
        ).to_executorch()
        with open(model_path.replace('.pth', '.pte'), 'wb') as f:
            f.write(edge_model.buffer)
        return
    dataset = _dataset(
        max_dataset_size=max_dataset_size,
        kifu_path_pattern=kifu_path_pattern,
        kifu_fraction=kifu_fraction,
        discount_factor=discount_factor,
        importance_decay=importance_decay,
        default_result_rate=default_result_rate,
    )
    if len(dataset) != 0:
        print(f"Start training: {model_path}")
        network.to(th.device(device))
        _train(
            network,
            dataset,
            minibatch_size=minibatch_size,
            learning_rate=learning_rate,
            epochs=epochs,
            coeff_policy_loss=coeff_policy_loss,
            coeff_entropy_regularization=coeff_entropy_regularization,
            grad_accumulations=grad_accumulations,
        )
        network.to(th.device('cpu'))
        print(f"Saving trained parameters: {model_path}")
        th.save(network.state_dict(), model_path)

    sample_inputs = (
        th.randn(
            1,
            game_class.files,
            game_class.ranks,
            game_class.feature_channels,
        ),
    )
    exported = th.export.export(network.eval(), sample_inputs, strict=True)
    edge_model = to_edge_transform_and_lower(
        exported, partitioner=[XnnpackPartitioner()]
    ).to_executorch()

    with tempfile.NamedTemporaryFile(delete=True) as t:
        t.write(edge_model.buffer)
        player_curr = _engine(
            engine,
            t.name,
            name=model_path.split('/')[-1].split('.')[0],
        )
    player_prev = _engine(
        engine,
        prev_model_path.replace('.pth', '.pte'),
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
    if player_curr.name == name_better:
        with open(model_path.replace('.pth', '.pte'), 'wb') as f:
            f.write(edge_model.buffer)


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@cl.option("--hidden-channels", default=128, show_default=True)
@cl.option("--bottleneck-channels", default=32, show_default=True)
@cl.option("--backbone-blocks", default=4, show_default=True)
@cl.option("--dataset-size", default=100000, show_default=True)
@cl.option("--kifu-path-pattern", default="datasets/dataset_*/kifu_*.tsv")
@cl.option("--kifu-fraction", default=0.8, show_default=True)
@cl.option("--discount-factor", default=0.99, show_default=True)
@cl.option("--importance-decay", default=0.7, show_default=True)
@cl.option("--default-result-rate", default=0.5, show_default=True)
@cl.option("--minibatch-size", default=32, show_default=True)
@cl.option("--learning-rate", default=1e-2, show_default=True)
@cl.option("--epochs", default=5, show_default=True)
@cl.option("--coeff-policy-loss", default=0.1, show_default=True)
@cl.option("--coeff-policy-entropy", default=1e-2, show_default=True)
@cl.option("--grad-accumulations", default=1, show_default=True)
@cl.option("--win-ratio-threshold", default=0.55, show_default=True)
@cl.option(
    "--device",
    default='cpu',
    type=cl.Choice(['cpu', 'cuda', 'mps']),
    show_default=True,
)
@cl.option(
    "--engine",
    default='AlphaZero',
    type=cl.Choice(['AlphaZero']),
    show_default=True,
)
def _nn_trainer(**kwargs):
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    def _resume_from() -> int:
        pte_list = sorted(glob('models/model_*.pte'))
        if not pte_list:
            return 0
        return int(pte_list[-1].split('_')[-1].split('.')[0]) + 1

    ii = _resume_from()
    model_path = 'models/model_{:04d}.pth'
    while ii < 10000:
        _train_step(
            model_path=model_path.format(ii),
            prev_model_path=None if ii == 0 else model_path.format(ii - 1),
            shogi_variant=kwargs['shogi'],
            network_hidden_channels=kwargs['hidden_channels'],
            network_bottleneck_channels=kwargs['bottleneck_channels'],
            network_backbone_blocks=kwargs['backbone_blocks'],
            max_dataset_size=kwargs['dataset_size'],
            kifu_path_pattern=kwargs['kifu_path_pattern'],
            kifu_fraction=kwargs['kifu_fraction'],
            discount_factor=kwargs['discount_factor'],
            importance_decay=kwargs['importance_decay'],
            default_result_rate=kwargs['default_result_rate'],
            minibatch_size=kwargs['minibatch_size'],
            learning_rate=kwargs['learning_rate'],
            epochs=0 if ii == 0 else kwargs['epochs'],
            coeff_policy_loss=kwargs['coeff_policy_loss'],
            coeff_entropy_regularization=kwargs['coeff_policy_entropy'],
            grad_accumulations=kwargs['grad_accumulations'],
            win_ratio_threshold=kwargs['win_ratio_threshold'],
            device=kwargs['device'],
            engine=kwargs['engine'],
        )
        if os.path.exists(model_path.format(ii).replace('.pth', '.pte')):
            ii += 1


if __name__ == '__main__':
    _nn_trainer()
