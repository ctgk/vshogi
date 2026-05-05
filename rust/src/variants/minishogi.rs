use crate::common::direction::Direction;
use crate::common::parameters::BaseParameters;
use crate::common::piece::BasePiece;
use crate::common::piece_type::BasePieceType;
use crate::common::square::{BaseFile, BaseRank, BaseSquare};
use num_enum::FromPrimitive;
use num_enum::IntoPrimitive;

pub struct Parameters;

impl BaseParameters for Parameters {
    const NUM_PIECE_TYPES: u8 = 10;
    const NUM_STAND_PIECE_TYPES: u8 = 5;
    const NUM_FILES: u8 = 5;
    const NUM_RANKS: u8 = 5;
    const NUM_PROMOTION_RANKS: u8 = 1;
}

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

impl BasePieceType<Parameters> for PieceType {
    fn is_promotion_always_better(self) -> bool {
        matches!(self, Self::Fu | Self::Ka | Self::Hi)
    }
    fn is_slider(self) -> bool {
        matches!(self, Self::Ka | Self::Hi | Self::Um | Self::Ry)
    }
    fn is_attacking_to(self, dir: Direction) -> bool {
        #[rustfmt::skip]
        const TABLE: [[bool; 9]; 11] = [
            // NWe,   Nth,   NEa,   Wst,          Est,   SWe,   Sth,   SEa
            [false,  true, false, false, false, false, false, false, false],  // Fu
            [ true,  true,  true, false, false, false,  true, false,  true],  // Gi
            [ true, false,  true, false, false, false,  true, false,  true],  // Ka
            [false,  true, false,  true, false,  true, false,  true, false],  // Hi
            [ true,  true,  true,  true, false,  true, false,  true, false],  // Ki
            [ true,  true,  true,  true, false,  true,  true,  true,  true],  // Ou
            [ true,  true,  true,  true, false,  true, false,  true, false],  // To
            [ true,  true,  true,  true, false,  true, false,  true, false],  // Ng
            [ true,  true,  true,  true, false,  true,  true,  true,  true],  // Um
            [ true,  true,  true,  true, false,  true,  true,  true,  true],  // Ry
            [false, false, false, false, false, false, false, false, false],  // Na
        ];
        TABLE[self as usize][(dir as i8 + 4) as usize]
    }
}

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive, IntoPrimitive)]
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

impl BasePiece<Parameters> for Piece {
    type PieceType = PieceType;
}

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive, IntoPrimitive)]
pub enum File {
    F1 = 0,
    F2,
    F3,
    F4,
    F5,
    #[num_enum(default)]
    F0,
}

impl BaseFile<Parameters> for File {}

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive, IntoPrimitive)]
pub enum Rank {
    RA = 0,
    RB,
    RC,
    RD,
    RE,
    #[num_enum(default)]
    R0,
}

impl BaseRank<Parameters> for Rank {}

#[rustfmt::skip]
#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive, IntoPrimitive)]
pub enum Square {
    Sq5A = 20, Sq4A = 15, Sq3A = 10, Sq2A = 5, Sq1A = 0,
    Sq5B = 21, Sq4B = 16, Sq3B = 11, Sq2B = 6, Sq1B = 1,
    Sq5C = 22, Sq4C = 17, Sq3C = 12, Sq2C = 7, Sq1C = 2,
    Sq5D = 23, Sq4D = 18, Sq3D = 13, Sq2D = 8, Sq1D = 3,
    Sq5E = 24, Sq4E = 19, Sq3E = 14, Sq2E = 9, Sq1E = 4,
    #[num_enum(default)]
    SqNa = 25,
}

impl BaseSquare<Parameters> for Square {
    type File = File;
    type Rank = Rank;
}
