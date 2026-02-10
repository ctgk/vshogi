import typing as tp

import numpy as np

from vshogi._game import Game
from vshogi.engine._engine import Engine
from vshogi._move import Move


class DfpnSearcher(Engine):
    """Mate-moves searcher using DFPN algorithm.

    Examples
    --------
    >>> import vshogi.judkins_shogi as shogi
    >>> from vshogi.engine import DfpnSearcher
    >>>
    >>> # Turn: BLACK
    >>> # White: -
    >>> #     6   5   4   3   2   1
    >>> #   +---+---+---+---+---+---+
    >>> # A |   |   |   |   |-FU|-FU|
    >>> #   +---+---+---+---+---+---+
    >>> # B |   |   |   |-FU|-KA|-OU|
    >>> #   +---+---+---+---+---+---+
    >>> # C |   |   |   |-KE|   |+KE|
    >>> #   +---+---+---+---+---+---+
    >>> # D |   |   |   |+FU|   |+FU|
    >>> #   +---+---+---+---+---+---+
    >>> # E |   |   |+GI|   |+GI|+HI|
    >>> #   +---+---+---+---+---+---+
    >>> # F |   |   |   |+KI|+HI|+KI|
    >>> #   +---+---+---+---+---+---+
    >>> # Black: KA
    >>> game = shogi.Game("4pp/3pbk/3n1N/3P1P/2S1SR/3GRG b B")
    >>> searcher = DfpnSearcher()
    >>> searcher.set_game(game)
    >>> searcher.search(budget=4)
    >>> searcher.proved()
    False
    >>>
    >>> # Finds mates by restarting from the searches of the previous call!
    >>> searcher.search(budget=1) # Note that numbers of searches add up to 5.
    >>> searcher.proved()
    True
    >>> [m.to_sfen() for m in searcher.get_mate_moves()]
    ['B*2c', '1b2c', '2e2d', '2c1b', '2d2c']
    """

    def __init__(
        self,
        tree_size: int = 100000,
        name: str | None = None,
    ) -> None:
        """Initialize DFPN mate-moves searcher object.

        Parameters
        ----------
        tree_size : int
            Max size of the tree, by default 100000
        name : str | None, optional
            Name of the engine, by default None
        """
        super().__init__(tree_size=tree_size, name=name)
        self._searcher = None

    def _set_game(self, game: Game):
        if self._searcher is None:
            cls_ = game._get_dfpn_searcher_class()
            self._searcher = cls_(self._tree_size)
        self._searcher.init()
        self._game = game.copy()

    def _is_ready(self) -> bool:
        return self._searcher is not None

    def _clear(self) -> None:
        self._searcher = None

    def _apply(self, _: Move):
        raise NotImplementedError

    def _search(self, budget: int | float) -> None:
        if self._searcher is None:
            return
        sfen = self._game.to_sfen()
        ply = self._game.ply()
        if isinstance(budget, int):
            self._searcher.search(self._game._game, budget)
        else:
            for _ in self._count(budget):
                self._searcher.search(self._game._game, 1)
        if self._game.ply() != ply:
            raise ValueError(
                f"Failed to run DFPN searches on the game position: {sfen}"
            )

    def _select(self, temperature: float | None = None) -> Move:
        if temperature is not None:
            raise ValueError(
                "DFPN algorithm does not support selection temperature"
            )
        return self._searcher.select()

    def proved(self) -> bool:
        """Return true if there is a mate or no-mate proved.

        Returns
        -------
        bool
            True if there is a mate or no-mate proved.
        """
        self._raise_error_if_not_ready()
        return self._searcher.proved()

    def proved_mate(self) -> bool:
        """Return true if there is a mate.

        Returns
        -------
        bool
            True if there is a mate.
        """
        self._raise_error_if_not_ready()
        return self._searcher.proved_mate()

    def proved_no_mate(self) -> bool:
        """Return true if there is no-mate proved.

        Returns
        -------
        bool
            True if there is no-mate proved.
        """
        self._raise_error_if_not_ready()
        return self._searcher.proved_no_mate()

    def get_mate_move(self) -> Move:
        """Return move to mate found.

        Returns
        -------
        Move
            Move to mate found.
        """
        self._raise_error_if_not_ready()
        return self._searcher.get_mate_move()

    def _get_mate_moves(self) -> list[Move] | None:
        sfen = self._game.to_sfen()
        ply = self._game.ply()
        if not self.proved_mate():
            return None
        mate_moves = self._searcher.get_mate_moves(self._game._game)
        if self._game.ply() != ply:
            raise ValueError(
                f"Failed to run DFPN searches on the game position: {sfen}"
            )
        return mate_moves

    def _tree(
        self,
        depth: int = 1,
        breadth: int = 3,
        pv_line: tp.List[tp.Union[Move, str]] = [],
    ):
        self._raise_error_if_not_ready()
        root = self._searcher.get_root()
        if root is None:
            return None
        root_offence: bool = True
        move_class = self._game._get_move_class()
        pv_line = [move_class(m) if isinstance(m, str) else m for m in pv_line]
        for m in pv_line:
            for child in root.get_children():
                if move_class(child.get_action()) == m:
                    root = child
                    break
            else:
                raise ValueError(f'Cannot find child with action, {m}')
            root_offence = not root_offence
        return _tree(root_offence, move_class, root, depth, breadth)


def _tree(offence, move_class, node, depth: int, breadth: int):
    out = _repr_node(offence, node)
    if depth == 0:
        return out

    offence = not offence
    children = node.get_children()
    children.sort(key=lambda c: c.dn(offence) if offence else c.pn(offence))
    if breadth >= 0:
        children = children[:breadth]
    for i, child in enumerate(children):
        s = _tree(offence, move_class, child, depth - 1, breadth)
        if i == len(children) - 1:
            s = s.replace('\n', '\n    ')
        else:
            s = s.replace('\n', '\n|   ')
        out += f'\n+-- {move_class(child.get_action()).to_sfen()} -> {s}'
    return out


def _repr_node(offence: bool, n) -> str:
    name = 'OR' if offence else 'AND'
    return (
        f'{name}(#P={np.round(n.pn(offence), 2)}, '
        f'#D={np.round(n.dn(offence), 2)})'
    )
