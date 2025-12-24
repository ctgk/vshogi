import os
import sys
import warnings
from datetime import datetime
from glob import glob

import click as cl

from vshogi.dlshogi._cli._nn_trainer import _train_step
from vshogi.dlshogi._cli._self_play_worker import (
    _compute_random_moves,
    _get_previous_models_superior_to_latest,
    _run_self_play,
)


def _train(nth_cycle: int, **kwargs):
    weight_path = os.path.join(kwargs['output'], 'models/model_{:04d}.pth')
    _train_step(
        model_path=weight_path.format(nth_cycle),
        prev_model_path=(
            None if nth_cycle == 0 else
            weight_path.format(nth_cycle - 1)
        ),
        shogi_variant=kwargs['shogi'],
        network_hidden_channels=kwargs['train_hidden_channels'],
        network_bottleneck_channels=kwargs['train_bottleneck_channels'],
        network_backbone_blocks=kwargs['train_backbone_blocks'],
        max_dataset_size=kwargs['train_dataset_size'],
        kifu_path_pattern=os.path.join(
            kwargs['output'],
            'datasets/dataset_*/kifu_*.tsv',
        ),
        kifu_fraction=kwargs['train_kifu_fraction'],
        discount_factor=kwargs['train_discount_factor'],
        importance_decay=kwargs['train_importance_decay'],
        minibatch_size=kwargs['train_minibatch_size'],
        learning_rate=kwargs['train_learning_rate'],
        epochs=0 if nth_cycle == 0 else kwargs['train_epochs'],
        coeff_policy_loss=kwargs['train_coeff_policy_loss'],
        coeff_entropy_regularization=kwargs['train_coeff_policy_entropy'],
        grad_accumulations=kwargs['train_grad_accumulations'],
        win_ratio_threshold=kwargs['train_win_ratio_threshold'],
        device=kwargs['train_device'],
        engine='AlphaZero',
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
@cl.option(
    "--play-engine",
    default='AlphaZero',
    type=cl.Choice(['AlphaZero', 'GumbelAlphaZero']),
    show_default=True,
)
@cl.option("--play-num-games", default=100, show_default=True)
@cl.option("--play-coeff-puct", default=4., show_default=True)
@cl.option("--play-kldgain-threshold", default=1e-4, show_default=True)
@cl.option("--play-dfpn-root", default=10000, show_default=True)
@cl.option("--play-dfpn-leaf", default=100, show_default=True)
@cl.option("--play-num-simulations", default=100, show_default=True)
@cl.option("--play-temperature", default=1., show_default=True)
@cl.option("--play-dump-q-greedy-depth", default=1, show_default=True)
@cl.option("--play-random-rate", default=0.5, show_default=True)
@cl.option("--play-gumbel-actions", default=16, show_default=True)
@cl.option("--play-jobs", default=1, show_default=True)
@cl.option("--train-hidden-channels", default=128, show_default=True)
@cl.option("--train-bottleneck-channels", default=32, show_default=True)
@cl.option("--train-backbone-blocks", default=4, show_default=True)
@cl.option("--train-dataset-size", default=100000, show_default=True)
@cl.option("--train-kifu-fraction", default=0.8, show_default=True)
@cl.option("--train-discount-factor", default=0.99, show_default=True)
@cl.option("--train-importance-decay", default=0.7, show_default=True)
@cl.option("--train-minibatch-size", default=32, show_default=True)
@cl.option("--train-learning-rate", default=1e-2, show_default=True)
@cl.option("--train-epochs", default=5, show_default=True)
@cl.option("--train-coeff-policy-loss", default=0.1, show_default=True)
@cl.option("--train-coeff-policy-entropy", default=1e-2, show_default=True)
@cl.option("--train-grad-accumulations", default=1, show_default=True)
@cl.option("--train-win-ratio-threshold", default=0.55, show_default=True)
@cl.option(
    "--train-device",
    default='cpu',
    type=cl.Choice(['cpu', 'cuda', 'mps']),
    show_default=True,
)
@cl.option(
    "-o", "--output",
    default='',
    type=str,
    help='Output directory (default: cwd)',
)
def _cycle_selfplay_and_train(**kwargs):
    print('kwargs:', kwargs)

    now = datetime.now().strftime('%Y%m%d_%H%M%S')
    if kwargs['output'] != '':
        if os.path.isdir(kwargs['output']):
            warnings.warn(
                f'Output directory ({kwargs["output"]}) already exists')
        else:
            os.makedirs(kwargs['output'])
    with open(os.path.join(kwargs['output'], f'command_{now}.txt'), 'w') as f:
        f.write(f'python {" ".join(sys.argv)}')

    tflite_path = os.path.join(kwargs['output'], 'models/model_{:04d}.tflite')

    def _resume_from() -> int:
        tflite_list = sorted(glob(os.path.join(
            kwargs['output'], 'models/model_*.tflite')))
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
                tflite_path.format(j)
                for j in list(range(i - 2, -1, -1))[:10]
            ],
            engine=kwargs['play_engine'],
            num_games=10,
            coeff_puct=kwargs['play_coeff_puct'],
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
