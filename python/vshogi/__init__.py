"""Shogi API for shogi and its variants."""

from vshogi import animal_shogi, judkins_shogi, minishogi, shogi
from vshogi import engine
from vshogi._game import Game
from vshogi._play import play_game
from vshogi._vshogi_extension import Color, Result


_classes = [Color, Result, Game]
_enums = [Color, Result]
_modules = [animal_shogi, engine, judkins_shogi, minishogi, shogi]
_functions = [play_game]


for _e in _enums:
    locals().update({
        k: getattr(_e, k) for k in dir(_e)
        if not (k.startswith('@') or k.startswith('_'))
    })


for _cls in _classes:
    _cls.__module__ = 'vshogi'


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
