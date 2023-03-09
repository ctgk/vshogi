"""Animal Shogi module."""

from vshogi.animal_shogi._color import Color
from vshogi.animal_shogi._game import Game
from vshogi.animal_shogi._move import Move, MoveSourceEnum
from vshogi.animal_shogi._piece import BoardPiece, CapturedPiece, Piece
from vshogi.animal_shogi._square import Square


_classes = [
    BoardPiece, CapturedPiece, Color, Game, Move, MoveSourceEnum, Piece,
    Square,
]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
