from vshogi._game import Game as BaseGame
from vshogi._vshogi.minishogi import _Game as _MinishogiGame


class Game(BaseGame):
    """Minishogi game class.

    Examples
    --------
    >>> game = Game()
    >>> game.turn
    Color.BLACK
    >>> game.result
    Result.ONGOING
    >>> game
    Turn: BLACK
    White: -
        1   2   3   4   5
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
    """

    @classmethod
    @property
    def _backend_game_class(cls) -> type:
        return _MinishogiGame
