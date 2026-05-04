#[cfg(test)]
mod test_piece_type {
    use crate::common::direction::Direction;
    use crate::common::piece_type::BasePieceType;
    use crate::variants::minishogi::PieceType;

    #[test]
    fn test_from_primitive() {
        assert_eq!(PieceType::from(0u8), PieceType::Fu);
        assert_eq!(PieceType::from(1u8), PieceType::Gi);
        assert_eq!(PieceType::from(2u8), PieceType::Ka);
        assert_eq!(PieceType::from(3u8), PieceType::Hi);
        assert_eq!(PieceType::from(4u8), PieceType::Ki);
        assert_eq!(PieceType::from(5u8), PieceType::Ou);
        assert_eq!(PieceType::from(6u8), PieceType::To);
        assert_eq!(PieceType::from(7u8), PieceType::Ng);
        assert_eq!(PieceType::from(8u8), PieceType::Um);
        assert_eq!(PieceType::from(9u8), PieceType::Ry);
        assert_eq!(PieceType::from(10u8), PieceType::Na);
        assert_eq!(PieceType::from(u8::MAX), PieceType::Na);
    }

    #[test]
    fn test_is_promotable() {
        assert_eq!(PieceType::Fu.is_promotable(), true);
        assert_eq!(PieceType::Ki.is_promotable(), false);
    }

    #[test]
    fn test_is_promoted() {
        assert_eq!(PieceType::Fu.is_promoted(), false);
        assert_eq!(PieceType::Ou.is_promoted(), false);
        assert_eq!(PieceType::To.is_promoted(), true);
        assert_eq!(PieceType::Um.is_promoted(), true);
    }

    #[test]
    fn test_is_promotion_always_better() {
        assert_eq!(PieceType::Fu.is_promotion_always_better(), true);
        assert_eq!(PieceType::Gi.is_promotion_always_better(), false);
        assert_eq!(PieceType::Ka.is_promotion_always_better(), true);
        assert_eq!(PieceType::Hi.is_promotion_always_better(), true);
        assert_eq!(PieceType::Ki.is_promotion_always_better(), false);
        assert_eq!(PieceType::Ou.is_promotion_always_better(), false);
        assert_eq!(PieceType::To.is_promotion_always_better(), false);
        assert_eq!(PieceType::Ng.is_promotion_always_better(), false);
        assert_eq!(PieceType::Um.is_promotion_always_better(), false);
        assert_eq!(PieceType::Ry.is_promotion_always_better(), false);
        assert_eq!(PieceType::Na.is_promotion_always_better(), false);
    }

    #[test]
    fn test_promote() {
        assert_eq!(PieceType::Fu.promote(), PieceType::To);
        assert_eq!(PieceType::Gi.promote(), PieceType::Ng);
        assert_eq!(PieceType::Ka.promote(), PieceType::Um);
        assert_eq!(PieceType::Hi.promote(), PieceType::Ry);
        assert_eq!(PieceType::Ki.promote(), PieceType::Na);
        assert_eq!(PieceType::Ou.promote(), PieceType::Na);
        assert_eq!(PieceType::To.promote(), PieceType::Na);
        assert_eq!(PieceType::Ng.promote(), PieceType::Na);
        assert_eq!(PieceType::Um.promote(), PieceType::Na);
        assert_eq!(PieceType::Ry.promote(), PieceType::Na);
        assert_eq!(PieceType::Na.promote(), PieceType::Na);
    }

    #[test]
    fn test_demote() {
        assert_eq!(PieceType::Fu.demote(), PieceType::Na);
        assert_eq!(PieceType::Gi.demote(), PieceType::Na);
        assert_eq!(PieceType::Ka.demote(), PieceType::Na);
        assert_eq!(PieceType::Hi.demote(), PieceType::Na);
        assert_eq!(PieceType::Ki.demote(), PieceType::Na);
        assert_eq!(PieceType::Ou.demote(), PieceType::Na);
        assert_eq!(PieceType::To.demote(), PieceType::Fu);
        assert_eq!(PieceType::Ng.demote(), PieceType::Gi);
        assert_eq!(PieceType::Um.demote(), PieceType::Ka);
        assert_eq!(PieceType::Ry.demote(), PieceType::Hi);
        assert_eq!(PieceType::Na.demote(), PieceType::Na);
    }

    #[test]
    fn test_is_slider() {
        assert_eq!(PieceType::Fu.is_slider(), false);
        assert_eq!(PieceType::Gi.is_slider(), false);
        assert_eq!(PieceType::Ka.is_slider(), true);
        assert_eq!(PieceType::Hi.is_slider(), true);
        assert_eq!(PieceType::Ki.is_slider(), false);
        assert_eq!(PieceType::Ou.is_slider(), false);
        assert_eq!(PieceType::To.is_slider(), false);
        assert_eq!(PieceType::Ng.is_slider(), false);
        assert_eq!(PieceType::Um.is_slider(), true);
        assert_eq!(PieceType::Ry.is_slider(), true);
        assert_eq!(PieceType::Na.is_slider(), false);
    }

