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
    use crate::common::color::Color;
    use crate::common::direction::Direction;
    use crate::common::piece::BasePiece;
    use crate::variants::minishogi::{Piece, PieceType};

    #[test]
    fn test_new() {
        assert_eq!(Piece::new(Color::Black, PieceType::Fu), Piece::BlFu);
        assert_eq!(Piece::new(Color::Black, PieceType::Ry), Piece::BlRy);
        assert_eq!(Piece::new(Color::White, PieceType::Fu), Piece::WhFu);
        assert_eq!(Piece::new(Color::White, PieceType::Ry), Piece::WhRy);
    }

    #[test]
    fn test_new_invalid_inputs() {
        assert_eq!(Piece::new(Color::None, PieceType::Fu), Piece::Void);
        assert_eq!(Piece::new(Color::Black, PieceType::Na), Piece::Void);
        assert_eq!(Piece::new(Color::White, PieceType::Na), Piece::Void);
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
        assert_eq!(Piece::BlFu.get_color(), Color::Black);
        assert_eq!(Piece::BlOu.get_color(), Color::Black);
        assert_eq!(Piece::BlRy.get_color(), Color::Black);
        assert_eq!(Piece::WhFu.get_color(), Color::White);
        assert_eq!(Piece::WhOu.get_color(), Color::White);
        assert_eq!(Piece::WhRy.get_color(), Color::White);
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

    #[test]
    fn test_is_attacking_to() {
        assert_eq!(Piece::BlFu.is_attacking_to(Direction::NWe), false);
        assert_eq!(Piece::BlFu.is_attacking_to(Direction::Nth), true);
        assert_eq!(Piece::BlFu.is_attacking_to(Direction::NEa), false);
        assert_eq!(Piece::BlFu.is_attacking_to(Direction::Wst), false);
        assert_eq!(Piece::BlFu.is_attacking_to(Direction::Est), false);
        assert_eq!(Piece::BlFu.is_attacking_to(Direction::SWe), false);
        assert_eq!(Piece::BlFu.is_attacking_to(Direction::Sth), false);
        assert_eq!(Piece::BlFu.is_attacking_to(Direction::SEa), false);

        assert_eq!(Piece::WhFu.is_attacking_to(Direction::NWe), false);
        assert_eq!(Piece::WhFu.is_attacking_to(Direction::Nth), false);
        assert_eq!(Piece::WhFu.is_attacking_to(Direction::NEa), false);
        assert_eq!(Piece::WhFu.is_attacking_to(Direction::Wst), false);
        assert_eq!(Piece::WhFu.is_attacking_to(Direction::Est), false);
        assert_eq!(Piece::WhFu.is_attacking_to(Direction::SWe), false);
        assert_eq!(Piece::WhFu.is_attacking_to(Direction::Sth), true);
        assert_eq!(Piece::WhFu.is_attacking_to(Direction::SEa), false);

        assert_eq!(Piece::BlGi.is_attacking_to(Direction::NWe), true);
        assert_eq!(Piece::BlGi.is_attacking_to(Direction::Nth), true);
        assert_eq!(Piece::BlGi.is_attacking_to(Direction::NEa), true);
        assert_eq!(Piece::BlGi.is_attacking_to(Direction::Wst), false);
        assert_eq!(Piece::BlGi.is_attacking_to(Direction::Est), false);
        assert_eq!(Piece::BlGi.is_attacking_to(Direction::SWe), true);
        assert_eq!(Piece::BlGi.is_attacking_to(Direction::Sth), false);
        assert_eq!(Piece::BlGi.is_attacking_to(Direction::SEa), true);

        assert_eq!(Piece::WhGi.is_attacking_to(Direction::NWe), true);
        assert_eq!(Piece::WhGi.is_attacking_to(Direction::Nth), false);
        assert_eq!(Piece::WhGi.is_attacking_to(Direction::NEa), true);
        assert_eq!(Piece::WhGi.is_attacking_to(Direction::Wst), false);
        assert_eq!(Piece::WhGi.is_attacking_to(Direction::Est), false);
        assert_eq!(Piece::WhGi.is_attacking_to(Direction::SWe), true);
        assert_eq!(Piece::WhGi.is_attacking_to(Direction::Sth), true);
        assert_eq!(Piece::WhGi.is_attacking_to(Direction::SEa), true);

        for p in [Piece::BlKa, Piece::WhKa] {
            assert_eq!(p.is_attacking_to(Direction::NWe), true);
            assert_eq!(p.is_attacking_to(Direction::Nth), false);
            assert_eq!(p.is_attacking_to(Direction::NEa), true);
            assert_eq!(p.is_attacking_to(Direction::Wst), false);
            assert_eq!(p.is_attacking_to(Direction::Est), false);
            assert_eq!(p.is_attacking_to(Direction::SWe), true);
            assert_eq!(p.is_attacking_to(Direction::Sth), false);
            assert_eq!(p.is_attacking_to(Direction::SEa), true);
        }

        for p in [Piece::BlHi, Piece::WhHi] {
            assert_eq!(p.is_attacking_to(Direction::NWe), false);
            assert_eq!(p.is_attacking_to(Direction::Nth), true);
            assert_eq!(p.is_attacking_to(Direction::NEa), false);
            assert_eq!(p.is_attacking_to(Direction::Wst), true);
            assert_eq!(p.is_attacking_to(Direction::Est), true);
            assert_eq!(p.is_attacking_to(Direction::SWe), false);
            assert_eq!(p.is_attacking_to(Direction::Sth), true);
            assert_eq!(p.is_attacking_to(Direction::SEa), false);
        }

        for pt in [Piece::BlKi, Piece::BlTo, Piece::BlNg] {
            assert_eq!(pt.is_attacking_to(Direction::NWe), true);
            assert_eq!(pt.is_attacking_to(Direction::Nth), true);
            assert_eq!(pt.is_attacking_to(Direction::NEa), true);
            assert_eq!(pt.is_attacking_to(Direction::Wst), true);
            assert_eq!(pt.is_attacking_to(Direction::Est), true);
            assert_eq!(pt.is_attacking_to(Direction::SWe), false);
            assert_eq!(pt.is_attacking_to(Direction::Sth), true);
            assert_eq!(pt.is_attacking_to(Direction::SEa), false);
        }

        for pt in [Piece::WhKi, Piece::WhTo, Piece::WhNg] {
            assert_eq!(pt.is_attacking_to(Direction::NWe), false);
            assert_eq!(pt.is_attacking_to(Direction::Nth), true);
            assert_eq!(pt.is_attacking_to(Direction::NEa), false);
            assert_eq!(pt.is_attacking_to(Direction::Wst), true);
            assert_eq!(pt.is_attacking_to(Direction::Est), true);
            assert_eq!(pt.is_attacking_to(Direction::SWe), true);
            assert_eq!(pt.is_attacking_to(Direction::Sth), true);
            assert_eq!(pt.is_attacking_to(Direction::SEa), true);
        }

        for pt in [
            Piece::BlOu,
            Piece::BlUm,
            Piece::BlRy,
            Piece::WhOu,
            Piece::WhUm,
            Piece::WhRy,
        ] {
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

mod test_square {
    use crate::common::color::Color;
    use crate::common::square::BaseSquare;
    use crate::variants::minishogi::{File, Rank, Square};
    #[test]
    fn test_new() {
        assert_eq!(Square::new(File::F1, Rank::RA), Square::Sq1A);
        assert_eq!(Square::new(File::F1, Rank::RB), Square::Sq1B);
        assert_eq!(Square::new(File::F1, Rank::RC), Square::Sq1C);
        assert_eq!(Square::new(File::F1, Rank::RD), Square::Sq1D);
        assert_eq!(Square::new(File::F1, Rank::RE), Square::Sq1E);
        assert_eq!(Square::new(File::F2, Rank::RA), Square::Sq2A);
        assert_eq!(Square::new(File::F2, Rank::RB), Square::Sq2B);
        assert_eq!(Square::new(File::F2, Rank::RC), Square::Sq2C);
        assert_eq!(Square::new(File::F2, Rank::RD), Square::Sq2D);
        assert_eq!(Square::new(File::F2, Rank::RE), Square::Sq2E);
        assert_eq!(Square::new(File::F3, Rank::RA), Square::Sq3A);
        assert_eq!(Square::new(File::F3, Rank::RB), Square::Sq3B);
        assert_eq!(Square::new(File::F3, Rank::RC), Square::Sq3C);
        assert_eq!(Square::new(File::F3, Rank::RD), Square::Sq3D);
        assert_eq!(Square::new(File::F3, Rank::RE), Square::Sq3E);
        assert_eq!(Square::new(File::F4, Rank::RA), Square::Sq4A);
        assert_eq!(Square::new(File::F4, Rank::RB), Square::Sq4B);
        assert_eq!(Square::new(File::F4, Rank::RC), Square::Sq4C);
        assert_eq!(Square::new(File::F4, Rank::RD), Square::Sq4D);
        assert_eq!(Square::new(File::F4, Rank::RE), Square::Sq4E);
        assert_eq!(Square::new(File::F5, Rank::RA), Square::Sq5A);
        assert_eq!(Square::new(File::F5, Rank::RB), Square::Sq5B);
        assert_eq!(Square::new(File::F5, Rank::RC), Square::Sq5C);
        assert_eq!(Square::new(File::F5, Rank::RD), Square::Sq5D);
        assert_eq!(Square::new(File::F5, Rank::RE), Square::Sq5E);
    }

    #[test]
    fn test_file() {
        assert_eq!(Square::Sq1A.file(), File::F1);
        assert_eq!(Square::Sq1B.file(), File::F1);
        assert_eq!(Square::Sq1C.file(), File::F1);
        assert_eq!(Square::Sq1D.file(), File::F1);
        assert_eq!(Square::Sq1E.file(), File::F1);
        assert_eq!(Square::Sq2A.file(), File::F2);
        assert_eq!(Square::Sq2B.file(), File::F2);
        assert_eq!(Square::Sq2C.file(), File::F2);
        assert_eq!(Square::Sq2D.file(), File::F2);
        assert_eq!(Square::Sq2E.file(), File::F2);
        assert_eq!(Square::Sq3A.file(), File::F3);
        assert_eq!(Square::Sq3B.file(), File::F3);
        assert_eq!(Square::Sq3C.file(), File::F3);
        assert_eq!(Square::Sq3D.file(), File::F3);
        assert_eq!(Square::Sq3E.file(), File::F3);
        assert_eq!(Square::Sq4A.file(), File::F4);
        assert_eq!(Square::Sq4B.file(), File::F4);
        assert_eq!(Square::Sq4C.file(), File::F4);
        assert_eq!(Square::Sq4D.file(), File::F4);
        assert_eq!(Square::Sq4E.file(), File::F4);
        assert_eq!(Square::Sq5A.file(), File::F5);
        assert_eq!(Square::Sq5B.file(), File::F5);
        assert_eq!(Square::Sq5C.file(), File::F5);
        assert_eq!(Square::Sq5D.file(), File::F5);
        assert_eq!(Square::Sq5E.file(), File::F5);
    }

    #[test]
    fn test_rank() {
        assert_eq!(Square::Sq1A.rank(), Rank::RA);
        assert_eq!(Square::Sq1B.rank(), Rank::RB);
        assert_eq!(Square::Sq1C.rank(), Rank::RC);
        assert_eq!(Square::Sq1D.rank(), Rank::RD);
        assert_eq!(Square::Sq1E.rank(), Rank::RE);
        assert_eq!(Square::Sq2A.rank(), Rank::RA);
        assert_eq!(Square::Sq2B.rank(), Rank::RB);
        assert_eq!(Square::Sq2C.rank(), Rank::RC);
        assert_eq!(Square::Sq2D.rank(), Rank::RD);
        assert_eq!(Square::Sq2E.rank(), Rank::RE);
        assert_eq!(Square::Sq3A.rank(), Rank::RA);
        assert_eq!(Square::Sq3B.rank(), Rank::RB);
        assert_eq!(Square::Sq3C.rank(), Rank::RC);
        assert_eq!(Square::Sq3D.rank(), Rank::RD);
        assert_eq!(Square::Sq3E.rank(), Rank::RE);
        assert_eq!(Square::Sq4A.rank(), Rank::RA);
        assert_eq!(Square::Sq4B.rank(), Rank::RB);
        assert_eq!(Square::Sq4C.rank(), Rank::RC);
        assert_eq!(Square::Sq4D.rank(), Rank::RD);
        assert_eq!(Square::Sq4E.rank(), Rank::RE);
        assert_eq!(Square::Sq5A.rank(), Rank::RA);
        assert_eq!(Square::Sq5B.rank(), Rank::RB);
        assert_eq!(Square::Sq5C.rank(), Rank::RC);
        assert_eq!(Square::Sq5D.rank(), Rank::RD);
        assert_eq!(Square::Sq5E.rank(), Rank::RE);
    }

    #[test]
    fn test_hflip() {
        assert_eq!(Square::Sq1A.hflip(), Square::Sq5A);
        assert_eq!(Square::Sq1B.hflip(), Square::Sq5B);
        assert_eq!(Square::Sq1C.hflip(), Square::Sq5C);
        assert_eq!(Square::Sq1D.hflip(), Square::Sq5D);
        assert_eq!(Square::Sq1E.hflip(), Square::Sq5E);
        assert_eq!(Square::Sq2A.hflip(), Square::Sq4A);
        assert_eq!(Square::Sq2B.hflip(), Square::Sq4B);
        assert_eq!(Square::Sq2C.hflip(), Square::Sq4C);
        assert_eq!(Square::Sq2D.hflip(), Square::Sq4D);
        assert_eq!(Square::Sq2E.hflip(), Square::Sq4E);
        assert_eq!(Square::Sq3A.hflip(), Square::Sq3A);
        assert_eq!(Square::Sq3B.hflip(), Square::Sq3B);
        assert_eq!(Square::Sq3C.hflip(), Square::Sq3C);
        assert_eq!(Square::Sq3D.hflip(), Square::Sq3D);
        assert_eq!(Square::Sq3E.hflip(), Square::Sq3E);
        assert_eq!(Square::Sq4A.hflip(), Square::Sq2A);
        assert_eq!(Square::Sq4B.hflip(), Square::Sq2B);
        assert_eq!(Square::Sq4C.hflip(), Square::Sq2C);
        assert_eq!(Square::Sq4D.hflip(), Square::Sq2D);
        assert_eq!(Square::Sq4E.hflip(), Square::Sq2E);
        assert_eq!(Square::Sq5A.hflip(), Square::Sq1A);
        assert_eq!(Square::Sq5B.hflip(), Square::Sq1B);
        assert_eq!(Square::Sq5C.hflip(), Square::Sq1C);
        assert_eq!(Square::Sq5D.hflip(), Square::Sq1D);
        assert_eq!(Square::Sq5E.hflip(), Square::Sq1E);
    }

    #[test]
    fn test_rotate() {
        assert_eq!(Square::Sq1A.rotate(), Square::Sq5E);
        assert_eq!(Square::Sq1B.rotate(), Square::Sq5D);
        assert_eq!(Square::Sq1C.rotate(), Square::Sq5C);
        assert_eq!(Square::Sq1D.rotate(), Square::Sq5B);
        assert_eq!(Square::Sq1E.rotate(), Square::Sq5A);
        assert_eq!(Square::Sq2A.rotate(), Square::Sq4E);
        assert_eq!(Square::Sq2B.rotate(), Square::Sq4D);
        assert_eq!(Square::Sq2C.rotate(), Square::Sq4C);
        assert_eq!(Square::Sq2D.rotate(), Square::Sq4B);
        assert_eq!(Square::Sq2E.rotate(), Square::Sq4A);
        assert_eq!(Square::Sq3A.rotate(), Square::Sq3E);
        assert_eq!(Square::Sq3B.rotate(), Square::Sq3D);
        assert_eq!(Square::Sq3C.rotate(), Square::Sq3C);
        assert_eq!(Square::Sq3D.rotate(), Square::Sq3B);
        assert_eq!(Square::Sq3E.rotate(), Square::Sq3A);
        assert_eq!(Square::Sq4A.rotate(), Square::Sq2E);
        assert_eq!(Square::Sq4B.rotate(), Square::Sq2D);
        assert_eq!(Square::Sq4C.rotate(), Square::Sq2C);
        assert_eq!(Square::Sq4D.rotate(), Square::Sq2B);
        assert_eq!(Square::Sq4E.rotate(), Square::Sq2A);
        assert_eq!(Square::Sq5A.rotate(), Square::Sq1E);
        assert_eq!(Square::Sq5B.rotate(), Square::Sq1D);
        assert_eq!(Square::Sq5C.rotate(), Square::Sq1C);
        assert_eq!(Square::Sq5D.rotate(), Square::Sq1B);
        assert_eq!(Square::Sq5E.rotate(), Square::Sq1A);
    }

    #[test]
    fn test_chebyshev_distance() {
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq1A), 0);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq1B), 1);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq1C), 2);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq1D), 3);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq1E), 4);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq3A), 2);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq3B), 2);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq3C), 2);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq3D), 3);
        assert_eq!(Square::Sq1A.chebyshev_distance(Square::Sq3E), 4);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq1A), 4);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq1B), 4);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq1C), 4);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq1D), 4);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq1E), 4);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq3A), 2);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq3B), 2);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq3C), 2);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq3D), 3);
        assert_eq!(Square::Sq5A.chebyshev_distance(Square::Sq3E), 4);
    }

    #[test]
    fn test_in_promotion_zone() {
        assert_eq!(Square::Sq1A.in_promotion_zone(Color::Black), true);
        assert_eq!(Square::Sq1B.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq1C.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq1D.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq1E.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq2A.in_promotion_zone(Color::Black), true);
        assert_eq!(Square::Sq2B.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq2C.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq2D.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq2E.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq3A.in_promotion_zone(Color::Black), true);
        assert_eq!(Square::Sq3B.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq3C.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq3D.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq3E.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq4A.in_promotion_zone(Color::Black), true);
        assert_eq!(Square::Sq4B.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq4C.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq4D.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq4E.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq5A.in_promotion_zone(Color::Black), true);
        assert_eq!(Square::Sq5B.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq5C.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq5D.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq5E.in_promotion_zone(Color::Black), false);
        assert_eq!(Square::Sq1A.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq1B.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq1C.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq1D.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq1E.in_promotion_zone(Color::White), true);
        assert_eq!(Square::Sq2A.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq2B.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq2C.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq2D.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq2E.in_promotion_zone(Color::White), true);
        assert_eq!(Square::Sq3A.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq3B.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq3C.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq3D.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq3E.in_promotion_zone(Color::White), true);
        assert_eq!(Square::Sq4A.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq4B.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq4C.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq4D.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq4E.in_promotion_zone(Color::White), true);
        assert_eq!(Square::Sq5A.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq5B.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq5C.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq5D.in_promotion_zone(Color::White), false);
        assert_eq!(Square::Sq5E.in_promotion_zone(Color::White), true);
    }
}
