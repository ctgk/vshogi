import typing as tp
from copy import deepcopy

import numpy as np
import scipy.special as sp

from vshogi._game import Game


Policy = tp.Dict['Move', float]
Value = float


class _Node:

    def __init__(
        self,
        game: Game,
        policy: Policy,
        value: Value,
        parent: '_Node' = None,
    ) -> None:
        self._game = game
        self._policy, self._value = policy, value
        self._visit_count = 1

        # Computing sqrt of `self._visit_count` beforehand cuts half of the
        # computation time of `self._u()` function.
        self._sqrt_visit_count = 1

        self._children: tp.Dict['Move', '_Node'] = {}
        self._parent: '_Node' = parent
        self._q_value: float = self._value
        if parent is not None:
            parent._update_q(-self._value)

    def __repr__(self) -> str:
        return f'_Node(#visit={self._visit_count},Q={self._q_value})'

    def _get_policy(self, add_dirichlet_noise: bool):
        if add_dirichlet_noise:
            n = len(self._policy)
            p_d = {
                k: p for k, p in zip(
                    self._policy, np.random.dirichlet(np.ones(n) * 10 / n))
            }
            return {k: (3 * p + p_d[k]) / 4 for k, p in self._policy.items()}
        else:
            return self._policy

    def _u(self, add_dirichlet_noise: bool) -> tp.Dict['Move', float]:
        policy = self._get_policy(add_dirichlet_noise)
        return {
            k: (
                p * self._sqrt_visit_count / (
                    1 + (
                        self._children[k]._visit_count
                        if k in self._children else 0
                    )
                )
            )
            for k, p in policy.items()
        }

    def _update_q(self, value):
        self._q_value = (
            value + (self._visit_count - 1) * self._q_value
        ) / self._visit_count
        if self._parent is not None:
            self._parent._update_q(-value)

    def _q(self) -> tp.Dict['Move', float]:
        return {
            k: 0 if k not in self._children else -self._children[k]._q_value
            for k in self._policy
        }

    def _puct_score(
        self,
        c_puct: float = 1.,
        add_dirichlet_noise: bool = False,
    ) -> tp.Dict['Move', float]:
        q, u = self._q(), self._u(add_dirichlet_noise)
        scores = {k: q[k] + u[k] * c_puct for k in q.keys()}
        return scores

    def _get_action_with_max_puct_score(
        self,
        c_puct: float = 1.,
        add_dirichlet_noise: bool = False,
    ) -> 'Move':
        return max(
            list(self._puct_score(c_puct, add_dirichlet_noise).items()),
            key=lambda t: t[1],
        )[0]

    def add_node(
        self,
        action: 'Move',
        game_after_action: Game,
        policy: Policy,
        value: Value,
    ):
        self._children[action] = _Node(game_after_action, policy, value, self)

    def _increment_visit_count(self):
        self._visit_count += 1
        self._sqrt_visit_count = np.sqrt(self._visit_count)

    def explore(
        self,
        c_puct: float = 1.,
        dirichlet_noise_depth: int = 0,
    ) -> tp.Tuple['_Node', 'Move']:
        self._increment_visit_count()
        if len(self._policy) == 0:
            self._update_q(self._value)
            return (self, None)
        action = self._get_action_with_max_puct_score(
            c_puct, dirichlet_noise_depth > 0)
        if action in self._children:
            return self._children[action].explore(
                c_puct, dirichlet_noise_depth - 1)
        else:
            return (self, action)


class MonteCarloTreeSearcher:
    """Monte Carlo Tree Searcher."""

    def __init__(
        self,
        policy_value_func: tp.Callable[['Game'], tp.Tuple[Policy, Value]],
        num_explorations: int = 100,
        coeff_puct: float = 1.,
        depth_to_add_dirichlet_noise: int = 1,
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
        depth_to_add_dirichlet_noise : int, optional
            Default depth to add dirichlet noise for exploration, by default 1.
        select_move_by : tp.Literal['visit_counts', 'q_values']
            Default strategy to select best move by. Default is 'visit_counts'.
        temperature_for_move_selection : tp.Union[float, tp.Literal['max']]
            Temperature parameter for the random selection, by default 'max'
            which returns a move with maximum index determined at
            `select_move_by`.
        """
        self._policy_value_func = policy_value_func
        self._root: _Node = None

        self._num_explorations = num_explorations
        self._coeff_puct = coeff_puct
        self._depth_to_add_dirichlet_noise = depth_to_add_dirichlet_noise
        self._select_move_by = select_move_by
        self._temperature_for_move_selection = temperature_for_move_selection

    def set_root(self, game: Game):
        """Set root node.

        Parameters
        ----------
        game : Game
            Game to set at root node.
        """
        policy, value = self._policy_value_func(game)
        self._root = _Node(game, policy, value)

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

    def apply(self, move: 'Move'):
        """Apply a move to the current root node.

        Parameters
        ----------
        move : Move
            Move to apply to the current root node.
        """
        if not self.is_ready():
            return
        if move in self._root._children:
            self._root = self._root._children[move]
            self._root._parent = None
        else:
            self._root = None

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
        return self._root._visit_count

    def explore(
        self,
        n: tp.Optional[int] = None,
        c_puct: tp.Optional[float] = None,
        dirichlet_noise_depth: tp.Optional[int] = None,
    ):
        """Explore from root node for n times.

        Parameters
        ----------
        n : int, optional
            Number of times to explore, by default None
        c_puct : float, optional
            Coefficient used to compute PUCT score. Higher the value is,
            the more weight on action policy than state value.
        dirichlet_noise_depth : int, optional
            Depth to add dirichlet noise for exploration, by default None
        """
        if n is None:
            n = self._num_explorations
        if c_puct is None:
            c_puct = self._coeff_puct
        if dirichlet_noise_depth is None:
            dirichlet_noise_depth = self._depth_to_add_dirichlet_noise

        for _ in range(n):
            node, action = self._root.explore(c_puct, dirichlet_noise_depth)
            if action is None:
                continue
            game = deepcopy(node._game).apply(action)
            policy, value = self._policy_value_func(game)
            node.add_node(action, game, policy, value)

    def get_q_values(self) -> tp.Dict['Move', float]:
        """Return Q value of each action.

        Returns
        -------
        tp.Dict[Move, float]
            Q value of each action.
        """
        move_q_pair_list = [
            (m, -self._root._children[m]._q_value)
            if m in self._root._children else (m, 0)
            for m in self._root._policy.keys()
        ]
        move_q_pair_list.sort(key=lambda a: a[1], reverse=True)
        return {m: q for m, q in move_q_pair_list}

    def get_visit_counts(self) -> tp.Dict['Move', int]:
        """Return visit counts of each action.

        Returns
        -------
        tp.Dict[Move, int]
            Visit counts of each action.
        """
        move_visit_count_pair_list = [
            (m, self._root._children[m]._visit_count)
            if m in self._root._children else (m, 0)
            for m in self._root._policy.keys()
        ]
        move_visit_count_pair_list.sort(key=lambda a: a[1], reverse=True)
        return {m: v for m, v in move_visit_count_pair_list}

    def select(
        self,
        by: tp.Optional[tp.Literal['visit_counts', 'q_values']] = None,
        temperature: tp.Union[float, tp.Literal['max'], None] = None,
    ) -> 'Move':
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
