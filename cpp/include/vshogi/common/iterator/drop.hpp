#ifndef VSHOGI_COMMON_ITERATOR_DROP_HPP
#define VSHOGI_COMMON_ITERATOR_DROP_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

template <class P>
class DropEvasionIterator
{
private:
    using C = Configuration<P>;
    using S = Squares<P>;
    using PieceType = typename C::PieceType;
    using Square = typename C::Square;
    using SquareIterator = typename BitBoard<P>::SquareIterator;
    static constexpr auto pt_end
        = static_cast<PieceType>(C::num_stand_piece_types);

    const State<P>& m_state;
    const Square m_sq_end;
    const Square* m_sq_iter; //!< outer loop
    PieceType m_pt_iter; //!< inner loop

public:
    DropEvasionIterator(const State<P>& state)
        : m_state{state}, m_sq_end{state.get_checker_square()}, m_sq_iter{},
          m_pt_iter{pt_end}
    {
        assert(state.in_check());
        if (!state.can_apply_drop_move())
            return;
        const auto k = state.get_king_square();
        m_sq_iter = S::get_squares_along(S::get_direction(m_sq_end, k), k);
        assert(m_sq_iter != nullptr);
        if (*m_sq_iter == m_sq_end) {
            m_pt_iter = pt_end;
            return;
        }
        m_pt_iter = C::FU;
        while (true) {
            if (is_end_or_valid_move())
                break;
            increment();
        }
    }
    Move<P> operator*() const
    {
        assert(m_sq_iter != nullptr);
        return Move<P>(m_pt_iter, *m_sq_iter);
    }
    DropEvasionIterator& operator++()
    {
        m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
        while (true) {
            if (is_end_or_valid_move())
                break;
            increment();
        }
        return *this;
    }
    DropEvasionIterator begin()
    {
        return *this;
    }
    DropEvasionIterator end()
    {
        const auto end_iter
            = DropEvasionIterator(m_state, m_sq_end, m_sq_iter, pt_end);
        return end_iter;
    }
    bool operator!=(const DropEvasionIterator& other) const
    {
        return (m_sq_iter != other.m_sq_iter) || (m_pt_iter != other.m_pt_iter);
    }
    bool is_end() const
    {
        return (m_sq_iter == nullptr)
               || ((*m_sq_iter == m_sq_end) && (m_pt_iter == pt_end));
    }

private:
    DropEvasionIterator(
        const State<P>& state,
        const Square sq_end,
        const Square* const sq_iter,
        const PieceType pt)
        : m_state(state), m_sq_end{sq_end}, m_sq_iter{sq_iter}, m_pt_iter{pt}
    {
        if (m_sq_iter == nullptr)
            return;
        while (*m_sq_iter != m_sq_end) {
            ++m_sq_iter;
        }
    }
    bool is_end_or_valid_move() const
    {
        if (is_end())
            return true;
        if ((*m_sq_iter == m_sq_end) || (m_pt_iter == pt_end))
            return false;

        const ColorEnum turn = m_state.get_turn();
        const Square dst = *m_sq_iter;
        const Stand<P>& stand = m_state.get_stand();
        if (!stand.exist(m_pt_iter))
            return false;
        const auto p = Pieces<P>::to_board_piece(turn, m_pt_iter);
        const DirectionEnum* const dirs = Pieces<P>::get_attack_directions(p);
        if (dirs[1] == DIR_NA) { // FU or KY
            if (S::to_rank(dst) == (turn == BLACK ? C::RANK_A : C::RANK_Z))
                return false;
            const Board<P>& b = m_state.get_board();
            if (m_pt_iter == C::FU) {
                if (b.has_pawn_in_file(S::to_file(dst), turn))
                    return false;
                if (b.is_drop_pawn_mate_square(dst, turn))
                    return false;
            }
        } else if (dirs[2] == DIR_NA) { // KE
            static_assert(
                (DIR_SSW < DIR_NNW) && (DIR_SSE < DIR_NNW)
                && (DIR_NNW < DIR_NNE));
            if (dirs[0] >= DIR_NNW)
                return S::to_rank(dst) > C::RANK_B;
            else
                return S::to_rank(dst) < C::RANK_Y;
        }
        return true;
    }
    void increment()
    {
        // for (sq : m_sq_iter) // outer loop
        //     for (pt : m_pt_iter) // inner loop
        m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
        if (m_pt_iter < pt_end)
            return;
        ++m_sq_iter;
        if (*m_sq_iter != m_sq_end)
            m_pt_iter = C::FU;
        else
            m_pt_iter = pt_end;
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_ITERATOR_DROP_HPP
