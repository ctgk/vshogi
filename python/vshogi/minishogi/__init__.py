"""Minishogi module.

cf. https://en.wikipedia.org/wiki/Minishogi
"""

from vshogi._enum import _enum_repr
from vshogi._vshogi.minishogi import (
    Board, BoardPiece, Move, Piece, Square, Stand,
)


BoardPiece.__repr__ = _enum_repr
Piece.__repr__ = _enum_repr
Square.__repr__ = _enum_repr


_classes = [Board, BoardPiece, Move, Piece, Square, Stand]
_enums = [BoardPiece, Piece, Square]

for _cls in _classes:
    _cls.__module__ = __name__
for _e in _enums:
    locals().update(_e.__members__)


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
del _e
del _enums
