import typing as tp

import numpy as np
import scipy.special as sp

from vshogi._game import Game
from vshogi.engine._engine import Engine
from vshogi._move import Move
from vshogi.engine._dfpn import DfpnSearcher


class Klent(Engine):
    """Kullback-Leibler and Entropy Regularized Policy Optimization class.

    Notes
    -----
    KLENT : https://arxiv.org/pdf/2602.10894
    """

    def __init__(
        self,
        policy_value_func: tp.Callable[
            [Game],
            tp.Tuple[np.ndarray, np.ndarray | dict[Move, float]],
        ] = lambda g: (
            g.to_dlshogi_policy({}),
            np.tanh(g.to_dlshogi_policy({})),
        ),
        *,
        coeff_kl: float = 0.1,
        coeff_entropy: float = 0.03,
        dfpn_search: int = 0,
        name: str | None = None,
    ):
        """Initialize a KLENT engine instance.

        Parameters
        ----------
        policy_value_func : Callable[[Game], Tuple[np.ndarray, np.ndarray]]
            Function that takes a game state and returns `(policy_logits,
            value_array)`. `policy_logits` are converted to legal-move
            probabilities via ``Game.masked_softmax`` and `value_array` is used
            as move values for policy improvement.
        coeff_kl : float, optional
            Coefficient of the KL regularization term.
        coeff_entropy : float, optional
            Coefficient of the entropy regularization term.
        dfpn_search : int, optional
            Number of DFPN search iterations performed before policy
            improvement.
        name : str | None, optional
            Engine name. If ``None``, the default engine naming behavior is
            used.
        """
        super().__init__(tree_size=1, name=name)
        self._policy_value_func = policy_value_func
        self._coeff_kl = coeff_kl
        self._coeff_entropy = coeff_entropy
        self._dfpn = DfpnSearcher(dfpn_search * 10 + 1)
        self._dfpn_search = dfpn_search

        # Logarithm of eq. (3) in https://arxiv.org/pdf/2602.10894
        self._log_pi_prime: dict[Move, float] | None = None
        self._value: float | None = None
        self._game: Game | None = None

    def search(self) -> None:
        """Compute improved policy."""
        self._dfpn.search(self._dfpn_search)
        if self._dfpn.proved_mate():
            self._log_pi_prime = {self._dfpn.select(): 0.0}
            self._value = 1.0
            return

        p_logits, q_values = self._policy_value_func(self._game)
        probas = self._game.masked_softmax(p_logits)
        if isinstance(q_values, dict):
            self._log_pi_prime = {
                m: (q_values[m] + self._coeff_kl * p)
                / (self._coeff_entropy + self._coeff_kl)
                for m, p in probas.items()
            }
            pi_prime = sp.softmax(list(self._log_pi_prime.values()))
            self._value = sum(
                p * q_values[m] for p, m in zip(pi_prime, self._log_pi_prime)
            )
        else:
            q_values = q_values.ravel()
            self._log_pi_prime = {
                m: (
                    q_values[m._to_dlshogi_policy_index(self._game.turn)]
                    + self._coeff_kl * p
                )
                / (self._coeff_entropy + self._coeff_kl)
                for m, p in probas.items()
            }
            pi_prime = sp.softmax(list(self._log_pi_prime.values()))
            self._value = sum(
                p * q_values[m._to_dlshogi_policy_index(self._game.turn)]
                for p, m in zip(pi_prime, self._log_pi_prime)
            )

    def proved_mate(self) -> bool:
        """Return true if the engine proved a checkmate, otherwise false.

        Returns
        -------
        bool
            True if there is a checkmate, otherwise false.
        """
        return self._dfpn.proved_mate()

    def get_q_value(self) -> float:
        """Return value estimate of the current game position.

        Returns
        -------
        float
            Value estimate of the current game position.
        """
        return self._value

    def improved_policy(self) -> dict[Move, float]:
        """Return improved policy.

        Returns
        -------
        dict[Move, float]
            Improved policy
        """
        pi_prime = sp.softmax(list(self._log_pi_prime.values()))
        policy = {m: p for m, p in zip(self._log_pi_prime, pi_prime)}
        policy = dict(sorted(policy.items(), key=lambda t: t[1], reverse=True))
        return policy

    def _set_game(self, game: Game) -> None:
        self._game = game  # no copy required because there is no tree search.
        self._log_pi_prime = None
        self._dfpn.set_game(game)

    def _is_ready(self) -> bool:
        return self._game is not None

    def _clear(self) -> None:
        self._game = None
        self._log_pi_prime = None
        self._value = None

    def _select(self, temperature: float | None) -> Move:
        if temperature is None:
            return max(self._log_pi_prime, key=self._log_pi_prime.get)
        probas = sp.softmax(
            [v / temperature for v in self._log_pi_prime.values()]
        )
        return np.random.choice(tuple(self._log_pi_prime.keys()), p=probas)

    def _get_mate_moves(self) -> list[Move] | None:
        if not self.proved_mate():
            return None
        return self._dfpn.get_mate_moves()

    def _search(self) -> None:
        pass
