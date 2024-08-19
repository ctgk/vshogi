import warnings

from vshogi._game import Game as BaseGame
from vshogi._vshogi.animal_shogi import MCTS, MctsNode, Move
from vshogi._vshogi.animal_shogi import Stand
from vshogi._vshogi.animal_shogi import _Game as _AnimalshogiGame


Stand.__str__ = lambda self: ','.join([
    k.name[0] + ('' if v == 1 else str(v)) for k, v in self.to_dict().items()
    if v > 0
]) if self.any() else '-'


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
    >>> moves = game.get_legal_moves()
    >>> Move("b3b2") in moves
    True
    >>> Move("a4a3") in moves
    False
    >>> game.is_legal(Move(B2, B3))
    True
    >>> game.is_legal(Move(A3, A4))
    False
    >>> game.apply(Move(B2, B3))
    Game(sfen="gle/1C1/3/ELG w C 2")
    >>> game.stand(WHITE)
    {Piece.CH: 0, Piece.EL: 0, Piece.GI: 0}
    >>> game.stand(BLACK)
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
    >>> game.get_move_at(1)
    Move(dst=A2, src=A1)
    """

    @classmethod
    def _get_backend_game_class(cls) -> type:
        return _AnimalshogiGame

    @classmethod
    def _get_move_class(cls) -> type:
        return Move

    @classmethod
    def _get_mcts_node_class(cls) -> type:
        return MctsNode

    @classmethod
    def _get_mcts_searcher_class(cls) -> type:
        return MCTS

    @classmethod
    def _get_dfpn_searcher_class(cls) -> type:
        raise NotImplementedError(
            "`DfpnSearcher` does not support Animal Shogi")

    def get_mate_moves_if_any(
        self,
        num_dfpn_nodes: int = 10000,
    ) -> None:
        """Not implemented yet for animal shogi."""
        warnings.warn(
            "`vshogi.animal_shogi.Game.get_mate_moves_if_any()` "
            "is not supported yet.")
        return None
