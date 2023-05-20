"""Module for Shogi engine."""

from vshogi.engine._mcts import MonteCarloTreeSearcher


_classes = [MonteCarloTreeSearcher]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
