"""Minishogi module.

cf. https://en.wikipedia.org/wiki/Minishogi
"""

from vshogi._enum import _enum_repr
from vshogi._vshogi.minishogi import Square


Square.__repr__ = _enum_repr


_classes = [Square]
_enums = [Square]

for _cls in _classes:
    _cls.__module__ = __name__
for _e in _enums:
    locals().update(_e.__members__)


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
del _e
del _enums
