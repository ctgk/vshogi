from ast import literal_eval

import numpy as np
import torch as th

from vshogi.dlshogi._data import Data
from vshogi.judkins_shogi._game import Game as JudkinsGame  # noqa: F401
from vshogi.minishogi._game import Game as MinishogiGame  # noqa: F401
from vshogi.shogi._game import Game as StandardGame  # noqa: F401


def _add_dicts(d1: dict, d2: dict) -> dict:
    if set(d1.keys()) != set(d2.keys()):
        raise ValueError(
            'Cannot add two dicts with unmatching keys: '
            f'd1({set(d1.keys())}), d2({set(d2.keys())})')
    return {k: d1[k] + d2[k] for k in d1}


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
    >>> b = ReplayBuffer(buffer_size=2)
    >>> b.add(Data('4k/5/4P/5/5 b G 1', {}, 1., 1.))
    >>> b.is_full()
    False
    >>> b.add(Data('4k/5/4P/5/5 b G 3', {}, 0.5, 0.9))
    >>> b.is_full()
    True
    >>> b[0][2:]  # value01, weight
    (array([1.], dtype=float32), array(1., dtype=float32))
    >>> _ = b.deduplicate()
    >>> b[0][2:]  # value01, weight
    (array([0.75], dtype=float32), array(1., dtype=float32))
    >>> b[1][2:]  # weights remain as they are
    (array([0.75], dtype=float32), array(0.9, dtype=float32))
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
        self._buffer.append(data)
        if self._game_variant is None:
            self._game_variant = self._infer_game_variant(data.sfen)
        while len(self._buffer) > self._buffer_size:
            self._buffer.pop(0)  # FIFO

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
        summary = self._summarize()
        for data in self._buffer:
            data.value01 = summary[data.sfen]['value01']
            data.policy = summary[data.sfen]['policy']
        return summary

    def _summarize(self):
        data_summed = {}
        for data in self._buffer:
            if data.sfen not in data_summed:
                data_summed[data.sfen] = {
                    'value01': 0.,
                    'policy': {m: 0 for m in data.policy},
                    'count': 0,
                }
            data_summed[data.sfen]['value01'] += data.value01
            data_summed[data.sfen]['policy'] = _add_dicts(
                data_summed[data.sfen]['policy'],
                data.policy,
            )
            data_summed[data.sfen]['count'] += 1
        for value in data_summed.values():
            value['value01'] = value['value01'] / value['count']
            value['policy'] = _normalize(value['policy'])
        return data_summed

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
        g = literal_eval(self._game_variant)(self._buffer[ii].sfen)
        policy = self._buffer[ii].policy
        if (index >= len(self._buffer)):
            g = g.hflip()
            policy = {m.hflip(): v for m, v in policy.items()}
        x = g.to_dlshogi_features().squeeze()
        policy = g.to_dlshogi_policy(policy, default_value=-100000.)
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
