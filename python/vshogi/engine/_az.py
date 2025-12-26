import typing as tp

import numpy as np

from vshogi._game import Game
from vshogi.engine._engine import Engine


Move = tp.TypeVar('Move')
Policy = np.ndarray
Value = float


def _repr_node(n, greedy_detph: int = 0) -> str:
    d = greedy_detph
    return f"Node(q{d}={n.get_q_value(d):.2f}, count={n.get_visit_count()})"


def _tree(
    root,
    move_type: type,
    depth: int = 1,
    breadth: int = 3,
    *,
    sort_key=lambda n: -n.get_visit_count(),
    greedy_depth: int = 0,
) -> str:
    out = _repr_node(root, greedy_detph=greedy_depth)
    if depth == 0:
        return out
    children = [(a, root.get_child_of(a)) for a in root.get_actions()]
    children.sort(key=lambda t: sort_key(t[1]), reverse=False)
    if breadth > 0:
        children = children[:breadth]
    for i, (a, child) in enumerate(children):
        s = _tree(
            child,
            move_type,
            depth - 1,
            breadth,
            sort_key=sort_key,
            greedy_depth=greedy_depth,
        )
        if i == len(children) - 1:
            s = s.replace('\n', '\n    ')
        else:
            s = s.replace('\n', '\n|   ')
        out += f'\n+-- p={child.get_proba():.4f} {move_type(a)} -> {s}'
    return out


