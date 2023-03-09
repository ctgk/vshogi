import typing as tp

from vshogi._vshogi.animal_shogi import _Game
from vshogi.animal_shogi._board import Board
from vshogi.animal_shogi._color import Color
from vshogi.animal_shogi._move import Move


class Game:
    """Animal Shogi game.

    Examples
    --------
    >>> game = Game()
    >>> game.turn
    Color.BLACK
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
        return f'Turn: {self.turn.name}\n' + repr(self.board)
