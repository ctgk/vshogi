import typing as tp

import numpy as np

from vshogi._game import Game
from vshogi.engine._engine import Engine


Move = tp.TypeVar('Move')
Policy = tp.Dict[Move, float]
Value = float


def _repr_node(n) -> str:
    return (
        f"Node(v={n.get_value():.2f}, q={n.get_q_value():.2f}, "
        f"count={n.get_visit_count()})"
    )


def _tree(
    root,
    depth: int = 1,
    breadth: int = 3,
    sort_key=lambda n: -n.get_visit_count(),
) -> str:
    out = _repr_node(root)
    if depth == 0:
        return out
    children = [(a, root.get_child(a)) for a in root.get_actions()]
    children.sort(key=lambda t: sort_key(t[1]), reverse=False)
    if breadth > 0:
        children = children[:breadth]
    for i, (a, child) in enumerate(children):
        s = _tree(child, depth - 1, breadth, sort_key)
        if i == len(children) - 1:
            s = s.replace('\n', '\n    ')
        else:
            s = s.replace('\n', '\n|   ')
        out += f'\n+-- p={child.get_proba():.4f} {a} -> {s}'
    return out


class MonteCarloTreeSearcher(Engine):
    """Monte Carlo Tree Searcher engine."""

    def __init__(
        self,
        policy_value_func: tp.Callable[[Game], tp.Tuple[Policy, Value]],
        coeff_puct: float = 1.,
        non_random_ratio: int = 3,
        random_depth: int = 1,
    ) -> None:
        """Initialize MCT searcher.

        Parameters
        ----------
        policy_value_func : tp.Callable[[Game], tp.Tuple[Policy, Value]]
            Function to return policy and value given a game.
        coeff_puct : float, optional
            Default coefficient used to compute PUCT score. Higher the value
            is, the more weight on action policy than state value.
        non_random_ratio : int, optional
            Default ratio of selecting action in a non-random manner,
            by default 3.
        random_depth : int, optional
            Default depth of explorations to select action in a random manner,
            by default 1.
        """
        self._policy_value_func = policy_value_func
        self._root = None

        self._coeff_puct = coeff_puct
        self._non_random_ratio = non_random_ratio
        self._random_depth = random_depth

    def _set_game(self, game: Game):
        policy_logits, value = self._policy_value_func(game)
        self._root = game._get_mcts_node_class()(
            game._game, value, policy_logits)
        self._game = game

    def _is_ready(self) -> bool:
        return self._root is not None

    def _clear(self) -> None:
        self._root = None
        self._game = None

    def apply(self, move: Move):
        """Apply a move on the game.

        Parameters
        ----------
        move : Move
            Move to apply
        """
        if self._is_ready():
            self._root.apply(move)

    @property
    def num_searched(self) -> int:
        """Return number of game positions searched so far.

        Returns
        -------
        int
            Number of game positions searched so far.
        """
        if self._root is None:
            return 0
        return self._root.get_visit_count()

    def search(self, n: int = 100):
        """Explore from root node for n times.

        Parameters
        ----------
        n : int, optional
            Number of game positions to search, by default 100
        """
        for _ in range(n):
            game = self._game.copy()
            node = self._root._select_node_to_explore(
                game._game, self._coeff_puct, self._non_random_ratio,
                self._random_depth,
            )
            if node is None:
                continue
            policy_logits, value = self._policy_value_func(game)
            node.simulate_expand_and_backprop(game._game, value, policy_logits)

    def get_value(self) -> float:
        """Return raw value estimate of the current game position.

        Returns
        -------
        float
            Raw value estimate of the current game position.
        """
        return self._root.get_value()

    def get_probas(self) -> tp.Dict[Move, float]:
        """Return raw probabilities of selecting actions.

        Returns
        -------
        tp.Dict[Move, float]
            Raw probabilities of selecting actions by `policy_value_func`.
        """
        move_proba_pair_list = [
            (m, self._root.get_child(m).get_proba())
            for m in self._root.get_actions()
        ]
        move_proba_pair_list.sort(key=lambda t: t[1], reverse=True)
        return {m: p for m, p in move_proba_pair_list}

    def get_q_values(self) -> tp.Dict[Move, float]:
        """Return Q value of each action.

        Returns
        -------
        tp.Dict[Move, float]
            Q value of each action.
        """
        move_q_pair_list = [
            (m, -self._root.get_child(m).get_q_value())
            for m in self._root.get_actions()
        ]
        move_q_pair_list.sort(key=lambda a: a[1], reverse=True)
        return {m: q for m, q in move_q_pair_list}

    def get_visit_counts(
        self,
        include_random: bool = True,
    ) -> tp.Dict[Move, int]:
        """Return visit counts of each action.

        Parameters
        ----------
        include_random : bool, optional
            Include visit counts by random selection if true, by default true.

        Returns
        -------
        tp.Dict[Move, int]
            Visit counts of each action.
        """
        move_visit_count_pair_list = [
            (
                m,
                self._root.get_child(m).get_visit_count()
                if include_random else
                self._root.get_child(m).get_visit_count_excluding_random(),
            )
            for m in self._root.get_actions()
        ]
        move_visit_count_pair_list.sort(key=lambda a: a[1], reverse=True)
        return {m: v for m, v in move_visit_count_pair_list}

    def select(self, temperature: tp.Optional[float] = None) -> Move:
        """Return selected action based on visit counts.

        Parameters
        ----------
        temperature : tp.Optional[float], optional
            Temperature parameter for action selection, by default None.
            If `None`, then select the most searched action.

        Returns
        -------
        Move
            Selected action.
        """
        if (temperature is None) or np.isclose(temperature, 0):
            return self._root.get_action_by_visit_max()
        else:
            return self._root.get_action_by_visit_distribution(temperature)

    def _tree(
        self,
        depth: int = 1,
        breadth: int = 3,
        sort_key: callable = lambda n: -n.get_visit_count(),
    ) -> str:
        return _tree(self._root, depth, breadth, sort_key)
