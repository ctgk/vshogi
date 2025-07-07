import abc
import typing as tp
from time import time

from vshogi._game import Game


Move = tp.TypeVar('Move')


class Engine(abc.ABC):
    """Base Shogi engine class."""

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

    @abc.abstractmethod
    def search(self, *args, **kwargs):
        """Run search engine."""
        pass

    @abc.abstractmethod
    def select(self, *args, **kwargs) -> Move:
        """Select action based on the search result.

        Returns
        -------
        Move
            Selected action
        """
        pass

    @property
    def num_searched(self) -> int:
        """Return number of game positions searched so far.

        Returns
        -------
        int
            Number of game positions searched so far.
        """
        return self._get_num_searched()

    @abc.abstractmethod
    def _set_game(self, game: Game):
        pass

    @abc.abstractmethod
    def _is_ready(self) -> bool:
        pass

    @abc.abstractmethod
    def _clear(self) -> None:
        pass

    def _get_num_searched(self) -> int:
        return 0

    def _raise_error_if_not_ready(self):
        method = f'{self.__class__.__name__}.set_game()'
        if not self.is_ready():
            raise ValueError(f"Please call `{method}` beforehand.")

    @classmethod
    def _count(
        cls,
        n_or_t: tp.Union[int, float],
    ) -> tp.Generator[int, int, int]:
        if isinstance(n_or_t, int):
            for i in range(n_or_t):
                yield i
        else:
            start_sec = time()
            c = 0
            while True:
                now = time()
                if (now - start_sec) > n_or_t:
                    break
                yield c
                c += 1
