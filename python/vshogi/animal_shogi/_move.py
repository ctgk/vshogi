import typing as tp

import vshogi._vshogi.animal_shogi as _as
from vshogi._enum import _Enum
from vshogi.animal_shogi._square import SquareEnum


class MoveSourceEnum(_Enum):
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
    >>> m = Move(shogi.SquareEnum.B2, shogi.MoveSourceEnum.B3)
    >>> m
    Move(MoveSourceEnum.B3 -> SquareEnum.B2)
    >>> m.source
    MoveSourceEnum.B3
    >>> m.destination
    SquareEnum.B2
    >>> m.is_drop()
    False
    >>> Move(shogi.SquareEnum.B2, shogi.MoveSourceEnum.GI).is_drop()
    True
    """

    def __init__(
        self,
        destination: SquareEnum,
        source: tp.Union[SquareEnum, MoveSourceEnum],
    ) -> None:
        """Initialize move object.

        Parameters
        ----------
        destination : SquareEnum
            Destination on the board.
        source : tp.Union[SquareEnum, MoveSourceEnum]
            Source, either from piece stand or a board square.
        """
        self._move = _as.Move(destination.value, source.value)

    def __repr__(self) -> str:
        return f'Move({repr(self.source)} -> {repr(self.destination)})'

    @property
    def destination(self) -> SquareEnum:
        """Destination on the board.

        Returns
        -------
        SquareEnum
            A board square.
        """
        return SquareEnum(self._move.destination())

    @property
    def source(self) -> MoveSourceEnum:
        """Source, either from piece stand or a board square.

        Returns
        -------
        MoveSourceEnum
            A board square or piece stand.
        """
        return MoveSourceEnum(self._move.source())

    def is_drop(self) -> bool:
        """Return true if the move is dropping move.

        Returns
        -------
        bool
            True if the move is dropping move, otherwise false.
        """
        return self._move.is_drop()
