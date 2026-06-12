use crate::common::color::Color;
use crate::common::config::Configuration;
use crate::common::parameters::BaseParameters;
use crate::common::square::BaseSquare;
use core::{fmt, marker::PhantomData};

/// A compact 16-bit move representation.
/// Wrapper newtype around `u16` parameterized by the variant parameters `P`.
#[derive(Copy, Clone)]
pub struct Move<P: BaseParameters>(pub u16, PhantomData<P>);

impl<P: BaseParameters> From<u16> for Move<P> {
    fn from(v: u16) -> Self {
        Self(v, PhantomData)
    }
}

impl<P: BaseParameters> From<Move<P>> for u16 {
    fn from(m: Move<P>) -> u16 {
        m.0
    }
}

impl<P: BaseParameters> fmt::Display for Move<P> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Move({})", self.0)
    }
}

impl<P: BaseParameters> fmt::Debug for Move<P> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Move({})", self.0)
    }
}

impl<P: BaseParameters> PartialEq for Move<P> {
    fn eq(&self, other: &Self) -> bool {
        self.0 == other.0
    }
}

impl<P: BaseParameters> Eq for Move<P> {}

impl<P: BaseParameters> Move<P> {
    const SRC_SHIFT: u16 = 8u16;
    const PRM_SHIFT: u16 = 7u16;
    const DST_MASK: u16 = 0x007f;
    const PRM_MASK: u16 = 0x0080;

    pub fn new(src: usize, dst: P::Square, promote: bool) -> Self {
        let dst_u = dst.into() as u16 & Self::DST_MASK;
        Self(
            ((src as u16) << Self::SRC_SHIFT) | ((promote as u16) << Self::PRM_SHIFT) | dst_u,
            PhantomData,
        )
    }

    pub fn from_sq(src: P::Square, dst: P::Square, promote: bool) -> Self {
        Self::new(src.into() as usize, dst, promote)
    }

    pub fn from_pt(src: P::PieceType, dst: P::Square) -> Self {
        Self::new(
            src.into() as usize + Configuration::<P>::NUM_SQUARES,
            dst,
            false,
        )
    }

    pub fn dst(&self) -> P::Square {
        P::Square::from((self.0 & Self::DST_MASK) as u8)
    }

    pub fn src_sq(&self) -> P::Square {
        P::Square::from((self.0 >> Self::SRC_SHIFT) as u8)
    }

    pub fn src_pt(&self) -> P::PieceType {
        let src_raw: u8 = (self.0 >> Self::SRC_SHIFT) as u8;
        let pt_idx = src_raw.wrapping_sub(Configuration::<P>::NUM_SQUARES as u8);
        P::PieceType::from(pt_idx)
    }

    pub fn promote(&self) -> bool {
        (self.0 & Self::PRM_MASK) != 0
    }

    pub fn is_drop(&self) -> bool {
        self.src_sq().into() >= Configuration::<P>::NUM_SQUARES as u8
    }

    pub fn rotate(&self) -> Self {
        let dst_rot = self.dst().rotate();
        if self.is_drop() {
            return Self::from_pt(self.src_pt(), dst_rot);
        }
        let src_rot = self.src_sq().rotate();
        Self::from_sq(src_rot, dst_rot, self.promote())
    }

    pub fn hflip(&self) -> Self {
        let dst_h = self.dst().hflip();
        if self.is_drop() {
            return Self::from_pt(self.src_pt(), dst_h);
        }
        let src_h = self.src_sq().hflip();
        Self::from_sq(src_h, dst_h, self.promote())
    }

    pub fn num_policy_per_square() -> usize {
        2 * Configuration::<P>::NUM_DIR_DL + Configuration::<P>::NUM_STAND_PIECE_TYPES
    }

    pub fn to_policy_index(self, by_side: Color) -> usize {
        let mut dst = self.dst();
        if by_side == Color::White {
            dst = dst.rotate();
        }
        (dst.into() as usize) * Self::num_policy_per_square() + self.to_policy_index_src(by_side)
    }

    fn to_policy_index_src(self, by_side: Color) -> usize {
        if self.is_drop() {
            return Configuration::<P>::NUM_DIR_DL * 2usize + self.src_pt().into() as usize;
        }
        let promo_offset = if self.promote() {
            Configuration::<P>::NUM_DIR_DL
        } else {
            0
        };
        let mut dir = Self::direction(self.dst(), self.src_sq());
        if by_side == Color::White {
            dir = Self::rotate_dir(dir);
        }
        dir as usize + promo_offset
    }

