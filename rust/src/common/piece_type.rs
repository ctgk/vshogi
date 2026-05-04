use crate::common::parameters::BaseParameters;

pub trait BasePieceType<P: BaseParameters> {
    fn is_promotable(self) -> bool
    where
        Self: Into<u8>,
    {
        (self.into() + 1) < P::NUM_STAND_PIECE_TYPES
    }
    fn is_promoted(self) -> bool
    where
        Self: Into<u8>,
    {
        self.into() > P::NUM_STAND_PIECE_TYPES
    }
    fn is_promotion_always_better(self) -> bool;
    fn promote(self) -> Self;
    fn demote(self) -> Self;
    fn is_slider(self) -> bool;
}
