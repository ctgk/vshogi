"""Minishogi module.

cf. https://en.wikipedia.org/wiki/Minishogi
"""

import numpy as np

from vshogi._enum import _enum_repr
from vshogi._vshogi import Color, Result
from vshogi._vshogi.minishogi import (
    Board, BoardPiece, Move, Piece, Square, Stand,
)
from vshogi.minishogi._game import Game


_board_template = '''\
    5   4   3   2   1
  *---*---*---*---*---*
A |{}|{}|{}|{}|{}|
  *---*---*---*---*---*
B |{}|{}|{}|{}|{}|
  *---*---*---*---*---*
C |{}|{}|{}|{}|{}|
  *---*---*---*---*---*
D |{}|{}|{}|{}|{}|
  *---*---*---*---*---*
E |{}|{}|{}|{}|{}|
  *---*---*---*---*---*'''


def _board_repr(self: Board) -> str:
    return _board_template.format(
        *[self.__getitem__(Square(i))._to_3char() for i in range(25)],
    )


Board.__array__ = lambda self: np.array([
    self[Square(i)] for i in range(25)
], dtype=BoardPiece).reshape(5, 5)
Board.__repr__ = _board_repr
BoardPiece.__repr__ = _enum_repr
BoardPiece._to_3char = lambda self: (
    "   " if self == BoardPiece.VOID
    else {'B': '+', 'W': '-'}[self.name[0]] + self.name[2:4]
)
Piece.__repr__ = _enum_repr
Stand.__str__ = lambda self: '-' if not self.any() else ','.join([
    k.name + ('' if v == 1 else f'x{v}') for k, v in self.to_dict().items()
    if v > 0
])
Square.__repr__ = lambda a: f'{a.__class__.__name__}.{a.name[-2:]}'

_classes = [Board, BoardPiece, Move, Piece, Square, Stand, Game]
_enums = [BoardPiece, Color, Piece, Result, Square]

for _cls in _classes:
    _cls.__module__ = __name__
for _e in _enums:
    locals().update(_e.__members__)


__all__ = (
    [_cls.__name__ for _cls in _classes] + ['Color', 'Result']
    + [m for _e in _enums for m in _e.__members__]
)


del _cls
del _classes
del _e
del _enums
