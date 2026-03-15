import os
import sys
import warnings
from datetime import datetime
from glob import glob

import click as cl
import numpy as np

from vshogi.dlshogi._alpha_zero._network_trainer import _NetworkTrainer
from vshogi.dlshogi._alpha_zero._self_play_worker import _SelfPlayWorker


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
        coeff_puct=kwargs['play_coeff_puct'],
        kldgain_threshold=kwargs['play_kldgain_threshold'],
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
@_NetworkTrainer.wrap_options()
def _nn_trainer(**kwargs):
    print(f"{kwargs=}")
    trainer = _NetworkTrainer(
        shogi_variant=kwargs["shogi"],
        buffer_size=kwargs["buffer_size"],
        device=kwargs["device"],
        **{
            prefix: {
                k.removeprefix(prefix + "_"): v
                for k, v in kwargs.items()
                if k.startswith(prefix)
            }
            for prefix in ("network", "optimization", "loss", "validation")
        },
    )
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    def _resume_from() -> int:
        tflite_list = sorted(glob('models/model_*.tflite'))
        if not tflite_list:
            return 0
        return int(tflite_list[-1].split('_')[-1].split('.')[0]) + 1

    ii = _resume_from()
    model_path = 'models/model_{:04d}.pth'
    while ii < 10000:
        trainer(
            model_path=model_path.format(ii),
            prev_model_path=None if ii == 0 else model_path.format(ii - 1),
        )
        if os.path.exists(model_path.format(ii).replace('.pth', '.tflite')):
            ii += 1


@cl.command()
@cl.argument("shogi", type=cl.Choice(['minishogi', 'judkins_shogi', 'shogi']))
@_SelfPlayWorker.wrap_options()
def _selfplay_worker(**kwargs):
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    worker = _SelfPlayWorker(
        shogi_variant=kwargs['shogi'],
        num_games=kwargs["play_num_games"],
        coeff_puct=kwargs['coeff_puct'],
        kldgain_threshold=kwargs['kldgain_threshold'],
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


def _average_kifu_length(kifu_dir: str) -> float:
    line_length_list = []
    for path in glob(os.path.join(kifu_dir, 'kifu_*.tsv')):
        if ('B' in path) or ('W' in path):
            continue
        with open(path, 'rb') as f:
            line_length_list.append(sum(1 for _ in f) - 1)
    if line_length_list:
        return np.mean(line_length_list)
    return np.inf


def _compute_random_moves(random_rate: float, kifu_dir: str):
    max_random_moves = random_rate * _average_kifu_length(kifu_dir=kifu_dir)
    if max_random_moves != float('inf'):
        max_random_moves = int(np.ceil(max_random_moves / 2)) * 2
    print(f'max_random_moves = {max_random_moves}')
    return max_random_moves


@cl.group()
def _alpha_zero():
    pass


_alpha_zero.add_command(_cycle_selfplay_and_train, 'cycler')
_alpha_zero.add_command(_nn_trainer, 'nn-trainer')
_alpha_zero.add_command(_selfplay_worker, 'self-play-worker')
