import vshogi._vshogi.animal_shogi as _as
from vshogi._enum import _Enum


class Piece(_Enum):
    """Enumeration of piece types."""

    CH = _as.CH
    EL = _as.EL
    GI = _as.GI
    LI = _as.LI
    HE = _as.HE
    NA = _as.NA


class BoardPiece(_Enum):
    """Enumeration of board pieces (plus VOID)."""

    B_CH = _as.B_CH
    B_EL = _as.B_EL
    B_GI = _as.B_GI
    B_LI = _as.B_LI
    B_HE = _as.B_HE
    W_CH = _as.W_CH
    W_EL = _as.W_EL
    W_GI = _as.W_GI
    W_LI = _as.W_LI
    W_HE = _as.W_HE
    VOID = _as.VOID
