from vshogi._game import Game as BaseGame
from vshogi._vshogi.minishogi import Move, _Game as _MinishogiGame


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
      *---*---*---*---*---*
    A |-HI|-KA|-GI|-KI|-OU|
      *---*---*---*---*---*
    B |   |   |   |   |-FU|
      *---*---*---*---*---*
    C |   |   |   |   |   |
      *---*---*---*---*---*
    D |+FU|   |   |   |   |
      *---*---*---*---*---*
    E |+OU|+KI|+GI|+KA|+HI|
      *---*---*---*---*---*
    Black: -
    >>> game.apply(SQ_4C, SQ_2E)
    Game(sfen="rbsgk/4p/1B3/P4/KGS1R w - 2")
    """

    @classmethod
    def _get_backend_game_class(cls) -> type:
        return _MinishogiGame

    @classmethod
    def _get_move_class(cls) -> type:
        return Move
