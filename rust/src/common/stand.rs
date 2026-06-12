use crate::common::config::Configuration;
use crate::common::parameters::BaseParameters;
use core::marker::PhantomData;

/// Stand: counts of captured pieces for a player.
///
/// Implementation stores counts in a boxed slice because the length depends
/// on the `Parameters` type and therefore cannot be used as an array length
/// directly in a generic context.
#[derive(Debug)]
pub struct Stand<P: BaseParameters> {
    counts: Box<[u8]>,
    _p: PhantomData<P>,
}

impl<P: BaseParameters> Clone for Stand<P> {
    fn clone(&self) -> Self {
        Self {
            counts: self.counts.clone(),
            _p: PhantomData,
        }
    }
}

impl<P: BaseParameters> Default for Stand<P> {
    fn default() -> Self {
        let len = Configuration::<P>::NUM_STAND_PIECE_TYPES;
        let vec = vec![0u8; len];
        Self {
            counts: vec.into_boxed_slice(),
            _p: PhantomData,
        }
    }
}

impl<P: BaseParameters> Stand<P> {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn value(&self) -> u128 {
        let mut out: u128 = 0;
        for (i, &c) in self.counts.iter().enumerate() {
            let shift = 8 * i;
            out |= (c as u128) << shift;
        }
        out
    }

    pub fn count(&self, p: P::PieceType) -> u32 {
        let idx = p.into() as usize;
        assert!(idx < self.counts.len());
        self.counts[idx] as u32
    }

    pub fn unique_count(&self) -> u32 {
        let mut out = 0u32;
        for &c in self.counts.iter() {
            if c > 0 {
                out += 1;
            }
        }
        out
    }

    pub fn exist(&self, p: P::PieceType) -> bool {
        let idx = p.into() as usize;
        assert!(idx < self.counts.len());
        self.counts[idx] > 0
    }

    pub fn any(&self) -> bool {
        self.counts.iter().any(|&c| c > 0)
    }

    pub fn add(&mut self, p: P::PieceType, num: u32) -> &mut Self {
        let idx = p.into() as usize;
        assert!(idx < self.counts.len());
        let before = self.counts[idx] as u32;
        self.counts[idx] = (before + num) as u8;
        self
    }

    pub fn subtract(&mut self, p: P::PieceType) -> &mut Self {
        let idx = p.into() as usize;
        assert!(idx < self.counts.len());
        let before = self.counts[idx];
        assert!(before > 0);
        self.counts[idx] = before - 1;
        self
    }

    /// Component-wise >= comparison: true if for every piece-type
    /// self.counts[i] >= other.counts[i].
    pub fn ge(&self, other: &Self) -> bool {
        self.counts
            .iter()
            .zip(other.counts.iter())
            .all(|(&a, &b)| a >= b)
    }

    /// Component-wise > comparison: >= and not equal.
    pub fn gt(&self, other: &Self) -> bool {
        self.ge(other) && (self.counts != other.counts)
    }

    /// Component-wise <= comparison.
    pub fn le(&self, other: &Self) -> bool {
        other.ge(self)
    }

    /// Component-wise < comparison.
    pub fn lt(&self, other: &Self) -> bool {
        other.gt(self)
    }
}

impl<P: BaseParameters> PartialEq for Stand<P> {
    fn eq(&self, other: &Self) -> bool {
        self.counts.as_ref() == other.counts.as_ref()
    }
}
impl<P: BaseParameters> Eq for Stand<P> {}

impl<P: BaseParameters> PartialOrd for Stand<P> {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        use std::cmp::Ordering;
        if self.counts.as_ref() == other.counts.as_ref() {
            return Some(Ordering::Equal);
        }
        if self.ge(other) {
            return Some(Ordering::Greater);
        }
        if self.le(other) {
            return Some(Ordering::Less);
        }
        None
    }
}

/// Simple container for two players' stands.
#[derive(Debug, Clone)]
pub struct BlackWhiteStands<P: BaseParameters> {
    stands: [Stand<P>; 2],
}

impl<P: BaseParameters> Default for BlackWhiteStands<P> {
    fn default() -> Self {
        Self {
            stands: [Stand::<P>::new(), Stand::<P>::new()],
        }
    }
}

impl<P: BaseParameters> BlackWhiteStands<P> {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn black(&self) -> &Stand<P> {
        &self.stands[0]
    }
    pub fn white(&self) -> &Stand<P> {
        &self.stands[1]
    }

    /// Parse SFEN holdings using a caller-provided mapper that converts a
    /// single-letter piece code (lowercase for white, uppercase for black)
    /// into an optional `PieceType` for this variant.
    /// Returns the remainder slice of the input (like the C++ API).
    pub fn set_sfen_with_mapper<'a>(
        &mut self,
        sfen: &'a str,
        mut mapper: impl FnMut(char) -> Option<P::PieceType>,
    ) -> &'a str {
        let mut num: u32 = 0;
        let bytes = sfen.as_bytes();
        let mut i = 0usize;
        while i < bytes.len() && i < 100usize {
            let c = bytes[i] as char;
            match c {
                '-' => {
                    i += 1;
                    break;
                }
                ' ' | '\0' => {
                    i += 1; // consume and break
                    break;
                }
                _ => {}
            }
            // accumulate number
            if c.is_ascii_digit() {
                num = num * 10 + (c as u32 - '0' as u32);
                i += 1;
                continue;
            }
            // letter: map and add
            if let Some(pt) = mapper(c) {
                let count = if num == 0 { 1 } else { num };
                if c.is_ascii_uppercase() {
                    // black
                    let idx = 0usize;
                    for _ in 0..count {
                        self.stands[idx].add(pt, 1);
                    }
                } else {
                    // white
                    let idx = 1usize;
                    for _ in 0..count {
                        self.stands[idx].add(pt, 1);
                    }
                }
            }
            num = 0;
            i += 1;
        }
        // return remainder: if next char is space, skip it (mimic C++ behavior)
        let rem = if i < bytes.len() && bytes[i] == b' ' {
            i + 1
        } else {
            i
        };
        &sfen[rem..]
    }

    pub fn add_captured_piece<T>(&mut self, captured: T)
    where
        T: crate::common::piece::BasePiece<P> + Copy,
        P::PieceType: crate::common::piece_type::BasePieceType<P> + Copy + PartialEq,
    {
        use crate::common::piece_type::BasePieceType;
        let pt = captured.to_piece_type();
        if pt == P::PieceType::from(u8::MAX) {
            return;
        }
        // skip if king or NA
        if pt == P::PieceType::from(P::NUM_STAND_PIECE_TYPES) {
            return;
        }
        let c = !captured.get_color();
        let demoted = if pt.is_promoted() { pt.demote() } else { pt };
        let idx = match c {
            crate::common::color::Color::Black => 0usize,
            crate::common::color::Color::White => 1usize,
            _ => return,
        };
        self.stands[idx].add(demoted, 1);
    }

    pub fn rotate(&self) -> Self {
        let mut out = Self::new();
        out.stands[0] = self.stands[1].clone();
        out.stands[1] = self.stands[0].clone();
        out
    }
}
