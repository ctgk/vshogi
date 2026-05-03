use num_enum::FromPrimitive;

#[rustfmt::skip]
#[repr(i8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive)]
pub enum Direction {
    #[num_enum(default)]
    NAN = 0,
    NNW =-7,          NNE =-5,
    NWe =-4, Nth =-3, NEa =-2,
    Wst =-1,          Est = 1,
    SWe = 2, Sth = 3, SEa = 4,
    SSW = 5,          SSE = 7,
}

impl Direction {
    pub fn rotate(self) -> Direction {
        Self::from(-1 * (self as i8))
    }
    pub fn has_north(self) -> bool {
        self <= Self::NEa
    }
    pub fn has_east(self) -> bool {
        (self as i8).rem_euclid(3) == 1
    }
    pub fn has_west(self) -> bool {
        (self as i8).rem_euclid(3) == 2
    }
    pub fn has_south(self) -> bool {
        self >= Self::SWe
    }
    pub fn is_knight_dir(self) -> bool {
        (self as i8).abs() >= 5
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_rotate() {
        assert_eq!(Direction::NAN.rotate(), Direction::NAN);
        assert_eq!(Direction::NNW.rotate(), Direction::SSE);
        assert_eq!(Direction::NNE.rotate(), Direction::SSW);
        assert_eq!(Direction::NWe.rotate(), Direction::SEa);
        assert_eq!(Direction::Nth.rotate(), Direction::Sth);
        assert_eq!(Direction::NEa.rotate(), Direction::SWe);
        assert_eq!(Direction::Wst.rotate(), Direction::Est);
        assert_eq!(Direction::Est.rotate(), Direction::Wst);
        assert_eq!(Direction::SWe.rotate(), Direction::NEa);
        assert_eq!(Direction::Sth.rotate(), Direction::Nth);
        assert_eq!(Direction::SEa.rotate(), Direction::NWe);
        assert_eq!(Direction::SSW.rotate(), Direction::NNE);
        assert_eq!(Direction::SSE.rotate(), Direction::NNW);
    }

    #[test]
    fn test_has_north() {
        assert_eq!(Direction::NAN.has_north(), false);
        assert_eq!(Direction::NNW.has_north(), true);
        assert_eq!(Direction::NNE.has_north(), true);
        assert_eq!(Direction::NWe.has_north(), true);
        assert_eq!(Direction::Nth.has_north(), true);
        assert_eq!(Direction::NEa.has_north(), true);
        assert_eq!(Direction::Wst.has_north(), false);
        assert_eq!(Direction::Est.has_north(), false);
        assert_eq!(Direction::SWe.has_north(), false);
        assert_eq!(Direction::Sth.has_north(), false);
        assert_eq!(Direction::SEa.has_north(), false);
        assert_eq!(Direction::SSW.has_north(), false);
        assert_eq!(Direction::SSE.has_north(), false);
    }

    #[test]
    fn test_has_east() {
        assert_eq!(Direction::NAN.has_east(), false);
        assert_eq!(Direction::NNW.has_east(), false);
        assert_eq!(Direction::NNE.has_east(), true);
        assert_eq!(Direction::NWe.has_east(), false);
        assert_eq!(Direction::Nth.has_east(), false);
        assert_eq!(Direction::NEa.has_east(), true);
        assert_eq!(Direction::Wst.has_east(), false);
        assert_eq!(Direction::Est.has_east(), true);
        assert_eq!(Direction::SWe.has_east(), false);
        assert_eq!(Direction::Sth.has_east(), false);
        assert_eq!(Direction::SEa.has_east(), true);
        assert_eq!(Direction::SSW.has_east(), false);
        assert_eq!(Direction::SSE.has_east(), true);
    }

    #[test]
    fn test_has_west() {
        assert_eq!(Direction::NAN.has_west(), false);
        assert_eq!(Direction::NNW.has_west(), true);
        assert_eq!(Direction::NNE.has_west(), false);
        assert_eq!(Direction::NWe.has_west(), true);
        assert_eq!(Direction::Nth.has_west(), false);
        assert_eq!(Direction::NEa.has_west(), false);
        assert_eq!(Direction::Wst.has_west(), true);
        assert_eq!(Direction::Est.has_west(), false);
        assert_eq!(Direction::SWe.has_west(), true);
        assert_eq!(Direction::Sth.has_west(), false);
        assert_eq!(Direction::SEa.has_west(), false);
        assert_eq!(Direction::SSW.has_west(), true);
        assert_eq!(Direction::SSE.has_west(), false);
    }

    #[test]
    fn test_has_south() {
        assert_eq!(Direction::NAN.has_south(), false);
        assert_eq!(Direction::NNW.has_south(), false);
        assert_eq!(Direction::NNE.has_south(), false);
        assert_eq!(Direction::NWe.has_south(), false);
        assert_eq!(Direction::Nth.has_south(), false);
        assert_eq!(Direction::NEa.has_south(), false);
        assert_eq!(Direction::Wst.has_south(), false);
        assert_eq!(Direction::Est.has_south(), false);
        assert_eq!(Direction::SWe.has_south(), true);
        assert_eq!(Direction::Sth.has_south(), true);
        assert_eq!(Direction::SEa.has_south(), true);
        assert_eq!(Direction::SSW.has_south(), true);
        assert_eq!(Direction::SSE.has_south(), true);
    }

    #[test]
    fn test_is_knight_dir() {
        assert_eq!(Direction::NAN.is_knight_dir(), false);
        assert_eq!(Direction::NNW.is_knight_dir(), true);
        assert_eq!(Direction::NNE.is_knight_dir(), true);
        assert_eq!(Direction::NWe.is_knight_dir(), false);
        assert_eq!(Direction::Nth.is_knight_dir(), false);
        assert_eq!(Direction::NEa.is_knight_dir(), false);
        assert_eq!(Direction::Wst.is_knight_dir(), false);
        assert_eq!(Direction::Est.is_knight_dir(), false);
        assert_eq!(Direction::SWe.is_knight_dir(), false);
        assert_eq!(Direction::Sth.is_knight_dir(), false);
        assert_eq!(Direction::SEa.is_knight_dir(), false);
        assert_eq!(Direction::SSW.is_knight_dir(), true);
        assert_eq!(Direction::SSE.is_knight_dir(), true);
    }
}
