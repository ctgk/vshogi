import click as cl

from vshogi.dlshogi._klent._network_trainer import _NetworkTrainer
from vshogi.dlshogi._klent._self_play_worker import _SelfPlayWorker
from vshogi.dlshogi._cli._alpha_zero import (
    _cycle_selfplay_and_train,
    _selfplay,
    _train,
)


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


@cl.group()
def _klent():
    pass


_klent.add_command(_cycler, "cycler")
_klent.add_command(_nn_trainer, 'nn-trainer')
_klent.add_command(_self_play_worker, 'self-play-worker')
