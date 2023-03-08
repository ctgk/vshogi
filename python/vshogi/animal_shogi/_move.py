import vshogi._vshogi.animal_shogi as _as
from vshogi._enum import _Enum


class MoveSourceEnum(_Enum):
    """Enumeration of move sources.

    ```
    Board squares + Piece Stand
        A  B  C     CH, EL, GI
      *--*--*--*
    1 |A1|B1|C1|
      *--*--*--*
    2 |A2|B2|C2|
      *--*--*--*
    3 |A3|B3|C3|
      *--*--*--*
    4 |A4|B4|C4|
      *--*--*--*
    ```
    """

    A1 = _as.MS_A1
    B1 = _as.MS_B1
    C1 = _as.MS_C1
    A2 = _as.MS_A2
    B2 = _as.MS_B2
    C2 = _as.MS_C2
    A3 = _as.MS_A3
    B3 = _as.MS_B3
    C3 = _as.MS_C3
    A4 = _as.MS_A4
    B4 = _as.MS_B4
    C4 = _as.MS_C4
    CH = _as.MS_CH
    EL = _as.MS_EL
    GI = _as.MS_GI


def _move_repr(self: _as.Move) -> str:
    return f'Move({self.source().name} -> {self.destination().name})'


_as.Move.__repr__ = _move_repr
