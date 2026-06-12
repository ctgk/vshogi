use crate::common::color::Color;
use crate::common::direction::Direction;
use num_enum::FromPrimitive;

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive)]
pub enum FullPieceTypes {
    Fu = 0,
    Ky,
    Ke,
    Gi,
    Ka,
    Hi,
    Ki,
    Ou,
    To,
    Ny,
    Nk,
    Ng,
    Um,
    Ry,
    #[num_enum(default)]
    Na,
}

impl FullPieceTypes {
    pub fn to_fpt(c: char) -> FullPieceTypes {
        match c.to_ascii_lowercase() {
            'p' => FullPieceTypes::Fu,
            'l' => FullPieceTypes::Ky,
            'n' => FullPieceTypes::Ke,
            's' => FullPieceTypes::Gi,
            'b' => FullPieceTypes::Ka,
            'r' => FullPieceTypes::Hi,
            'g' => FullPieceTypes::Ki,
            'k' => FullPieceTypes::Ou,
            _ => FullPieceTypes::Na,
        }
    }

    pub fn is_promotable(self) -> bool {
        (self as u8) < (FullPieceTypes::Ki as u8)
    }

    pub fn is_promoted(self) -> bool {
        (self as u8) > (FullPieceTypes::Ou as u8)
    }

    pub fn is_promotion_fully_superior(self) -> bool {
        matches!(
            self,
            FullPieceTypes::Fu | FullPieceTypes::Ka | FullPieceTypes::Hi
        )
    }

    pub fn to_point(self) -> u32 {
        const TABLE: [u32; 15] = [1, 1, 1, 1, 5, 5, 1, 0, 1, 1, 1, 1, 5, 5, 0];
        TABLE[self as usize]
    }

    pub fn to_value(self) -> u32 {
        const TABLE: [u32; 15] = [5, 30, 35, 55, 95, 100, 60, 0, 60, 60, 60, 60, 115, 120, 0];
        TABLE[self as usize]
    }

    pub fn get_attack_directions(self, c: Color) -> &'static [Direction; 9] {
        // Table layout mirrors the C++ attack_directions_table
        #[rustfmt::skip]
        const TABLE: &[[Direction; 9]] = &[
            [Direction::Nth, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::Nth, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NNW, Direction::NNE, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::SWe, Direction::SEa, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NWe, Direction::NEa, Direction::SWe, Direction::SEa, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::Nth, Direction::Wst, Direction::Est, Direction::Sth, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::Wst, Direction::Est, Direction::Sth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::Wst, Direction::Est, Direction::SWe, Direction::Sth, Direction::SEa, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::Wst, Direction::Est, Direction::Sth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::Wst, Direction::Est, Direction::Sth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::Wst, Direction::Est, Direction::Sth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::Wst, Direction::Est, Direction::Sth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::Wst, Direction::Est, Direction::SWe, Direction::Sth, Direction::SEa, Direction::NAN],
            [Direction::NWe, Direction::Nth, Direction::NEa, Direction::Wst, Direction::Est, Direction::SWe, Direction::Sth, Direction::SEa, Direction::NAN],
            [Direction::Sth, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::Sth, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::SSE, Direction::SSW, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::SEa, Direction::Sth, Direction::SWe, Direction::NEa, Direction::NWe, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::SEa, Direction::SWe, Direction::NEa, Direction::NWe, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::Sth, Direction::Est, Direction::Wst, Direction::Nth, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::SEa, Direction::Sth, Direction::SWe, Direction::Est, Direction::Wst, Direction::Nth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::SEa, Direction::Sth, Direction::SWe, Direction::Est, Direction::Wst, Direction::NEa, Direction::Nth, Direction::NWe, Direction::NAN],
            [Direction::SEa, Direction::Sth, Direction::SWe, Direction::Est, Direction::Wst, Direction::Nth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::SEa, Direction::Sth, Direction::SWe, Direction::Est, Direction::Wst, Direction::Nth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::SEa, Direction::Sth, Direction::SWe, Direction::Est, Direction::Wst, Direction::Nth, Direction::NAN, Direction::NAN, Direction::NAN],
            [Direction::SEa, Direction::Sth, Direction::SWe, Direction::Est, Direction::Wst, Direction::Nth, Direction::NEa, Direction::Nth, Direction::NWe],
            [Direction::SEa, Direction::Sth, Direction::SWe, Direction::Est, Direction::Wst, Direction::Nth, Direction::NEa, Direction::Nth, Direction::NWe],
            [Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN, Direction::NAN],
        ];

        let idx = if self == FullPieceTypes::Na {
            28usize
        } else {
            (self as usize) + (c as usize) * 14usize
        };
        &TABLE[idx]
    }
    pub fn is_slider(self) -> bool {
        const TABLE: [bool; 15] = [
            false, true, false, false, true, true, false, false, false, false, false, false, true,
            true, false,
        ];
        TABLE[self as usize]
    }

    pub fn slidable_to(self, d: Direction) -> bool {
        match self {
            FullPieceTypes::Ky => d == Direction::Nth,
            FullPieceTypes::Ka | FullPieceTypes::Um => matches!(
                d,
                Direction::NWe | Direction::NEa | Direction::SWe | Direction::SEa
            ),
            FullPieceTypes::Hi | FullPieceTypes::Ry => matches!(
                d,
                Direction::Nth | Direction::Wst | Direction::Est | Direction::Sth
            ),
            _ => false,
        }
    }

    pub fn is_attacking_to(self, d: Direction) -> bool {
        match d {
            Direction::Nth => (self != FullPieceTypes::Ke) && (self != FullPieceTypes::Ka),
            Direction::NWe | Direction::NEa => {
                !((self as u8) < (FullPieceTypes::Gi as u8) || (self == FullPieceTypes::Hi))
            }
            Direction::Wst | Direction::Est | Direction::Sth => {
                (self as u8) > (FullPieceTypes::Ka as u8) && (self != FullPieceTypes::Na)
            }
            Direction::SWe | Direction::SEa => matches!(
                self,
                FullPieceTypes::Gi
                    | FullPieceTypes::Ka
                    | FullPieceTypes::Ou
                    | FullPieceTypes::Um
                    | FullPieceTypes::Ry
            ),
            Direction::NNW | Direction::NNE => self == FullPieceTypes::Ke,
            _ => false,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_point_and_value_tables() {
        assert_eq!(FullPieceTypes::Fu.to_point(), 1);
        assert_eq!(FullPieceTypes::Fu.to_value(), 5);
        assert_eq!(FullPieceTypes::Ry.to_value(), 120);
    }
}
