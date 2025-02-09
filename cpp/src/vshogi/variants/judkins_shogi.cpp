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

template <>
judkins_shogi::BitBoard
judkins_shogi::Board::get_occupied_by_ranging(const ColorEnum& c) const
{
    using namespace judkins_shogi;
    return get_occupied<KA, HI, UM, RY>(c);
}

template <>
judkins_shogi::Move
NonKingBoardMoveGenerator<judkins_shogi::Parameters>::random_select()
{
    using namespace judkins_shogi;
    const auto src_fgke = m_board.get_occupied<FU, GI, KE>(m_turn);
    const auto src_kahi = m_board.get_occupied<KA, HI>(m_turn);
    const auto src_gold = m_board.get_occupied<KI, TO, NK, NG>(m_turn);
    const auto src_umry = m_board.get_occupied<UM, RY>(m_turn);
    auto iter_fgke
        = NonKingBoardMoveGenerator<Parameters>(m_state, src_fgke, m_pinned);
    auto iter_kahi
        = NonKingBoardMoveGenerator<Parameters>(m_state, src_kahi, m_pinned);
    auto iter_gold
        = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
    auto iter_umry
        = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
    const auto num_fgke = iter_fgke.is_end()
                              ? 0.f
                              : static_cast<float>(src_fgke.hamming_weight());
    const auto num_kahi = iter_kahi.is_end()
                              ? 0.f
                              : static_cast<float>(src_kahi.hamming_weight());
    const auto num_gold = iter_gold.is_end()
                              ? 0.f
                              : static_cast<float>(src_gold.hamming_weight());
    const auto num_umry = iter_umry.is_end()
                              ? 0.f
                              : static_cast<float>(src_umry.hamming_weight());
    const auto num_src = num_fgke + num_kahi + num_gold + num_umry;
    float r = dist01(random_engine);
    const auto fraction_fgke = num_fgke / num_src;
    if (r < fraction_fgke)
        return iter_fgke.random_select_by_iterating_all();
    r -= fraction_fgke;
    const auto fraction_kahi = num_kahi / num_src;
    if (r < fraction_kahi)
        return iter_kahi.random_select_by_iterating_all();
    r -= fraction_kahi;
    const auto fraction_gold = num_gold / num_src;
    if (r < fraction_gold)
        return iter_gold.random_select();
    return iter_umry.random_select();
}

} // namespace vshogi
