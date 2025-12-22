import click as cl

from vshogi.dlshogi._cli._cycler import _cycle_selfplay_and_train
from vshogi.dlshogi._cli._nn_trainer import _nn_trainer


@cl.group()
def _dlshogi():
    pass


_dlshogi.add_command(_cycle_selfplay_and_train, 'cycler')
_dlshogi.add_command(_nn_trainer, 'nn-trainer')
