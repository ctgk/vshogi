use crate::common::color::ColorEnum;
use num_enum::FromPrimitive;

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive)]
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

impl PieceType {
    pub fn is_promotable(self) -> bool {
        self < Self::Ki
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
        PieceType::from((self as u8) % 10)
    }
}

#[cfg(test)]
mod tests {
    mod test_piece_type {
        use super::super::*;

        #[test]
        fn test_from_primitive() {
            assert_eq!(PieceType::from(0u8), PieceType::Fu);
            assert_eq!(PieceType::from(1u8), PieceType::Gi);
            assert_eq!(PieceType::from(2u8), PieceType::Ka);
            assert_eq!(PieceType::from(3u8), PieceType::Hi);
            assert_eq!(PieceType::from(4u8), PieceType::Ki);
            assert_eq!(PieceType::from(5u8), PieceType::Ou);
            assert_eq!(PieceType::from(6u8), PieceType::To);
            assert_eq!(PieceType::from(7u8), PieceType::Ng);
            assert_eq!(PieceType::from(8u8), PieceType::Um);
            assert_eq!(PieceType::from(9u8), PieceType::Ry);
            assert_eq!(PieceType::from(10u8), PieceType::Na);
            assert_eq!(PieceType::from(u8::MAX), PieceType::Na);
        }

        #[test]
        fn test_is_promotable_true() {
            assert_eq!(PieceType::Fu.is_promotable(), true);
        }

        #[test]
        fn test_is_promotable_false() {
            assert_eq!(PieceType::Ki.is_promotable(), false);
        }
    }

    mod test_piece {
        use super::super::*;

        #[test]
        fn test_new() {
            assert_eq!(Piece::new(ColorEnum::Black, PieceType::Fu), Piece::BlFu);
            assert_eq!(Piece::new(ColorEnum::Black, PieceType::Ry), Piece::BlRy);
            assert_eq!(Piece::new(ColorEnum::White, PieceType::Fu), Piece::WhFu);
            assert_eq!(Piece::new(ColorEnum::White, PieceType::Ry), Piece::WhRy);
        }

        #[test]
        fn test_new_invalid_inputs() {
            assert_eq!(Piece::new(ColorEnum::None, PieceType::Fu), Piece::Void);
            assert_eq!(Piece::new(ColorEnum::Black, PieceType::Na), Piece::Void);
            assert_eq!(Piece::new(ColorEnum::White, PieceType::Na), Piece::Void);
        }

        #[test]
        fn test_from_primitive() {
            assert_eq!(Piece::from(0u8), Piece::BlFu);
            assert_eq!(Piece::from(9u8), Piece::BlRy);
            assert_eq!(Piece::from(10u8), Piece::WhFu);
            assert_eq!(Piece::from(19u8), Piece::WhRy);
            assert_eq!(Piece::from(20u8), Piece::Void);
            assert_eq!(Piece::from(u8::MAX), Piece::Void);
        }

        #[test]
        fn test_get_color() {
            assert_eq!(Piece::BlFu.get_color(), ColorEnum::Black);
            assert_eq!(Piece::BlOu.get_color(), ColorEnum::Black);
            assert_eq!(Piece::BlRy.get_color(), ColorEnum::Black);
            assert_eq!(Piece::WhFu.get_color(), ColorEnum::White);
            assert_eq!(Piece::WhOu.get_color(), ColorEnum::White);
            assert_eq!(Piece::WhRy.get_color(), ColorEnum::White);
        }

        #[test]
        fn test_to_piece_type() {
            assert_eq!(Piece::BlFu.to_piece_type(), PieceType::Fu);
            assert_eq!(Piece::BlNg.to_piece_type(), PieceType::Ng);
            assert_eq!(Piece::WhFu.to_piece_type(), PieceType::Fu);
            assert_eq!(Piece::WhOu.to_piece_type(), PieceType::Ou);
        }
    }
}
