import numpy as np
import torch as th

from vshogi.dlshogi._data import Data
from vshogi.judkins_shogi._game import Game as JudkinsGame  # noqa: F401
from vshogi.minishogi._game import Game as MinishogiGame  # noqa: F401
from vshogi.shogi._game import Game as StandardGame  # noqa: F401


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
    >>> b = ReplayBuffer(buffer_size=2, alpha=0.5)
    >>> b.add(Data('4k/5/4P/5/5 b G 1', {Move('1c1b'): 1}, 1., 1.))
    >>> len(b)  # Note that the length is doubled
    2
    >>> b.add(Data('4k/5/4P/5/5 b G 3', {}, 0.5, 0.6))
    >>> len(b)
    4
    >>> b[1][2:]  # value01, weight
    (array([0.75], dtype=float32), array(1., dtype=float32))
    """

    def __init__(self, buffer_size: int = 100000, alpha: float = 0.9):
        """Initialize dataset class.

        Parameters
        ----------
        buffer_size : int
            Max size of the buffer
        alpha : float
            Decay rate to update duplicating data.
        """
        super().__init__()
        self._ema: list[Data] = []
        self._buffer: list[Data] = []
        self._buffer_size = buffer_size
        self._alpha: float = alpha
        self._game_variant: str | None = None

    def _update(self, a: Data, b: Data) -> Data:
        assert a.sfen == b.sfen
        r: float = self._alpha  # rate of `a`
        return Data(
            sfen=a.sfen,
            policy={}
            if (not a.policy) or (not b.policy)
            else {
                m: (r * a.policy[m] + (1 - r) * b.policy[m])
                for m in set(a.policy.keys()) | set(b.policy.keys())
            },
            value01=(r * a.value01 + (1 - r) * b.value01),
        )

    def _update_ema(self, data: Data) -> Data:
        index = next(
            (i for i, d in enumerate(self._ema) if d.sfen == data.sfen),
            None,
        )
        if index is not None:
            moving_average = self._ema.pop(index)
            data = self._update(moving_average, data)
        self._ema.append(data)
        while len(self._ema) > self._buffer_size:
            self._ema.pop(0)  # FIFO
        return data

    def add(self, data: Data):
        """Add data to the buffer.

        Parameters
        ----------
        data : Data
            Data to add.
        """
        data = self._update_ema(data)
        self._buffer.append(data)
        if self._game_variant is None:
            self._game_variant = self._infer_game_variant(data.sfen)
        while len(self._buffer) > self._buffer_size:
            self._buffer.pop(0)  # FIFO

    def get_ema_of(self, sfen: str) -> Data | None:
        """Get exponential moving average data of the given SFEN game position.

        Parameters
        ----------
        sfen : str
            SFEN representation of the game position.

        Returns
        -------
        Data | None
            EMA data of the game position if found, otherwise None.
        """
        return next(
            (b for b in self._ema if b.sfen == sfen),
            None,
        )

    def is_full(self) -> bool:
        """Return true if the buffer is full of data.

        Returns
        -------
        bool
            True if the buffer is full of data, otherwise false.
        """
        return len(self._buffer) == self._buffer_size

    def normalize(self) -> None:
        """Normalize policy distributions of all data."""
        for d in self._buffer:
            d.policy = _normalize(d.policy)

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
