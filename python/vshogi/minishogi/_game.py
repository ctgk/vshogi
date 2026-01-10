from vshogi._game import Game as BaseGame
from vshogi._vshogi.minishogi import (
    AlphaZero,
    BoardPiece,
    DfpnSearcher,
    GumbelAlphaZero,
    Move,
    _Game as _MinishogiGame,
    piece_value_func,
)


class Game(BaseGame):
    """Minishogi game class.

    Examples
    --------
    >>> from vshogi.minishogi import *
    >>> game = Game()
    >>> game.turn
    Color.BLACK
    >>> game.result
    Result.ONGOING
    >>> repr(game)
    'Game(sfen="rbsgk/4p/5/P4/KGSBR b - 1")'
    >>> print(game)
    Turn: BLACK
    White: -
        5   4   3   2   1
      +---+---+---+---+---+
    A |-HI|-KA|-GI|-KI|-OU|
      +---+---+---+---+---+
    B |   |   |   |   |-FU|
      +---+---+---+---+---+
    C |   |   |   |   |   |
      +---+---+---+---+---+
    D |+FU|   |   |   |   |
      +---+---+---+---+---+
    E |+OU|+KI|+GI|+KA|+HI|
      +---+---+---+---+---+
    Black: -
    >>> game.apply('2e4c')
    Game(sfen="rbsgk/4p/1B3/P4/KGS1R w - 2")
    """

    @classmethod
    def _get_backend_game_class(cls) -> type:
        return _MinishogiGame

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
    def _get_dfpn_searcher_class(cls) -> type:
        return DfpnSearcher

    def _piece_value_func(self) -> float:
        return piece_value_func(self._game)
