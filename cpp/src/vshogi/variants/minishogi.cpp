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
    const vshogi::minishogi::ColoredPieceEnum& p,
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

template <>
minishogi::Move
NonKingBoardMoveGenerator<minishogi::Parameters>::random_select()
{
    using namespace minishogi;
    const auto src_fugi = m_board.get_occupied<FU, GI>(m_turn);
    const auto src_kahi = m_board.get_occupied<KA, HI>(m_turn);
    const auto src_gold = m_board.get_occupied<KI, TO, NG>(m_turn);
    const auto src_umry = m_board.get_occupied<UM, RY>(m_turn);
    auto iter_fugi = NonKingBoardMoveGenerator(m_state, src_fugi, m_pinned);
    auto iter_kahi = NonKingBoardMoveGenerator(m_state, src_kahi, m_pinned);
    auto iter_gold
        = NoPromoMoveGenerator<Parameters>(m_state, src_gold, m_pinned);
    auto iter_umry
        = NoPromoMoveGenerator<Parameters>(m_state, src_umry, m_pinned);
    const auto num_fugi = iter_fugi.is_end()
                              ? 0.f
                              : static_cast<float>(src_fugi.hamming_weight());
    const auto num_kahi = iter_kahi.is_end()
                              ? 0.f
                              : static_cast<float>(src_kahi.hamming_weight());
    const auto num_gold = iter_gold.is_end()
                              ? 0.f
                              : static_cast<float>(src_gold.hamming_weight());
    const auto num_umry = iter_umry.is_end()
                              ? 0.f
                              : static_cast<float>(src_umry.hamming_weight());
    const auto num_src = num_fugi + num_kahi + num_gold + num_umry;
    float r = dist01(random_engine);
    const auto fraction_fugi = num_fugi / num_src;
    if (r < fraction_fugi)
        return iter_fugi.random_select_by_iterating_all();
    r -= fraction_fugi;
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
