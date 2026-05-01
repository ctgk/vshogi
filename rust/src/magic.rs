pub fn rust_to_magic_table_index_u32(relevant_occ: u32, magic: u32, shift: u32) -> u32 {
    let product = relevant_occ.wrapping_mul(magic);
    product >> (32 - shift)
}

pub fn rust_to_magic_table_index_u64(relevant_occ: u64, magic: u32, shift: u32) -> u32 {
    let mut product = (relevant_occ as u32).wrapping_mul(magic);
    product ^= ((relevant_occ >> 31) as u32).wrapping_mul(magic);
    product >> (32 - shift)
}

pub fn rust_to_magic_table_index_u128(high: u64, low: u64, mut magic: u32, shift: u32) -> u32 {
    let relevant_occ = ((high as u128) << 64) | (low as u128);
    let mut product = (relevant_occ as u32).wrapping_mul(magic);
    magic = magic.wrapping_mul(magic);
    product ^= ((relevant_occ >> 29) as u32).wrapping_mul(magic);
    magic = magic.wrapping_mul(magic);
    product ^= ((relevant_occ >> 58) as u32).wrapping_mul(magic);
    product >> (32 - shift)
}
