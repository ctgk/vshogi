#ifndef VSHOGI_COMMON_ITERATOR_DROP_HPP
#define VSHOGI_COMMON_ITERATOR_DROP_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/iterator/iterator.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

/**
 *
 * DropMoveIterator<LEGAL>
 * for (pt : piece_types)
 *     for (dst : destinations)
 *
 * DropMoveIterator<CHECK>
 * for (pt : piece_types)
 *     for (dst : destinations)
 *
 * DropMoveIterator<EVADE>
 * for (dst : destinations)
 *     for (pt : piece_types)
 */

template <class P, IterEnum IterType = IterEnum::LEGAL>
class DropMoveIterator
{
private:
    using C = Configuration<P>;
    using SquareIterator = typename BitBoard<P>::SquareIterator;
    using PieceType = typename C::PieceType;
    using PHelper = Pieces<P>;

private:
    const State<P>& m_state;
    const ColorEnum m_turn;
    const Stand<P>& m_stand;
    SquareIterator m_sq_iter;
    PieceType m_pt_iter;

public:
    DropMoveIterator(const State<P>& state)
        : m_state(state), m_turn(state.get_turn()),
          m_stand(state.get_stand()), m_sq_iter{}, m_pt_iter{}
    {
        if (!state.can_apply_drop_move()) {
            m_pt_iter = static_cast<PieceType>(C::num_stand_piece_types);
            return;
        }
        increment_piece_type_unless_in_stand();
        if (m_pt_iter == C::num_stand_piece_types)
            return;
        init_sq_iter();
        increment_piece_type_while_no_dst();
    }
    DropMoveIterator& operator++()
    {
        ++m_sq_iter;
        increment_piece_type_while_no_dst();
        return *this;
    }
    Move<P> operator*() const
    {
        return Move<P>(m_pt_iter, *m_sq_iter);
    }
    DropMoveIterator begin()
    {
        return *this;
    }
    DropMoveIterator end()
    {
        static const auto end_iter = DropMoveIterator(
            m_state, static_cast<PieceType>(C::num_stand_piece_types));
        return end_iter;
    }
    bool operator!=(const DropMoveIterator& other) const
    {
        return (m_sq_iter != other.m_sq_iter) || (m_pt_iter != other.m_pt_iter);
    }
    bool is_end() const
    {
        return m_sq_iter.is_end() && (m_pt_iter == C::num_stand_piece_types);
    }

private:
    DropMoveIterator(const State<P>& state, const PieceType pt)
        : m_state(state), m_turn(state.get_turn()), m_stand(state.get_stand()),
          m_sq_iter(), m_pt_iter(pt)
    {
    }
    void init_sq_iter()
    {
        const auto& b = m_state.get_board();
        const auto p = PHelper::to_board_piece(m_turn, m_pt_iter);
        if (m_state.in_check()) {
            m_sq_iter
                = b.template compute_droppable<IterType == IterEnum::CHECK>(
                       p,
                       BitBoard<P>::get_line_segment(
                           m_state.get_checker_square(),
                           b.get_king_square(m_turn)))
                      .square_iterator();
        } else {
            m_sq_iter
                = b.template compute_droppable<IterType == IterEnum::CHECK>(p)
                      .square_iterator();
        }
    }
    void increment_piece_type_while_no_dst()
    {
        while (m_sq_iter.is_end()) {
            m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
            increment_piece_type_unless_in_stand();
            if (m_pt_iter >= C::num_stand_piece_types)
                break;
            init_sq_iter();
        }
    }
    void increment_piece_type_unless_in_stand()
    {
        while ((m_pt_iter < C::num_stand_piece_types)
               && !m_stand.exist(m_pt_iter)) {
            m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
        }
    }
};

template <class P>
class DropMoveIterator<P, IterEnum::EVADE>
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
    DropMoveIterator(const State<P>& state)
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
    DropMoveIterator& operator++()
    {
        m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
        while (true) {
            if (is_end_or_valid_move())
                break;
            increment();
        }
        return *this;
    }
    DropMoveIterator begin()
    {
        return *this;
    }
    DropMoveIterator end()
    {
        const auto end_iter
            = DropMoveIterator(m_state, m_sq_end, m_sq_iter, pt_end);
        return end_iter;
    }
    bool operator!=(const DropMoveIterator& other) const
    {
        return (m_sq_iter != other.m_sq_iter) || (m_pt_iter != other.m_pt_iter);
    }
    bool is_end() const
    {
        return (m_sq_iter == nullptr)
               || ((*m_sq_iter == m_sq_end) && (m_pt_iter == pt_end));
    }

private:
    DropMoveIterator(
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
