use crate::common::parameters::BaseParameters;

pub trait BaseFile<P: BaseParameters>: Into<u8> + From<u8> {}
pub trait BaseRank<P: BaseParameters>: Into<u8> + From<u8> {}

pub trait BaseSquare<P: BaseParameters>: Into<u8> + From<u8> {
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
}
