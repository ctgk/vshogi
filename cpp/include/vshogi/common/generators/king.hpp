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

private:
    const Square m_src; //!< King square
    typename BT::Iterator m_iter;

public:
    KingMoveGenerator(const State<P>& state)
        : m_src(state.get_board().get_king_square(state.get_turn())), m_iter()
    {
        if constexpr (GenType == GenEnum::CHECK) {
            const auto t = state.get_turn();
            const auto& b = state.get_board();
            const auto enemy_king_sq = b.get_king_square(~t);
            const auto checker_dir = ST::direction(enemy_king_sq, m_src);
            const auto checker_sq
                = b.find_sliding_attacker(t, enemy_king_sq, checker_dir, m_src);
            if (checker_sq == C::SQ_NA) {
                return;
            } else {
                const auto m = BT::invert(
                    BT::get_mask_between(checker_sq, enemy_king_sq));
                m_iter = BT::iterator(state.compute_king_movable(m));
            }
        } else {
            m_iter = BT::iterator(state.compute_king_movable());
        }
    }
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
};

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATORS_KING_HPP
