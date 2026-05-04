use crate::common::color::Color;

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum ResultEnum {
    Ongoing,
    Draw,
    BlackWin,
    WhiteWin,
}

impl ResultEnum {
    pub fn winner(self) -> Color {
        match self {
            Self::BlackWin => Color::Black,
            Self::WhiteWin => Color::White,
            Self::Ongoing | Self::Draw => Color::None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_winner_for_ongoing() {
        assert_eq!(ResultEnum::Ongoing.winner(), Color::None);
    }

    #[test]
    fn test_winner_for_draw() {
        assert_eq!(ResultEnum::Draw.winner(), Color::None);
    }

    #[test]
    fn test_winner_for_black_win() {
        assert_eq!(ResultEnum::BlackWin.winner(), Color::Black);
    }

    #[test]
    fn test_winner_for_white_win() {
        assert_eq!(ResultEnum::WhiteWin.winner(), Color::White);
    }
}
