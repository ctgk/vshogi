"""Shogi module.

cf. https://en.wikipedia.org/wiki/Shogi
"""

import numpy as np

from vshogi._repr import (
    _repr_board,
    _repr_enum,
    _repr_move,
    _repr_square,
    _repr_stand,
)
from vshogi._vshogi import Color, Result
from vshogi._vshogi.shogi import (
    Board,
    BoardPiece,
    Move,
    Piece,
    Square,
    Stand,
    State,
    to_jpn,
    to_sfen,
)
from vshogi.shogi._game import Game


Board.__array__ = lambda self: np.array(
    [self[Square(i)] for i in range(81)],
    dtype=BoardPiece,
).reshape(9, 9)
Board.__repr__ = _repr_board
BoardPiece.__repr__ = _repr_enum
BoardPiece.to_sfen = lambda self: (
    'P', 'L', 'N', 'S', 'B', 'R', 'G', 'K',
    '+P', '+L', '+N', '+S', '+B', '+R',
    'p', 'l', 'n', 's', 'b', 'r', 'g', 'k',
    '+p', '+l', '+n', '+s', '+b', '+r',
    '',
)[self.value]
BoardPiece._to_3char = lambda self: (
    "   "
    if self == BoardPiece.VOID
    else {"B": "+", "W": "-"}[self.name[0]] + self.name[2:4]
)
BoardPiece.to_jpn = lambda self: to_jpn(self)
BoardPiece.to_sfen = lambda self: to_sfen(self)
Move.__repr__ = _repr_move
Piece.__repr__ = _repr_enum
Piece.to_jpn = lambda self: to_jpn(self)
Piece.to_sfen = lambda self: to_sfen(self)
Stand.__repr__ = _repr_stand
Square.__repr__ = _repr_square
Square.to_jpn = lambda self: to_jpn(self)

_classes = [Board, BoardPiece, Move, Piece, Square, Stand, State, Game]
_enums = [BoardPiece, Color, Piece, Result, Square]

for _cls in _classes:
    _cls.__module__ = __name__
for _e in _enums:
    locals().update(_e.__members__)


__all__ = (
    [_cls.__name__ for _cls in _classes]
    + ["Color", "Result"]
    + [m for _e in _enums for m in _e.__members__]
)


del _cls
del _classes
del _e
del _enums
