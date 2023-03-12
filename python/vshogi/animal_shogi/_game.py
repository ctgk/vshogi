import typing as tp

import vshogi._vshogi.animal_shogi as _as
from vshogi._enum import _Enum
from vshogi._vshogi.animal_shogi import _Game
from vshogi.animal_shogi._board import Board
from vshogi.animal_shogi._color import Color
from vshogi.animal_shogi._move import Move


class Result(_Enum):
    """Enumeration of game results."""

    ONGOING = _as.ONGOING
    DRAW = _as.DRAW
    BLACK_WIN = _as.BLACK_WIN
    WHITE_WIN = _as.WHITE_WIN


class Game:
    """Animal Shogi game.

    Examples
    --------
    >>> import vshogi.animal_shogi as shogi
    >>> game = Game()
    >>> game.turn
    Color.BLACK
    >>> game.result
    Result.ONGOING
    >>> game
    Turn: BLACK
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
    >>> game.apply_move(Move(shogi.B2, shogi.B3))
    Turn: WHITE
        A  B  C
      *--*--*--*
    1 |-G|-L|-E|
      *--*--*--*
    2 |  |+C|  |
      *--*--*--*
    3 |  |  |  |
      *--*--*--*
    4 |+E|+L|+G|
      *--*--*--*
    >>> game.apply_move(Move(shogi.A2, shogi.A1)).apply_move(
    ...     Move(shogi.B1, shogi.B2))
    BLACK_WIN
        A  B  C
      *--*--*--*
    1 |  |+H|-E|
      *--*--*--*
    2 |-G|  |  |
      *--*--*--*
    3 |  |  |  |
      *--*--*--*
    4 |+E|+L|+G|
      *--*--*--*
    """

    def __init__(self, sfen: tp.Optional[str] = None) -> None:
        """Initialize Animal Shogi game.

        Parameters
        ----------
        sfen : tp.Optional[str], optional
            Initial state of the game, by default None
        """
        self._game = _Game() if sfen is None else _Game(sfen)

    @property
    def turn(self) -> Color:
        """Return current turn.

        Returns
        -------
        Color
        """
        return Color(self._game.get_turn())

    @property
    def board(self) -> Board:
        """Return current board.

        Returns
        -------
        Board
            Current board.
        """
        return Board(_board=self._game.get_board())

    @property
    def result(self) -> Result:
        """Return result of the game.

        Returns
        -------
        Result
            Result of the game.
        """
        return Result(self._game.get_result())

    def apply_move(self, move: Move) -> 'Game':
        """Apply a move.

        Parameters
        ----------
        move : Move
            Move to apply.

        Returns
        -------
        Game
            Game with the move applied.
        """
        self._game.apply_move(move._move)
        return self

    def __repr__(self) -> str:
        r = self.result
        return '\n'.join((
            f'Turn: {self.turn.name}' if r == Result.ONGOING else r.name,
            repr(self.board),
        ))
