#include "vshogi/variants/shogi.hpp"

namespace vshogi
{

template <>
shogi::bitboard_t
shogi::BitboardTraits::get_placeable(const shogi::PieceEnum& p)
{
    constexpr bitboard_t table[C::num_colored_piece_types + 1u] = {
        mask & (~top_n_rank<1u>()), // B_FU
        mask & (~top_n_rank<1u>()), // B_KY
        mask & (~top_n_rank<2u>()), // B_KE
        mask, // B_GI
        mask, // B_KA
        mask, // B_HI
        mask, // B_KI
        mask, // B_OU
        mask, // B_TO
        mask, // B_NY
        mask, // B_NK
        mask, // B_NG
        mask, // B_UM
        mask, // B_RY
        mask & top_n_rank<8u>(), // W_FU
        mask & top_n_rank<8u>(), // W_KY
        mask & top_n_rank<7u>(), // W_KE
        mask, // W_GI
        mask, // W_KA
        mask, // W_HI
        mask, // W_KI
        mask, // W_OU
        mask, // W_TO
        mask, // W_NY
        mask, // W_NK
        mask, // W_NG
        mask, // W_UM
        mask, // W_RY
        mask, // VOID
    };
    return table[p];
}

template <>
shogi::bitboard_t shogi::BitboardTraits::get_attack_by(
    const shogi::PieceEnum& p,
    const shogi::SquareEnum& sq,
    const shogi::bitboard_t& occupied)
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
               | shogi::BitboardTraits::get_attack_by(shogi::B_OU, sq);
    case shogi::B_RY:
    case shogi::W_RY:
        return shogi::Magic::get_adjacent_attack(sq, occupied)
               | shogi::BitboardTraits::get_attack_by(shogi::B_OU, sq);
    default:
        return get_attack_by(p, sq);
    }
}

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
    : Stand(
          static_cast<std::uint32_t>(
              (num_fu << shift_bits[shogi::FU])
              + (num_ky << shift_bits[shogi::KY])
              + (num_ke << shift_bits[shogi::KE])
              + (num_gi << shift_bits[shogi::GI])
              + (num_ka << shift_bits[shogi::KA])
              + (num_hi << shift_bits[shogi::HI])
              + (num_ki << shift_bits[shogi::KI])))
{
}

} // namespace vshogi
