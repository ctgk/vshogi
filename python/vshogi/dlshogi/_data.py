import typing as tp
from dataclasses import dataclass


Move = tp.TypeVar('Move')


@dataclass
class Data:
    """Input and target data for training DL-Shogi network."""

    sfen: str
    visit_dist: dict[Move, float]
    value01: float
    weight: float
