#ifndef VSHOGI_COMMON_ITERATOR_KING_HPP
#define VSHOGI_COMMON_ITERATOR_KING_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/iterator/iterator.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

template <class P, IterEnum IterType = IterEnum::LEGAL>
class KingMoveIterator
{
private:
    using C = Configuration<P>;
    using SHelper = Squares<P>;
    using BitboardSquareIterator = typename BitBoard<P>::BitboardSquareIterator;
    using Square = typename C::Square;

private:
    const Square m_src; //!< King square
    BitboardSquareIterator m_iter;

    KingMoveIterator() : m_src(C::SQ_NA), m_iter()
    {
    }

public:
    KingMoveIterator(const State<P>& state)
        : m_src(state.get_board().get_king_square(state.get_turn())), m_iter()
    {
        if constexpr (IterType == IterEnum::CHECK) {
            const auto t = state.get_turn();
            const auto& b = state.get_board();
            const auto enemy_king_sq = b.get_king_square(~t);
            const auto checker_dir = SHelper::direction(enemy_king_sq, m_src);
            const auto checker_sq
                = b.find_ranging_attacker(t, enemy_king_sq, checker_dir, m_src);
            if (checker_sq == C::SQ_NA) {
                return;
            } else {
                m_iter
                    = state
                          .compute_king_movable(~BitBoard<P>::get_line_segment(
                              checker_sq, enemy_king_sq))
                          .iterator();
            }
        } else {
            m_iter = state.compute_king_movable().iterator();
        }
    }
    KingMoveIterator& operator++()
    {
        ++m_iter;
        return *this;
    }
    Move<P> operator*() const
    {
        return Move<P>(m_src, *m_iter, false);
    }
    KingMoveIterator begin() const
    {
        return *this;
    }
    KingMoveIterator end() const
    {
        static const auto end_iter = KingMoveIterator();
        return end_iter;
    }
    bool operator!=(const KingMoveIterator& other) const
    {
        return m_iter != other.m_iter;
    }
    bool is_end() const
    {
        return m_iter.is_end();
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_ITERATOR_KING_HPP
