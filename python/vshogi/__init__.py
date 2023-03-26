"""Shogi API for shogi and its variants."""

from vshogi import animal_shogi
from vshogi import mcts


_modules = [animal_shogi, mcts]


__all__ = [_m.__name__ for _m in _modules]


del _modules
