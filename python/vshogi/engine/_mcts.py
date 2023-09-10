import typing as tp
from copy import deepcopy

import numpy as np
import scipy.special as sp

from vshogi._game import Game


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
        out += f'\n+-- p={root.get_proba(a):.4f} {a} -> {s}'
    return out


class MonteCarloTreeSearcher:
    """Monte Carlo Tree Searcher."""

    def __init__(
        self,
        policy_value_func: tp.Callable[['Game'], tp.Tuple[Policy, Value]],
        num_explorations: int = 100,
        coeff_puct: float = 1.,
        random_proba: float = 0.25,
        random_depth: int = 1,
        select_move_by: tp.Literal[
            'visit_counts', 'q_values',
        ] = 'visit_counts',
        temperature_for_move_selection: tp.Union[
            float, tp.Literal['max'],
        ] = 'max',
    ) -> None:
        """Initialize MCT searcher.

        Parameters
        ----------
        policy_value_func : tp.Callable[[Game], tp.Tuple[Policy, Value]]
            Function to return policy and value given a game.
        num_explorations : int, optional
            Default number of times to explore, by default 100
        coeff_puct : float, optional
            Default coefficient used to compute PUCT score. Higher the value
            is, the more weight on action policy than state value.
        random_proba : float, optional
            Default probability of selecting action in a random manner,
            by default 0.25.
        random_depth : int, optional
            Default depth of explorations to select action in a random manner,
            by default 1.
        select_move_by : tp.Literal['visit_counts', 'q_values']
            Default strategy to select best move by. Default is 'visit_counts'.
        temperature_for_move_selection : tp.Union[float, tp.Literal['max']]
            Temperature parameter for the random selection, by default 'max'
            which returns a move with maximum index determined at
            `select_move_by`.
        """
        self._policy_value_func = policy_value_func
        self._root = None

        self._num_explorations = num_explorations
        self._coeff_puct = coeff_puct
        self._random_proba = random_proba
        self._random_depth = random_depth
        self._select_move_by = select_move_by
        self._temperature_for_move_selection = temperature_for_move_selection

    def set_root(self, game: Game):
        """Set root node.

        Parameters
        ----------
        game : Game
            Game to set at root node.
        """
        game = deepcopy(game)
        policy, value = self._policy_value_func(game)
        self._root = game._get_node_class()(
            value, list(policy.keys()), list(policy.values()))
        self._game = game

    def is_ready(self) -> bool:
        """Return true if it is ready to explore otherwise false.

        Returns
        -------
        bool
            True if it is ready to start exploration otherwise false.
        """
        return self._root is not None

    def clear(self) -> None:
        """Clear explorations done so far."""
        self._root = None

    def apply(self, move: Move):
        """Apply a move to the current root node.

        Parameters
        ----------
        move : Move
            Move to apply to the current root node.
        """
        if not self.is_ready():
            return
        try:
            self._root = self._root.pop_child(move)
            self._game.apply(move)
        except Exception:
            self._root = None
            self._game = None

    @property
    def num_explored(self) -> int:
        """Return number of times explored so far.

        Returns
        -------
        int
            Number of times explored so far.
        """
        if self._root is None:
            return 0
        return self._root.get_visit_count()

    def explore(
        self,
        n: tp.Optional[int] = None,
        c_puct: tp.Optional[float] = None,
        random_proba: tp.Optional[float] = None,
        random_depth: tp.Optional[int] = None,
    ):
        """Explore from root node for n times.

        Parameters
        ----------
        n : int, optional
            Number of times to explore, by default None
        c_puct : float, optional
            Coefficient used to compute PUCT score. Higher the value is,
            the more weight on action policy than state value.
        random_proba : float, optional
            Probability of selecting an action in a random manner,
            by default None
        random_depth : int, optional
            Depth to select action in a random manner, by default None
        """
        if n is None:
            n = self._num_explorations
        if c_puct is None:
            c_puct = self._coeff_puct
        if random_proba is None:
            random_proba = self._random_proba
        if random_depth is None:
            random_depth = self._random_depth

        for _ in range(n):
            game = deepcopy(self._game)
            node = self._root.explore(
                game._game, c_puct, random_proba, random_depth)
            if node is None:
                continue
            policy, value = self._policy_value_func(game)
            node.set_value_action_proba(
                value, list(policy.keys()), list(policy.values()))

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

    def get_visit_counts(self) -> tp.Dict[Move, int]:
        """Return visit counts of each action.

        Returns
        -------
        tp.Dict[Move, int]
            Visit counts of each action.
        """
        move_visit_count_pair_list = [
            (m, self._root.get_child(m).get_visit_count())
            for m in self._root.get_actions()
        ]
        move_visit_count_pair_list.sort(key=lambda a: a[1], reverse=True)
        return {m: v for m, v in move_visit_count_pair_list}

    def select(
        self,
        by: tp.Optional[tp.Literal['visit_counts', 'q_values']] = None,
        temperature: tp.Union[float, tp.Literal['max'], None] = None,
    ) -> Move:
        """Return selected action based on visit counts.

        Parameters
        ----------
        by : tp.Optional[tp.Literal['visit_counts', 'q_values']]
            Strategy to select a best move by. Default is None, which refers to
            default strategy set at initialization of the object.
        temperature : tp.Union[float, tp.Literal['max'], None], optional
            Temperature parameter for the random selection, by default None

        Returns
        -------
        Move
            Selected action.
        """
        if by is None:
            by = self._select_move_by
        if temperature is None:
            temperature = self._temperature_for_move_selection

        d = getattr(self, f'get_{by}')()
        moves = list(d.keys())
        v = np.array(list(d.values()))

        if temperature == 'max':
            return moves[np.argmax(v)]
        else:
            probas = sp.softmax(np.log(v + 1e-3) / temperature)
            return np.random.choice(moves, p=probas)

    def _tree(
        self,
        depth: int = 1,
        breadth: int = 3,
        sort_key: callable = lambda n: -n.get_visit_count(),
    ) -> str:
        return _tree(self._root, depth, breadth, sort_key)
