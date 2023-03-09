import vshogi._vshogi.animal_shogi as _as
from vshogi._enum import _Enum


class Color(_Enum):
    """Enumeration of colors.

    Members
    -------
    BLACK :
        First player's color
    WHITE :
        Second player's color
    """

    BLACK = _as.BLACK
    WHITE = _as.WHITE
