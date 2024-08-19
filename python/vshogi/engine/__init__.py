"""Module for Shogi engine."""

from vshogi.engine._dfpn import DfpnSearcher
from vshogi.engine._dfpn_mcts import DfpnMcts
from vshogi.engine._engine import Engine
from vshogi.engine._mcts import Mcts


_classes = [
    DfpnMcts,
    DfpnSearcher,
    Engine,
    Mcts,
]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
