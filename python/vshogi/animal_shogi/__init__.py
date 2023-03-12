"""Animal Shogi module.

cf. https://en.wikipedia.org/wiki/D%C5%8Dbutsu_sh%C5%8Dgi
"""

from vshogi.animal_shogi._board import Board
from vshogi.animal_shogi._color import Color
from vshogi.animal_shogi._game import Game
from vshogi.animal_shogi._move import Move
from vshogi.animal_shogi._piece import BoardPiece, Piece
from vshogi.animal_shogi._square import Square


_classes = [
    Board, BoardPiece, Color, Game, Move, Piece, Square,
]


locals().update(Piece.__members__)
locals().update(Square.__members__)


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