    // --- Helper utilities: shift / direction computation ---
    fn shift_by_dir(sq: P::Square, dir: usize) -> P::Square {
        const DF: [i32; 12] = [1, 0, -1, 1, -1, 1, 0, -1, 2, 2, -2, -2];
        const DR: [i32; 12] = [-1, -1, -1, 0, 0, 1, 1, 1, 2, 2, -2, -2];
        if dir >= Configuration::<P>::NUM_DIR {
            return P::Square::from(Configuration::<P>::SQ_NA as u8);
        }
        let f = sq.file().into() as i32 + DF[dir];
        let r = sq.rank().into() as i32 + DR[dir];
        if (0..(Configuration::<P>::NUM_FILES as i32)).contains(&f)
            && (0..(Configuration::<P>::NUM_RANKS as i32)).contains(&r)
        {
            let file = P::File::from(f as u8);
            let rank = P::Rank::from(r as u8);
            P::Square::new(file, rank)
        } else {
            P::Square::from(Configuration::<P>::SQ_NA as u8)
        }
    }

    fn is_knight_direction(dir: usize) -> bool {
        dir >= 8 && dir < Configuration::<P>::NUM_DIR
    }

    fn direction(src: P::Square, dst: P::Square) -> u8 {
        let sq_na = Configuration::<P>::SQ_NA as u8;
        for dir in 0..Configuration::<P>::NUM_DIR {
            let mut cur = Self::shift_by_dir(src, dir);
            while cur.into() != sq_na {
                if cur.into() == dst.into() {
                    return dir as u8;
                }
                if Self::is_knight_direction(dir) {
                    break;
                }
                cur = Self::shift_by_dir(cur, dir);
            }
        }
        Configuration::<P>::NUM_DIR as u8 // DIR_NA
    }

