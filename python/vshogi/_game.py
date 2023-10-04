import abc
import sys
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
        if sfen is None:
            self._game = cls_()
        elif isinstance(sfen, cls_):
            self._game = sfen
        else:
            self._game = cls_(sfen)

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

        Examples
        --------
        >>> import vshogi.animal_shogi as shogi
        >>> game = shogi.Game()
        >>> game.result
        Result.ONGOING
        >>> game.apply(shogi.A3, shogi.B4).apply(
        ...     shogi.A2, shogi.B1).apply(shogi.A2, shogi.A3)
        Game(sfen="g1e/Lc1/1C1/E1G w - 4")
        >>> game.result
        Result.BLACK_WIN
        """
        return self._game.get_result()

    @property
    def record_length(self) -> int:
        """Return length of the game record.

        Returns
        -------
        int
            Length of the game record.

        Examples
        --------
        >>> import vshogi.minishogi as shogi
        >>> game = shogi.Game()
        >>> game.record_length
        0
        >>> game.apply(shogi.D3, shogi.E2).record_length
        1
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

    def dump_records(
        self,
        getters: tp.Tuple[
            tp.Callable[['Game', int], object],
            tp.Iterable[tp.Callable[['Game', int], object]],
        ] = lambda g, i: g.get_sfen_at(i),
        names: tp.Optional[tp.Iterable[str]] = None,
        sep: str = '\t',
        file_: tp.TextIO = sys.stdout,
    ) -> None:
        r"""Dump game records.

        Parameters
        ----------
        getters :
            Callable or iterable of callables to get desired values.
        names : tp.Optional[tp.Iterable[str]], optional
            Dump a header if passed, by default None
        sep : str, optional
            Separator of column names and values, by default '\t'
        file_ : tp.TextIO, optional
            Location to dump to, by default sys.stdout.

        Examples
        --------
        >>> import vshogi.animal_shogi as shogi; import io
        >>> game = shogi.Game().apply(shogi.A3, shogi.B4).apply(
        ...     shogi.A2, shogi.B1).apply(shogi.A2, shogi.A3)
        >>> with io.StringIO() as f:
        ...     game.dump_records(file_=f)
        ...     _ = f.seek(0)
        ...     print(f.read())
        gle/1c1/1C1/ELG b - 1
        gle/1c1/LC1/E1G w - 2
        g1e/lc1/LC1/E1G b - 3
        <BLANKLINE>
        >>> with io.StringIO() as f:
        ...     game.dump_records(
        ...         (
        ...             lambda g, i: g.get_sfen_at(i),
        ...             lambda g, i: g.get_move_at(i),
        ...             lambda g, i: g.result,
        ...         ),
        ...         names=('sfen', 'move', 'result'),
        ...         file_=f,
        ...     )
        ...     _ = f.seek(0)
        ...     print(f.read())  #doctest: +NORMALIZE_WHITESPACE
        sfen        move    result
        gle/1c1/1C1/ELG b - 1       Move(dst=A3, src=B4)    Result.BLACK_WIN
        gle/1c1/LC1/E1G w - 2       Move(dst=A2, src=B1)    Result.BLACK_WIN
        g1e/lc1/LC1/E1G b - 3       Move(dst=A2, src=A3)    Result.BLACK_WIN
        <BLANKLINE>
        """
        if names is not None:
            print(*names, file=file_, sep=sep)
        if callable(getters):
            getters = (getters,)
        for i in range(self.record_length):
            print(
                *(getter(self, i) for getter in getters),
                sep='\t', file=file_,
            )

    def to_dlshogi_features(self, *, out: np.ndarray = None) -> np.ndarray:
        """Return DL-shogi features.

        Parameters
        ----------
        out : np.ndarray, optional
            Dump DL-shogi features into `out` if given.

        Returns
        -------
        np.ndarray
            Return DL-shogi feature array if `out` is not None.

        Examples
        --------
        >>> import vshogi.animal_shogi as shogi
        >>> x = shogi.Game("3/elg/1C1/ELG b C").to_dlshogi_features()
        >>> print(x[0, ..., 0]) # Black's CH on stand
        [[1. 1. 1.]
         [1. 1. 1.]
         [1. 1. 1.]
         [1. 1. 1.]]
        >>> print(x[0, ..., 6]) # Black's LI on board
        [[0. 0. 0.]
         [0. 0. 0.]
         [0. 0. 0.]
         [0. 1. 0.]]
        >>> print(x[0, ..., 8]) # White's CH on stand
        [[0. 0. 0.]
         [0. 0. 0.]
         [0. 0. 0.]
         [0. 0. 0.]]
        >>> print(x[0, ..., 14]) # White's LI on board
        [[0. 0. 0.]
         [0. 1. 0.]
         [0. 0. 0.]
         [0. 0. 0.]]
        >>> shogi.Game("3/elg/1C1/ELG C w").to_dlshogi_features(out=x)
        >>> print(x[0, ..., 6]) # White's LI on board from white's view
        [[0. 0. 0.]
         [0. 0. 0.]
         [0. 1. 0.]
         [0. 0. 0.]]
        >>> print(x[0, ..., 14]) # Black's LI on board from white's view
        [[0. 1. 0.]
         [0. 0. 0.]
         [0. 0. 0.]
         [0. 0. 0.]]
        """
        if out is None:
            return self._game.to_dlshogi_features()
        else:
            return self._game.to_dlshogi_features(out)

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
        return self.__class__(self._game.copy())
