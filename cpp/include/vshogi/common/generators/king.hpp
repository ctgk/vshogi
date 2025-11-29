#ifndef VSHOGI_COMMON_GENERATORS_KING_HPP
#define VSHOGI_COMMON_GENERATORS_KING_HPP

#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/generators/gentype.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

template <class P, GenEnum GenType = GenEnum::LEGAL>
class KingMoveGenerator
{
private:
    using C = Configuration<P>;
    using BT = BitboardTraits<P>;
    using ST = SquareTraits<P>;
    using Square = typename C::Square;
    using bitboard_t = typename C::bitboard_t;

private:
    const Square m_src; //!< King square
    typename BT::Iterator m_iter;

public:
    KingMoveGenerator(const State<P>& state);
    KingMoveGenerator& operator++()
    {
        ++m_iter;
        return *this;
    }
    Move<P> operator*() const
    {
        return Move<P>(m_src, *m_iter, false);
    }
    operator bool() const
    {
        return m_iter;
    }

private:
    static bitboard_t compute_movable(const State<P>& s);
};

template <class P, GenEnum GenType>
KingMoveGenerator<P, GenType>::KingMoveGenerator(const State<P>& state)
    : m_src(state.get_board().get_king_square(state.get_turn())),
      m_iter(BT::iterator(compute_movable(state)))
{
}

template <class P, GenEnum GenType>
typename Configuration<P>::bitboard_t
KingMoveGenerator<P, GenType>::compute_movable(const State<P>& s)
{
    const auto t = s.get_turn();
    const Board<P>& b = s.get_board();
    const auto src = b.get_king_square(t);
    if (src == C::SQ_NA)
        return static_cast<bitboard_t>(0);

    if constexpr (GenType == GenEnum::CHECK) {
        const auto enemy_king_sq = b.get_king_square(~t);
        const auto checker_dir = ST::direction(enemy_king_sq, src);
        const auto checker_sq
            = b.find_sliding_attacker(t, enemy_king_sq, checker_dir, src);
        if (checker_sq == C::SQ_NA) {
            return static_cast<bitboard_t>(0);
        } else {
            const auto m = BT::get_mask_between(checker_sq, enemy_king_sq);
            return b.compute_king_movable(t, ~m);
        }
    } else {
        return b.compute_king_movable(t);
    }
}

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATORS_KING_HPP
