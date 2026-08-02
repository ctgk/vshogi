"""Leshogi module.

cf. https://www.logygames.com/shogi/leshogi.html

Examples
--------
>>> from vshogi.leshogi import *
>>> game = Game()
>>> print(game)
Turn: BLACK
White: -
    4   3   2   1
  +---+---+---+---+
A |-GI|-OU|-KE|-KY|
  +---+---+---+---+
B |-FU|-HI|-KA|-KI|
  +---+---+---+---+
C |   |   |   |   |
  +---+---+---+---+
D |   |   |   |   |
  +---+---+---+---+
E |+KI|+KA|+HI|+FU|
  +---+---+---+---+
F |+KY|+KE|+OU|+GI|
  +---+---+---+---+
Black: -
>>> game.apply("4e4d")
Game(sfen="sknl/prbg/4/G3/1BRP/LNKS w - 2")
>>> game.apply("2b4d")
Game(sfen="sknl/pr1g/4/b3/1BRP/LNKS b g 3")
>>> game.apply("2e2d")
Game(sfen="sknl/pr1g/4/b1R1/1B1P/LNKS w g 4")
>>> game.apply("3b3e+")
Game(sfen="sknl/p2g/4/b1R1/1+r1P/LNKS b bg 5")
>>> print(game)
WHITE_WIN
White: KA,KI
    4   3   2   1
  +---+---+---+---+
A |-GI|-OU|-KE|-KY|
  +---+---+---+---+
B |-FU|   |   |-KI|
  +---+---+---+---+
C |   |   |   |   |
  +---+---+---+---+
D |-KA|   |+HI|   |
  +---+---+---+---+
E |   |-RY|   |+FU|
  +---+---+---+---+
F |+KY|+KE|+OU|+GI|
  +---+---+---+---+
Black: -
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
from vshogi.leshogi._leshogi import (
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
from vshogi.leshogi._game import Game


Board.__array__ = lambda self: np.array(
    [self[i] for i in range(24)],
    dtype=BoardPiece,
).reshape(4, 6)
Board.__repr__ = _repr_board
BoardPiece.__repr__ = _repr_enum
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
Stand.__str__ = _repr_stand
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
