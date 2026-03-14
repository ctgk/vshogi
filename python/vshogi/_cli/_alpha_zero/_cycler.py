import os
import sys
import warnings
from datetime import datetime
from glob import glob

import click as cl

from vshogi._cli._alpha_zero._nn_trainer import _NetworkTrainer
from vshogi._cli._alpha_zero._self_play_worker import (
    _compute_random_moves,
    _get_previous_models_superior_to_latest,
    _run_self_play,
    _self_play_parameters,
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
        max_random_moves=max_random_moves,
        n_jobs=kwargs['play_jobs'],
    )


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@cl.option("--cycles", default=10, show_default=True)
@_self_play_parameters(prefix="play")
@_NetworkTrainer.wrap_options(prefix="train")
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
    weight_path = os.path.join(kwargs['output'], 'models/model_{:04d}.pth')
    trainer = _NetworkTrainer(
        shogi_variant=kwargs["shogi"],
        buffer_size=kwargs["train_buffer_size"],
        device=kwargs["train_device"],
        network={
            "hidden_channels": kwargs["train_hidden_channels"],
            "bottleneck_channels": kwargs["train_bottleneck_channels"],
            "backbone_blocks": kwargs["train_backbone_blocks"],
        },
        optimization={
            "learning_rate": kwargs["train_learning_rate"],
            "epochs": kwargs["train_epochs"],
            "minibatch": kwargs["train_minibatch_size"],
        },
        loss={
            "coeff_policy": kwargs["train_coeff_policy_loss"],
            "coeff_entropy": kwargs["train_coeff_policy_entropy"],
        },
        validation={"threshold": kwargs["train_win_ratio_threshold"]},
    )
    if start == 0:
        trainer(
            weight_path.format(0),
            None,
            os.path.join(kwargs["output"], "datasets/dataset_*/kifu_*.tsv"),
        )
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
            trainer(
                weight_path.format(i),
                weight_path.format(i - 1),
                os.path.join(
                    kwargs["output"], "datasets/dataset_*/kifu_*.tsv"
                ),
            )
            if os.path.exists(tflite_path.format(i)):
                break
