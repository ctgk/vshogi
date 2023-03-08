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
  *--*--*--*
'''


class Board:
    """Animal Shogi board.

    Examples
    --------
    >>> import vshogi.animal_shogi as shogi
    >>> board = Board()
    >>> board[shogi.Square.B3]
    BoardPiece.B_CH
    """

    def __init__(self) -> None:
        """Initialize board."""
        self._board = _Board()

    def __getitem__(self, sq: Square) -> BoardPiece:
        return BoardPiece(self._board.get_piece_at(sq.value))
