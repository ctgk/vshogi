"""Minishogi module.

cf. https://en.wikipedia.org/wiki/Minishogi

Examples
--------
>>> from vshogi.minishogi import *
>>> game = Game()
>>> print(game)
Turn: BLACK
White: -
    5   4   3   2   1
  *---*---*---*---*---*
A |-HI|-KA|-GI|-KI|-OU|
  *---*---*---*---*---*
B |   |   |   |   |-FU|
  *---*---*---*---*---*
C |   |   |   |   |   |
  *---*---*---*---*---*
D |+FU|   |   |   |   |
  *---*---*---*---*---*
E |+OU|+KI|+GI|+KA|+HI|
  *---*---*---*---*---*
Black: -
>>> game.apply(SQ_4C, SQ_2E)
Game(sfen="rbsgk/4p/1B3/P4/KGS1R w - 2")
>>> game.apply(SQ_1C, SQ_1B)
Game(sfen="rbsgk/5/1B2p/P4/KGS1R b - 3")
>>> game.apply(SQ_2E, SQ_1E)
Game(sfen="rbsgk/5/1B2p/P4/KGSR1 w - 4")
>>> game.apply(SQ_1D, SQ_1C)
Game(sfen="rbsgk/5/1B3/P3p/KGSR1 b - 5")
>>> game.apply(SQ_2A, SQ_2E, promote=True)
Game(sfen="rbs+Rk/5/1B3/P3p/KGS2 w G 6")
>>> print(game)
BLACK_WIN
White: -
    5   4   3   2   1
  *---*---*---*---*---*
A |-HI|-KA|-GI|+RY|-OU|
  *---*---*---*---*---*
B |   |   |   |   |   |
  *---*---*---*---*---*
C |   |+KA|   |   |   |
  *---*---*---*---*---*
D |+FU|   |   |   |-FU|
  *---*---*---*---*---*
E |+OU|+KI|+GI|   |   |
  *---*---*---*---*---*
Black: KI
"""

import numpy as np

from vshogi._enum import _enum_repr
from vshogi._vshogi import Color, Direction, Result
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
Move.__repr__ = lambda m: (
    f'{m.__class__.__name__}(dst={m.destination.name}, src={m.source.name}'
    + (', promote=True' if m.promote else '')
    + ')'
)
Piece.__repr__ = _enum_repr
Stand.__str__ = lambda self: '-' if not self.any() else ','.join([
    k.name + ('' if v == 1 else f'x{v}') for k, v in self.to_dict().items()
    if v > 0
])
Square.__repr__ = lambda a: f'{a.__class__.__name__}.{a.name[-2:]}'

_classes = [Board, BoardPiece, Move, Piece, Square, Stand, Game]
_enums = [BoardPiece, Color, Direction, Piece, Result, Square]

for _cls in _classes:
    _cls.__module__ = __name__
for _e in _enums:
    locals().update(_e.__members__)


__all__ = (
    [_cls.__name__ for _cls in _classes] + ['Color', 'Direction', 'Result']
    + [m for _e in _enums for m in _e.__members__]
)


del _cls
del _classes
del _e
del _enums
