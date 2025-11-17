#include "vshogi/variants/minishogi.hpp"

namespace vshogi
{

template <>
template <>
minishogi::Stand::Stand(
    const int num_fu,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(static_cast<std::uint16_t>(
        (num_ki << shift_bits[minishogi::KI])
        + (num_hi << shift_bits[minishogi::HI])
        + (num_ka << shift_bits[minishogi::KA])
        + (num_gi << shift_bits[minishogi::GI])
        + (num_fu << shift_bits[minishogi::FU])))
{
}

template <>
minishogi::BitBoard minishogi::BitBoard::get_attacks_by(
    const vshogi::minishogi::PieceEnum& p,
    const vshogi::minishogi::SquareEnum& sq,
    const vshogi::minishogi::BitBoard& occupied)
{
    switch (p) {
    case minishogi::B_KA:
    case minishogi::W_KA:
        return minishogi::Magic::get_diagonal_attack(sq, occupied);
    case minishogi::B_HI:
    case minishogi::W_HI:
        return minishogi::Magic::get_adjacent_attack(sq, occupied);
    case minishogi::B_UM:
    case minishogi::W_UM:
        return minishogi::Magic::get_diagonal_attack(sq, occupied)
               | attacks_table[minishogi::B_OU][sq];
    case minishogi::B_RY:
    case minishogi::W_RY:
        return minishogi::Magic::get_adjacent_attack(sq, occupied)
               | attacks_table[minishogi::B_OU][sq];
    default:
        return get_attacks_by(p, sq);
    }
}

} // namespace vshogi
