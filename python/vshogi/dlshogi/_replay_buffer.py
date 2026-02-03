import numpy as np
import torch as th

from vshogi.dlshogi._data import Data
from vshogi.judkins_shogi._game import Game as JudkinsGame  # noqa: F401
from vshogi.minishogi._game import Game as MinishogiGame  # noqa: F401
from vshogi.shogi._game import Game as StandardGame  # noqa: F401


def _add_dicts(d1: dict, d2: dict) -> dict:
    if (not d1) or (not d2):
        return {}
    out = {
        k: d1.get(k, 0.0) + d2.get(k, 0.0)
        for k in (set(d1.keys()) | set(d2.keys()))
    }
    if np.isclose(sum(out.values()), 0):
        msg = f"Adding two invalid dictionaries: {d1} and {d2}"
        raise ValueError(msg)
    return out


def _normalize(d: dict) -> dict:
    if not d:
        return d
    s = sum(d.values())
    if s < 0.01:
        raise ValueError(f'Too small value: {d}')
    return {k: v / s for k, v in d.items()}


class ReplayBuffer(th.utils.data.Dataset):
    """Storage of data to train DL-Shogi network.

    Examples
    --------
    >>> from vshogi.minishogi import Move; import numpy as np
    >>> b = ReplayBuffer(buffer_size=2)
    >>> b.add(Data('4k/5/4P/5/5 b G 1', {Move('1c1b'): 1}, 1., 1.))
    >>> len(b)  # Note that the length is doubled
    2
    >>> b.add(Data('4k/5/4P/5/5 b G 3', {}, 0.5, 0.9))
    >>> len(b)
    2
    >>> b[0][2:]  # value01, weight
    (array([0.75], dtype=float32), array(1.9, dtype=float32))
    """

    def __init__(self, buffer_size: int = 100000):
        """Initialize dataset class.

        Parameters
        ----------
        buffer_size : int
            Max size of the buffer
        """
        super().__init__()
        self._buffer: list[Data] = []
        self._buffer_size = buffer_size
        self._game_variant: str | None = None

    def add(self, data: Data):
        """Add data to the buffer.

        Parameters
        ----------
        data : Data
            Data to add.
        """
        index = next(
            (i for i, b in enumerate(self._buffer) if b.sfen == data.sfen),
            None,
        )
        if index is not None:
            old = self._buffer.pop(index)
            data = self._merge(old, data)
        self._buffer.append(data)
        if self._game_variant is None:
            self._game_variant = self._infer_game_variant(data.sfen)
        while len(self._buffer) > self._buffer_size:
            self._buffer.pop(0)  # FIFO

    @staticmethod
    def _merge(a: Data, b: Data) -> Data:
        assert a.sfen == b.sfen
        s = a.count + b.count
        return Data(
            sfen=a.sfen,
            policy={}
            if (not a.policy) or (not b.policy)
            else {
                m: (a.count * a.policy[m] + b.count * b.policy[m]) / s
                for m in set(a.policy.keys()) | set(b.policy.keys())
            },
            value01=(a.value01 * a.count + b.value01 * b.count) / s,
            weight=a.weight + b.weight,
            count=s,
        )

    def is_full(self) -> bool:
        """Return true if the buffer is full of data.

        Returns
        -------
        bool
            True if the buffer is full of data, otherwise false.
        """
        return len(self._buffer) == self._buffer_size

    def deduplicate(self) -> dict:
        """Update value01 and policy by averaging all the data.

        Returns
        -------
        dict
            Summary.
        """
        summary: dict[str, dict] = {
            d.sfen: {
                "value01": d.value01,
                "policy": d.policy,
                "count": d.count,
                "weight": d.weight,
            }
            for d in self._buffer
        }
        scale = 1 / np.mean([v["weight"] for v in summary.values()])
        for d in self._buffer:
            d.weight /= scale
            d.policy = _normalize(d.policy)
        return summary

    def __len__(self):
        """Return the length of the dataset."""
        return len(self._buffer) * 2

    def __getitem__(self, index: int) -> tuple:
        """Get data at the given index.

        Parameters
        ----------
        index : int
            Index of the list.

        Returns
        -------
        tuple
            The output is comprised of the following 4 elements:

                1. Input that encodes game position. (F, R, C)
                2. Target policy distribution. (P,)
                3. Target value in 0-1 scale. (1,)
                4. Data importance of the output. (1,)
        """
        if self._game_variant is None:
            raise ValueError("Please add data before trying to get items.")
        ii = index % len(self._buffer)
        g = eval(self._game_variant)(self._buffer[ii].sfen)
        policy = self._buffer[ii].policy
        if index >= len(self._buffer):
            g = g.hflip()
            policy = {m.hflip(): v for m, v in policy.items()}
        x = g.to_dlshogi_features().squeeze()
        try:
            policy = g.to_dlshogi_policy(policy, default_value=-100000.0)
        except ZeroDivisionError:
            msg = f"Invalid policy ({policy}) at: {self._buffer[ii].sfen}"
            raise ZeroDivisionError(msg)
        value01 = np.array([np.float32(self._buffer[ii].value01)])
        w = np.array(np.float32(self._buffer[ii].weight))
        return x.squeeze(), policy.squeeze(), value01, w

    def _infer_game_variant(self, sfen: str) -> str:
        num_slashes = sfen.split(' ')[0].count('/')
        if num_slashes == 4:
            return 'MinishogiGame'
        elif num_slashes == 5:
            return 'JudkinsGame'
        elif num_slashes == 8:
            return 'StandardGame'
        else:
            raise ValueError(f'Invalid SFEN: {sfen}')
