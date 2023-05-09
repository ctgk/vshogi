import typing as tp
from copy import deepcopy

import numpy as np
import scipy.special as sp


Policy = tp.Dict['Move', float]
Value = float


class _Node:

    def __init__(
        self,
        game: 'Game',
        policy_value_func: tp.Callable[['Game'], tp.Tuple[Policy, Value]],
        parent: '_Node' = None,
    ) -> None:
        self._game = game
        self._policy_value_func = policy_value_func
        self._policy, self._value = self._policy_value_func(self._game)
        self._visit_count = 1
        self._children: tp.Dict['Move', '_Node'] = {}
        self._parent: '_Node' = parent
        self._q_value: float = self._value
        if parent is not None:
            parent._update_q(-self._value)

    def __repr__(self) -> str:
        return f'_Node(#visit={self._visit_count},Q={self._q_value})'

    def _u(self, add_dirichlet_noise: bool) -> tp.Dict['Move', float]:
        if add_dirichlet_noise:
            n = len(self._policy)
            p_d = {
                k: p for k, p in zip(
                    self._policy, np.random.dirichlet(np.ones(n) * 10 / n))
            }
            policy = {k: (3 * p + p_d[k]) / 4 for k, p in self._policy.items()}
        else:
            policy = self._policy
        assert max(policy.values()) <= 1, (self._policy, p_d)
        return {
            k: (
                p * np.sqrt(self._visit_count) / (
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

    def _add_node(self, action: 'Move'):
        g = deepcopy(self._game).apply(action)
        self._children[action] = _Node(g, self._policy_value_func, self)

    def explore(self, c_puct: float = 1., dirichlet_noise_depth: int = 0):
        self._visit_count += 1
        if len(self._policy) == 0:
            self._update_q(self._value)
            return
        action = self._get_action_with_max_puct_score(
            c_puct, dirichlet_noise_depth > 0)
        if action in self._children:
            self._children[action].explore(c_puct, dirichlet_noise_depth - 1)
        else:
            self._add_node(action)


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
        self._pv_cache: tp.Dict[tp.Hashable, tp.Tuple[Policy, Value]] = {}

        self._num_explorations = num_explorations
        self._coeff_puct = coeff_puct
        self._depth_to_add_dirichlet_noise = depth_to_add_dirichlet_noise
        self._select_move_by = select_move_by
        self._temperature_for_move_selection = temperature_for_move_selection

    def _get_policy_value(self, game: 'Game') -> tp.Tuple[Policy, Value]:
        sfen = game.to_sfen(include_move_count=False)
        if sfen in self._pv_cache:
            return self._pv_cache[sfen]
        pv = self._policy_value_func(game)
        self._pv_cache[sfen] = pv
        return pv

    def set_root(self, game: 'Game'):
        """Set root node.

        Parameters
        ----------
        game : Game
            Game to set at root node.
        """
        self._root = _Node(game, self._get_policy_value)

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
            self._root.explore(c_puct, dirichlet_noise_depth)

    def get_q_values(self) -> tp.Dict['Move', float]:
        """Return Q value of each action.

        Returns
        -------
        tp.Dict[Move, float]
            Q value of each action.
        """
        return {
            m: (
                -self._root._children[m]._q_value
                if m in self._root._children else 0
            )
            for m in self._root._policy.keys()
        }

    def get_visit_counts(self) -> tp.Dict['Move', int]:
        """Return visit counts of each action.

        Returns
        -------
        tp.Dict[Move, int]
            Visit counts of each action.
        """
        return {
            m: (
                self._root._children[m]._visit_count
                if m in self._root._children else 0
            )
            for m in self._root._policy.keys()
        }

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
