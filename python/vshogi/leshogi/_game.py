from vshogi._game import Game as BaseGame
from vshogi.leshogi._leshogi import (
    AlphaZero,
    BoardPiece,
    DfpnSearcher,
    GumbelAlphaZero,
    Mcgs,
    Move,
    _Game as _LeshogiGame,
    piece_value_func,
)


class Game(BaseGame):
    """Leshogi game class.

    Examples
    --------
    >>> from vshogi.leshogi import *
    >>> game = Game()
    >>> game.turn
    Color.BLACK
    >>> game.result
    Result.ONGOING
    >>> repr(game)
    'Game(sfen="sknl/prbg/4/4/GBRP/LNKS b - 1")'
    >>> print(game)
    Turn: BLACK
    White: -
        4   3   2   1
      +---+---+---+---+
    A |-GI|-OU|-KE|-KY|
      +---+---+---+---+
    B |-FU|-HI|-KA|-KI|
      +---+---+---+---+
    C |   |   |   |   |
      +---+---+---+---+
    D |   |   |   |   |
      +---+---+---+---+
    E |+KI|+KA|+HI|+FU|
      +---+---+---+---+
    F |+KY|+KE|+OU|+GI|
      +---+---+---+---+
    Black: -
    >>> game.apply('2e2b+')
    Game(sfen="sknl/pr+Rg/4/4/GB1P/LNKS w B 2")
    """

    @classmethod
    def _get_backend_game_class(cls) -> type:
        return _LeshogiGame

    @classmethod
    def _get_board_piece_class(cls) -> type:
        return BoardPiece

    @classmethod
    def _get_move_class(cls) -> type:
        return Move

    @classmethod
    def _get_az_searcher_class(cls) -> type:
        return AlphaZero

    @classmethod
    def _get_gaz_searcher_class(cls):
        return GumbelAlphaZero

    @classmethod
    def _get_mcgs_class(cls):
        return Mcgs

    @classmethod
    def _get_dfpn_searcher_class(cls) -> type:
        return DfpnSearcher

    def _piece_value_func(self) -> float:
        return piece_value_func(self._game)
