use crate::common::color::ColorEnum;
use crate::common::parameters::BaseParameters;
use crate::common::piece_type::BasePieceType;

pub trait BasePiece<P: BaseParameters>: Into<u8> + From<u8> {
    fn new(c: ColorEnum, pt: impl BasePieceType<P>) -> Self {
        let pt = pt.into();
        if (c == ColorEnum::None) || (pt == P::NUM_PIECE_TYPES) {
            return Self::from(2u8 * P::NUM_PIECE_TYPES);
        }
        Self::from(c as u8 * P::NUM_PIECE_TYPES + pt)
    }
}
