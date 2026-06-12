use crate::common::direction::Direction;
use crate::common::parameters::BaseParameters;

pub trait BasePieceType<P: BaseParameters>: Copy + Into<u8> + From<u8> {
    const PROMOTE_MAX_FROM_STAND: u8 = P::NUM_STAND_PIECE_TYPES - 1u8;
    const DEMOTE_OFFSET: u8 = P::NUM_STAND_PIECE_TYPES + 1u8;

    fn is_promotable(self) -> bool {
        (self.into() + 1) < P::NUM_STAND_PIECE_TYPES
    }
    fn is_promoted(self) -> bool {
        self.into() > P::NUM_STAND_PIECE_TYPES
    }
    fn is_promotion_always_better(self) -> bool;
    fn is_promotion_fully_superior(self) -> bool {
        let s: u8 = self.into();
        if (s as usize) < (P::NUM_PIECE_TYPES as usize) {
            P::PIECE_TYPES[s as usize].is_promotion_fully_superior()
        } else {
            false
        }
    }
    fn promote(self) -> Self {
        let s: u8 = self.into();
        if s < Self::PROMOTE_MAX_FROM_STAND {
            Self::from(s + Self::DEMOTE_OFFSET)
        } else {
            Self::from(P::NUM_PIECE_TYPES)
        }
    }
    fn demote(self) -> Self {
        let s: u8 = self.into();
        if Self::DEMOTE_OFFSET <= s && s < P::NUM_PIECE_TYPES {
            Self::from(s - Self::DEMOTE_OFFSET)
        } else {
            Self::from(P::NUM_PIECE_TYPES)
        }
    }
    fn is_slider(self) -> bool;
    fn is_attacking_to(self, dir: Direction) -> bool;
}
