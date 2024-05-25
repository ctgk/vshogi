import abc
import typing as tp

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

    @abc.abstractmethod
    def _set_game(self, game: Game):
        pass

    @abc.abstractmethod
    def _is_ready(self) -> bool:
        pass

    @abc.abstractmethod
    def _clear(self) -> None:
        pass

    def _raise_error_if_not_ready(self):
        method = f'{self.__class__.__name__}.set_game()'
        if not self.is_ready():
            raise ValueError(f"Please call `{method}` beforehand.")
