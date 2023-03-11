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
    Move(MoveSource.B3 -> Square.B2)
    >>> m.source
    MoveSource.B3
    >>> m.destination
    Square.B2
    >>> m.is_drop()
    False
    >>> Move(shogi.B2, shogi.GI).is_drop()
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
            f'{self.__class__.__name__}({repr(self.source)} '
            f'-> {repr(self.destination)})'
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
    def source(self) -> MoveSource:
        """Source, either from piece stand or a board square.

        Returns
        -------
        MoveSourceEnum
            A board square or piece stand.
        """
        return MoveSource(self._move.source())

    def is_drop(self) -> bool:
        """Return true if the move is dropping move.

        Returns
        -------
        bool
            True if the move is dropping move, otherwise false.
        """
        return self._move.is_drop()
