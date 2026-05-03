use num_enum::FromPrimitive;

#[rustfmt::skip]
#[repr(i8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, FromPrimitive)]
pub enum Direction {
    #[num_enum(default)]
    NAN = 0,
    NNW =-6,          NNE =-5,
    NWe =-4, Nth =-3, NEa =-2,
    Wst =-1,          Est = 1,
    SWe = 2, Sth = 3, SEa = 4,
    SSW = 5,          SSE = 6,
}

impl Direction {
    pub fn rotate(self) -> Direction {
        Self::from(-1 * (self as i8))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_rotate() {
        assert_eq!(Direction::NAN.rotate(), Direction::NAN);
        assert_eq!(Direction::SSW.rotate(), Direction::NNE);
        assert_eq!(Direction::SSE.rotate(), Direction::NNW);
        assert_eq!(Direction::NWe.rotate(), Direction::SEa);
        assert_eq!(Direction::Nth.rotate(), Direction::Sth);
        assert_eq!(Direction::NEa.rotate(), Direction::SWe);
        assert_eq!(Direction::Wst.rotate(), Direction::Est);
        assert_eq!(Direction::Est.rotate(), Direction::Wst);
        assert_eq!(Direction::SWe.rotate(), Direction::NEa);
        assert_eq!(Direction::Sth.rotate(), Direction::Nth);
        assert_eq!(Direction::SEa.rotate(), Direction::NWe);
        assert_eq!(Direction::NNW.rotate(), Direction::SSE);
        assert_eq!(Direction::NNE.rotate(), Direction::SSW);
    }
}
