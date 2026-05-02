#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum PieceTypeEnum {
    Fu = 0, // Fu (Pawn)
    Gi,     // Gin (Silver)
    Ka,     // Kaku (Bishop)
    Hi,     // Hisha (Rook)
    Ki,     // Kin (Gold)
    Ou,     // Ou, Gyoku (King)
    To,     // Tokin (Promoted Pawn)
    Ng,     // Nari-Gin (Promoted Silver)
    Um,     // Uma (Promoted Bishop)
    Ry,     // Ryu (Promoted Rook)
    Na,     // Not available
}

impl From<u8> for PieceTypeEnum {
    fn from(value: u8) -> Self {
        match value {
            0 => Self::Fu,
            1 => Self::Gi,
            2 => Self::Ka,
            3 => Self::Hi,
            4 => Self::Ki,
            5 => Self::Ou,
            6 => Self::To,
            7 => Self::Ng,
            8 => Self::Um,
            9 => Self::Ry,
            _ => Self::Na,
        }
    }
}

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum PieceEnum {
    BlFu = 0, // Black Fu (Pawn)
    BlGi,     // Black Gin (Silver)
    BlKa,     // Black Kaku (Bishop)
    BlHi,     // Black Hisha (Rook)
    BlKi,     // Black Kin (Gold)
    BlOu,     // Black Ou, Gyoku (King)
    BlTo,     // Black Tokin (Promoted Pawn)
    BlNg,     // Black Nari-Gin (Promoted Silver)
    BlUm,     // Black Uma (Promoted Bishop)
    BlRy,     // Black Ryu (Promoted Rook)
    WhFu,     // White Fu (Pawn)
    WhGi,     // White Gin (Silver)
    WhKa,     // White Kaku (Bishop)
    WhHi,     // White Hisha (Rook)
    WhKi,     // White Kin (Gold)
    WhOu,     // White Ou, Gyoku (King)
    WhTo,     // White Tokin (Promoted Pawn)
    WhNg,     // White Nari-Gin (Promoted Silver)
    WhUm,     // White Uma (Promoted Bishop)
    WhRy,     // White Ryu (Promoted Rook)
    Void,     // Empty square
}

impl From<u8> for PieceEnum {
    fn from(value: u8) -> Self {
        match value {
            0 => Self::BlFu,
            1 => Self::BlGi,
            2 => Self::BlKa,
            3 => Self::BlHi,
            4 => Self::BlKi,
            5 => Self::BlOu,
            6 => Self::BlTo,
            7 => Self::BlNg,
            8 => Self::BlUm,
            9 => Self::BlRy,
            10 => Self::WhFu,
            11 => Self::WhGi,
            12 => Self::WhKa,
            13 => Self::WhHi,
            14 => Self::WhKi,
            15 => Self::WhOu,
            16 => Self::WhTo,
            17 => Self::WhNg,
            18 => Self::WhUm,
            19 => Self::WhRy,
            _ => Self::Void,
        }
    }
}

pub struct PieceTraits;

impl PieceTraits {
    pub fn is_promotable(pt: PieceTypeEnum) -> bool {
        pt < PieceTypeEnum::Ki
    }
}
