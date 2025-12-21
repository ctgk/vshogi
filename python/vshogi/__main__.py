"""Command Line Interface of VShogi package."""

import click as cl

from vshogi._cli._match import _match
from vshogi.dlshogi._cli_nn_trainer import _nn_trainer


@cl.group()
def _dlshogi():
    pass


_dlshogi.add_command(_nn_trainer, 'nn-trainer')


@cl.group()
def _main():
    pass


_main.add_command(_match, 'match')
_main.add_command(_dlshogi, 'dlshogi')


if __name__ == "__main__":
    _main()
