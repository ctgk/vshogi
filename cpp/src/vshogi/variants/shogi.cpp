#include "vshogi/variants/shogi.hpp"

namespace vshogi
{

template <>
template <>
shogi::Stand::Stand(
    const int num_fu,
    const int num_ky,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<std::uint32_t>(
        (num_fu << shift_bits[shogi::FU]) + (num_ky << shift_bits[shogi::KY])
        + (num_ke << shift_bits[shogi::KE]) + (num_gi << shift_bits[shogi::GI])
        + (num_ka << shift_bits[shogi::KA]) + (num_hi << shift_bits[shogi::HI])
        + (num_ki << shift_bits[shogi::KI])))
{
}

template <>
shogi::BitBoard shogi::BitBoard::get_attacks_by(
    const shogi::PieceEnum& p,
    const shogi::SquareEnum& sq,
    const shogi::BitBoard& occupied)
{
    switch (p) {
    case shogi::B_KY:
        return shogi::Magic::get_north_attack(sq, occupied);
    case shogi::W_KY:
        return shogi::Magic::get_south_attack(sq, occupied);
    case shogi::B_KA:
    case shogi::W_KA:
        return shogi::Magic::get_diagonal_attack(sq, occupied);
    case shogi::B_HI:
    case shogi::W_HI:
        return shogi::Magic::get_adjacent_attack(sq, occupied);
    case shogi::B_UM:
    case shogi::W_UM:
        return shogi::Magic::get_diagonal_attack(sq, occupied)
               | attacks_table[shogi::B_OU][sq];
    case shogi::B_RY:
    case shogi::W_RY:
        return shogi::Magic::get_adjacent_attack(sq, occupied)
               | attacks_table[shogi::B_OU][sq];
    default:
        return get_attacks_by(p, sq);
    }
}

} // namespace vshogi