    fn rotate_dir(d: u8) -> u8 {
        // Mapping copied from C++ rotate table in direction.hpp
        const TABLE: [u8; 13] = [7, 6, 5, 4, 3, 2, 1, 0, 11, 10, 9, 8, 12];
        let idx = d as usize;
        if idx < TABLE.len() {
            TABLE[idx]
        } else {
            d
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::common::color::Color;
    use crate::variants::minishogi::Parameters;
    use crate::variants::minishogi::PieceType as Pt;
    use crate::variants::minishogi::Square as Sq;

    type MV = Move<Parameters>;

    #[test]
    fn basic_pack_unpack() {
        let m = MV::from_sq(Sq::Sq1A, Sq::Sq2B, true);
        let dst_u: u8 = m.dst().into();
        let src_u: u8 = m.src_sq().into();
        assert_eq!(dst_u, Sq::Sq2B as u8);
        assert_eq!(src_u, Sq::Sq1A as u8);
        assert_eq!(m.promote(), true);
        let d = MV::from_pt(Pt::Fu, Sq::Sq3C);
        assert!(d.is_drop());
    }

    #[test]
    fn policy_index_roundtrip() {
        use crate::variants::minishogi::Square as Sq;
        let m = MV::from_sq(Sq::Sq1A, Sq::Sq2B, false);
        let idx = m.to_policy_index(Color::Black);
        let per_sq = MV::num_policy_per_square();
        assert_eq!(idx / per_sq, Sq::Sq2B as u8 as usize);
    }

    #[test]
    fn dst() {
        use crate::variants::minishogi::PieceType as Pt;
        use crate::variants::minishogi::Square as Sq;
        assert_eq!(MV::from_sq(Sq::Sq1B, Sq::Sq1A, true).dst(), Sq::Sq1A);
        assert_eq!(MV::from_pt(Pt::Gi, Sq::Sq3E).dst(), Sq::Sq3E);
    }

    #[test]
    fn src() {
        use crate::variants::minishogi::PieceType as Pt;
        use crate::variants::minishogi::Square as Sq;
        assert_eq!(MV::from_sq(Sq::Sq1B, Sq::Sq1A, true).src_sq(), Sq::Sq1B);
        assert_eq!(MV::from_pt(Pt::Gi, Sq::Sq3E).src_pt(), Pt::Gi);
    }

    #[test]
    fn promote() {
        use crate::variants::minishogi::PieceType as Pt;
        assert!(MV::from_sq(Sq::Sq1B, Sq::Sq1A, true).promote());
        assert!(!MV::from_pt(Pt::Gi, Sq::Sq3E).promote());
    }

    #[test]
    fn is_drop_test() {
        use crate::variants::minishogi::PieceType as Pt;
        assert!(!MV::from_sq(Sq::Sq1B, Sq::Sq1A, true).is_drop());
        assert!(MV::from_pt(Pt::Gi, Sq::Sq3E).is_drop());
    }

    #[test]
    fn rotate_exact() {
        use crate::variants::minishogi::PieceType as Pt;
        use crate::variants::minishogi::Square as Sq;
        let a = MV::from_sq(Sq::Sq1B, Sq::Sq1A, true);
        let a_rot = MV::from_sq(Sq::Sq5D, Sq::Sq5E, true);
        assert_eq!(a.rotate(), a_rot);

        let d = MV::from_pt(Pt::Gi, Sq::Sq3E);
        let d_rot = MV::from_pt(Pt::Gi, Sq::Sq3A);
        assert_eq!(d.rotate(), d_rot);
    }

    #[test]
    fn hflip_exact() {
        use crate::variants::minishogi::PieceType as Pt;
        use crate::variants::minishogi::Square as Sq;
        let a = MV::from_sq(Sq::Sq1B, Sq::Sq1A, true);
        let a_h = MV::from_sq(Sq::Sq5B, Sq::Sq5A, true);
        assert_eq!(a.hflip(), a_h);

        let d = MV::from_pt(Pt::Gi, Sq::Sq3E);
        let d_h = MV::from_pt(Pt::Gi, Sq::Sq3E);
        assert_eq!(d.hflip(), d_h);
    }

    #[test]
    fn to_policy_index_tests() {
        use crate::variants::minishogi::PieceType as Pt;
        use crate::variants::minishogi::Square as Sq;
        let per = MV::num_policy_per_square();
        // Black
        let idx1 = MV::from_sq(Sq::Sq1E, Sq::Sq3C, false).to_policy_index(Color::Black);
        assert_eq!(idx1, (Sq::Sq3C as u8 as usize) * per + 7);

        let idx2 = MV::from_sq(Sq::Sq1A, Sq::Sq4D, true).to_policy_index(Color::Black);
        assert_eq!(idx2, (Sq::Sq4D as u8 as usize) * per + 2 + 8);

        let idx3 = MV::from_pt(Pt::Gi, Sq::Sq4B).to_policy_index(Color::Black);
        assert_eq!(
            idx3,
            (Sq::Sq4B as u8 as usize) * per + 8 * 2 + (Pt::Gi as u8 as usize)
        );

        // White
        let idx4 = MV::from_sq(Sq::Sq5A, Sq::Sq3C, false).to_policy_index(Color::White);
        assert_eq!(idx4, (Sq::Sq3C as u8 as usize) * per + 7);

        let idx5 = MV::from_sq(Sq::Sq5E, Sq::Sq2B, true).to_policy_index(Color::White);
        assert_eq!(idx5, (Sq::Sq4D as u8 as usize) * per + 2 + 8);

        let idx6 = MV::from_pt(Pt::Gi, Sq::Sq2D).to_policy_index(Color::White);
        assert_eq!(
            idx6,
            (Sq::Sq4B as u8 as usize) * per + 8 * 2 + (Pt::Gi as u8 as usize)
        );
    }

    #[test]
    fn rotate_roundtrip() {
        use crate::variants::minishogi::PieceType as Pt;
        use crate::variants::minishogi::Square as Sq;
        // non-drop move
        let m = MV::from_sq(Sq::Sq1A, Sq::Sq2B, false);
        let dst_u: u8 = m.rotate().dst().into();
        let src_u: u8 = m.rotate().src_sq().into();
        assert_eq!(dst_u, Sq::Sq4D as u8);
        assert_eq!(src_u, Sq::Sq5E as u8);
        assert_eq!(m.rotate().promote(), false);
        assert_eq!(m.rotate().rotate(), m);
        // drop move
        let d = MV::from_pt(Pt::Fu, Sq::Sq2C);
        let d_dst_u: u8 = d.rotate().dst().into();
        let d_pt_u: u8 = d.rotate().src_pt().into();
        assert_eq!(d_dst_u, Sq::Sq4C as u8);
        assert_eq!(d_pt_u, Pt::Fu as u8);
        assert_eq!(d.rotate().rotate(), d);
    }
}