    #[test]
    fn test_is_attacking_to() {
        assert_eq!(PieceType::Fu.is_attacking_to(Direction::NWe), false);
        assert_eq!(PieceType::Fu.is_attacking_to(Direction::Nth), true);
        assert_eq!(PieceType::Fu.is_attacking_to(Direction::NEa), false);
        assert_eq!(PieceType::Fu.is_attacking_to(Direction::Wst), false);
        assert_eq!(PieceType::Fu.is_attacking_to(Direction::Est), false);
        assert_eq!(PieceType::Fu.is_attacking_to(Direction::SWe), false);
        assert_eq!(PieceType::Fu.is_attacking_to(Direction::Sth), false);
        assert_eq!(PieceType::Fu.is_attacking_to(Direction::SEa), false);

        assert_eq!(PieceType::Gi.is_attacking_to(Direction::NWe), true);
        assert_eq!(PieceType::Gi.is_attacking_to(Direction::Nth), true);
        assert_eq!(PieceType::Gi.is_attacking_to(Direction::NEa), true);
        assert_eq!(PieceType::Gi.is_attacking_to(Direction::Wst), false);
        assert_eq!(PieceType::Gi.is_attacking_to(Direction::Est), false);
        assert_eq!(PieceType::Gi.is_attacking_to(Direction::SWe), true);
        assert_eq!(PieceType::Gi.is_attacking_to(Direction::Sth), false);
        assert_eq!(PieceType::Gi.is_attacking_to(Direction::SEa), true);

        assert_eq!(PieceType::Ka.is_attacking_to(Direction::NWe), true);
        assert_eq!(PieceType::Ka.is_attacking_to(Direction::Nth), false);
        assert_eq!(PieceType::Ka.is_attacking_to(Direction::NEa), true);
        assert_eq!(PieceType::Ka.is_attacking_to(Direction::Wst), false);
        assert_eq!(PieceType::Ka.is_attacking_to(Direction::Est), false);
        assert_eq!(PieceType::Ka.is_attacking_to(Direction::SWe), true);
        assert_eq!(PieceType::Ka.is_attacking_to(Direction::Sth), false);
        assert_eq!(PieceType::Ka.is_attacking_to(Direction::SEa), true);

        assert_eq!(PieceType::Hi.is_attacking_to(Direction::NWe), false);
        assert_eq!(PieceType::Hi.is_attacking_to(Direction::Nth), true);
        assert_eq!(PieceType::Hi.is_attacking_to(Direction::NEa), false);
        assert_eq!(PieceType::Hi.is_attacking_to(Direction::Wst), true);
        assert_eq!(PieceType::Hi.is_attacking_to(Direction::Est), true);
        assert_eq!(PieceType::Hi.is_attacking_to(Direction::SWe), false);
        assert_eq!(PieceType::Hi.is_attacking_to(Direction::Sth), true);
        assert_eq!(PieceType::Hi.is_attacking_to(Direction::SEa), false);

        for pt in [PieceType::Ki, PieceType::To, PieceType::Ng] {
            assert_eq!(pt.is_attacking_to(Direction::NWe), true);
            assert_eq!(pt.is_attacking_to(Direction::Nth), true);
            assert_eq!(pt.is_attacking_to(Direction::NEa), true);
            assert_eq!(pt.is_attacking_to(Direction::Wst), true);
            assert_eq!(pt.is_attacking_to(Direction::Est), true);
            assert_eq!(pt.is_attacking_to(Direction::SWe), false);
            assert_eq!(pt.is_attacking_to(Direction::Sth), true);
            assert_eq!(pt.is_attacking_to(Direction::SEa), false);
        }

        for pt in [PieceType::Ou, PieceType::Um, PieceType::Ry] {
            assert_eq!(pt.is_attacking_to(Direction::NWe), true);
            assert_eq!(pt.is_attacking_to(Direction::Nth), true);
            assert_eq!(pt.is_attacking_to(Direction::NEa), true);
            assert_eq!(pt.is_attacking_to(Direction::Wst), true);
            assert_eq!(pt.is_attacking_to(Direction::Est), true);
            assert_eq!(pt.is_attacking_to(Direction::SWe), true);
            assert_eq!(pt.is_attacking_to(Direction::Sth), true);
            assert_eq!(pt.is_attacking_to(Direction::SEa), true);
        }
    }
}

