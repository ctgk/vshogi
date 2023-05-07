import typing as tp

import numpy as np

from vshogi._game import Game as BaseGame
from vshogi._vshogi import Color
from vshogi._vshogi.animal_shogi import Board
from vshogi._vshogi.animal_shogi import BoardPiece
from vshogi._vshogi.animal_shogi import Move
from vshogi._vshogi.animal_shogi import Square
from vshogi._vshogi.animal_shogi import Stand
from vshogi._vshogi.animal_shogi import _Game as _AnimalshogiGame


Stand.__str__ = lambda self: ','.join([
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
    dtype=BoardPiece,
)


class Game(BaseGame):
    """Animal Shogi game.

    Examples
    --------
    >>> from vshogi.animal_shogi import *
    >>> game = Game()
    >>> game.turn
    Color.BLACK
    >>> game.result
    Result.ONGOING
    >>> repr(game)
    'Game(sfen="gle/1c1/1C1/ELG b - 1")'
    >>> print(game)
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
    >>> game.get_legal_moves()
    [Move(dst=B2, src=B3), Move(dst=A3, src=B4), ...
    >>> game.is_legal(Move(B2, B3))
    True
    >>> game.is_legal(Move(A3, A4))
    False
    >>> game.apply(Move(B2, B3))
    Game(sfen="gle/1C1/3/ELG w C 2")
    >>> game.white_stand
    {Piece.CH: 0, Piece.EL: 0, Piece.GI: 0}
    >>> game.black_stand
    {Piece.CH: 1, Piece.EL: 0, Piece.GI: 0}
    >>> game.apply(Move(A2, A1)).apply(Move(B1, B2))
    Game(sfen="1He/g2/3/ELG w C 4")
    >>> print(game)
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
    Game(sfen="gle/1C1/3/ELG w C 1")
    >>> game.get_move_at(1)
    Move(dst=A2, src=A1)
    """

    def __init__(self, hash_or_sfen: tp.Union[int, str, None] = None) -> None:
        """Initialize Animal Shogi game.

        Parameters
        ----------
        hash_or_sfen : tp.Union[int, str, None], optional
            Initial state of the game, by default None
        """
        cls_ = self._get_backend_game_class()
        self._game = cls_() if hash_or_sfen is None else cls_(hash_or_sfen)

    @classmethod
    def _get_backend_game_class(cls) -> type:
        return _AnimalshogiGame

    @classmethod
    def _get_move_class(cls) -> type:
        return Move

    @property
    def black_stand(self) -> dict:
        """Return pieces on black stand.

        Returns
        -------
        dict
            Pieces on black stand.
        """
        return self._game.get_stand(Color.BLACK).to_dict()

    @property
    def white_stand(self) -> dict:
        """Return pieces on white stand.

        Returns
        -------
        dict
            Pieces on white stand.
        """
        return self._game.get_stand(Color.WHITE).to_dict()

    def hash_current_state(self) -> int:
        """Return hash value of the current state.

        Returns
        -------
        int
            Hash value of the current state.
        """
        return self._game.hash_current_state()

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
