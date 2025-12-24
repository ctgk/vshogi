#include "vshogi/variants/judkins_shogi.hpp"

namespace vshogi
{

template <>
judkins_shogi::bitboard_t
judkins_shogi::BitboardTraits::get_placeable(const judkins_shogi::PieceEnum& p)
{
    constexpr bitboard_t table[C::num_colored_piece_types + 1u] = {
        mask & (~top_n_rank<1u>()), // B_FU
        mask & (~top_n_rank<2u>()), // B_KE
        mask, // B_GI
        mask, // B_KA
        mask, // B_HI
        mask, // B_KI
        mask, // B_OU
        mask, // B_TO
        mask, // B_NK
        mask, // B_NG
        mask, // B_UM
        mask, // B_RY
        mask & top_n_rank<5u>(), // W_FU
        mask & top_n_rank<4u>(), // W_KE
        mask, // W_GI
        mask, // W_KA
        mask, // W_HI
        mask, // W_KI
        mask, // W_OU
        mask, // W_TO
        mask, // W_NK
        mask, // W_NG
        mask, // W_UM
        mask, // W_RY
        mask, // VOID
    };
    return table[p];
}

template <>
judkins_shogi::bitboard_t judkins_shogi::BitboardTraits::get_attack_by(
    const judkins_shogi::PieceEnum& p,
    const judkins_shogi::SquareEnum& sq,
    const judkins_shogi::bitboard_t& occupied)
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
               | judkins_shogi::BitboardTraits::get_attack_by(
                   judkins_shogi::B_OU, sq);
    case judkins_shogi::B_RY:
    case judkins_shogi::W_RY:
        return judkins_shogi::Magic::get_adjacent_attack(sq, occupied)
               | judkins_shogi::BitboardTraits::get_attack_by(
                   judkins_shogi::B_OU, sq);
    default:
        return get_attack_by(p, sq);
    }
}

template <>
template <>
judkins_shogi::Stand::Stand(
    const int num_fu,
    const int num_ke,
    const int num_gi,
    const int num_ka,
    const int num_hi,
    const int num_ki)
    : Stand(
          static_cast<Int>(
              (num_fu << shift_bits[judkins_shogi::FU])
              + (num_ke << shift_bits[judkins_shogi::KE])
              + (num_gi << shift_bits[judkins_shogi::GI])
              + (num_ka << shift_bits[judkins_shogi::KA])
              + (num_hi << shift_bits[judkins_shogi::HI])
              + (num_ki << shift_bits[judkins_shogi::KI])))
{
}

} // namespace vshogi
