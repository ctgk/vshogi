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
    >>> d = Data(sfen='4k/4G/4P/5/5 w - 1', policy={}, value01=0.9)
    >>> d.sfen
    '4k/4G/4P/5/5 w -'
    >>> out = d.merge(Data(sfen="4k/4G/4P/5/5 w -", policy={}, value01=0.7))
    >>> out.value01
    0.8
    >>> out.count
    2
    """

    sfen: str
    policy: dict[Move, float]
    value01: float | dict[Move, float]
    weight: float = 1.0
    malignancy: float = 0.0
    count: int = 1

    def __post_init__(self):
        if self.sfen.count(' ') == 3:
            self.sfen = ' '.join(self.sfen.split(' ')[:3])

    def merge(self, other: "Data", *, max_weight: float = 10.0) -> "Data":
        if not isinstance(other, Data):
            raise TypeError(f"Cannot merge Data with {type(other)} type.")
        if self.sfen != other.sfen:
            raise ValueError("Cannot merge data with different game positions")
        total_count = self.count + other.count
        return Data(
            sfen=self.sfen,
            policy=self._normalize(
                {
                    m: (
                        self.policy.get(m, 0.0)
                        + other.count * other.policy.get(m, 0.0)
                    )
                    for m in (
                        set(self.policy.keys()) | set(other.policy.keys())
                    )
                }
            ),
            value01=(
                (self.value01 + other.count * other.value01) / total_count
                if isinstance(self.value01, float)
                else {
                    m: (
                        self.value01.get(m, 0.5)
                        + other.count * other.value01.get(m, 0.5)
                    )
                    / total_count
                    for m in set(self.value01.keys())
                    | set(other.value01.keys())
                }
            ),
            weight=min(self.weight + other.weight, max_weight),
            malignancy=self.malignancy,
            count=total_count,
        )

    @staticmethod
    def _normalize(d: dict) -> dict:
        if not d:
            return d
        s = sum(d.values())
        return {m: v / s for m, v in d.items()}
