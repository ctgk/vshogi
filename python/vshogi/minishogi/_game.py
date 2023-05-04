from vshogi._game import Game as BaseGame
from vshogi._vshogi.minishogi import _Game as _MinishogiGame


class Game(BaseGame):
    """Minishogi game class.

    Examples
    --------
    >>> import vshogi.minishogi as shogi
    >>> game = Game()
    >>> game.turn
    Color.BLACK
    >>> game.result
    Result.ONGOING
    >>> game
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
    >>> game.apply(shogi.Move(shogi.SQ_4C, shogi.SQ_2E))
    Turn: WHITE
    White: -
        5   4   3   2   1
      *---*---*---*---*---*
    A |-HI|-KA|-GI|-KI|-OU|
      *---*---*---*---*---*
    B |   |   |   |   |-FU|
      *---*---*---*---*---*
    C |   |+KA|   |   |   |
      *---*---*---*---*---*
    D |+FU|   |   |   |   |
      *---*---*---*---*---*
    E |+OU|+KI|+GI|   |+HI|
      *---*---*---*---*---*
    Black: -
    >>> game.apply(shogi.Move(shogi.SQ_1C, shogi.SQ_1B))
    Turn: BLACK
    White: -
        5   4   3   2   1
      *---*---*---*---*---*
    A |-HI|-KA|-GI|-KI|-OU|
      *---*---*---*---*---*
    B |   |   |   |   |   |
      *---*---*---*---*---*
    C |   |+KA|   |   |-FU|
      *---*---*---*---*---*
    D |+FU|   |   |   |   |
      *---*---*---*---*---*
    E |+OU|+KI|+GI|   |+HI|
      *---*---*---*---*---*
    Black: -
    >>> game.apply(shogi.Move(shogi.SQ_2A, shogi.SQ_4C, True))
    Turn: WHITE
    White: -
        5   4   3   2   1
      *---*---*---*---*---*
    A |-HI|-KA|-GI|+UM|-OU|
      *---*---*---*---*---*
    B |   |   |   |   |   |
      *---*---*---*---*---*
    C |   |   |   |   |-FU|
      *---*---*---*---*---*
    D |+FU|   |   |   |   |
      *---*---*---*---*---*
    E |+OU|+KI|+GI|   |+HI|
      *---*---*---*---*---*
    Black: KI
    """

    @classmethod
    def _get_backend_game_class(cls) -> type:
        return _MinishogiGame
