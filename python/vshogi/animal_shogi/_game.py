import typing as tp

import vshogi._vshogi.animal_shogi as _shogi
from vshogi._vshogi.animal_shogi import Board
from vshogi._vshogi.animal_shogi import Color
from vshogi._vshogi.animal_shogi import Result
from vshogi._vshogi.animal_shogi import _Game
from vshogi.animal_shogi._move import Move


_shogi.Stand.__str__ = lambda self: ','.join([
    k.name[0] + ('' if v == 1 else str(v)) for k, v in self.to_dict().items()
    if v > 0
]) if self.any() else '-'


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
        self._game.apply(move._move)
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
        return self._game.is_applicable(move._move)

    def __repr__(self) -> str:
        r = self.result
        return '\n'.join((
            f'Turn: {self.turn.name}' if r == Result.ONGOING else r.name,
            f'White: {str(self._game.get_stand(_shogi.Color.WHITE))}',
            repr(self.board),
            f'Black: {str(self._game.get_stand(_shogi.Color.BLACK))}',
        ))
