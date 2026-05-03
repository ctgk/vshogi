use crate::common::color::ColorEnum;
use crate::common::piece_type::BasePieceType;
use num_enum::FromPrimitive;
use num_enum::IntoPrimitive;

const NUM_PIECE_TYPES: u8 = 10;
const NUM_STAND_PIECE_TYPES: u8 = 5;

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive, IntoPrimitive)]
pub enum PieceType {
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
    #[num_enum(default)]
    Na, // Not available
}

impl BasePieceType<NUM_PIECE_TYPES, NUM_STAND_PIECE_TYPES> for PieceType {
    fn is_promotion_always_better(self) -> bool {
        matches!(self, Self::Fu | Self::Ka | Self::Hi)
    }
    fn is_slider(self) -> bool {
        matches!(self, Self::Ka | Self::Hi | Self::Um | Self::Ry)
    }
    fn promote(self) -> PieceType {
        match self as u8 {
            0..4 => Self::from(self as u8 + 6),
            _ => Self::Na,
        }
    }
    fn demote(self) -> PieceType {
        match self as u8 {
            6..10 => Self::from(self as u8 - 6u8),
            _ => Self::Na,
        }
    }
}

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive)]
pub enum Piece {
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
    #[num_enum(default)]
    Void, // Empty square
}

impl Piece {
    pub fn new(color: ColorEnum, piece_type: PieceType) -> Self {
        if matches!(color, ColorEnum::None) || matches!(piece_type, PieceType::Na) {
            return Self::Void;
        }

        Self::from(color as u8 * 10 + piece_type as u8)
    }
    pub fn get_color(self) -> ColorEnum {
        match self as u8 {
            0..=9 => ColorEnum::Black,
            10..=19 => ColorEnum::White,
            _ => ColorEnum::None,
        }
    }
    pub fn to_piece_type(self) -> PieceType {
        if self < Self::WhFu {
            return PieceType::from(self as u8);
        }
        PieceType::from(self as u8 - 10u8)
    }
    pub fn is_promotable(self) -> bool {
        self.to_piece_type().is_promotable()
    }
    pub fn is_promoted(self) -> bool {
        self.to_piece_type().is_promoted()
    }
    pub fn is_promotion_always_better(self) -> bool {
        self.to_piece_type().is_promotion_always_better()
    }
    pub fn promote(self) -> Piece {
        match self as u8 {
            0..=3 | 10..=13 => Self::from(self as u8 + 6u8),
            _ => Self::Void,
        }
    }
    pub fn demote(self) -> Piece {
        match self as u8 {
            6..=9 | 16..=19 => Self::from(self as u8 - 6u8),
            _ => Self::Void,
        }
    }
    pub fn is_slider(self) -> bool {
        self.to_piece_type().is_slider()
    }
}
