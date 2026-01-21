import os
import sys
import warnings
from datetime import datetime
from glob import glob

import click as cl

from vshogi.dlshogi._cli._nn_trainer import _train_step, _trainer_parameters
from vshogi.dlshogi._cli._self_play_worker import (
    _compute_random_moves,
    _get_previous_models_superior_to_latest,
    _run_self_play,
    _self_play_parameters,
)


def _train(nth_cycle: int, **kwargs):
    weight_path = os.path.join(kwargs['output'], 'models/model_{:04d}.pth')
    _train_step(
        model_path=weight_path.format(nth_cycle),
        prev_model_path=(
            None if nth_cycle == 0 else weight_path.format(nth_cycle - 1)
        ),
        shogi_variant=kwargs['shogi'],
        network_hidden_channels=kwargs['train_hidden_channels'],
        network_bottleneck_channels=kwargs['train_bottleneck_channels'],
        network_backbone_blocks=kwargs['train_backbone_blocks'],
        max_dataset_size=kwargs['train_dataset_size'],
        kifu_path_pattern=os.path.join(
            kwargs['output'],
            "datasets/dataset_*/kifu_*.tsv",
        ),
        kifu_fraction=kwargs['train_kifu_fraction'],
        discount_factor=kwargs['train_discount_factor'],
        importance_decay=kwargs['train_importance_decay'],
        default_result_rate=kwargs['train_default_result_rate'],
        minibatch_size=kwargs['train_minibatch_size'],
        learning_rate=kwargs['train_learning_rate'],
        epochs=0 if nth_cycle == 0 else kwargs['train_epochs'],
        beta2=(
            kwargs['train_beta2']
            if kwargs['train_beta2'] > 0
            else 0.999 ** (kwargs['train_minibatch_size'] / 1024)
        ),
        coeff_policy_loss=kwargs['train_coeff_policy_loss'],
        coeff_entropy_regularization=kwargs['train_coeff_policy_entropy'],
        win_ratio_threshold=kwargs['train_win_ratio_threshold'],
        device=kwargs['train_device'],
    )


def _selfplay(
    tflite_path: str,
    other_path: list[str],
    kifu_dir: str,
    max_random_moves: int,
    **kwargs,
):
    _run_self_play(
        shogi_variant=kwargs['shogi'],
        engine=kwargs['play_engine'],
        tflite_path=tflite_path,
        tflite_path_others=other_path,
        kifu_dir=kifu_dir,
        num_selfplay=kwargs['play_num_games'],
        coeff_puct=kwargs['play_coeff_puct'],
        kldgain_threshold=kwargs['play_kldgain_threshold'],
        dfpn_search_root=kwargs['play_dfpn_root'],
        dfpn_search_leaf=kwargs['play_dfpn_leaf'],
        num_simulations=kwargs['play_num_simulations'],
        temperature=kwargs['play_temperature'],
        q_greedy_depth=kwargs['play_dump_q_greedy_depth'],
        max_random_moves=max_random_moves,
        gumbel_actions=kwargs["play_gumbel_actions"],
        n_jobs=kwargs['play_jobs'],
    )


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@cl.option("--cycles", default=10, show_default=True)
@_self_play_parameters(prefix="play")
@_trainer_parameters(prefix="train")
@cl.option(
    "-o",
    "--output",
    default='',
    show_default=True,
    type=str,
    help='Output directory',
)
def _cycle_selfplay_and_train(**kwargs):
    print('kwargs:', kwargs)

    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    if kwargs['output'] != '':
        if os.path.isdir(kwargs['output']):
            warnings.warn(
                f'Output directory ({kwargs["output"]}) already exists'
            )
        else:
            os.makedirs(kwargs['output'])
    with open(os.path.join(kwargs['output'], f'command_{now}.txt'), 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    tflite_path = os.path.join(kwargs['output'], 'models/model_{:04d}.tflite')

    def _resume_from() -> int:
        tflite_list = sorted(
            glob(os.path.join(kwargs['output'], 'models/model_*.tflite'))
        )
        if not tflite_list:
            return 0
        return int(tflite_list[-1].split('_')[-1].split('.')[0]) + 1

    start = _resume_from()
    if start == 0:
        _train(nth_cycle=0, **kwargs)
        start += 1
    else:
        print(f"Resume cycle from {start}")
    for i in range(start, kwargs['cycles'] + 1):
        max_random_moves = _compute_random_moves(
            kwargs['play_random_rate'],
            os.path.join(kwargs['output'], f'datasets/dataset_{i - 1:04d}'),
        )
        others = _get_previous_models_superior_to_latest(
            shogi_variant=kwargs['shogi'],
            latest=tflite_path.format(i - 1),
            previous=[
                tflite_path.format(j) for j in list(range(i - 2, -1, -1))[:10]
            ],
            engine=kwargs['play_engine'],
            num_games=10,
            coeff_puct=kwargs['play_coeff_puct'],
            n_jobs=kwargs['play_jobs'],
        )
        while True:
            _selfplay(
                tflite_path=None if i == 1 else tflite_path.format(i - 1),
                other_path=others,
                kifu_dir=os.path.join(
                    kwargs['output'],
                    f'datasets/dataset_{i:04d}',
                ),
                max_random_moves=max_random_moves,
                **kwargs,
            )
            _train(nth_cycle=i, **kwargs)
            if os.path.exists(tflite_path.format(i)):
                break
