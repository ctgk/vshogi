"""Animal Shogi module.

cf. https://en.wikipedia.org/wiki/D%C5%8Dbutsu_sh%C5%8Dgi
"""

import numpy as np

from vshogi._repr import _repr_enum, _repr_move
from vshogi._vshogi import Color, Result
from vshogi._vshogi.animal_shogi import (
    Board, BoardPiece, Move, Piece, Square,
)
from vshogi.animal_shogi._game import Game


_board_template = '''\
    A  B  C
  *--*--*--*
1 |{}|{}|{}|
  *--*--*--*
2 |{}|{}|{}|
  *--*--*--*
3 |{}|{}|{}|
  *--*--*--*
4 |{}|{}|{}|
  *--*--*--*'''


def _board_repr(self: Board) -> str:
    return _board_template.format(
        *[
            self.__getitem__(sq)._to_2char()
            for sq in (
                Square.A1, Square.B1, Square.C1,
                Square.A2, Square.B2, Square.C2,
                Square.A3, Square.B3, Square.C3,
                Square.A4, Square.B4, Square.C4,
            )
        ],
    )


Board.__array__ = lambda self: np.array(
    [
        [self[Square.A1], self[Square.B1], self[Square.C1]],
        [self[Square.A2], self[Square.B2], self[Square.C2]],
        [self[Square.A3], self[Square.B3], self[Square.C3]],
        [self[Square.A4], self[Square.B4], self[Square.C4]],
    ],
    dtype=BoardPiece,
)
Board.__repr__ = _board_repr
BoardPiece.__repr__ = _repr_enum
BoardPiece._to_2char = lambda self: (
    "  " if self == BoardPiece.VOID
    else {'B': '+', 'W': '-'}[self.name[0]] + self.name[2]
)
Piece.__repr__ = _repr_enum
Move.__repr__ = _repr_move
Square.__repr__ = _repr_enum


_classes = [
    Board, BoardPiece, Game, Move, Piece, Square,
]
_enums = [BoardPiece, Color, Piece, Square, Result]

for _e in _enums:
    locals().update(_e.__members__)


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = (
    [_cls.__name__ for _cls in _classes] + ['Color', 'Result']
    + [m for _e in _enums for m in _e.__members__]
)


del _cls
del _classes
del _e
del _enums
