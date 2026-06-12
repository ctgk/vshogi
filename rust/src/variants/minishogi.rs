use crate::common::direction::Direction;
use crate::common::full_piece_type::FullPieceTypes;
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
    const PIECE_TYPES: &'static [FullPieceTypes] = &[
        FullPieceTypes::Fu,
        FullPieceTypes::Gi,
        FullPieceTypes::Ka,
        FullPieceTypes::Hi,
        FullPieceTypes::Ki,
        FullPieceTypes::Ou,
        FullPieceTypes::To,
        FullPieceTypes::Ng,
        FullPieceTypes::Um,
        FullPieceTypes::Ry,
        FullPieceTypes::Na,
    ];
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

impl BasePieceType<Parameters> for PieceType {}

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
