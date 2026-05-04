use std::ops::Not;

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum Color {
    Black,
    White,
    None,
}

impl From<u8> for Color {
    fn from(value: u8) -> Self {
        match value {
            0 => Self::Black,
            1 => Self::White,
            _ => Self::None,
        }
    }
}

impl Not for Color {
    type Output = Self;

    fn not(self) -> Self {
        match self {
            Color::Black => Color::White,
            Color::White => Color::Black,
            Color::None => Color::None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_from_u8_black() {
        assert_eq!(Color::from(0), Color::Black);
    }

    #[test]
    fn test_from_u8_white() {
        assert_eq!(Color::from(1), Color::White);
    }

    #[test]
    fn test_from_u8_none() {
        assert_eq!(Color::from(2), Color::None);
        assert_eq!(Color::from(255), Color::None);
    }

    #[test]
    fn test_not_black() {
        assert_eq!(!Color::Black, Color::White);
    }

    #[test]
    fn test_not_white() {
        assert_eq!(!Color::White, Color::Black);
    }

    #[test]
    fn test_not_none() {
        assert_eq!(!Color::None, Color::None);
    }

    #[test]
    fn test_double_not() {
        assert_eq!(!!Color::Black, Color::Black);
        assert_eq!(!!Color::White, Color::White);
        assert_eq!(!!Color::None, Color::None);
    }

    #[test]
    fn test_equality() {
        assert_eq!(Color::Black, Color::Black);
        assert_ne!(Color::Black, Color::White);
        assert_ne!(Color::Black, Color::None);
    }

    #[test]
    fn test_clone() {
        let color = Color::Black;
        let cloned = color.clone();
        assert_eq!(color, cloned);
    }

    #[test]
    fn test_copy() {
        let color = Color::White;
        let copied = color;
        // color is still available (Copy trait allows this)
        assert_eq!(color, Color::White);
        assert_eq!(copied, Color::White);
    }
}
