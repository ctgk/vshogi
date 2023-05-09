import abc
import typing as tp

import numpy as np

from vshogi._vshogi import Color, Result


class Game(abc.ABC):
    """Base class for Shogi games."""

    @classmethod
    @abc.abstractmethod
    def _get_backend_game_class(cls) -> type:
        pass

    @classmethod
    @abc.abstractmethod
    def _get_move_class(cls) -> type:
        pass

    def __init__(self, sfen: tp.Optional[str] = None) -> None:
        """Initialize shogi game.

        Parameters
        ----------
        sfen : tp.Optional[str], optional
            Initial game status in SFEN, by default None
        """
        cls_ = self._get_backend_game_class()
        self._game = cls_() if sfen is None else cls_(sfen)

    def __array__(self) -> np.ndarray:
        """Return numpy array of DL-shogi input format.

        Returns
        -------
        np.ndarray
            DL-shogi input.
        """
        return np.asarray(self._game)

    @property
    def turn(self) -> Color:
        """Return current turn.

        Returns
        -------
        Color
            Current turn.
        """
        return self._game.get_turn()

    @property
    def board(self):
        """Return current game board.

        Returns
        -------
        Board
            Current game board.
        """
        return self._game.get_board()

    def stand(self, c: Color) -> dict:
        """Return pieces on the given color stand.

        Parameters
        ----------
        c : Color
            Color of the stand to get.

        Returns
        -------
        dict
            Pieces on the color stand.
        """
        return self._game.get_stand(c).to_dict()

    @property
    def result(self) -> Result:
        """Return result of the game.

        Returns
        -------
        Result
            Result of the game.
        """
        return self._game.get_result()

    @property
    def record_length(self) -> int:
        """Return length of the game record.

        Returns
        -------
        int
            Length of the game record.
        """
        return self._game.record_length()

    def apply(self, move=None, *arg, **kwargs) -> 'Game':
        """Apply a move.

        Parameters
        ----------
        move : Move
            Move to apply to the current state.
            But if there are multiple arguments, they are treated as parameters
            of move class initialization and converted automatically.

        Returns
        -------
        Game
            Game with the move applied.
        """
        if arg or kwargs:
            move = self._get_move_class()(move, *arg, **kwargs)
        self._game.apply(move)
        return self

    def is_legal(self, move=None, *arg, **kwargs) -> bool:
        """Return true if the move is legal at the current state.

        Parameters
        ----------
        move : Move
            Input move to check.
            But if there are multiple arguments, they are treated as parameters
            of move class initialization and converted automatically.

        Returns
        -------
        bool
            True if the move is legal, otherwise false.
        """
        if arg or kwargs:
            move = self._get_move_class()(move, *arg, **kwargs)
        return self._game.is_legal(move)

    def get_legal_moves(self) -> list:
        """Return list of legal moves at the current state.

        Returns
        -------
        list
            List of legal moves.
        """
        return self._game.get_legal_moves()

    def to_sfen(self, include_move_count: bool = True) -> str:
        """Return current game state in SFEN.

        Parameters
        ----------
        include_move_count : bool, optional
            Include move count after pieces in hand if True.

        Returns
        -------
        str
            Current game state in SFEN.
        """
        return self._game.to_sfen(include_move_count)

    def get_move_at(self, n: int):
        """Return n-th move applied to the game, where n starts from 0.

        Parameters
        ----------
        n : int
            Input index.

        Returns
        -------
        Move
            N-th move.
        """
        return self._game.get_move_at(n)

    def get_sfen_at(self, n: int) -> str:
        """Return n-th game state in SFEN, where n starts from 0.

        Parameters
        ----------
        n : int
            Input index.

        Returns
        -------
        str
            N-th game state in SFEN.
        """
        return self._game.get_sfen_at(n)

    def __repr__(self) -> str:
        """Return representation of the object for debugging.

        Returns
        -------
        str
            Representation of the object for debugging.
        """
        return f'{self.__class__.__name__}(sfen="{self.to_sfen()}")'

    def __str__(self) -> str:
        """Return string describing current game state.

        Returns
        -------
        str
            Current game state.
        """
        r = self.result
        return '\n'.join((
            f'Turn: {self.turn.name}' if r == Result.ONGOING else r.name,
            f'White: {str(self._game.get_stand(Color.WHITE))}',
            repr(self.board),
            f'Black: {str(self._game.get_stand(Color.BLACK))}',
        ))

    def copy(self) -> 'Game':
        """Return copy of the game.

        Returns
        -------
        Game
            Copy of the game object.
        """
        g = self.__class__()
        g._game = self._game.copy()
        return g
