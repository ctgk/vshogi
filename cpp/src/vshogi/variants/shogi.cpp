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
    const shogi::ColoredPieceEnum& p,
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

template <>
shogi::BitBoard shogi::Board::get_occupied_by_ranging(const ColorEnum& c) const
{
    using namespace shogi;
    return get_occupied<KY, KA, HI, UM, RY>(c);
}

template <>
shogi::Move NonKingBoardMoveGenerator<shogi::Parameters>::random_select()
{
    using namespace shogi;
    const auto src_fgke = m_board.get_occupied<FU, GI, KE>(m_turn);
    const auto src_kkhi = m_board.get_occupied<KY, KA, HI>(m_turn);
    const auto src_gold = m_board.get_occupied<KI, TO, NY, NK, NG>(m_turn);
    const auto src_umry = m_board.get_occupied<UM, RY>(m_turn);
    const auto num_fgke = static_cast<float>(src_fgke.hamming_weight());
    const auto num_kkhi = static_cast<float>(src_kkhi.hamming_weight());
    const auto num_gold = static_cast<float>(src_gold.hamming_weight());
    const auto num_umry = static_cast<float>(src_umry.hamming_weight());
    const auto num_src = num_fgke + num_kkhi + num_gold + num_umry;
    float r = dist01(random_engine);
    const auto fraction_fgke = num_fgke / num_src;
    if (r < fraction_fgke) {
        auto iter_fgke = NonKingBoardMoveGenerator<Parameters>(
            m_state, src_fgke, m_pinned);
        if (!iter_fgke.is_end())
            return iter_fgke.random_select_by_iterating_all();
        auto iter_kkhi = NonKingBoardMoveGenerator<Parameters>(
            m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
        return iter_umry.random_select();
    }
    r -= fraction_fgke;
    const auto fraction_kkhi = num_kkhi / num_src;
    if (r < fraction_kkhi) {
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
        if (!iter_umry.is_end())
            return iter_umry.random_select();
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        return iter_fgke.random_select_by_iterating_all();
    }
    r -= fraction_kkhi;
    const auto fraction_gold = num_gold / num_src;
    if (r < fraction_gold) {
        auto iter_gold
            = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
        if (!iter_gold.is_end())
            return iter_gold.random_select();
        auto iter_umry
            = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
        if (!iter_umry.is_end())
            return iter_umry.random_select();
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        if (!iter_fgke.is_end())
            return iter_fgke.random_select_by_iterating_all();
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        return iter_kkhi.random_select_by_iterating_all();
    }
    {
        auto iter_umry
            = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
        if (!iter_umry.is_end())
            return iter_umry.random_select();
        auto iter_fgke = NonKingBoardMoveGenerator(m_state, src_fgke, m_pinned);
        if (!iter_fgke.is_end())
            return iter_fgke.random_select_by_iterating_all();
        auto iter_kkhi = NonKingBoardMoveGenerator(m_state, src_kkhi, m_pinned);
        if (!iter_kkhi.is_end())
            return iter_kkhi.random_select_by_iterating_all();
        auto iter_gold
            = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
        return iter_gold.random_select();
    }
}

} // namespace vshogi
