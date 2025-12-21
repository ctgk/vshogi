"""Command Line Interface of VShogi package."""

import click as cl

from vshogi._cli._match import _match


@cl.group()
def _main():
    pass


_main.add_command(_match, 'match')


if __name__ == "__main__":
    _main()
