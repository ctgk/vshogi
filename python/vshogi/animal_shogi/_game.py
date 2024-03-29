from vshogi._game import Game as BaseGame
from vshogi._vshogi.animal_shogi import Move, Node
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
    >>> game.get_legal_moves()
    [Move(dst=B2, src=B3), Move(dst=A3, src=B4), ...
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
    def _get_node_class(cls) -> type:
        return Node
