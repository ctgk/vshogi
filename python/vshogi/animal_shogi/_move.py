import typing as tp

import vshogi._vshogi.animal_shogi as _as
from vshogi._enum import _Enum
from vshogi.animal_shogi._piece import Piece
from vshogi.animal_shogi._square import Square


class MoveSource(_Enum):
    """Enumeration of move sources.

    ```
    Board squares + Piece Stand
        A  B  C     CH, EL, GI
      *--*--*--*
    1 |A1|B1|C1|
      *--*--*--*
    2 |A2|B2|C2|
      *--*--*--*
    3 |A3|B3|C3|
      *--*--*--*
    4 |A4|B4|C4|
      *--*--*--*
    ```
    """

    A1 = _as.MS_A1
    B1 = _as.MS_B1
    C1 = _as.MS_C1
    A2 = _as.MS_A2
    B2 = _as.MS_B2
    C2 = _as.MS_C2
    A3 = _as.MS_A3
    B3 = _as.MS_B3
    C3 = _as.MS_C3
    A4 = _as.MS_A4
    B4 = _as.MS_B4
    C4 = _as.MS_C4
    CH = _as.MS_CH
    EL = _as.MS_EL
    GI = _as.MS_GI


class Move:
    """Move of a piece.

    Examples
    --------
    >>> import vshogi.animal_shogi as shogi
    >>> m = Move(shogi.B2, shogi.B3)
    >>> m
    Move(B3 -> B2)
    >>> m.source
    Square.B3
    >>> m.destination
    Square.B2
    >>> m.is_drop()
    False
    >>> m = Move(shogi.A2, shogi.GI)
    >>> m
    Move(GI -> A2)
    >>> m.destination
    Square.A2
    >>> m.source
    Piece.GI
    >>> m.is_drop()
    True
    """

    def __init__(
        self,
        destination: Square,
        source: tp.Union[Square, MoveSource, Piece],
    ) -> None:
        """Initialize move object.

        Parameters
        ----------
        destination : Square
            Destination on the board.
        source : tp.Union[Square, MoveSource, Piece]
            Source, either from piece stand or a board square.
        """
        self._move = _as.Move(destination.value, source.value)

    def __repr__(self) -> str:
        return (
            f'{self.__class__.__name__}({self.source.name} '
            f'-> {self.destination.name})'
        )

    @property
    def destination(self) -> Square:
        """Destination on the board.

        Returns
        -------
        Square
            A board square.
        """
        return Square(self._move.destination())

    @property
    def source(self) -> tp.Union[Piece, Square]:
        """Source, either from piece stand or a board square.

        Returns
        -------
        tp.Union[Piece, Square]
            A board square or piece stand.
        """
        value = self._move.source().value
        if value < _as.MS_CH.value:
            return Square(_as.SquareEnum(value))
        return Piece(_as.PieceTypeEnum(value - 12))

    def is_drop(self) -> bool:
        """Return true if the move is dropping move.

        Returns
        -------
        bool
            True if the move is dropping move, otherwise false.
        """
        return self._move.is_drop()
