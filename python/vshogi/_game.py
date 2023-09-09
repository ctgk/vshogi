import abc
import typing as tp

import numpy as np

from vshogi._vshogi import Color, Result


Move = tp.TypeVar('Move')


class _ClassProperty:

    def __init__(self, func: callable):
        self._func = func

    def __get__(self, _, owner):
        return self._func(owner)


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

    @classmethod
    @abc.abstractmethod
    def _get_node_class(cls) -> type:
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

    @_ClassProperty
    def ranks(self) -> int:
        """Return number of horizontal rows of the board.

        Returns
        -------
        int
            Number of horizontal rows of the board.
        """
        return self._get_backend_game_class().ranks()

    @_ClassProperty
    def files(self) -> int:
        """Return number of vertical columns of the board.

        Returns
        -------
        int
            Number of vertical columns of the board.
        """
        return self._get_backend_game_class().files()

    @_ClassProperty
    def feature_channels(self) -> int:
        """Return number of channels of feature map of the game.

        Returns
        -------
        int
            Number of channels of feature map of the game.
        """
        return self._get_backend_game_class().feature_channels()

    @_ClassProperty
    def num_dlshogi_policy(self) -> int:
        """Return number of DL-shogi policy options in the game.

        Returns
        -------
        int
            Number of DL-shogi policy options in the game.
        """
        return self._get_backend_game_class().num_dlshogi_policy()

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

    @classmethod
    def _get_move(cls, move=None, *arg, **kwargs) -> Move:
        if not (arg or kwargs):
            return move
        if move is None:
            return cls._get_move_class()(*arg, **kwargs)
        return cls._get_move_class()(move, *arg, **kwargs)

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

        Examples
        --------
        >>> import vshogi.minishogi as shogi
        >>> game = shogi.Game()
        >>> game.apply(shogi.Move(shogi.D3, shogi.E2))
        Game(sfen="rbsgk/4p/5/P1B2/KGS1R w - 2")
        >>> game.apply(shogi.B2, shogi.A3)
        Game(sfen="rb1gk/3sp/5/P1B2/KGS1R b - 3")
        >>> game.apply(dst=shogi.D2, src=shogi.E3)
        Game(sfen="rb1gk/3sp/5/P1BS1/KG2R w - 4")
        >>> game.apply(shogi.B3, shogi.A4).apply(shogi.D4, shogi.E4)
        Game(sfen="r2gk/2bsp/5/PGBS1/K3R w - 6")
        """
        move = self._get_move(move, *arg, **kwargs)
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
        move = self._get_move(move, *arg, **kwargs)
        return self._game.is_legal(move)

    def get_legal_moves(self) -> tp.List[Move]:
        """Return list of legal moves at the current state.

        Returns
        -------
        tp.List[Move]
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

        Examples
        --------
        >>> import vshogi.minishogi as shogi
        >>> game = shogi.Game()
        >>> game.apply(shogi.D3, shogi.E2).apply(shogi.B2, shogi.A3)
        Game(sfen="rb1gk/3sp/5/P1B2/KGS1R b - 3")
        >>> game.get_move_at(0)
        Move(dst=SQ_3D, src=SQ_2E)
        >>> game.get_move_at(1)
        Move(dst=SQ_2B, src=SQ_3A)
        >>> game.get_move_at(-1)
        Move(dst=SQ_2B, src=SQ_3A)
        """
        if n < 0:
            n = self.record_length + n
        return self._game.get_move_at(n)

    def get_sfen_at(self, n: int, include_move_count: bool = True) -> str:
        """Return n-th game state in SFEN, where n starts from 0.

        Parameters
        ----------
        n : int
            Input index.
        include_move_count : bool
            Include move count in SFEN if true, otherwise excluded.

        Returns
        -------
        str
            N-th game state in SFEN.

        Examples
        --------
        >>> import vshogi.minishogi as shogi
        >>> game = shogi.Game()
        >>> game.apply(shogi.D3, shogi.E2).apply(shogi.B2, shogi.A3)
        Game(sfen="rb1gk/3sp/5/P1B2/KGS1R b - 3")
        >>> game.get_sfen_at(0)
        'rbsgk/4p/5/P4/KGSBR b - 1'
        >>> game.get_sfen_at(1)
        'rbsgk/4p/5/P1B2/KGS1R w - 2'
        >>> game.get_sfen_at(-1)
        'rbsgk/4p/5/P1B2/KGS1R w - 2'
        """
        if n < 0:
            n = self.record_length + n
        return self._game.get_sfen_at(n, include_move_count)

    def to_policy_probas(
        self,
        dlshogi_logits: np.ndarray,
    ) -> tp.Dict[Move, float]:
        """Return dict of probabilities of legal actions.

        Parameters
        ----------
        dlshogi_logits : np.ndarray
            Array of DL-shogi format policy logits.
            Note that the logits are in view from the turn player. Please see
            the examples.

        Returns
        -------
        tp.Dict[Move, float]
            Dictionary of probabilities of legal actions.

        Examples
        --------
        >>> import numpy as np; import vshogi.animal_shogi as shogi
        >>> onehot = lambda index: np.eye(12 * 11, dtype=np.float32)[index]
        >>> # Note that `Move(dst=B2, src=B3)._to_dlshogi_policy_index() == 50`
        >>> shogi.Game('gle/1c1/1C1/ELG b -').to_policy_probas(
        ...     onehot(50) * 10)[shogi.Move(dst=shogi.B2, src=shogi.B3)]
        ...     #doctest: +ELLIPSIS
        0.999...
        >>> shogi.Game('gle/1c1/1C1/ELG w -').to_policy_probas(
        ...     onehot(50) * 10)[shogi.Move(dst=shogi.B3, src=shogi.B2)]
        ...     #doctest: +ELLIPSIS
        0.999...
        """
        return self._game._policy_logits_to_policy_dict_probas(dlshogi_logits)

    def to_dlshogi_policy(
        self,
        action: Move,
        max_value: float = 1.,
    ) -> np.ndarray:
        """Convert an action into DL-shogi policy array.

        A value of an element corresponding to `action` equals to `max_value`.
        The rest of the value (`1 - max_value`) is equally divided to the
        elements of the rest of the legal actions.

        Parameters
        ----------
        action : Move
            Action to turn into DL-shogi policy format.
        max_value : float, optional
            Policy value for the action, by default 1.

        Returns
        -------
        np.ndarray
            DL-shogi policy array.

        Raises
        ------
        ValueError
            `max_value` is out-of-range.
        """
        if ((max_value > 1) or (max_value <= 0)):
            raise ValueError(
                f"`max_value` must be in range (0, 1], but was {max_value}.")
        return self._game.to_dlshogi_policy(action, max_value)

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
