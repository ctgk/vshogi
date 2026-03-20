import os
import sys
import warnings
from datetime import datetime
from glob import glob

import click as cl

from vshogi.dlshogi._klent._network_trainer import _NetworkTrainer
from vshogi.dlshogi._klent._self_play_worker import _SelfPlayWorker
from vshogi.dlshogi._cli._alpha_zero import _compute_random_moves


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@cl.option("--cycles", default=10, show_default=True)
@_SelfPlayWorker.wrap_options(prefix="play")
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
    print(f"{kwargs=}")

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
        **{
            prefix.removeprefix("train_"): {
                k.removeprefix(prefix + "_"): v
                for k, v in kwargs.items()
                if k.startswith(prefix)
            }
            for prefix in (
                "train_network",
                "train_optimization",
                "train_loss",
                "train_validation",
            )
        },
    )
    worker = _SelfPlayWorker(
        shogi_variant=kwargs['shogi'],
        num_games=kwargs["play_num_games"],
        coeff_kl=kwargs["play_coeff_kl"],
        coeff_entropy=kwargs["play_coeff_entropy"],
        dfpn_search=kwargs['play_dfpn'],
        temperature=kwargs['play_temperature'],
        n_jobs=kwargs['play_jobs'],
        job_size=kwargs['play_job_size'],
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
        others = worker.validate(tflite_path.format(i - 1))
        while True:
            worker(
                tflite_path=None if i == 1 else tflite_path.format(i - 1),
                tflite_path_others=others,
                kifu_dir=os.path.join(
                    kwargs['output'],
                    f'datasets/dataset_{i:04d}',
                ),
                max_random_moves=max_random_moves,
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


@cl.group()
def _klent():
    pass


_klent.add_command(_cycle_selfplay_and_train, "cycler")
