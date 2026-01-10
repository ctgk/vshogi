"""Command Line Interface of VShogi package."""

import click as cl

from vshogi._cli._match import _match
from vshogi.dlshogi._cli import _dlshogi


@cl.group()
def _main():
    pass


_main.add_command(_match, 'match')
_main.add_command(_dlshogi, 'dlshogi')


if __name__ == "__main__":
    _main()
