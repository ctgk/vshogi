from vshogi._vshogi.animal_shogi import Board as _Board
from vshogi.animal_shogi._piece import BoardPiece
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


class Board:
    """Animal Shogi board.

    Examples
    --------
    >>> import vshogi.animal_shogi as shogi
    >>> board = Board()
    >>> board[shogi.Square.B3]
    BoardPiece.B_CH
    >>> board
        A  B  C
      *--*--*--*
    1 |-G|-L|-E|
      *--*--*--*
    2 |  |-C|  |
      *--*--*--*
    3 |  |+C|  |
      *--*--*--*
    4 |+E|+L|+G|
      *--*--*--*
    """

    def __init__(self, **kwargs) -> None:
        """Initialize board."""
        self._board = kwargs.get('_board', _Board())

    def __getitem__(self, sq: Square) -> BoardPiece:
        return BoardPiece(self._board.get_piece_at(sq.value))

    def __repr__(self) -> str:
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
