use crate::common::full_piece_type::FullPieceTypes;
use crate::common::square::{BaseFile, BaseRank};

pub trait BaseParameters: Sized {
    type File: BaseFile<Self>;
    type Rank: BaseRank<Self>;
    /// The piece-type enum used by this variant.
    type PieceType: crate::common::piece_type::BasePieceType<Self>;

    const NUM_PIECE_TYPES: u8;
    const NUM_STAND_PIECE_TYPES: u8;
    const NUM_FILES: u8;
    const NUM_RANKS: u8;
    const NUM_PROMOTION_RANKS: u8;

    /// Ordered list of full piece types for this variant. The last entry
    /// should be the `Na` / not-available sentinel.
    const PIECE_TYPES: &'static [FullPieceTypes];
}
