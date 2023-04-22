"""Shogi API for shogi and its variants."""

from vshogi import animal_shogi
from vshogi import mcts
from vshogi._enum import _enum_repr
from vshogi._vshogi import Color, Result

Color.__repr__ = _enum_repr
Result.__repr__ = _enum_repr

_classes = [Color, Result]
_enums = [Color, Result]
_modules = [animal_shogi, mcts]


for _e in _enums:
    locals().update(_e.__members__)


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = (
    [_cls.__name__ for _cls in _classes]
    + [_m.__name__ for _m in _modules]
)


del _e
del _enums
del _cls
del _classes
del _modules
