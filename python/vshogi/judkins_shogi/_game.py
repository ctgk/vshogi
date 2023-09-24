from vshogi._game import Game as BaseGame
from vshogi._vshogi.judkins_shogi import Move, Node, _Game as _ShogiGame


class Game(BaseGame):
    """Judkins shogi game class.

    Examples
    --------
    >>> from vshogi.judkins_shogi import *
    >>> game = Game()
    >>> game.turn
    Color.BLACK
    >>> game.result
    Result.ONGOING
    >>> repr(game)
    'Game(sfen="rbnsgk/5p/6/6/P5/KGSNBR b - 1")'
    >>> print(game)
    Turn: BLACK
    White: -
        6   5   4   3   2   1
      +---+---+---+---+---+---+
    A |-HI|-KA|-KE|-GI|-KI|-OU|
      +---+---+---+---+---+---+
    B |   |   |   |   |   |-FU|
      +---+---+---+---+---+---+
    C |   |   |   |   |   |   |
      +---+---+---+---+---+---+
    D |   |   |   |   |   |   |
      +---+---+---+---+---+---+
    E |+FU|   |   |   |   |   |
      +---+---+---+---+---+---+
    F |+OU|+KI|+GI|+KE|+KA|+HI|
      +---+---+---+---+---+---+
    Black: -
    >>> game.apply(SQ_6D, SQ_6E)
    Game(sfen="rbnsgk/5p/6/P5/6/KGSNBR w - 2")
    """

    @classmethod
    def _get_backend_game_class(cls) -> type:
        return _ShogiGame

    @classmethod
    def _get_move_class(cls) -> type:
        return Move

    @classmethod
    def _get_node_class(cls) -> type:
        return Node
