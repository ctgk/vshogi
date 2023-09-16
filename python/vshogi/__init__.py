"""Shogi API for shogi and its variants."""

from vshogi import animal_shogi, minishogi, shogi
from vshogi import engine
from vshogi._game import Game
from vshogi._play import play_game
from vshogi._repr import _repr_enum
from vshogi._vshogi import Color, Result

Color.__repr__ = _repr_enum
Result.__repr__ = _repr_enum

_classes = [Color, Result, Game]
_enums = [Color, Result]
_modules = [animal_shogi, engine, minishogi, shogi]
_functions = [play_game]


for _e in _enums:
    locals().update(_e.__members__)


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = (
    [_cls.__name__ for _cls in _classes]
    + [_m.__name__ for _m in _modules]
    + [_f.__name__ for _f in _functions]
)


del _e
del _enums
del _cls
del _classes
del _modules
del _functions
