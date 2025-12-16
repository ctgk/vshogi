import typing as tp

import numpy as np

from vshogi._game import Game
from vshogi.engine._engine import Engine


Move = tp.TypeVar('Move')
Policy = np.ndarray
Value = float


def _repr_node(n) -> str:
    return f"Node(q={n.get_q_value(0):.2f}, count={n.get_visit_count()})"


def _tree(
    root,
    move_type: type,
    depth: int = 1,
    breadth: int = 3,
    *,
    sort_key=lambda n: -n.get_visit_count(),
) -> str:
    out = _repr_node(root)
    if depth == 0:
        return out
    actions = root.get_actions()
    probas = root.get_probas()
    children = [(a, p, root.get_child_of(a)) for a, p in zip(actions, probas)]
    children.sort(key=lambda t: sort_key(t[2]), reverse=False)
    if breadth > 0:
        children = children[:breadth]
    for i, (a, p, child) in enumerate(children):
        s = _tree(
            child,
            move_type,
            depth - 1,
            breadth,
            sort_key=sort_key,
        )
        if i == len(children) - 1:
            s = s.replace('\n', '\n    ')
        else:
            s = s.replace('\n', '\n|   ')
        out += f'\n+-- p={p:.4f} {move_type(a)} -> {s}'
    return out


class GumbelAlphaZero(Engine):
    """Gumbel Alpha Zero engine."""

    def __init__(
        self,
        policy_value_func: tp.Callable[
            [Game], tp.Tuple[Policy, Value],
        ] = lambda g: (g.to_dlshogi_policy({}), 0.),
        *,
        tree_size: int = 1000000,
        dfpn_search_root: int = 0,
        dfpn_search_leaf: int = 0,
        name: tp.Optional[str] = None,
    ) -> None:
        """Initialize a Gumbel Alpha Zero engine.

        Parameters
        ----------
        policy_value_func : tp.Callable[[Game], tp.Tuple[Policy, Value]]
            Function that computes the policy distribution and state value for
            a given game position.
        tree_size : int, optional
            Size of the tree search. Default is 1000000.
        dfpn_search_root : int, optional
            Number of DFPN searches to run at the root node. Default is 0.
        dfpn_search_leaf : int, optional
            Number of DFPN searches to run at leaf nodes. Default is 0.
        name : tp.Optional[str], optional
            Name of the search engine instance. Default is None.
        """
        super().__init__(name=name)
        self._policy_value_func = policy_value_func
        self._searcher = None
        self._game = None

        self._tree_size = tree_size
        self._dfpn_search_root = dfpn_search_root
        self._dfpn_search_leaf = dfpn_search_leaf

    def _set_game(self, game: Game):
        self._game = game.copy()
        if self._searcher is None:
            self._searcher = game._get_gaz_searcher_class()(
                self._tree_size,
                self._dfpn_search_root,
                self._dfpn_search_leaf,
            )
        else:
            self._searcher.init()

    def _is_ready(self) -> bool:
        return self._game is not None

    def proved_mate(self) -> bool:
        """Return true if the engine proved a checkmate, otherwise false.

        Returns
        -------
        bool
            True if there is a checkmate, otherwise false.
        """
        return (self._searcher is not None) and self._searcher.proved_mate()

    def _clear(self) -> None:
        self._searcher.init()
        self._game = None

    def search(self, num_sims: int, num_actions: tp.Optional[int] = None):
        """Explore nodes using sequential halving.

        Parameters
        ----------
        num_sims : int
            Number of game positions (including root) to simulate.
        num_actions : int, optional
            Number of initial actions at root node to search for,
            by default `None`.
            Note that 16 actions are sampled in the original paper:
            https://openreview.net/pdf?id=bERaNdoegnO
            "we sample m = min(n, 16) actions without replacement."
        """
        if not self.is_ready():
            raise ValueError("The engine is not ready to run")
        if num_actions is None:
            self._search(num_sims)
        else:
            self._search_with_gumbel_planning(num_sims, num_actions)

    def _search(self, num_sims: int):
        for _ in range(num_sims):
            self._select_simulate_expand_backprop()

    def _search_with_gumbel_planning(self, num_sims: int, num_actions: int):
        n_phase = max(int(np.ceil(np.log2(num_actions))), 1)
        n_per_p = (num_sims - 1) // n_phase
        if n_per_p <= num_actions:
            raise ValueError(
                f"Number of simulations in a phase (={n_per_p}) should be "
                f"larger than `num_actions` (={num_actions}). Please pass a "
                f"value larger than {num_actions * n_phase} to `num_sims`.")
        if self.get_search_count() == 0:
            self._select_simulate_expand_backprop()
            num_sims -= 1

        budgets = (
            [n_per_p] * (n_phase - 1) + [num_sims - n_per_p * (n_phase - 1)])
        for budget in budgets:
            self._searcher.keep_top_n_actions(num_actions)
            for _ in range(budget):
                self._select_simulate_expand_backprop()
            num_actions = num_actions // 2

    def _select_simulate_expand_backprop(self):
        node = self._searcher.search(self._game._game)
        if node is None:
            return
        policy_logits, value = self._policy_value_func(self._game)
        self._searcher.simulate_expand_backprop(
            node, self._game._game, value, policy_logits)

    def get_q_value(self, greedy_depth: int = 0) -> float:
        """Return Q-value estimate of the current game position.

        Parameters
        ----------
        greedy_depth : int, optional
            Number of depth to select nodes greedily instead of averaging,
            by default 0.

        Returns
        -------
        float
            Q-value estimate of the current game position.
        """
        return self._searcher.get_root().get_q_value(greedy_depth)

    def select(self, temperature: float = None) -> Move:
        """Return selected action based on the algorithm.

        Parameters
        ----------
        temperature : float, optional
            Temperature parameter for selecting an action.

        Returns
        -------
        Move
            Selected action.
        """
        if temperature is None:
            return self._searcher.select_action()
        if self._searcher.count_active_childs() > 0:
            raise ValueError(
                "Do not pass `temperature` parameter after running "
                "sequential halving")
        return self._searcher.select_action(temperature)

    def apply(self, move: Move):
        """Apply a move and make a corresponding child node be the new root.

        Parameters
        ----------
        move : Move
            Move to apply.
        """
        if self._is_ready():
            self._searcher.apply(self._game._game, move)

    def _tree(
        self,
        depth: int = 1,
        breadth: int = 3,
        pv_line: tp.List[tp.Union[Move, str]] = [],
        *,
        sort_key: callable = lambda n: -n.get_visit_count(),
    ) -> str:
        node = self._searcher.get_root()
        if node is None:
            return None
        pv_line = [
            self._game._get_move_class()(m) if isinstance(m, str) else m
            for m in pv_line
        ]
        move_type = self._game._get_move_class()
        for m in pv_line:
            for a in node.get_actions():
                if move_type(a) == m:
                    node = node.get_child_of(a)
                    break
            else:
                raise ValueError(f'Cannot find child with action, {m}')
        return _tree(
            node,
            move_type,
            depth,
            breadth,
            sort_key=sort_key,
        )
