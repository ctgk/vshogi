use crate::common::color::ColorEnum;

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum ResultEnum {
    Ongoing,
    Draw,
    BlackWin,
    WhiteWin,
}

impl ResultEnum {
    pub fn winner(self) -> ColorEnum {
        match self {
            Self::BlackWin => ColorEnum::Black,
            Self::WhiteWin => ColorEnum::White,
            Self::Ongoing | Self::Draw => ColorEnum::None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_winner_for_ongoing() {
        assert_eq!(ResultEnum::Ongoing.winner(), ColorEnum::None);
    }

    #[test]
    fn test_winner_for_draw() {
        assert_eq!(ResultEnum::Draw.winner(), ColorEnum::None);
    }

    #[test]
    fn test_winner_for_black_win() {
        assert_eq!(ResultEnum::BlackWin.winner(), ColorEnum::Black);
    }

    #[test]
    fn test_winner_for_white_win() {
        assert_eq!(ResultEnum::WhiteWin.winner(), ColorEnum::White);
    }
}
