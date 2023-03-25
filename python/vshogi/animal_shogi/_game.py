import typing as tp

import numpy as np

import vshogi._vshogi.animal_shogi as _shogi
from vshogi._vshogi.animal_shogi import Board
from vshogi._vshogi.animal_shogi import Color
from vshogi._vshogi.animal_shogi import Move
from vshogi._vshogi.animal_shogi import Result
from vshogi._vshogi.animal_shogi import Square
from vshogi._vshogi.animal_shogi import _Game


_shogi.Stand.__str__ = lambda self: ','.join([
    k.name[0] + ('' if v == 1 else str(v)) for k, v in self.to_dict().items()
    if v > 0
]) if self.any() else '-'
Board.__array__ = lambda self: np.array(
    [
        [self[Square.A1], self[Square.B1], self[Square.C1]],
        [self[Square.A2], self[Square.B2], self[Square.C2]],
        [self[Square.A3], self[Square.B3], self[Square.C3]],
        [self[Square.A4], self[Square.B4], self[Square.C4]],
    ],
    dtype=_shogi.BoardPiece,
)


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
    White: -
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
    Black: -
    >>> game.get_applicable_moves()
    [Move(B2 <- B3), Move(A3 <- B4), Move(C3 <- B4), Move(C3 <- C4)]
    >>> game.is_applicable(Move(shogi.B2, shogi.B3))
    True
    >>> game.is_applicable(Move(shogi.A3, shogi.A4))
    False
    >>> game.apply(Move(shogi.B2, shogi.B3))
    Turn: WHITE
    White: -
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
    Black: C
    >>> game.white_stand
    {Piece.CH: 0, Piece.EL: 0, Piece.GI: 0}
    >>> game.black_stand
    {Piece.CH: 1, Piece.EL: 0, Piece.GI: 0}
    >>> game.apply(Move(shogi.A2, shogi.A1)).apply(Move(shogi.B1, shogi.B2))
    BLACK_WIN
    White: -
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
    Black: C
    >>> game.record_length
    3
    >>> Game(game.get_state_hash_at(1))
    Turn: WHITE
    White: -
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
    Black: C
    >>> game.get_move_at(1)
    Move(A2 <- A1)
    """

    def __init__(self, hash_or_sfen: tp.Union[int, str, None] = None) -> None:
        """Initialize Animal Shogi game.

        Parameters
        ----------
        hash_or_sfen : tp.Union[int, str, None], optional
            Initial state of the game, by default None
        """
        self._game = _Game() if hash_or_sfen is None else _Game(hash_or_sfen)

    def __array__(self) -> np.ndarray:
        return np.asarray(self._game)

    @property
    def turn(self) -> Color:
        """Return current turn.

        Returns
        -------
        Color
        """
        return self._game.get_turn()

    @property
    def board(self) -> Board:
        """Return current board.

        Returns
        -------
        Board
            Current board.
        """
        return self._game.get_board()

    @property
    def black_stand(self) -> dict:
        """Return pieces on black stand.

        Returns
        -------
        dict
            Pieces on black stand.
        """
        return self._game.get_stand(_shogi.Color.BLACK).to_dict()

    @property
    def white_stand(self) -> dict:
        """Return pieces on white stand.

        Returns
        -------
        dict
            Pieces on white stand.
        """
        return self._game.get_stand(_shogi.Color.WHITE).to_dict()

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
            Length of the game record
        """
        return self._game.record_length()

    def hash_current_state(self) -> int:
        """Return hash value of the current state.

        Returns
        -------
        int
            Hash value of the current state.
        """
        return self._game.hash_current_state()

    def apply(self, move: Move) -> 'Game':
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
        self._game.apply(move)
        return self

    def is_applicable(self, move: Move) -> bool:
        """Return true if the move is applicable to the current game status.

        Unlike ordinary Shogi, the followings are legal (applicable):
        - Two Chicks in one file.
        - Checkmate by dropping a Chick.
        - Dropping a Chick on the final rank (, which will never promote)
        - Perpetual check.

        cf. https://en.wikipedia.org/wiki/D%C5%8Dbutsu_sh%C5%8Dgi#Play

        Parameters
        ----------
        move : Move
            Input move.

        Returns
        -------
        bool
            True if the move is applicable, otherwise false.
        """
        return self._game.is_applicable(move)

    def get_applicable_moves(self) -> tp.List[Move]:
        """Return list of applicable moves to the current state.

        Returns
        -------
        tp.List[Move]
            Applicable moves.
        """
        return self._game.get_applicable_moves()

    def get_move_at(self, n: int) -> Move:
        """Return n-th move of the game, where n starts from 0.

        Parameters
        ----------
        n : int
            Input index.

        Returns
        -------
        Move
            N-th move of the game.
        """
        return self._game.get_move_at(n)

    def get_state_hash_at(self, n: int) -> int:
        """Return hash value of n-th state of the game, where n starts from 0.

        Parameters
        ----------
        n : int
            Input index.

        Returns
        -------
        int
            N-th state of the game.
        """
        return self._game.get_state_hash_at(n)

    def __repr__(self) -> str:
        r = self.result
        return '\n'.join((
            f'Turn: {self.turn.name}' if r == Result.ONGOING else r.name,
            f'White: {str(self._game.get_stand(_shogi.Color.WHITE))}',
            repr(self.board),
            f'Black: {str(self._game.get_stand(_shogi.Color.BLACK))}',
        ))

    def copy(self) -> 'Game':
        """Return copy of the game object.

        Returns
        -------
        Game
            Copy of the game object.
        """
        g = Game()
        g._game = self._game.copy()
        return g
