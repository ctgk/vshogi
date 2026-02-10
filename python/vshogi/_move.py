import abc

import typing as tp


Square = tp.TypeVar("Square")
Piece = tp.TypeVar("Piece")


class Move(abc.ABC):
    """A class that represents a move in a game."""

    @tp.overload
    def __init__(
        self,
        src: Square,
        dst: Square,
        promote: bool = False,
    ) -> None:
        """Initialize a move given source, destination, and promotion.

        Parameters
        ----------
        src : Square
            Source square of the move
        dst : Square
            Destination square of the move
        promote : bool, optional
            Promotion flag, by default False
        """
        pass

    @tp.overload
    def __init__(self, src: Piece, dst: Square) -> None:
        """Initialize a drop move.

        Parameters
        ----------
        src : Piece
            Piece to drop from hand.
        dst : Square
            Destination square of the drop.
        """
        pass

    @tp.overload
    def __init__(self, sfen: str) -> None:
        """Initialize a move from its SFEN representation.

        Parameters
        ----------
        sfen : str
            SFEN string of the move.
        """
        pass

    @tp.overload
    def __init__(self, value: int) -> None:
        """Initialize a move from its integer encoding.

        Parameters
        ----------
        value : int
            Encoded move value.
        """
        pass

    @property
    @abc.abstractmethod
    def destination(self) -> Square:
        """Return the destination square of the move."""
        pass

    @property
    @abc.abstractmethod
    def promote(self) -> bool:
        """Return whether the move promotes a piece."""
        pass

    @property
    @abc.abstractmethod
    def source(self) -> Square | Piece:
        """Return the source square or dropped piece of the move."""
        pass

    @abc.abstractmethod
    def is_drop(self) -> bool:
        """Return whether the move is a drop from hand."""
        pass

    @abc.abstractmethod
    def rotate(self) -> "Move":
        """Return a rotated version of the move (board rotated 180 degrees)."""
        pass

    @abc.abstractmethod
    def hflip(self) -> "Move":
        """Return a horizontally flipped version of the move."""
        pass

    @staticmethod
    @abc.abstractmethod
    def _num_policy_per_square() -> int:
        """Return the number of policy indices per board square."""
        pass

    @abc.abstractmethod
    def __hash__(self) -> int:
        """Return the hash value of the move."""
        pass

    @abc.abstractmethod
    def to_sfen(self) -> str:
        """Return the SFEN representation of the move."""
        pass

    @abc.abstractmethod
    def __eq__(self, other: "Move") -> bool:
        """Return True if two moves are equal."""
        pass

    @abc.abstractmethod
    def __ne__(self, other: "Move") -> bool:
        """Return True if two moves are not equal."""
        pass
