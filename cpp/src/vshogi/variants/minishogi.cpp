#include "vshogi/variants/minishogi.hpp"

namespace vshogi
{

template <>
minishogi::bitboard_t
minishogi::BitboardTraits::get_placeable(const minishogi::PieceEnum& p)
{
    constexpr bitboard_t table[C::num_colored_piece_types + 1u] = {
        mask & (~top_n_rank<1u>()), // B_FU
        mask, // B_GI
        mask, // B_KA
        mask, // B_HI
        mask, // B_KI
        mask, // B_OU
        mask, // B_TO
        mask, // B_NG
        mask, // B_UM
        mask, // B_RY
        mask & top_n_rank<4u>(), // W_FU
        mask, // W_GI
        mask, // W_KA
        mask, // W_HI
        mask, // W_KI
        mask, // W_OU
        mask, // W_TO
        mask, // W_NG
        mask, // W_UM
        mask, // W_RY
        mask, // VOID
    };
    return table[p];
}

template <>
minishogi::bitboard_t minishogi::BitboardTraits::get_attack_by(
    const vshogi::minishogi::PieceEnum& p,
    const vshogi::minishogi::SquareEnum& sq,
    const vshogi::minishogi::bitboard_t& occupied)
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
               | minishogi::BitboardTraits::get_attack_by(minishogi::B_OU, sq);
    case minishogi::B_RY:
    case minishogi::W_RY:
        return minishogi::Magic::get_adjacent_attack(sq, occupied)
               | minishogi::BitboardTraits::get_attack_by(minishogi::B_OU, sq);
    default:
        return get_attack_by(p, sq);
    }
}

template <>
template <>
minishogi::Stand::Stand(
    const int num_fu,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(
          static_cast<std::uint16_t>(
              (num_ki << shift_bits[minishogi::KI])
              + (num_hi << shift_bits[minishogi::HI])
              + (num_ka << shift_bits[minishogi::KA])
              + (num_gi << shift_bits[minishogi::GI])
              + (num_fu << shift_bits[minishogi::FU])))
{
}

} // namespace vshogi
