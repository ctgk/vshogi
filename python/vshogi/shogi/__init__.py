"""Shogi module.

cf. https://en.wikipedia.org/wiki/Shogi
"""

import numpy as np

from vshogi._repr import (
    _repr_board, _repr_enum, _repr_move, _repr_square, _repr_stand,
)
from vshogi._vshogi import Color, Result
from vshogi._vshogi.shogi import (
    Board, BoardPiece, Move, Piece, Square, Stand, State,
)
from vshogi.shogi._game import Game


Board.__array__ = lambda self: np.array([
    self[Square(i)] for i in range(81)
], dtype=BoardPiece).reshape(9, 9)
Board.__repr__ = _repr_board
BoardPiece.__repr__ = _repr_enum
BoardPiece._to_3char = lambda self: (
    "   " if self == BoardPiece.VOID
    else {'B': '+', 'W': '-'}[self.name[0]] + self.name[2:4]
)
Move.__repr__ = _repr_move
Piece.__repr__ = _repr_enum
Stand.__repr__ = _repr_stand
Square.__repr__ = _repr_square

_classes = [Board, BoardPiece, Move, Piece, Square, Stand, State, Game]
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
