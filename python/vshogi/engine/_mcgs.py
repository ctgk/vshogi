import typing as tp

import numpy as np

from vshogi._game import Game
from vshogi._move import Move
from vshogi.engine._engine import Engine
from vshogi.engine._az import AlphaZero


Policy = np.ndarray
Value = float | np.ndarray | dict[Move, float]


class Mcgs(Engine):
    """Monte Carlo Graph Search engine."""

    def __init__(
        self,
        policy_value_func: tp.Callable[
            [Game],
            tp.Tuple[Policy, Value],
        ] = lambda g: (g.to_dlshogi_policy({}), 0.0),
        *,
        epsilon_greedy: float = 0.1,
        enhanced_checks: bool = True,
        tree_size: int = 1000000,
        dfpn_search_root: int = 0,
        dfpn_search_leaf: int = 0,
        name: str | None = None,
    ) -> None:
        super().__init__(tree_size=tree_size, name=name)
        self._policy_value_func = AlphaZero._wrap_pv_func(policy_value_func)
        self._searcher = None
        self._game = None

        self._epsilon_greedy = epsilon_greedy
        self._enhanced_checks = enhanced_checks
        self._dfpn_search_root = dfpn_search_root
        self._dfpn_search_leaf = dfpn_search_leaf

    def _set_game(self, game):
        self._game = game.copy()
        if type(self._searcher) is not game._get_mcgs_class():
            self._searcher = game._get_mcgs_class()(
                self._tree_size,
                self._dfpn_search_root,
                self._dfpn_search_leaf,
            )
        else:
            self._searcher.init()

    def _is_ready(self):
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
        if self._is_ready():
            self._searcher.apply(self._game._game, move)

    def _search(self, budget: int | float):
        for _ in self._count(budget):
            node = self._searcher.search(
                self._game._game,
                self._epsilon_greedy,
            )
            if node is None:
                continue
            policy_logits, value = self._policy_value_func(self._game)
            self._searcher.simulate_expand_backprop(
                node,
                self._game._game,
                value,
                policy_logits,
                self._enhanced_checks,
            )

    def _get_improved_policy(self) -> dict[Move, float]:
        if self._searcher is None:
            return {}
        move_type: type[Move] = self._game._get_move_class()
        root = self._searcher.get_root()
        expanded_actions = [move_type(m) for m in root.get_actions()]
        move_visits_pairs = [
            (m, getattr(root.get_child_of(hash(m)), "get_visits", lambda: 0)())
            for m in expanded_actions
        ]
        move_visits_pairs.extend(
            [
                (m, 0)
                for m in self._game.get_legal_moves()
                if m not in expanded_actions
            ]
        )
        move_visits_pairs.sort(key=lambda t: t[1], reverse=True)
        total = sum(t[1] for t in move_visits_pairs)
        if total == 0:
            msg = "Please run further searches to get valid improved policy."
            raise ValueError(msg)
        return {m: v / total for m, v in move_visits_pairs}

    def _select(self, temperature: float | None = None) -> Move:
        if (temperature is None) or np.isclose(temperature, 0):
            return self._searcher.select_action()
        return self._searcher.select_action(temperature)

    def _get_mate_moves(self) -> list[Move] | None:
        if not self.proved_mate():
            return None
        return self._searcher.get_mate_moves(self._game._game)

    def get_value(self) -> float:
        """Return value estimate of the current game position.

        Returns
        -------
        float
            Value estimate of the current game position.
        """
        return self._searcher.get_root().get_value()

    def get_q_values(self) -> dict[Move, float | None]:
        """Return Q-value for each action.

        Returns
        -------
        dict[Move, float | None]
            Q-value for each action.
        """
        if self._searcher is None:
            return {}
        move_type = self._game._get_move_class()
        root = self._searcher.get_root()
        m_q_pairs = [(move_type(m), q) for m, q in root.get_q_values().items()]
        for a in self._game.get_legal_moves():
            if a not in [t[0] for t in m_q_pairs]:
                m_q_pairs.append((a, None))
        m_q_pairs.sort(key=lambda t: t[1], reverse=True)
        return {m: q for m, q in m_q_pairs}

    def get_visit_counts(self) -> dict[Move, int]:
        """Return visit counts of each action.

        Returns
        -------
        dict[Move, int]
            Visit counts of each action.
        """
        if self._searcher is None:
            return {}
        root = self._searcher.get_root()
        actions = self._game.get_legal_moves()
        pairs = [
            (a, getattr(root.get_child_of(hash(a)), "get_visits", lambda: 0)())
            for a in actions
        ]
        pairs.sort(key=lambda t: t[1], reverse=True)
        return {m: c for m, c in pairs}

    def _tree(
        self,
        depth: int = 1,
        breadth: int = 3,
        pv_line: tp.List[tp.Union[Move, str]] = [],
        *,
        sort_key: callable = lambda n: 0 if n is None else -n.get_visits(),
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


def _tree(
    root,
    move_type: type,
    depth: int = 1,
    breadth: int = 3,
    *,
    sort_key=lambda n: -n.get_visit_count(),
) -> str:
    out = _repr_node(root)
    if depth == 0 or root is None:
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


def _repr_node(n) -> str:
    if n is None:
        return "Node(v=None, count=0)"
    return f"Node(v={n.get_value():.2f}, count={n.get_visits()})"
