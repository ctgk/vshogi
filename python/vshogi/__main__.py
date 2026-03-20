"""Command Line Interface of VShogi package."""

import click as cl

from vshogi._cli._match import _match
from vshogi.dlshogi._cli import _dlshogi
from vshogi.dlshogi._cli._alpha_zero import _alpha_zero
from vshogi.dlshogi._cli._gumbel_alpha_zero import _gumbel_alpha_zero
from vshogi.dlshogi._cli._klent import _klent


@cl.group()
def _main():
    pass


_main.add_command(_match, 'match')
_main.add_command(_dlshogi, 'dlshogi')
_main.add_command(_alpha_zero, "alpha-zero")
_main.add_command(_gumbel_alpha_zero, "gumbel-alpha-zero")
_main.add_command(_klent, "klent")


if __name__ == "__main__":
    _main()
