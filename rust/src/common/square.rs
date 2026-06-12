use crate::common::{color::Color, direction::Direction, parameters::BaseParameters};

pub trait BaseFile<P: BaseParameters>: Copy + Into<u8> + From<u8> {
    fn hflip(self) -> Self {
        Self::from(P::NUM_FILES - 1 - self.into())
    }
}
pub trait BaseRank<P: BaseParameters>: Copy + Into<u8> + From<u8> {}

pub trait BaseSquare<P: BaseParameters>: Copy + Into<u8> + From<u8> {
    const NUM_SQUARES: u8 = P::NUM_FILES * P::NUM_RANKS;
    fn new(f: impl BaseFile<P>, r: impl BaseRank<P>) -> Self {
        if f.into() >= P::NUM_FILES || r.into() >= P::NUM_RANKS {
            return Self::from(Self::NUM_SQUARES);
        }
        Self::from(f.into() * P::NUM_RANKS + r.into())
    }
    fn file(self) -> P::File {
        P::File::from(self.into() / P::NUM_RANKS)
    }
    fn rank(self) -> P::Rank {
        P::Rank::from(self.into() % P::NUM_RANKS)
    }
    fn hflip(self) -> Self {
        Self::new(self.file().hflip(), self.rank())
    }
    fn rotate(self) -> Self {
        Self::from(Self::NUM_SQUARES - 1 - self.into())
    }
    fn in_promotion_zone(self, by_side: Color) -> bool {
        match by_side {
            Color::Black => self.rank().into() < P::NUM_PROMOTION_RANKS,
            Color::White => self.rank().into() >= P::NUM_RANKS - P::NUM_PROMOTION_RANKS,
            Color::None => false,
        }
    }
    fn chebyshev_distance(self, other: Self) -> u32 {
        let df = (self.file().into() as i32 - other.file().into() as i32).abs() as u32;
        let dr = (self.rank().into() as i32 - other.rank().into() as i32).abs() as u32;
        df.max(dr)
    }
    fn shift(self, dir: Direction) -> Self {
        const TABLE_DF: [i32; 9] = [1, 0, -1, 1, 0, -1, 1, 0, -1];
        const TABLE_DR: [i32; 9] = [-1, -1, -1, 0, 0, 0, 1, 1, 1];
        let i = (dir as i32 + 4) as usize;
        let f = P::File::from((self.file().into() as i32 + TABLE_DF[i]) as u8);
        let r = P::Rank::from((self.rank().into() as i32 + TABLE_DR[i]) as u8);
        Self::new(f, r)
    }
}
