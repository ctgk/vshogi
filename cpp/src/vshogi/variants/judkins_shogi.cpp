#include "vshogi/variants/judkins_shogi.hpp"

namespace vshogi
{

template <>
template <>
judkins_shogi::Stand::Stand(
    const int num_fu,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<Int>(
        (num_fu << shift_bits[judkins_shogi::FU])
        + (num_ke << shift_bits[judkins_shogi::KE])
        + (num_gi << shift_bits[judkins_shogi::GI])
        + (num_ka << shift_bits[judkins_shogi::KA])
        + (num_hi << shift_bits[judkins_shogi::HI])
        + (num_ki << shift_bits[judkins_shogi::KI])))
{
}

template <>
judkins_shogi::BitBoard judkins_shogi::BitBoard::get_attacks_by(
    const judkins_shogi::ColoredPieceEnum& p,
    const judkins_shogi::SquareEnum& sq,
    const judkins_shogi::BitBoard& occupied)
{
    switch (p) {
    case judkins_shogi::B_KA:
    case judkins_shogi::W_KA:
        return judkins_shogi::Magic::get_diagonal_attack(sq, occupied);
    case judkins_shogi::B_HI:
    case judkins_shogi::W_HI:
        return judkins_shogi::Magic::get_adjacent_attack(sq, occupied);
    case judkins_shogi::B_UM:
    case judkins_shogi::W_UM:
        return judkins_shogi::Magic::get_diagonal_attack(sq, occupied)
               | attacks_table[judkins_shogi::B_OU][sq];
    case judkins_shogi::B_RY:
    case judkins_shogi::W_RY:
        return judkins_shogi::Magic::get_adjacent_attack(sq, occupied)
               | attacks_table[judkins_shogi::B_OU][sq];
    default:
        return get_attacks_by(p, sq);
    }
}

} // namespace vshogi
