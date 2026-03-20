from dataclasses import dataclass

from vshogi._move import Move


@dataclass
class Data:
    """Input and target data for training DL-Shogi network.

    Notes
    -----
    It removes ply count in SFEN in initialization.

    Examples
    --------
    >>> d = Data(sfen='4k/4G/4P/5/5 w - 1', policy={}, value01=-1., weight=1.)
    >>> d.sfen
    '4k/4G/4P/5/5 w -'
    """

    sfen: str
    policy: dict[Move, float]
    value01: float | dict[Move, float]
    weight: float = 1.0
    malignancy: float = 0.0

    def __post_init__(self):
        if self.sfen.count(' ') == 3:
            self.sfen = ' '.join(self.sfen.split(' ')[:3])
