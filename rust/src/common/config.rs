use crate::common::parameters::BaseParameters;
use core::marker::PhantomData;

/// Rust translation of the C++ `template <class ParamS> struct Configuration`.
///
/// This is a lightweight, idiomatic mapping that exposes the same kind of
/// compile-time constants derived from the parameters trait.
pub struct Configuration<P: BaseParameters> {
    _p: PhantomData<P>,
}

impl<P: BaseParameters> Configuration<P> {
    pub const NUM_PIECE_TYPES: usize = P::NUM_PIECE_TYPES as usize;
    pub const NUM_COLORED_PIECE_TYPES: usize = 2 * Self::NUM_PIECE_TYPES;
    pub const NUM_STAND_PIECE_TYPES: usize = P::NUM_STAND_PIECE_TYPES as usize;

    pub const NUM_FILES: usize = P::NUM_FILES as usize;
    pub const NUM_RANKS: usize = P::NUM_RANKS as usize;
    pub const BOARD_LENGTH: usize = if Self::NUM_FILES > Self::NUM_RANKS {
        Self::NUM_FILES
    } else {
        Self::NUM_RANKS
    };
    pub const NUM_SQUARES: usize = Self::NUM_FILES * Self::NUM_RANKS;
    pub const NUM_PROMOTION_RANKS: usize = P::NUM_PROMOTION_RANKS as usize;

    /// Number of directions. Heuristic: if there are >= 11 piece types,
    /// assume the knight exists and use 12 directions, otherwise 8.
    pub const NUM_DIR: usize = if P::NUM_PIECE_TYPES >= 11 { 12 } else { 8 };
    pub const NUM_DIR_DL: usize = if Self::NUM_DIR > 8 {
        Self::NUM_DIR - 2
    } else {
        Self::NUM_DIR
    };

    pub const SQ_NA: usize = Self::NUM_SQUARES;
    pub const RANK_A: usize = 0;
    pub const RANK_Z: usize = Self::NUM_RANKS - 1;

    /// Placeholder values: these require per-variant arrays (initial positions,
    /// piece value tables, etc.) which live with the variant implementations.
    pub const MAX_STAND_PIECE_COUNT: usize = 0;
    pub const MAX_STAND_SFEN_LENGTH: usize = 0;

    /// Iterators over the enum indices used by the rest of the code.
    pub fn direction_iterator() -> core::ops::Range<u8> {
        0..(Self::NUM_DIR as u8)
    }
    pub fn square_iterator() -> core::ops::Range<u32> {
        0..(Self::NUM_SQUARES as u32)
    }
    pub fn piece_type_iterator() -> core::ops::Range<u8> {
        0..(Self::NUM_PIECE_TYPES as u8)
    }
    pub fn stand_piece_type_iterator() -> core::ops::Range<u8> {
        0..(Self::NUM_STAND_PIECE_TYPES as u8)
    }
}

impl<P: BaseParameters> Default for Configuration<P> {
    fn default() -> Self {
        Self { _p: PhantomData }
    }
}

#[cfg(test)]
mod tests {
    use crate::common::config::Configuration;
    use crate::variants::minishogi::Parameters;

    #[test]
    fn test_minishogi_configuration_constants() {
        type C = Configuration<Parameters>;
        assert_eq!(C::NUM_FILES, 5);
        assert_eq!(C::NUM_RANKS, 5);
        assert_eq!(C::BOARD_LENGTH, 5);
        assert_eq!(C::NUM_PIECE_TYPES, 10);
        assert_eq!(C::NUM_SQUARES, 25);
        assert_eq!(C::NUM_DIR, 8);
        assert_eq!(C::NUM_DIR_DL, 8);
        assert_eq!(C::SQ_NA, 25);
        assert_eq!(C::RANK_Z, 4);

        // Iterator lengths
        assert_eq!(
            C::piece_type_iterator().count(),
            C::NUM_PIECE_TYPES as usize
        );
        assert_eq!(C::square_iterator().count(), C::NUM_SQUARES as usize);
    }
}
