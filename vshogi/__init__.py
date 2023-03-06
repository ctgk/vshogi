from ._vshogi import *
from vshogi._board import Board, LegalMoveList, PseudoLegalMoveList
from vshogi._constants import *
from vshogi._parser import Parser


_classes = [
    Board, LegalMoveList, PseudoLegalMoveList, Parser,
]


for _cls in _classes:
    _cls.__module__ = __name__


del _cls
del _classes
