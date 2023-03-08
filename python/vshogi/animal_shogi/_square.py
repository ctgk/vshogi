import vshogi._vshogi.animal_shogi as _as
from vshogi._enum import _Enum


class Square(_Enum):
    """Enumeration of board squares.

    ```
        A  B  C
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

    A1 = _as.SQ_A1
    B1 = _as.SQ_B1
    C1 = _as.SQ_C1
    A2 = _as.SQ_A2
    B2 = _as.SQ_B2
    C2 = _as.SQ_C2
    A3 = _as.SQ_A3
    B3 = _as.SQ_B3
    C3 = _as.SQ_C3
    A4 = _as.SQ_A4
    B4 = _as.SQ_B4
    C4 = _as.SQ_C4
