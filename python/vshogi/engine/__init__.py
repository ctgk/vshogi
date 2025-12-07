"""Module for Shogi engine."""

from vshogi.engine._az import AlphaZero
from vshogi.engine._dfpn import DfpnSearcher
from vshogi.engine._engine import Engine
from vshogi.engine._piece_value_func import piece_value_func
from vshogi.engine._search_nply_mate import search_nply_mate


_classes = [
    AlphaZero,
    DfpnSearcher,
    Engine,
]
_functions = [
    piece_value_func,
    search_nply_mate,
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
