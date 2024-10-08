"""Module for Shogi engine."""

from vshogi.engine._dfpn import DfpnSearcher
from vshogi.engine._dfpn_mcts import DfpnMcts
from vshogi.engine._engine import Engine
from vshogi.engine._mcts import Mcts
from vshogi.engine._piece_value_func import piece_value_func


_classes = [
    DfpnMcts,
    DfpnSearcher,
    Engine,
    Mcts,
]
_functions = [
    piece_value_func,
]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [
    _cls.__name__ for _cls in _classes
] + [
    _f.__name__ for _f in _functions
]


del _cls
del _classes
del _functions
