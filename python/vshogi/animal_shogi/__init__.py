"""Animal Shogi module.

cf. https://en.wikipedia.org/wiki/D%C5%8Dbutsu_sh%C5%8Dgi
"""

from vshogi._vshogi.animal_shogi import Board, Color
from vshogi.animal_shogi._game import Game
from vshogi.animal_shogi._move import Move
from vshogi.animal_shogi._piece import BoardPiece, Piece
from vshogi.animal_shogi._square import Square


_board_template = '''\
    A  B  C
  *--*--*--*
1 |{}|{}|{}|
  *--*--*--*
2 |{}|{}|{}|
  *--*--*--*
3 |{}|{}|{}|
  *--*--*--*
4 |{}|{}|{}|
  *--*--*--*'''


def _getitem_from_board(self: Board, sq: Square) -> BoardPiece:
    return BoardPiece(self.get_piece_at(sq.value))


def _board_repr(self: Board) -> str:
    return _board_template.format(
        *[
            self.__getitem__(sq)._to_2char()
            for sq in (
                Square.A1, Square.B1, Square.C1,
                Square.A2, Square.B2, Square.C2,
                Square.A3, Square.B3, Square.C3,
                Square.A4, Square.B4, Square.C4,
            )
        ],
    )


Board.__getitem__ = _getitem_from_board
Board.__repr__ = _board_repr


_classes = [
    Board, BoardPiece, Color, Game, Move, Piece, Square,
]

locals().update(Color.__members__)
locals().update(Piece.__members__)
locals().update(Square.__members__)


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
