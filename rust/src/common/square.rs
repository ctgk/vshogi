use crate::common::parameters::BaseParameters;

pub trait BaseFile<P: BaseParameters>: Copy + Into<u8> + From<u8> {
    fn hflip(self) -> Self {
        Self::from(P::NUM_FILES - 1 - self.into())
    }
}
pub trait BaseRank<P: BaseParameters>: Into<u8> + From<u8> {}

pub trait BaseSquare<P: BaseParameters>: Copy + Into<u8> + From<u8> {
    type File: BaseFile<P>;
    type Rank: BaseRank<P>;
    const NUM_SQUARES: u8 = P::NUM_FILES * P::NUM_RANKS;
    fn new(f: impl BaseFile<P>, r: impl BaseRank<P>) -> Self {
        Self::from(f.into() * P::NUM_RANKS + r.into())
    }
    fn file(self) -> Self::File {
        Self::File::from(self.into() / P::NUM_RANKS)
    }
    fn rank(self) -> Self::Rank {
        Self::Rank::from(self.into() % P::NUM_RANKS)
    }
    fn hflip(self) -> Self {
        Self::new(self.file().hflip(), self.rank())
    }
    fn rotate(self) -> Self {
        Self::from(Self::NUM_SQUARES - 1 - self.into())
    }
}
