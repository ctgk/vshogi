pub trait CountOnes {
    fn count_bits(self) -> u32;
}

impl CountOnes for u32 {
    fn count_bits(self) -> u32 {
        self.count_ones()
    }
}

impl CountOnes for u64 {
    fn count_bits(self) -> u32 {
        self.count_ones()
    }
}

impl CountOnes for u128 {
    fn count_bits(self) -> u32 {
        self.count_ones()
    }
}

pub fn generic_hamming_weight<T: CountOnes>(x: T) -> u32 {
    x.count_bits()
}

pub fn rust_hamming_weight_u32(x: u32) -> u32 {
    generic_hamming_weight(x)
}

pub fn rust_hamming_weight_u64(x: u64) -> u32 {
    generic_hamming_weight(x)
}

pub fn rust_hamming_weight_u128(high: u64, low: u64) -> u32 {
    let x = ((high as u128) << 64) | (low as u128);
    generic_hamming_weight(x)
}

pub fn rust_ntz_u32(x: u32) -> u32 {
    x.trailing_zeros()
}

pub fn rust_ntz_u64(x: u64) -> u32 {
    x.trailing_zeros()
}

pub fn rust_ntz_u128(high: u64, low: u64) -> u32 {
    let x = ((high as u128) << 64) | (low as u128);
    x.trailing_zeros()
}

pub fn rust_softmax(logits: &mut [f32]) {
    if logits.is_empty() {
        return;
    }

    let max_val = logits.iter().fold(f32::NEG_INFINITY, |a, &b| a.max(b));

    let mut sum = 0.0;
    for val in logits.iter_mut() {
        *val = (*val - max_val).exp();
        sum += *val;
    }

    if sum > 0.0 {
        for val in logits.iter_mut() {
            *val /= sum;
        }
    }
}
