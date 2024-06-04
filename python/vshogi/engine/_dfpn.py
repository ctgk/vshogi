import typing as tp

from vshogi._game import Game
from vshogi.engine._engine import Engine


Move = tp.TypeVar('Move')


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
    >>> searcher.search(n=4)
    False
    >>> searcher.found_conclusion()
    False
    >>>
    >>> # Finds mates by restarting from the searches of the previous call!
    >>> searcher.search(n=1) # Note that numbers of searches add up to 5.
    True
    >>> [m.to_usi() for m in searcher.get_mate_moves()]
    ['B*2c', '1b2c', '2e2d', '2c1b', '2d2c']
    """

    def __init__(self) -> None:
        """Initialize DFPN mate-moves searcher object."""
        self._searcher = None

    def _set_game(self, game: Game):
        if self._searcher is None:
            try:
                self._searcher = game._get_dfpn_searcher_class()()
            except:
                return
        self._searcher.set_game(game._game)

    def _is_ready(self) -> bool:
        return (self._searcher is not None) and (self._searcher.is_ready())

    def _clear(self) -> None:
        self._searcher = None

    def _apply(self, _: Move):
        raise NotImplementedError

    @property
    def num_searched(self) -> int:
        # flake8: noqa
        raise NotImplementedError

    def search(self, n: int = 100) -> bool:
        """Search for mate-moves.

        If you call this method multiple times, the following calls does not
        resume searches but restart searches retaining the searches of previous
        calls.

        Parameters
        ----------
        n : int, optional
            Number of nodes to search for, by default 100

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
        if self._searcher is None:
            return False
        return self._searcher.explore(n)

    def select(self) -> Move:
        """Get first action to mate.

        Returns
        -------
        Move
            First action to mate.
        """
        return self._searcher.get_mate_moves()[0]

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
        return self._searcher
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
