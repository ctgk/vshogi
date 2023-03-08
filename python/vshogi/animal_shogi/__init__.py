"""Animal Shogi module."""

from vshogi.animal_shogi._color import ColorEnum
from vshogi.animal_shogi._game import Game
from vshogi.animal_shogi._move import MoveSourceEnum
from vshogi.animal_shogi._square import SquareEnum


_classes = [ColorEnum, Game, MoveSourceEnum, SquareEnum]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
