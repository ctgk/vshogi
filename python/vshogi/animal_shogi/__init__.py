"""Animal Shogi module."""

from vshogi.animal_shogi._game import Game


_classes = [Game]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
