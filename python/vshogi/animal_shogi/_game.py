import typing as tp

from vshogi._vshogi.animal_shogi import _Game
from vshogi.animal_shogi._move import Move


class Game:
    """Animal Shogi game."""

    def __init__(self, sfen: tp.Optional[str] = None) -> None:
        """Initialize Animal Shogi game.

        Parameters
        ----------
        sfen : tp.Optional[str], optional
            Initial state of the game, by default None
        """
        self._game = _Game() if sfen is None else _Game(sfen)

    @property
    def turn(self) -> str:
        """Return current turn.

        Returns
        -------
        str
        """
        return self._game.get_turn()

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
        pass
