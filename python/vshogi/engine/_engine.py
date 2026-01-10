import abc
import typing as tp
from time import time

from vshogi._game import Game


Move = tp.TypeVar('Move')


class Engine(abc.ABC):
    """Base Shogi engine class."""

    def __init__(self, tree_size: int, name: str | None = None):
        """Initialize engine.

        Parameters
        ----------
        name : str | None, optional
            Name of the engine, by default None
        """
        if tree_size <= 0:
            msg = f"`tree_size` (={tree_size}) must be larger than 0."
            raise ValueError(msg)
        self._tree_size = tree_size
        self._name = name

    def set_game(self, game: Game) -> None:
        """Set game to run engine on.

        Parameters
        ----------
        game : Game
            Game to run engine on.
        """
        self._set_game(game)

    def is_ready(self) -> bool:
        """Return true if the engine is ready to run otherwise false.

        Returns
        -------
        bool
            True if the engine is ready to run otherwise false.
        """
        return self._is_ready()

    def clear(self) -> None:
        """Clear game set and searches done so far."""
        self._clear()

    def search(self, budget: int | float, **kwargs):
        """Run the search within the given budget.

        Parameters
        ----------
        budget : int | float
            If int, number of simulations.
            If float, search duration in seconds.
        """
        self._search(budget, **kwargs)

    @property
    def name(self) -> tp.Union[str, None]:
        """Return name of the engine.

        Returns
        -------
        tp.Union[str, None]
            Name of the engine.
        """
        return self._name

    def get_search_count(self) -> int:
        """Return number of game positions searched so far.

        Returns
        -------
        int
            Number of game positions searched so far.
        """
        return self._get_search_count()

    def select(self, temperature: float | None = None) -> Move:
        """Return best action based on the searches so far.

        Parameters
        ----------
        temperature : tp.Optional[float], optional
            Temperature parameter for action selection, by default None.
            If `None`, select the best action.

        Returns
        -------
        Move
            Selected action.
        """
        return self._select(temperature)

    def get_mate_moves(self) -> list[Move] | None:
        """Return a sequence of mate moves if found one.

        Returns
        -------
        list[Move] | None
            A sequence of mate moves found.
        """
        self._raise_error_if_not_ready()
        return self._get_mate_moves()

    @abc.abstractmethod
    def _set_game(self, game: Game):
        pass

    @abc.abstractmethod
    def _is_ready(self) -> bool:
        pass

    @abc.abstractmethod
    def _clear(self) -> None:
        pass

    @abc.abstractmethod
    def _select(self, temperature: float | None = None) -> Move:
        pass

    @abc.abstractmethod
    def _get_mate_moves(self) -> list[Move] | None:
        pass

    @abc.abstractmethod
    def _search(self, budget: int | float, **kwargs) -> None:
        pass

    def _get_search_count(self) -> int:
        if self._searcher is None:
            raise ValueError("There is no searcher to get the count from.")
        return self._searcher.get_search_count()

    def _raise_error_if_not_ready(self):
        method = f'{self.__class__.__name__}.set_game()'
        if not self.is_ready():
            raise ValueError(f"Please call `{method}` beforehand.")

    @classmethod
    def _count(
        cls,
        budget: int | float,
    ) -> tp.Generator[int, int, int]:
        if isinstance(budget, int):
            for i in range(budget):
                yield i
        else:
            start_sec = time()
            c = 0
            while True:
                now = time()
                if (now - start_sec) > budget:
                    break
                yield c
                c += 1
