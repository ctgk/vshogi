import typing as tp

from vshogi._game import Game


Move = tp.TypeVar('Move')


class DfpnSearcher:
    """Mate-moves searcher using DFPN algorithm.

    Examples
    --------
    >>> import vshogi.minishogi as shogi
    >>> from vshogi.engine import DfpnSearcher
    >>>
    >>> # Turn: BLACK
    >>> # White: -
    >>> #     5   4   3   2   1
    >>> #   *---*---*---*---*---*
    >>> # A |   |   |-HI|-KA|-OU|
    >>> #   *---*---*---*---*---*
    >>> # B |   |   |-FU|   |-FU|
    >>> #   *---*---*---*---*---*
    >>> # C |   |   |+FU|   |+FU|
    >>> #   *---*---*---*---*---*
    >>> # D |   |   |   |+KI|   |
    >>> #   *---*---*---*---*---*
    >>> # E |   |   |   |+HI|   |
    >>> #   *---*---*---*---*---*
    >>> # Black: KA
    >>> game = shogi.Game("2rbk/2p1p/2P1P/3G1/3R1 b B")
    >>> searcher = DfpnSearcher()
    >>> searcher.set_root(game)
    >>> searcher.search(n=4)
    False
    >>> searcher.found_conclusion()
    False
    >>>
    >>> # Finds mates by restarting from the searches of the previous call!
    >>> searcher.search(n=4)
    True
    >>> [m.to_usi() for m in searcher.get_mate_moves()]
    ['B*2b', '1a2b', '2d2c', '2b1a', '2c2b']
    """

    def __init__(self, num_nodes: int = 1000) -> None:
        """Initialize DFPN mate-moves searcher object.

        Parameters
        ----------
        num_nodes : int, optional
            Number of nodes to explore unless another value is given at
            `explore()`, by default 1000
        """
        self._num_nodes = num_nodes
        self._searcher = None

    def set_root(self, game: Game):
        """Set root game position to explore from.

        Parameters
        ----------
        game : Game
            Game position to explore.
        """
        self._searcher = game._get_dfpn_searcher_class()(
            game._game, self._num_nodes)

    def is_ready(self) -> bool:
        """Return true if the object is ready to search for mate moves.

        Returns
        -------
        bool
            True if the object is ready to search for mate moves,
            otherwise false.
        """
        return self._searcher is not None

    def clear(self) -> None:
        """Clear explorations done so far."""
        self._searcher = None

    def apply(self, _: Move):
        # flake8: noqa
        raise NotImplementedError

    def search(self, n: tp.Optional[int] = None) -> bool:
        """Search for mate-moves.

        If you call this method multiple times, the following calls does not
        resume searches but restart searches retaining the searches of previous
        calls.

        Parameters
        ----------
        n : tp.Optional[int], optional
            Number of nodes to search for, by default None

        Returns
        -------
        bool
            True if there is a mate-move, otherwise false.

        Note
        ----
        Note that returning false can mean two ways:
        - There is no ways to checkmate opponent king
        - It is not certain that there is a checkmate or no checkmates.
        """
        self._raise_error_if_not_ready()
        if n is None:
            return self._searcher.explore()
        else:
            return self._searcher.explore(n)

    def found_conclusion(self) -> bool:
        """Return true if there is a mate or no-mate for sure.

        Returns
        -------
        bool
            True if there is a mate or no-mate for sure.
        """
        self._raise_error_if_not_ready()
        return self._searcher.found_conclusion()

    def found_mate(self) -> bool:
        """Return true if there is a mate.

        Returns
        -------
        bool
            True if there is a mate.
        """
        self._raise_error_if_not_ready()
        return self._searcher.found_mate()

    def found_no_mate(self) -> bool:
        """Return true if there is no-mate for sure.

        Returns
        -------
        bool
            True if there is no mate for sure.
        """
        self._raise_error_if_not_ready()
        return self._searcher.found_no_mate()

    def get_mate_moves(self) -> tp.List[Move]:
        """Return mate moves found.

        Returns
        -------
        tp.List[Move]
            Mate moves found.
        """
        self._raise_error_if_not_ready()
        return self._searcher.get_mate_moves()

    def _raise_error_if_not_ready(self):
        method = f'{self.__class__.__name__}.set_root()'
        if not self.is_ready():
            raise ValueError(f"Please call `{method}` beforehand.")