mod test_piece {
    use crate::common::color::ColorEnum;
    use crate::common::piece::BasePiece;
    use crate::variants::minishogi::{Piece, PieceType};

    #[test]
    fn test_new() {
        assert_eq!(Piece::new(ColorEnum::Black, PieceType::Fu), Piece::BlFu);
        assert_eq!(Piece::new(ColorEnum::Black, PieceType::Ry), Piece::BlRy);
        assert_eq!(Piece::new(ColorEnum::White, PieceType::Fu), Piece::WhFu);
        assert_eq!(Piece::new(ColorEnum::White, PieceType::Ry), Piece::WhRy);
    }

    #[test]
    fn test_new_invalid_inputs() {
        assert_eq!(Piece::new(ColorEnum::None, PieceType::Fu), Piece::Void);
        assert_eq!(Piece::new(ColorEnum::Black, PieceType::Na), Piece::Void);
        assert_eq!(Piece::new(ColorEnum::White, PieceType::Na), Piece::Void);
    }

    #[test]
    fn test_from_primitive() {
        assert_eq!(Piece::from(0u8), Piece::BlFu);
        assert_eq!(Piece::from(9u8), Piece::BlRy);
        assert_eq!(Piece::from(10u8), Piece::WhFu);
        assert_eq!(Piece::from(19u8), Piece::WhRy);
        assert_eq!(Piece::from(20u8), Piece::Void);
        assert_eq!(Piece::from(u8::MAX), Piece::Void);
    }

    #[test]
    fn test_get_color() {
        assert_eq!(Piece::BlFu.get_color(), ColorEnum::Black);
        assert_eq!(Piece::BlOu.get_color(), ColorEnum::Black);
        assert_eq!(Piece::BlRy.get_color(), ColorEnum::Black);
        assert_eq!(Piece::WhFu.get_color(), ColorEnum::White);
        assert_eq!(Piece::WhOu.get_color(), ColorEnum::White);
        assert_eq!(Piece::WhRy.get_color(), ColorEnum::White);
    }

    #[test]
    fn test_to_piece_type() {
        assert_eq!(Piece::BlFu.to_piece_type(), PieceType::Fu);
        assert_eq!(Piece::BlNg.to_piece_type(), PieceType::Ng);
        assert_eq!(Piece::WhFu.to_piece_type(), PieceType::Fu);
        assert_eq!(Piece::WhOu.to_piece_type(), PieceType::Ou);
        assert_eq!(Piece::Void.to_piece_type(), PieceType::Na);
    }

    #[test]
    fn test_is_promotable() {
        assert_eq!(Piece::BlFu.is_promotable(), true);
        assert_eq!(Piece::WhKi.is_promotable(), false);
    }

    #[test]
    fn test_is_promoted() {
        assert_eq!(Piece::WhKi.is_promoted(), false);
        assert_eq!(Piece::BlHi.is_promoted(), false);
        assert_eq!(Piece::WhNg.is_promoted(), true);
        assert_eq!(Piece::BlRy.is_promoted(), true);
    }

    #[test]
    fn test_is_promotion_always_better() {
        assert_eq!(Piece::BlFu.is_promotion_always_better(), true);
        assert_eq!(Piece::BlGi.is_promotion_always_better(), false);
        assert_eq!(Piece::BlKa.is_promotion_always_better(), true);
        assert_eq!(Piece::BlHi.is_promotion_always_better(), true);
        assert_eq!(Piece::BlKi.is_promotion_always_better(), false);
        assert_eq!(Piece::BlOu.is_promotion_always_better(), false);
        assert_eq!(Piece::BlTo.is_promotion_always_better(), false);
        assert_eq!(Piece::BlNg.is_promotion_always_better(), false);
        assert_eq!(Piece::BlUm.is_promotion_always_better(), false);
        assert_eq!(Piece::BlRy.is_promotion_always_better(), false);
        assert_eq!(Piece::WhFu.is_promotion_always_better(), true);
        assert_eq!(Piece::WhGi.is_promotion_always_better(), false);
        assert_eq!(Piece::WhKa.is_promotion_always_better(), true);
        assert_eq!(Piece::WhHi.is_promotion_always_better(), true);
        assert_eq!(Piece::WhKi.is_promotion_always_better(), false);
        assert_eq!(Piece::WhOu.is_promotion_always_better(), false);
        assert_eq!(Piece::WhTo.is_promotion_always_better(), false);
        assert_eq!(Piece::WhNg.is_promotion_always_better(), false);
        assert_eq!(Piece::WhUm.is_promotion_always_better(), false);
        assert_eq!(Piece::WhRy.is_promotion_always_better(), false);
        assert_eq!(Piece::Void.is_promotion_always_better(), false);
    }

