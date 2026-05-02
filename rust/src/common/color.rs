use std::ops::Not;

#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum ColorEnum {
    Black,
    White,
    None,
}

impl From<u8> for ColorEnum {
    fn from(value: u8) -> Self {
        match value {
            0 => Self::Black,
            1 => Self::White,
            _ => Self::None,
        }
    }
}

impl Not for ColorEnum {
    type Output = Self;

    fn not(self) -> Self {
        match self {
            ColorEnum::Black => ColorEnum::White,
            ColorEnum::White => ColorEnum::Black,
            ColorEnum::None => ColorEnum::None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_from_u8_black() {
        assert_eq!(ColorEnum::from(0), ColorEnum::Black);
    }

    #[test]
    fn test_from_u8_white() {
        assert_eq!(ColorEnum::from(1), ColorEnum::White);
    }

    #[test]
    fn test_from_u8_none() {
        assert_eq!(ColorEnum::from(2), ColorEnum::None);
        assert_eq!(ColorEnum::from(255), ColorEnum::None);
    }

    #[test]
    fn test_not_black() {
        assert_eq!(!ColorEnum::Black, ColorEnum::White);
    }

    #[test]
    fn test_not_white() {
        assert_eq!(!ColorEnum::White, ColorEnum::Black);
    }

    #[test]
    fn test_not_none() {
        assert_eq!(!ColorEnum::None, ColorEnum::None);
    }

    #[test]
    fn test_double_not() {
        assert_eq!(!!ColorEnum::Black, ColorEnum::Black);
        assert_eq!(!!ColorEnum::White, ColorEnum::White);
        assert_eq!(!!ColorEnum::None, ColorEnum::None);
    }

    #[test]
    fn test_equality() {
        assert_eq!(ColorEnum::Black, ColorEnum::Black);
        assert_ne!(ColorEnum::Black, ColorEnum::White);
        assert_ne!(ColorEnum::Black, ColorEnum::None);
    }

    #[test]
    fn test_clone() {
        let color = ColorEnum::Black;
        let cloned = color.clone();
        assert_eq!(color, cloned);
    }

    #[test]
    fn test_copy() {
        let color = ColorEnum::White;
        let copied = color;
        // color is still available (Copy trait allows this)
        assert_eq!(color, ColorEnum::White);
        assert_eq!(copied, ColorEnum::White);
    }
}
