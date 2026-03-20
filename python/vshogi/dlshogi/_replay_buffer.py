import numpy as np
import torch as th

from vshogi._game import Game
from vshogi.dlshogi._data import Data
from vshogi.judkins_shogi._game import Game as JudkinsGame  # noqa: F401
from vshogi.minishogi._game import Game as MinishogiGame  # noqa: F401
from vshogi.shogi._game import Game as StandardGame  # noqa: F401


class ReplayBuffer(th.utils.data.Dataset):
    """Storage of data to train DL-Shogi network.

    Examples
    --------
    >>> from vshogi.minishogi import Move; import numpy as np
    >>> b = ReplayBuffer(buffer_size=2)
    >>> b.add(Data('4k/5/4P/5/5 b G 1', {Move('1c1b'): 1}, 1., 1.))
    >>> len(b)  # Note that the length is doubled
    2
    >>> b.add(Data('4k/5/4P/5/5 b G 3', {}, 0.5, 0.6))
    >>> len(b)
    4
    >>> b[1][2:]  # value01, weight
    (array([0.5], dtype=float32), array(0.6, dtype=float32))
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
        game_class = eval(self._game_variant)
        ii = index % len(self._buffer)
        g: Game = eval(self._game_variant)(self._buffer[ii].sfen)
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
        if isinstance(self._buffer[ii].value01, dict):
            value01 = np.zeros(
                (
                    game_class.files,
                    game_class.ranks,
                    game_class._get_move_class()._num_policy_per_square(),
                ),
                dtype=np.float32,
            )
            value01[...] = np.nan
            for m, v in self._buffer[ii].value01.items():
                value01.ravel()[m._to_dlshogi_policy_index(g.turn)] = v
        else:
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