    #[test]
    fn test_promote() {
        assert_eq!(Piece::BlFu.promote(), Piece::BlTo);
        assert_eq!(Piece::BlGi.promote(), Piece::BlNg);
        assert_eq!(Piece::BlKa.promote(), Piece::BlUm);
        assert_eq!(Piece::BlHi.promote(), Piece::BlRy);
        assert_eq!(Piece::BlKi.promote(), Piece::Void);
        assert_eq!(Piece::BlOu.promote(), Piece::Void);
        assert_eq!(Piece::BlTo.promote(), Piece::Void);
        assert_eq!(Piece::BlNg.promote(), Piece::Void);
        assert_eq!(Piece::BlUm.promote(), Piece::Void);
        assert_eq!(Piece::BlRy.promote(), Piece::Void);
        assert_eq!(Piece::WhFu.promote(), Piece::WhTo);
        assert_eq!(Piece::WhGi.promote(), Piece::WhNg);
        assert_eq!(Piece::WhKa.promote(), Piece::WhUm);
        assert_eq!(Piece::WhHi.promote(), Piece::WhRy);
        assert_eq!(Piece::WhKi.promote(), Piece::Void);
        assert_eq!(Piece::WhOu.promote(), Piece::Void);
        assert_eq!(Piece::WhTo.promote(), Piece::Void);
        assert_eq!(Piece::WhNg.promote(), Piece::Void);
        assert_eq!(Piece::WhUm.promote(), Piece::Void);
        assert_eq!(Piece::WhRy.promote(), Piece::Void);
        assert_eq!(Piece::Void.promote(), Piece::Void);
    }

    #[test]
    fn test_demote() {
        assert_eq!(Piece::BlFu.demote(), Piece::Void);
        assert_eq!(Piece::BlGi.demote(), Piece::Void);
        assert_eq!(Piece::BlKa.demote(), Piece::Void);
        assert_eq!(Piece::BlHi.demote(), Piece::Void);
        assert_eq!(Piece::BlKi.demote(), Piece::Void);
        assert_eq!(Piece::BlOu.demote(), Piece::Void);
        assert_eq!(Piece::BlTo.demote(), Piece::BlFu);
        assert_eq!(Piece::BlNg.demote(), Piece::BlGi);
        assert_eq!(Piece::BlUm.demote(), Piece::BlKa);
        assert_eq!(Piece::BlRy.demote(), Piece::BlHi);
        assert_eq!(Piece::WhFu.demote(), Piece::Void);
        assert_eq!(Piece::WhGi.demote(), Piece::Void);
        assert_eq!(Piece::WhKa.demote(), Piece::Void);
        assert_eq!(Piece::WhHi.demote(), Piece::Void);
        assert_eq!(Piece::WhKi.demote(), Piece::Void);
        assert_eq!(Piece::WhOu.demote(), Piece::Void);
        assert_eq!(Piece::WhTo.demote(), Piece::WhFu);
        assert_eq!(Piece::WhNg.demote(), Piece::WhGi);
        assert_eq!(Piece::WhUm.demote(), Piece::WhKa);
        assert_eq!(Piece::WhRy.demote(), Piece::WhHi);
        assert_eq!(Piece::Void.demote(), Piece::Void);
    }

    #[test]
    fn test_is_slider() {
        assert_eq!(Piece::BlFu.is_slider(), false);
        assert_eq!(Piece::BlGi.is_slider(), false);
        assert_eq!(Piece::BlKa.is_slider(), true);
        assert_eq!(Piece::BlHi.is_slider(), true);
        assert_eq!(Piece::BlKi.is_slider(), false);
        assert_eq!(Piece::BlOu.is_slider(), false);
        assert_eq!(Piece::BlTo.is_slider(), false);
        assert_eq!(Piece::BlNg.is_slider(), false);
        assert_eq!(Piece::BlUm.is_slider(), true);
        assert_eq!(Piece::BlRy.is_slider(), true);
        assert_eq!(Piece::WhFu.is_slider(), false);
        assert_eq!(Piece::WhGi.is_slider(), false);
        assert_eq!(Piece::WhKa.is_slider(), true);
        assert_eq!(Piece::WhHi.is_slider(), true);
        assert_eq!(Piece::WhKi.is_slider(), false);
        assert_eq!(Piece::WhOu.is_slider(), false);
        assert_eq!(Piece::WhTo.is_slider(), false);
        assert_eq!(Piece::WhNg.is_slider(), false);
        assert_eq!(Piece::WhUm.is_slider(), true);
        assert_eq!(Piece::WhRy.is_slider(), true);
        assert_eq!(Piece::Void.is_slider(), false);
    }
}
