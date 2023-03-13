import typing as tp

from vshogi._vshogi.animal_shogi import Move as _Move, Piece, Square


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
        source: tp.Union[Square, Piece],
    ) -> None:
        """Initialize move object.

        Parameters
        ----------
        destination : Square
            Destination on the board.
        source : tp.Union[Square, Piece]
            Source, either from piece stand or a board square.
        """
        self._move = _Move(destination, source)

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
        return self._move.destination()

    @property
    def source(self) -> tp.Union[Piece, Square]:
        """Source, either from piece stand or a board square.

        Returns
        -------
        tp.Union[Piece, Square]
            A board square or piece stand.
        """
        return self._move.source()

    def is_drop(self) -> bool:
        """Return true if the move is dropping move.

        Returns
        -------
        bool
            True if the move is dropping move, otherwise false.
        """
        return self._move.is_drop()
