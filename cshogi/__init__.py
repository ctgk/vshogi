from ._cshogi import *
from cshogi._board import Board, LegalMoveList, PseudoLegalMoveList
from cshogi._constants import *
from cshogi._parser import Parser


_classes = [
    Board, LegalMoveList, PseudoLegalMoveList, Parser,
]


for _cls in _classes:
    _cls.__module__ = __name__


del _cls
del _classes
