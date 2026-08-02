import os
import sys
import warnings
from datetime import datetime
from glob import glob
from itertools import count

import click as cl
import numpy as np

from vshogi.dlshogi._alpha_zero._network_trainer import _NetworkTrainer
from vshogi.dlshogi._alpha_zero._self_play_worker import _SelfPlayWorker


@cl.command()
@cl.argument(
    "shogi", type=cl.Choice(["leshogi", 'minishogi', 'judkins_shogi', 'shogi'])
)
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
def _cycler(**kwargs):
    _cycle_selfplay_and_train(_SelfPlayWorker, _NetworkTrainer, **kwargs)


@cl.command()
@cl.argument(
    "shogi", type=cl.Choice(["leshogi", 'minishogi', 'judkins_shogi', 'shogi'])
)
@_NetworkTrainer.wrap_options()
def _nn_trainer(**kwargs):
    _train(_NetworkTrainer, **kwargs)


@cl.command()
@cl.argument(
    "shogi", type=cl.Choice(["leshogi", 'minishogi', 'judkins_shogi', 'shogi'])
)
@_SelfPlayWorker.wrap_options()
def _self_play_worker(**kwargs):
    _selfplay(_SelfPlayWorker, **kwargs)


def _cycle_selfplay_and_train(worker_type: type, trainer_type: type, **kwargs):
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
    trainer = trainer_type(
        **{k.removeprefix("train_"): v for k, v in kwargs.items()}
    )
    worker = worker_type(
        **{k.removeprefix("play_"): v for k, v in kwargs.items()}
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
    others = (
        [] if start == 1 else worker.validate(tflite_path.format(start - 1))[1]
    )
    for ii in range(start, kwargs['cycles'] + 1):
        max_random_moves = _compute_random_moves(
            kwargs['play_random_rate'],
            os.path.join(kwargs['output'], f'datasets/dataset_{ii - 1:04d}'),
        )
        while True:
            worker(
                tflite_path=None if ii == 1 else tflite_path.format(ii - 1),
                tflite_path_others=others,
                kifu_dir=os.path.join(
                    kwargs['output'],
                    f'datasets/dataset_{ii:04d}',
                ),
                max_random_moves=max_random_moves,
            )
            trainer(
                weight_path.format(ii),
                weight_path.format(ii - 1),
                os.path.join(
                    kwargs["output"], "datasets/dataset_*/kifu_*.tsv"
                ),
            )
            if os.path.exists(tflite_path.format(ii)):
                score, others = worker.validate(tflite_path.format(ii))
                msg = (
                    f"Average score of model_{ii:04d} is {score}, where "
                    f"threshold is {kwargs['train_validation_threshold']}"
                )
                print(msg)
                if score > kwargs["train_validation_threshold"]:
                    break
                others = [o for o in others if o != tflite_path.format(ii - 1)]
                os.remove(tflite_path.format(ii))


def _train(trainer_type: type, **kwargs):
    print(f"{kwargs=}")
    trainer = trainer_type(**kwargs)
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
        ii = _resume_from()
        trainer(
            model_path=model_path.format(ii),
            prev_model_path=None if ii == 0 else model_path.format(ii - 1),
        )


def _selfplay(worker_type, **kwargs):
    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    with open(f'command_{now}.txt', 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')
    worker = worker_type(**kwargs)
    tflite_path = 'models/model_{:04d}.tflite'

    others = None
    for ii in range(10000):
        if os.path.exists(tflite_path.format(ii)) and os.path.exists(
            tflite_path.format(ii + 1)
        ):
            continue
        max_random_moves = _compute_random_moves(
            kwargs['random_rate'], f'datasets/dataset_{ii:04d}'
        )
        simulations = kwargs.get("simulations", 0)
        kldgain_threshold = kwargs.get("kldgain_threshold", 0.0)
        for key in ("simulations", "kldgain_threshold"):
            if (value := kwargs.get(key)) is not None:
                setattr(worker, key, value)
                print(f"{key}={getattr(worker, key)}")
        if others is None:
            if ii == 0:
                others = []
            else:
                others = worker.validate(tflite_path.format(ii))[1]
        for jj in count():
            if jj % 10 == 0 and jj != 0:
                simulations += kwargs.get("simulations", 0) // 10
                kldgain_threshold /= 1.1
                for key in ("simulations", "kldgain_threshold"):
                    if (value := kwargs.get(key)) is not None:
                        setattr(worker, key, locals()[key])
                        print(f"{key}={getattr(worker, key)}")
            worker(
                tflite_path=tflite_path.format(ii) if ii > 0 else None,
                tflite_path_others=others,
                kifu_dir=f'datasets/dataset_{ii + 1:04d}',
                max_random_moves=max_random_moves,
            )
            if os.path.exists(tflite_path.format(ii + 1)):
                for key in ("simulations", "kldgain_threshold"):
                    if (value := kwargs.get(key)) is not None:
                        setattr(worker, key, value)
                score, others = worker.validate(tflite_path.format(ii + 1))
                msg = (
                    f"Average score of model_{ii + 1:04d} is {score}, where "
                    f"threshold is 0.6"
                )
                print(msg)
                if score > 0.6:
                    break
                others = [o for o in others if o != tflite_path.format(ii)]
                for key in ("simulations", "kldgain_threshold"):
                    if (value := kwargs.get(key)) is not None:
                        setattr(worker, key, locals()[key])
                os.remove(tflite_path.format(ii + 1))


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


_alpha_zero.add_command(_cycler, 'cycler')
_alpha_zero.add_command(_nn_trainer, 'nn-trainer')
_alpha_zero.add_command(_self_play_worker, 'self-play-worker')
