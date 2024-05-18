from vshogi._game import Game as BaseGame
from vshogi._vshogi.shogi import (
    DfpnSearcher,
    MctsNode,
    Move,
    _Game as _ShogiGame,
)


class Game(BaseGame):
    """Shogi game class.

    Examples
    --------
    >>> from vshogi.shogi import *
    >>> game = Game()
    >>> game.turn
    Color.BLACK
    >>> game.result
    Result.ONGOING
    >>> repr(game)
    'Game(sfen="lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/\
LNSGKGSNL b - 1")'
    >>> print(game)
    Turn: BLACK
    White: -
        9   8   7   6   5   4   3   2   1
      +---+---+---+---+---+---+---+---+---+
    A |-KY|-KE|-GI|-KI|-OU|-KI|-GI|-KE|-KY|
      +---+---+---+---+---+---+---+---+---+
    B |   |-HI|   |   |   |   |   |-KA|   |
      +---+---+---+---+---+---+---+---+---+
    C |-FU|-FU|-FU|-FU|-FU|-FU|-FU|-FU|-FU|
      +---+---+---+---+---+---+---+---+---+
    D |   |   |   |   |   |   |   |   |   |
      +---+---+---+---+---+---+---+---+---+
    E |   |   |   |   |   |   |   |   |   |
      +---+---+---+---+---+---+---+---+---+
    F |   |   |   |   |   |   |   |   |   |
      +---+---+---+---+---+---+---+---+---+
    G |+FU|+FU|+FU|+FU|+FU|+FU|+FU|+FU|+FU|
      +---+---+---+---+---+---+---+---+---+
    H |   |+KA|   |   |   |   |   |+HI|   |
      +---+---+---+---+---+---+---+---+---+
    I |+KY|+KE|+GI|+KI|+OU|+KI|+GI|+KE|+KY|
      +---+---+---+---+---+---+---+---+---+
    Black: -
    >>> game.apply(SQ_2F, SQ_2G)
    Game(sfen="lnsgkgsnl/1r5b1/ppppppppp/9/9/7P1/PPPPPPP1P/1B5R1/\
LNSGKGSNL w - 2")
    """

    @classmethod
    def _get_backend_game_class(cls) -> type:
        return _ShogiGame

    @classmethod
    def _get_move_class(cls) -> type:
        return Move

    @classmethod
    def _get_mcts_node_class(cls) -> type:
        return MctsNode

    @classmethod
    def _get_dfpn_searcher_class(cls) -> type:
        return DfpnSearcher
