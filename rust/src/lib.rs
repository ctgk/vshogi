pub mod magic;
pub mod utils;

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
