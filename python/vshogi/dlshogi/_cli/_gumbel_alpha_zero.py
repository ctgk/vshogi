import os
import sys
import warnings
from datetime import datetime
from glob import glob

import click as cl

from vshogi.dlshogi._alpha_zero._network_trainer import _NetworkTrainer
from vshogi.dlshogi._gumbel_alpha_zero._self_play_worker import _SelfPlayWorker
from vshogi.dlshogi._cli._alpha_zero import _compute_random_moves, _nn_trainer


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
        num_actions=kwargs["play_num_actions"],
        dfpn_search_root=kwargs['play_dfpn_root'],
        dfpn_search_leaf=kwargs['play_dfpn_leaf'],
        simulations=kwargs['play_num_simulations'],
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


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@_SelfPlayWorker.wrap_options()
def _selfplay_worker(**kwargs):
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    worker = _SelfPlayWorker(
        shogi_variant=kwargs['shogi'],
        num_games=kwargs["num_games"],
        num_actions=kwargs["num_actions"],
        dfpn_search_root=kwargs['dfpn_root'],
        dfpn_search_leaf=kwargs['dfpn_leaf'],
        simulations=kwargs['num_simulations'],
        temperature=kwargs['temperature'],
        n_jobs=kwargs['play_jobs'],
        job_size=kwargs["job_size"],
    )

    tflite_path = 'models/model_{:04d}.tflite'
    for ii in range(10000):
        if os.path.exists(tflite_path.format(ii)) and os.path.exists(
            tflite_path.format(ii + 1)
        ):
            continue
        max_random_moves = _compute_random_moves(
            kwargs['random_rate'], f'datasets/dataset_{ii - 1:04d}'
        )
        others = worker.validate(tflite_path.format(ii))
        while True:
            worker(
                tflite_path=tflite_path.format(ii) if ii > 0 else None,
                tflite_path_others=others,
                kifu_dir=f'datasets/dataset_{ii:04d}',
                max_random_moves=max_random_moves,
            )
            if os.path.exists(tflite_path.format(ii + 1)):
                print(f"Found new model: {tflite_path.format(ii + 1)}")
                break
            else:
                msg = (
                    f"New model ({tflite_path.format(ii + 1)}) not found. "
                    f"Continue self-play with {tflite_path.format(ii)}"
                )
                print(msg)


@cl.group()
def _gumbel_alpha_zero():
    pass


_gumbel_alpha_zero.add_command(_cycle_selfplay_and_train, 'cycler')
_gumbel_alpha_zero.add_command(_nn_trainer, 'nn-trainer')
_gumbel_alpha_zero.add_command(_selfplay_worker, 'self-play-worker')
