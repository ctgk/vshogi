import os
import tempfile
import typing as tp
import warnings
from glob import glob

import click as cl
import pandas as pd
import torch as th
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
            warnings.warn(f'Failed loading weight: {path}')
        else:
            break
    return network


def _dataset(
    shogi_variant: str,
    max_dataset_size: int,
    kifu_path_pattern: str,
    *,
    kifu_fraction: float = 1.,
    discount_factor: float = 1.,
    importance_decay: float = 1.,
) -> th.utils.data.Dataset:
    move_type: type = getattr(getattr(vs, shogi_variant), 'Move')
    buffer = vs.dlshogi.ReplayBuffer(buffer_size=max_dataset_size)
    kifu_dir_list = sorted(
        glob('/'.join(kifu_path_pattern.split('/')[:-1])),
        reverse=True,
    )
    for kifu_dir, fr in zip(
        kifu_dir_list,
        (kifu_fraction ** i for i in range(len(kifu_dir_list))),
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
            )
            df = df.tail(int(len(df) * fr))
            for _, row in df.iterrows():
                buffer.add(vs.dlshogi.Data(
                    sfen=row['sfen'],
                    policy={move_type(m): v for m, v in row['policy'].items()},
                    value01=row['value01'],
                    weight=row['weight'],
                ))
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
    print(df_summary.sort_values(
        by='count', ascending=False).head(n=10)[['sfen', 'value', 'count']])
    return buffer


def _engine(engine: str, tflite_path: str, name: str) -> vs.engine.Engine:
    engine_class = {'AlphaZero': vs.engine.AlphaZero}[engine]
    if os.path.exists(tflite_path):
        return engine_class(
            vs.dlshogi.PolicyValueFunction(tflite_path),
            name=name,
        )
    else:
        warnings.warn(f"tflite model, {tflite_path}, not found")
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
        if (
            (record_curr.score() >= win_threshold)
            or ((~record_curr).score() > loss_threshold)
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
            f'{player_curr.name} vs {player_best.name} = {record_curr.wdl()}')
    return (
        player_curr.name if record_curr.score() >= win_threshold
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
    import ai_edge_torch
    shogi_module = getattr(vs, shogi_variant)
    game_class = getattr(shogi_module, 'Game')
    network = _network(
        game_class,
        network_hidden_channels,
        network_bottleneck_channels,
        network_backbone_blocks,
        weight_candidate_path=[model_path, prev_model_path],
    )
    dataset = _dataset(
        shogi_variant=shogi_variant,
        max_dataset_size=max_dataset_size,
        kifu_path_pattern=kifu_path_pattern,
        kifu_fraction=kifu_fraction,
        discount_factor=discount_factor,
        importance_decay=importance_decay,
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
    edge_model = ai_edge_torch.convert(network.eval(), sample_inputs)
    if prev_model_path is None:
        edge_model.export(model_path.replace('.pth', '.tflite'))
        return

    with tempfile.NamedTemporaryFile(delete=True) as t:
        edge_model.export(t.name)
        player_curr = _engine(
            engine,
            t.name,
            name=model_path.split('/')[-1].split('.')[0],
        )
    player_prev = _engine(
        engine,
        prev_model_path.replace('.pth', '.tflite'),
        name=prev_model_path.split('/')[-1].split('.')[0],
    )
    name_better = _get_best_player_index(
        game_class, player_curr, player_prev,
        {'n_or_t': 100}, {'temperature': None},
        win_ratio_threshold=win_ratio_threshold,
    )
    if player_curr.name == name_better:
        edge_model.export(model_path.replace('.pth', '.tflite'))


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@cl.option("--network-hidden-channels", default=32)
@cl.option("--network-bottleneck-channels", default=8)
@cl.option("--network-backbone-blocks", default=4)
@cl.option("--max-dataset-size", default=100000)
@cl.option("--kifu-path-pattern", default="datasets/dataset_*/kifu_*.tsv")
@cl.option("--kifu-fraction", default=0.8)
@cl.option("--discount-factor", default=0.99)
@cl.option("--importance-decay", default=0.7)
@cl.option("--minibatch-size", default=32)
@cl.option("--learning-rate", default=1e-2)
@cl.option("--epochs", default=5)
@cl.option("--coeff-policy-loss", default=0.1)
@cl.option("--coeff-entropy-regularization", default=1e-2)
@cl.option("--grad-accumulations", default=1)
@cl.option("--win-ratio-threshold", default=0.55)
@cl.option("--device", default='cpu')
def _nn_trainer(
    shogi: tp.Literal['minishogi', 'judkins_shogi', 'shogi'],
    network_hidden_channels: int,
    network_bottleneck_channels: int,
    network_backbone_blocks: int,
    max_dataset_size: int,
    kifu_path_pattern: str,
    kifu_fraction: float,
    discount_factor: float,
    importance_decay: float,
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
    ii = 0
    while ii < 10000:
        model_path = f'models/model_{ii:04d}.pth'
        prev_model_path = f'models/model_{ii - 1:04d}.pth'
        if not os.path.isdir(os.path.dirname(model_path)):
            os.makedirs(os.path.dirname(model_path))
        _train_step(
            model_path=model_path,
            prev_model_path=None if ii == 0 else prev_model_path,
            shogi_variant=shogi,
            network_hidden_channels=network_hidden_channels,
            network_bottleneck_channels=network_bottleneck_channels,
            network_backbone_blocks=network_backbone_blocks,
            max_dataset_size=max_dataset_size,
            kifu_path_pattern=kifu_path_pattern,
            kifu_fraction=kifu_fraction,
            discount_factor=discount_factor,
            importance_decay=importance_decay,
            minibatch_size=minibatch_size,
            learning_rate=learning_rate,
            epochs=epochs,
            coeff_policy_loss=coeff_policy_loss,
            coeff_entropy_regularization=coeff_entropy_regularization,
            grad_accumulations=grad_accumulations,
            win_ratio_threshold=win_ratio_threshold,
            device=device,
            engine=engine,
        )
        if os.path.exists(model_path.replace('.pth', '.tflite')):
            ii += 1


if __name__ == '__main__':
    _nn_trainer()
