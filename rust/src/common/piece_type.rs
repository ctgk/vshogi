use crate::common::parameters::BaseParameters;

pub trait BasePieceType<P: BaseParameters>: Into<u8> + From<u8> {
    fn is_promotable(self) -> bool {
        (self.into() + 1) < P::NUM_STAND_PIECE_TYPES
    }
    fn is_promoted(self) -> bool {
        self.into() > P::NUM_STAND_PIECE_TYPES
    }
    fn is_promotion_always_better(self) -> bool;
    fn promote(self) -> Self;
    fn demote(self) -> Self;
    fn is_slider(self) -> bool;
}
