"""Judkins Shogi module.

cf. https://en.wikipedia.org/wiki/Judkins_shogi
"""

import numpy as np

from vshogi._repr import _repr_enum, _repr_move, _repr_square, _repr_stand
from vshogi._vshogi import Color, Result
from vshogi._vshogi.judkins_shogi import (
    Board, BoardPiece, Move, Piece, Square, Stand,
)
from vshogi.judkins_shogi._game import Game


_board_template = '''\
    6   5   4   3   2   1
  +---+---+---+---+---+---+
A |{}|{}|{}|{}|{}|{}|
  +---+---+---+---+---+---+
B |{}|{}|{}|{}|{}|{}|
  +---+---+---+---+---+---+
C |{}|{}|{}|{}|{}|{}|
  +---+---+---+---+---+---+
D |{}|{}|{}|{}|{}|{}|
  +---+---+---+---+---+---+
E |{}|{}|{}|{}|{}|{}|
  +---+---+---+---+---+---+
F |{}|{}|{}|{}|{}|{}|
  +---+---+---+---+---+---+'''


def _board_repr(self: Board) -> str:
    return _board_template.format(
        *[self.__getitem__(Square(i))._to_3char() for i in range(36)],
    )


Board.__array__ = lambda self: np.array([
    self[Square(i)] for i in range(36)
], dtype=BoardPiece).reshape(6, 6)
Board.__repr__ = _board_repr
BoardPiece.__repr__ = _repr_enum
BoardPiece._to_3char = lambda self: (
    "   " if self == BoardPiece.VOID
    else {'B': '+', 'W': '-'}[self.name[0]] + self.name[2:4]
)
Move.__repr__ = _repr_move
Piece.__repr__ = _repr_enum
Stand.__repr__ = _repr_stand
Square.__repr__ = _repr_square

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
