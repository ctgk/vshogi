pub mod common;
pub mod variants;

use crate::common::magic;
use crate::common::utils;
use crate::variants::minishogi;
use crate::variants::minishogi::PieceTypeEnum;

#[no_mangle]
pub extern "C" fn rust_hamming_weight_u32(x: u32) -> u32 {
    utils::rust_hamming_weight_u32(x)
}

#[no_mangle]
pub extern "C" fn rust_hamming_weight_u64(x: u64) -> u32 {
    utils::rust_hamming_weight_u64(x)
}

#[no_mangle]
pub extern "C" fn rust_hamming_weight_u128(high: u64, low: u64) -> u32 {
    utils::rust_hamming_weight_u128(high, low)
}

#[no_mangle]
pub extern "C" fn rust_ntz_u32(x: u32) -> u32 {
    utils::rust_ntz_u32(x)
}

#[no_mangle]
pub extern "C" fn rust_ntz_u64(x: u64) -> u32 {
    utils::rust_ntz_u64(x)
}

#[no_mangle]
pub extern "C" fn rust_ntz_u128(high: u64, low: u64) -> u32 {
    utils::rust_ntz_u128(high, low)
}

#[no_mangle]
pub unsafe extern "C" fn rust_softmax(ptr: *mut f32, len: usize) {
    let slice = std::slice::from_raw_parts_mut(ptr, len);
    utils::rust_softmax(slice);
}

#[no_mangle]
pub extern "C" fn rust_to_magic_table_index_u32(relevant_occ: u32, magic: u32, shift: u32) -> u32 {
    magic::rust_to_magic_table_index_u32(relevant_occ, magic, shift)
}

#[no_mangle]
pub extern "C" fn rust_to_magic_table_index_u64(relevant_occ: u64, magic: u32, shift: u32) -> u32 {
    magic::rust_to_magic_table_index_u64(relevant_occ, magic, shift)
}

#[no_mangle]
pub extern "C" fn rust_to_magic_table_index_u128(
    high: u64,
    low: u64,
    magic: u32,
    shift: u32,
) -> u32 {
    magic::rust_to_magic_table_index_u128(high, low, magic, shift)
}

#[no_mangle]
pub extern "C" fn rust_minishogi_piece_traits_is_promotable(pt: u8) -> bool {
    minishogi::PieceTraits::is_promotable(PieceTypeEnum::from(pt))
}
