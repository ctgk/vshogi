import typing as tp

import numpy as np

from vshogi._game import Game
from vshogi.engine._dfpn import DfpnSearcher
from vshogi.engine._engine import Engine
from vshogi.engine._mcts import Mcts, _tree


Move = tp.TypeVar('Move')


class DfpnMcts(Engine):
    """Shogi engine using combination of DFPN and MCTS algorithms."""

    def __init__(
        self,
        dfpn: DfpnSearcher,
        mcts: Mcts,
    ) -> None:
        """Initialize DFPN+MCTS search engine.

        Parameters
        ----------
        dfpn : DfpnSearcher
            Searcher based on DFPN algorithm.
        mcts : Mcts
            Monte-Carlo tree searcher.
        """
        self._dfpn = dfpn
        self._mcts = mcts
        self._found_mate: bool = False

    def _set_game(self, game: Game):
        self._mcts._set_game(game)
        self._found_mate = False

    def _is_ready(self) -> bool:
        return self._mcts._is_ready()

    def _clear(self):
        self._dfpn._clear()
        self._mcts._clear()
        self._found_mate = False

    def apply(self, move: Move):
        """Apply a move on the game.

        Parameters
        ----------
        move : Move
            Move to apply
        """
        if self._is_ready():
            self._mcts.apply(move)
        self._found_mate = False

    @property
    def mcts_num_searched(self) -> int:
        """Return number of game positions searched so far by MCTS.

        Returns
        -------
        int
            Number of game positions searched so far by MCTS.
        """
        return self._mcts.num_searched

    @property
    def dfpn_found_mate(self) -> bool:
        """Return true if DFPN found a mate-move otherwise false.

        Returns
        -------
        bool
            True if DFPN found a mate-move otherwise false.
        """
        return self._found_mate

    def search(
        self,
        dfpn_searches_at_root: int = 10000,
        mcts_searches: int = 100,
        dfpn_searches_at_vertex: int = 100,
        kldgain_threshold: float = None,
    ):
        """Search for subsequent game positions.

        Parameters
        ----------
        dfpn_searches_at_root : int, optional
            Number of searches by DFPN at root, by default 10000
        mcts_searches : int, optional
            Number of searches by MCTS, by default 100
        dfpn_searches_at_vertex : int, optional
            Number of searches by DFPN at every vertex of MCTS, by default 100
        kldgain_threshold : float, optional
            KL divergence threshold to stop MCT-search, by default None.
        """
        self._dfpn.set_game(self._mcts._game)
        if self._dfpn.search(dfpn_searches_at_root):
            self._found_mate = True
            return

        prev_visits = None
        kldgain_steps = 100
        for ii in range(mcts_searches):
            if ((ii % kldgain_steps == 0) and (kldgain_threshold is not None)):
                if prev_visits is None:
                    prev_visits = self._mcts.get_visit_counts()
                else:
                    kldgain = self._kldgain(prev_visits)
                    if kldgain < kldgain_threshold * kldgain_steps:
                        break
            game = self._mcts._game.copy()
            node = self._mcts._searcher.select(game._game)
            if node is None:
                continue

            self._dfpn.set_game(game)
            if self._dfpn.search(dfpn_searches_at_vertex):
                node.simulate_mate_and_backprop()
            else:
                policy, value = self._mcts._policy_value_func(game)
                node.simulate_expand_and_backprop(game._game, value, policy)

    def _kldgain(self, prev_visits: tp.Dict[Move, int]) -> float:
        prev_visits_added = {m: v + 1 for m, v in prev_visits.items()}
        prev_visits_sum = sum(prev_visits_added.values())
        prev_probas = {
            m: prev_visits_added[m] / prev_visits_sum
            for m in prev_visits_added.keys()
        }
        curr_visits = self._mcts.get_visit_counts()
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

    def select(self, temperature: tp.Optional[float] = None) -> Move:
        """Select action based on MCTS or DFPN.

        Parameters
        ----------
        temperature : tp.Optional[float], optional
            Temperature parameter for action selection based on MCTS,
            by default None

        Returns
        -------
        Move
            Selected action.
        """
        if self._found_mate:
            return self._dfpn.select()
        return self._mcts.select(temperature)

    def get_mate_moves(self) -> tp.List[Move]:
        """Return mate moves if found.

        Returns
        -------
        tp.List[Move]
            Mate moves found. If a mate move is found yet,
            it returns an empty list.
        """
        if self._found_mate:
            return self._dfpn.get_mate_moves()
        return []

    def get_value(self) -> float:
        """Return raw value estimate of the current game position.

        Returns
        -------
        float
            Raw value estimate of the current game position.
        """
        return self._mcts.get_value()

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
        return self._mcts.get_q_value(greedy_depth)

    def get_probas(self) -> tp.Dict[Move, float]:
        """Return raw probabilities of selecting actions.

        Returns
        -------
        tp.Dict[Move, float]
            Raw probabilities of selecting actions by `policy_value_func`.
        """
        move_proba_pair_list = [
            (m, self._mcts._root.get_child(m).get_proba())
            for m in self._mcts._root.get_actions()
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
        move_q_pair_list = [
            (m, -self._mcts._root.get_child(m).get_q_value(greedy_depth))
            for m in self._mcts._root.get_actions()
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
        return self._mcts.get_visit_counts(include_random=include_random)

    def _tree(
        self,
        depth: int = 1,
        breadth: int = 3,
        *,
        sort_key: callable = lambda n: -n.get_visit_count(),
        greedy_depth: int = 0,
    ) -> str:
        return _tree(
            self._mcts._searcher.get_root(),
            depth,
            breadth,
            sort_key=sort_key,
            greedy_depth=greedy_depth,
        )
