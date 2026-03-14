import click as cl

from vshogi._cli._alpha_zero._cycler import _cycle_selfplay_and_train
from vshogi._cli._alpha_zero._nn_trainer import _nn_trainer
from vshogi._cli._alpha_zero._self_play_worker import _selfplay_worker


@cl.group()
def _alpha_zero():
    pass


_alpha_zero.add_command(_cycle_selfplay_and_train, "cycler")
_alpha_zero.add_command(_nn_trainer, "nn-trainer")
_alpha_zero.add_command(_selfplay_worker, "self-play-worker")
