#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum PieceTypeEnum {
    FU = 0, // FU (Pawn)
    GI,     // Gin (Silver)
    KA,     // Kaku (Bishop)
    HI,     // Hisha (Rook)
    KI,     // Kin (Gold)
    OU,     // Ou, Gyoku (King)
    TO,     // Tokin (Promoted Pawn)
    NG,     // Nari-Gin (Promoted Silver)
    UM,     // Uma (Promoted Bishop)
    RY,     // Ryu (Promoted Rook)
    NA,     // Not available
}

impl From<u8> for PieceTypeEnum {
    fn from(value: u8) -> Self {
        match value {
            0 => Self::FU,
            1 => Self::GI,
            2 => Self::KA,
            3 => Self::HI,
            4 => Self::KI,
            5 => Self::OU,
            6 => Self::TO,
            7 => Self::NG,
            8 => Self::UM,
            9 => Self::RY,
            _ => Self::NA,
        }
    }
}

pub struct PieceTraits;

impl PieceTraits {
    pub fn is_promotable(pt: PieceTypeEnum) -> bool {
        pt < PieceTypeEnum::KI
    }
}
