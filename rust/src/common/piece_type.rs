pub trait BasePieceType<const NUM_PIECE_TYPES: u8, const NUM_STAND_PIECE_TYPES: u8> {
    fn is_promotable(self) -> bool
    where
        Self: Into<u8>,
    {
        (self.into() + 1) < NUM_STAND_PIECE_TYPES
    }
    fn is_promoted(self) -> bool
    where
        Self: Into<u8>,
    {
        self.into() > NUM_STAND_PIECE_TYPES
    }
    fn is_promotion_always_better(self) -> bool;
    fn promote(self) -> Self;
    fn demote(self) -> Self;
    fn is_slider(self) -> bool;
}
