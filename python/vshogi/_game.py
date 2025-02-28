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
    def _get_mcts_node_class(cls) -> type:
        pass

    @classmethod
    @abc.abstractmethod
    def _get_mcts_searcher_class(cls) -> type:
        pass

    @classmethod
    @abc.abstractmethod
    def _get_dfpn_searcher_class(cls) -> type:
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
        self._move_list: tp.List[Move] = []
        self._sfen_list: tp.List[str] = []

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

    @classmethod
    def get_attention(cls) -> np.ndarray:
        """Return attention matrix of the game.

        Returns
        -------
        np.ndarray
            Attention matrix whose size is (F, R, F, R) where F and R are
            number of files and ranks respectively.
            The matrix (A) has value 1 at (fi, ri, fj, rj) if there is a piece
            that can move from (fi, ri) square to (fj, rj) square, otherwise 0.
        """
        return cls._get_backend_game_class().get_attention().reshape(
            cls.files, cls.ranks, cls.files, cls.ranks)

    @classmethod
    def get_adjacent_attention(cls) -> np.ndarray:
        """Return adjacent attention matrix of the game.

        Returns
        -------
        np.ndarray
            Attention matrix whose size is (F, R, F, R) where F and R are
            number of files and ranks respectively.
            The matrix (A) has value 1 at (fi, ri, fj, rj) if (fi, ri) square
            lies along adjacent direction from (fj, rj) square, otherwise 0.
        """
        return cls._get_backend_game_class().get_adjacent_attention().reshape(
            cls.files, cls.ranks, cls.files, cls.ranks)

    @classmethod
    def get_diagonal_attention(cls) -> np.ndarray:
        """Return adjacent attention matrix of the game.

        Returns
        -------
        np.ndarray
            Attention matrix whose size is (F, R, F, R) where F and R are
            number of files and ranks respectively.
            The matrix (A) has value 1 at (fi, ri, fj, rj) if (fi, ri) square
            lies along diagonal direction from (fj, rj) square, otherwise 0.
        """
        return cls._get_backend_game_class().get_diagonal_attention().reshape(
            cls.files, cls.ranks, cls.files, cls.ranks)

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
        >>> import vshogi.minishogi as shogi
        >>> game = shogi.Game()
        >>> game.result
        Result.ONGOING
        >>> game.apply("4e3d").apply("2a3b").apply("3d2c").apply("3b4c").result
        Result.ONGOING
        >>> game.apply("2c1b").result
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

    @property
    def zobrist_hash(self) -> int:
        """Return zobrist hash of the current game position.

        Note
        ----
        - The hash value changes every time you import `vshogi` library.
        - Hash values can be the same even when the game positions differ.

        Returns
        -------
        int
            Zobrist hash of the current game position.
        """
        return self._game.get_zobrist_hash()

    @classmethod
    def _get_move(cls, move=None, *arg, **kwargs) -> Move:
        if not (arg or kwargs):
            if isinstance(move, str):
                return cls._get_move_class()(move)
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
        >>> game.apply('4a3b').apply('4e4d')
        Game(sfen="r2gk/2bsp/5/PGBS1/K3R w - 6")
        """
        move = self._get_move(move, *arg, **kwargs)
        self._move_list.append(move)
        self._sfen_list.append(self.to_sfen(False))
        self._game.apply(move)
        return self

    def resign(self) -> 'Game':
        """Resign game by current turn player.

        Returns
        -------
        Game
            Game after resignation.
        """
        self._game.resign()
        return self

    def undo(self) -> 'Game':
        """Undo a previous move.

        Returns
        -------
        Game
            Previous game position.

        Examples
        --------
        >>> from vshogi.minishogi import *
        >>> g = Game()
        >>> g.apply("1e1b")
        Game(sfen="rbsgk/4R/5/P4/KGSB1 w P 2")
        >>> g.undo()
        Game(sfen="rbsgk/4p/5/P4/KGSBR b - 1")
        """
        self._game.undo()
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
        return self._move_list[n]

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
        if include_move_count:
            return self._sfen_list[n] + f' {n + 1}'
        else:
            return self._sfen_list[n]

    def dump_records(
        self,
        getters: tp.Tuple[
            tp.Callable[['Game', int], object],
            tp.Iterable[tp.Callable[['Game', int], object]],
        ] = lambda g, i: g.get_sfen_at(i),
        names: tp.Optional[tp.Iterable[str]] = None,
        sep: str = '\t',
        file_: tp.TextIO = sys.stdout,
        color_filter: tp.Optional[Color] = None,
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
        color_filter : tp.Optional[Color], optional
            Dump only BLACK or WHITE turn records if given.

        Examples
        --------
        >>> import vshogi.minishogi as shogi; import io
        >>> game = shogi.Game()
        >>> moves = ["4e3d", "2a3b", "3d2c", "3b4c", "2c1b"]
        >>> for m in moves:
        ...     game = game.apply(m)
        >>> with io.StringIO() as f:
        ...     game.dump_records(file_=f)
        ...     _ = f.seek(0)
        ...     print(f.read())
        rbsgk/4p/5/P4/KGSBR b - 1
        rbsgk/4p/5/P1G2/K1SBR w - 2
        rbs1k/2g1p/5/P1G2/K1SBR b - 3
        rbs1k/2g1p/3G1/P4/K1SBR w - 4
        rbs1k/4p/1g1G1/P4/K1SBR b - 5
        <BLANKLINE>
        >>> with io.StringIO() as f:
        ...     game.dump_records(
        ...         (
        ...             lambda g, i: g.get_sfen_at(i),
        ...             lambda g, i: g.get_move_at(i).to_usi(),
        ...             lambda g, i: g.result,
        ...         ),
        ...         names=('sfen', 'move', 'result'),
        ...         file_=f,
        ...         color_filter=shogi.BLACK,
        ...     )
        ...     _ = f.seek(0)
        ...     print(f.read())  #doctest: +NORMALIZE_WHITESPACE
        sfen        move    result
        rbsgk/4p/5/P4/KGSBR b - 1   4e3d    Result.BLACK_WIN
        rbs1k/2g1p/5/P1G2/K1SBR b - 3       3d2c    Result.BLACK_WIN
        rbs1k/4p/1g1G1/P4/K1SBR b - 5       2c1b    Result.BLACK_WIN
        <BLANKLINE>
        """
        if names is not None:
            print(*names, file=file_, sep=sep)
        if callable(getters):
            getters = (getters,)
        for i in range(self.record_length):
            if (color_filter == Color.WHITE) and (i % 2 == 0):
                continue
            elif (color_filter == Color.BLACK) and (i % 2 == 1):
                continue
            print(
                *(getter(self, i) for getter in getters),
                sep='\t', file=file_,
            )

    def hflip(self) -> 'Game':
        """Return new game with horizontally flipped current positions.

        Returns
        -------
        Game
            New game with horizontally flipped current positions.

        Examples
        --------
        >>> import vshogi.minishogi as shogi
        >>> g = shogi.Game().apply("5d5c")
        >>> print(g)
        Turn: WHITE
        White: -
            5   4   3   2   1
          +---+---+---+---+---+
        A |-HI|-KA|-GI|-KI|-OU|
          +---+---+---+---+---+
        B |   |   |   |   |-FU|
          +---+---+---+---+---+
        C |+FU|   |   |   |   |
          +---+---+---+---+---+
        D |   |   |   |   |   |
          +---+---+---+---+---+
        E |+OU|+KI|+GI|+KA|+HI|
          +---+---+---+---+---+
        Black: -
        >>> g_hflip = g.hflip()
        >>> print(g_hflip)
        Turn: WHITE
        White: -
            5   4   3   2   1
          +---+---+---+---+---+
        A |-OU|-KI|-GI|-KA|-HI|
          +---+---+---+---+---+
        B |-FU|   |   |   |   |
          +---+---+---+---+---+
        C |   |   |   |   |+FU|
          +---+---+---+---+---+
        D |   |   |   |   |   |
          +---+---+---+---+---+
        E |+HI|+KA|+GI|+KI|+OU|
          +---+---+---+---+---+
        Black: -
        >>> g.record_length
        1
        >>> g_hflip.record_length
        0
        """
        return self.__class__(self._game.hflip())

    def rotate(self) -> 'Game':
        """Return new game with rotated current positions.

        Returns
        -------
        Game
            New game with rotated current positions.

        Examples
        --------
        >>> import vshogi.minishogi as shogi
        >>> g = shogi.Game().apply("5d5c")
        >>> print(g)
        Turn: WHITE
        White: -
            5   4   3   2   1
          +---+---+---+---+---+
        A |-HI|-KA|-GI|-KI|-OU|
          +---+---+---+---+---+
        B |   |   |   |   |-FU|
          +---+---+---+---+---+
        C |+FU|   |   |   |   |
          +---+---+---+---+---+
        D |   |   |   |   |   |
          +---+---+---+---+---+
        E |+OU|+KI|+GI|+KA|+HI|
          +---+---+---+---+---+
        Black: -
        >>> g_rotated = g.rotate()
        >>> print(g_rotated)
        Turn: BLACK
        White: -
            5   4   3   2   1
          +---+---+---+---+---+
        A |-HI|-KA|-GI|-KI|-OU|
          +---+---+---+---+---+
        B |   |   |   |   |   |
          +---+---+---+---+---+
        C |   |   |   |   |-FU|
          +---+---+---+---+---+
        D |+FU|   |   |   |   |
          +---+---+---+---+---+
        E |+OU|+KI|+GI|+KA|+HI|
          +---+---+---+---+---+
        Black: -
        >>> g.record_length
        1
        >>> g_rotated.record_length
        0
        """
        sfen = self.to_sfen(include_move_count=False)
        sfen_board, sfen_turn, sfen_stand = sfen.split(' ')
        sfen_board = ''.join([s if s == '+' else f'{s}.' for s in sfen_board])
        sfen_rotated = ''
        for s in reversed(sfen_board.split('.')):
            if s.islower():
                sfen_rotated = sfen_rotated + s.upper()
            elif s.isupper():
                sfen_rotated = sfen_rotated + s.lower()
            else:
                sfen_rotated = sfen_rotated + s  # '/', '1', ...

        sfen_rotated = sfen_rotated + ' ' + (
            'b ' if sfen_turn == 'w' else 'w ')

        for s in sfen_stand:
            if s.islower():
                sfen_rotated = sfen_rotated + s.upper()
            elif s.isupper():
                sfen_rotated = sfen_rotated + s.lower()
            else:
                sfen_rotated = sfen_rotated + s  # '/', '1', ...
        return self.__class__(sfen_rotated)

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
        >>> import vshogi.minishogi as shogi; import numpy as np
        >>> x = shogi.Game("rbs1k/4g/5/P4/KGSB1 b P").to_dlshogi_features()
        >>> print(x[0, ..., 0]) # Black's FU on stand
        [[1. 1. 1. 1. 1.]
         [1. 1. 1. 1. 1.]
         [1. 1. 1. 1. 1.]
         [1. 1. 1. 1. 1.]
         [1. 1. 1. 1. 1.]]
        >>> print(np.rot90(x[0, ..., 10], -1)) # Black's OU on board
        [[0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [1. 0. 0. 0. 0.]]
        >>> print(np.rot90(x[0, ..., 15], -1)) # White's FU on stand
        [[0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]]
        >>> print(np.rot90(x[0, ..., 25], -1)) # White's LI on board
        [[0. 0. 0. 0. 1.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]]
        >>> shogi.Game("rbs1k/4g/5/P4/KGSB1 w P").to_dlshogi_features(out=x)
        >>> print(np.rot90(x[0, ..., 9], -1)) # White's KI from white's view
        [[0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]
         [1. 0. 0. 0. 0.]
         [0. 0. 0. 0. 0.]]
        """
        if out is None:
            return self._game.to_dlshogi_features()
        else:
            return self._game.to_dlshogi_features(out)

    def to_dlshogi_policy(
        self,
        action_proba: tp.Union[Move, tp.Dict[Move, int]],
        *,
        default_value: float = 0.,
    ) -> np.ndarray:
        """Convert an action into DL-shogi policy array.

        A value of an element corresponding to `action` equals to `max_value`.
        The rest of the value (`1 - max_value`) is equally divided to the
        elements of the rest of the legal actions.

        Parameters
        ----------
        action_proba : tp.Union[Move, tp.Dict[Move, int]]
            One-hot action to turn into DL-shogi policy format,
            or dict of actions with their probabilities.
            If the probabilities do not sum up to 1, they will be normalized.
        default_value: float, optional


        Returns
        -------
        np.ndarray
            DL-shogi policy array.

        Raises
        ------
        ValueError
            `max_value` is out-of-range.
        """
        if isinstance(action_proba, dict):
            s = sum(action_proba.values())
            action_proba = {k: v / s for k, v in action_proba.items()}
        elif isinstance(action_proba, self._get_move_class()):
            action_proba = {
                k: 1. if k == action_proba else 0.
                for k in self.get_legal_moves()
            }
        else:
            raise TypeError(
                f'Unsupported type for `action_proba`: {type(action_proba)}')
        return self._game.to_dlshogi_policy(action_proba, default_value)

    def masked_softmax(self, logits: np.ndarray) -> tp.Dict[Move, float]:
        """Return masked softmax given logits.

        Parameters
        ----------
        logits : np.ndarray
            Logit values including values corresponding to invalid actions.

        Returns
        -------
        tp.Dict[Move, float]
            Probability of each action.

        Examples
        --------
        >>> import vshogi.minishogi as shogi
        >>> g = shogi.Game("4k/5/5/5/4S b -")
        >>> logits = np.zeros(g.num_dlshogi_policy)
        >>> proba = g.masked_softmax(logits)
        >>> proba[shogi.Move("1e1d")]
        0.5
        >>> proba[shogi.Move("1e2d")]
        0.5
        """
        return self._game.masked_softmax(logits)

    def get_mate_moves_if_any(
        self,
        num_dfpn_nodes: int = 10000,
    ) -> tp.Union[tp.List[Move], None]:
        """Return a sequence of moves leading to checkmate if there is any.

        Parameters
        ----------
        num_dfpn_nodes : int, optional
            Number of nodes to explore using Depth First Proof Number
            algorithm, by default 1000.

        Returns
        -------
        tp.Union[tp.List[Move], None]
            A sequence of moves to check mate if any otherwise `None`.

        Examples
        --------
        >>> import vshogi.minishogi as shogi
        >>> g = shogi.Game("5/2k2/5/2P2/2K2 b 2G")
        >>> g.get_mate_moves_if_any() # doctest: +ELLIPSIS
        [Move(dst=SQ_3C, src=KI), ...]
        """
        return self._game.get_mate_moves_if_any(num_dfpn_nodes)

    @abc.abstractmethod
    def _piece_value_func(self) -> float:
        pass

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