class AlphaZero(Engine):
    """Alpha Zero engine."""

    def __init__(
        self,
        policy_value_func: tp.Callable[
            [Game], tuple[Policy, Value],
        ] = lambda g: (g.to_dlshogi_policy({}), 0.),
        *,
        coeff_puct: float = 1.,
        random_rate: float = 0.25,
        kldgain_threshold: float = None,
        tree_size: int = 1000000,
        dfpn_search_root: int = 0,
        dfpn_search_leaf: int = 0,
        name: str = None,
    ) -> None:
        """Initialize an Alpha Zero agent.

        Parameters
        ----------
        policy_value_func : tp.Callable[[Game], tuple[Policy, Value]]
            Function that computes the policy distribution and state value for
            a given game position.
        coeff_puct : float, optional
            Coefficient used the PUCT formula. Higher values put more weight on
            the policy prior relative to the value estimate. Default is 1.0.
        random_rate : float, optional
            Probability of selecting a random node at root during exploration.
            Default is 0.25.
        kldgain_threshold : float, optional
            KL divergence threshold for early stopping of MCTS.
            Default is None.
        tree_size : int, optional
            Size of the tree search. Default is 1000000.
        dfpn_search_root : int, optional
            Number of DFPN searches to run at the root node. Default is 0.
        dfpn_search_leaf : int, optional
            Number of DFPN searches to run at leaf nodes. Default is 0.
        name : str, optional
            Name of the search engine instance. Default is None.
        """
        super().__init__(name=name)
        self._policy_value_func = policy_value_func
        self._searcher = None
        self._game = None

        self._coeff_puct = coeff_puct
        self._random_rate = random_rate
        self._kldgain_threshold = kldgain_threshold
        self._tree_size = tree_size
        self._dfpn_search_root = dfpn_search_root
        self._dfpn_search_leaf = dfpn_search_leaf

    def _set_game(self, game: Game):
        self._game = game.copy()
        if (type(self._searcher) is not game._get_az_searcher_class()):
            self._searcher = game._get_az_searcher_class()(
                self._coeff_puct, self._random_rate, self._tree_size,
                self._dfpn_search_root, self._dfpn_search_leaf,
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
        if self._searcher is not None:
            self._searcher.init()
        self._game = None

    def apply(self, move: Move):
        """Apply a move on the game.

        Parameters
        ----------
        move : Move
            Move to apply
        """
        if self._is_ready():
            self._searcher.apply(self._game._game, move)

    def search(self, n_or_t: int | float = 0.01):
        """Explore from root node for n times.

        Parameters
        ----------
        n_or_t : int | float, optional
            Number of game positions to search or period of time to search
            in second, by default 0.01
        """
        prev_visits = None
        kldgain_steps = 100
        for ii in self._count(n_or_t=n_or_t):
            if (self._kldgain_threshold and (ii % kldgain_steps == 0)):
                if prev_visits is None:
                    prev_visits = self.get_visit_counts()
                else:
                    kldgain = self._kldgain(prev_visits)
                    if kldgain < self._kldgain_threshold * kldgain_steps:
                        break
            node = self._searcher.search(self._game._game)
            if node is None:
                continue
            policy_logits, value = self._policy_value_func(self._game)
            self._searcher.simulate_expand_backprop(
                node, self._game._game, value, policy_logits)

    def _kldgain(self, prev_visits: tp.Dict[Move, int]) -> float:
        prev_visits_added = {m: v + 1 for m, v in prev_visits.items()}
        prev_visits_sum = sum(prev_visits_added.values())
        prev_probas = {
            m: prev_visits_added[m] / prev_visits_sum
            for m in prev_visits_added.keys()
        }
        curr_visits = self.get_visit_counts()
        curr_visits_added = {m: v + 1 for m, v in curr_visits.items()}
        curr_visits_sum = sum(curr_visits_added.values())
        curr_probas = {
            m: curr_visits_added[m] / curr_visits_sum
            for m in curr_visits_added.keys()
        }
        kldgain = sum(
            curr_probas[m] * np.log(curr_probas[m] / prev_probas[m])
            for m in prev_probas.keys()
        )
        for m in prev_visits.keys():
            prev_visits[m] = curr_visits[m]
        return kldgain

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

    def get_probas(self) -> tp.Dict[Move, float]:
        """Return raw probabilities of selecting actions.

        Returns
        -------
        tp.Dict[Move, float]
            Raw probabilities of selecting actions by `policy_value_func`.
        """
        if self._searcher is None:
            return {}
        root = self._searcher.get_root()
        move_proba_pair_list = [
            (m, root.get_child_of(m).get_proba())
            for m in root.get_actions()
        ]
        move_proba_pair_list.sort(key=lambda t: t[1], reverse=True)
        return {m: p for m, p in move_proba_pair_list}

    def get_q_values(self, greedy_depth: int = 0) -> tp.Dict[Move, float]:
        """Return Q value of each action.

        Parameters
        ----------
        greedy_depth : int, optional
            Number of depth to select nodes greedily instead of averaging,
            by default 0.

        Returns
        -------
        tp.Dict[Move, float]
            Q value of each action.
        """
        if self._searcher is None:
            return {}
        move_type = self._game._get_move_class()
        root = self._searcher.get_root()
        move_q_pair_list = [
            (m, -root.get_child_of(m).get_q_value(greedy_depth))
            for m in root.get_actions()
        ]
        move_q_pair_list.sort(key=lambda a: a[1], reverse=True)
        return {move_type(m): q for m, q in move_q_pair_list}

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
        if self._searcher is None:
            return {}
        move_type = self._game._get_move_class()
        root = self._searcher.get_root()
        move_visit_count_pair_list = [
            (
                move_type(m),
                root.get_child_of(m).get_visit_count()
                if include_random else
                root.get_child_of(m).get_visit_count_excluding_random(),
            )
            for m in root.get_actions()
        ]
        move_visit_count_pair_list.extend([
            (m, 0) for m in self._game.get_legal_moves()
            if m not in [t[0] for t in move_visit_count_pair_list]
        ])
        move_visit_count_pair_list.sort(key=lambda a: a[1], reverse=True)
        return {m: v for m, v in move_visit_count_pair_list}

    def _select(self, temperature: float | None = None) -> Move:
        if (temperature is None) or np.isclose(temperature, 0):
            return self._searcher.get_action_by_visit_max()
        else:
            return self._searcher.get_action_by_visit_distribution(temperature)

    def _tree(
        self,
        depth: int = 1,
        breadth: int = 3,
        pv_line: list[Move | str] = [],
        *,
        sort_key: callable = lambda n: -n.get_visit_count(),
        greedy_depth: int = 0,
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
            greedy_depth=greedy_depth,
        )
