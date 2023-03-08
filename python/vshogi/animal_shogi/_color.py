from enum import Enum

import vshogi._vshogi.animal_shogi as _as


class ColorEnum(Enum):
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

    def __repr__(self) -> str:
        return repr(self.value)
