use crate::common::color::Color;
use crate::common::direction::Direction;
use crate::common::parameters::BaseParameters;
use crate::common::piece_type::BasePieceType;

pub trait BasePiece<P: BaseParameters>: Into<u8> + From<u8> {
    type PieceType: BasePieceType<P>;

    fn new(c: Color, pt: impl BasePieceType<P>) -> Self {
        let pt = pt.into();
        if (c == Color::None) || (pt == P::NUM_PIECE_TYPES) {
            return Self::from(2u8 * P::NUM_PIECE_TYPES);
        }
        Self::from(c as u8 * P::NUM_PIECE_TYPES + pt)
    }
    fn get_color(self) -> Color {
        let s: u8 = self.into();
        if s < P::NUM_PIECE_TYPES {
            return Color::Black;
        } else if s < 2u8 * P::NUM_PIECE_TYPES {
            return Color::White;
        }
        Color::None
    }
    fn to_piece_type(self) -> Self::PieceType {
        let s: u8 = self.into();
        if s < P::NUM_PIECE_TYPES {
            return Self::PieceType::from(s);
        }
        Self::PieceType::from(s - P::NUM_PIECE_TYPES)
    }
    fn is_promotable(self) -> bool {
        self.to_piece_type().is_promotable()
    }
    fn is_promoted(self) -> bool {
        self.to_piece_type().is_promoted()
    }
    fn is_promotion_always_better(self) -> bool {
        self.to_piece_type().is_promotion_always_better()
    }
    fn promote(self) -> Self {
        let s: u8 = self.into();
        if (s % P::NUM_PIECE_TYPES) < (P::NUM_STAND_PIECE_TYPES - 1u8) {
            return Self::from(s + P::NUM_STAND_PIECE_TYPES + 1u8);
        }
        Self::from(2u8 * P::NUM_PIECE_TYPES)
    }
    fn demote(self) -> Self {
        let s: u8 = self.into();
        if (s % P::NUM_PIECE_TYPES) > P::NUM_STAND_PIECE_TYPES {
            return Self::from(s - P::NUM_STAND_PIECE_TYPES - 1u8);
        }
        Self::from(2u8 * P::NUM_PIECE_TYPES)
    }
    fn is_slider(self) -> bool {
        self.to_piece_type().is_slider()
    }
    fn is_attacking_to(self, dir: Direction) -> bool {
        let s: u8 = self.into();
        let c: Color = Self::from(s).get_color();
        let pt = Self::from(s).to_piece_type();
        pt.is_attacking_to(if c == Color::Black { dir } else { dir.rotate() })
    }
}
