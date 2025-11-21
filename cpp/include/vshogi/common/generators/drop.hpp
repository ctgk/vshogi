#ifndef VSHOGI_COMMON_GENERATORS_DROP_HPP
#define VSHOGI_COMMON_GENERATORS_DROP_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/generators/gentype.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

/**
 *
 * DropMoveGenerator<LEGAL>
 * for (pt : piece_types)
 *     for (dst : destinations)
 *
 * DropMoveGenerator<CHECK>
 * for (pt : piece_types)
 *     for (dst : destinations)
 *
 * DropMoveGenerator<EVADE>
 * for (dst : destinations)
 *     for (pt : piece_types)
 */

template <class P, GenEnum GenType = GenEnum::LEGAL>
class DropMoveGenerator
{
private:
    using C = Configuration<P>;
    using BitSquareIterator = typename BitBoard<P>::Iterator;
    using PieceType = typename C::PieceType;
    using Square = typename C::Square;
    using PT = PieceTraits<P>;

private:
    const State<P>& m_state;
    const ColorEnum m_turn;
    const Stand<P>& m_stand;
    BitSquareIterator m_sq_iter; //!< inner loop
    PieceType m_pt_iter; //!< outer loop

public:
    DropMoveGenerator(const State<P>& state)
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
    DropMoveGenerator(
        const State<P>& state, const PieceType pt, const Square sq)
        : m_state(state), m_turn(state.get_turn()),
          m_stand(state.get_stand()), m_sq_iter{}, m_pt_iter{pt}
    {
        if (!state.can_apply_drop_move()) {
            m_pt_iter = static_cast<PieceType>(C::num_stand_piece_types);
            return;
        }
        increment_piece_type_unless_in_stand();
        if (m_pt_iter == C::num_stand_piece_types)
            return;
        init_sq_iter(sq);
        increment_piece_type_while_no_dst();
    }
    DropMoveGenerator& operator++()
    {
        ++m_sq_iter;
        increment_piece_type_while_no_dst();
        return *this;
    }
    Move<P> operator*() const
    {
        return Move<P>(m_pt_iter, *m_sq_iter);
    }
    operator bool() const
    {
        return m_sq_iter; // && (m_pt_iter != C::num_stand_piece_types);
    }

private:
    void init_sq_iter()
    {
        const auto& b = m_state.get_board();
        const auto p = PT::make_piece(m_turn, m_pt_iter);
        if (m_state.in_check()) {
            const auto mask = BitBoard<P>::get_line_segment(
                m_state.find_checker_square(), b.get_king_square(m_turn));
            m_sq_iter = b.template compute_droppable<GenType == GenEnum::CHECK>(
                             p, mask)
                            .iterator();
        } else {
            m_sq_iter
                = b.template compute_droppable<GenType == GenEnum::CHECK>(p)
                      .iterator();
        }
    }
    void init_sq_iter(const Square begin)
    {
        init_sq_iter();
        while (m_sq_iter) {
            if (*m_sq_iter < begin)
                ++m_sq_iter;
            else
                break;
        }
    }
    void increment_piece_type_while_no_dst()
    {
        while (!m_sq_iter) {
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
class DropMoveGenerator<P, GenEnum::EVADE>
{
private:
    using C = Configuration<P>;
    using ST = SquareTraits<P>;
    using PT = PieceTraits<P>;
    using PieceType = typename C::PieceType;
    using Square = typename C::Square;
    using BitSquareIterator = typename BitBoard<P>::Iterator;
    static constexpr auto pt_end
        = static_cast<PieceType>(C::num_stand_piece_types);

    const State<P>& m_state;
    const Square m_sq_end; //!< exclusive
    const Square* m_sq_iter; //!< outer loop
    PieceType m_pt_iter; //!< inner loop

public:
    DropMoveGenerator(const State<P>& state)
        : m_state{state}, m_sq_end{state.find_checker_square()}, m_sq_iter{},
          m_pt_iter{pt_end}
    {
        assert(state.in_check());
        if (!state.can_apply_drop_move())
            return;
        const auto k = state.get_king_square();
        m_sq_iter = ST::ray_from(k, state.get_checker_dir());
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
    DropMoveGenerator& operator++()
    {
        m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
        while (true) {
            if (is_end_or_valid_move())
                break;
            increment();
        }
        return *this;
    }
    operator bool() const
    {
        return m_sq_iter && (*m_sq_iter != m_sq_end);
    }

private:
    bool is_end_or_valid_move() const
    {
        if (!operator bool())
            return true;
        if ((*m_sq_iter == m_sq_end) || (m_pt_iter == pt_end))
            return false;

        const ColorEnum turn = m_state.get_turn();
        const Square dst = *m_sq_iter;
        const Stand<P>& stand = m_state.get_stand();
        if (!stand.exist(m_pt_iter))
            return false;
        const auto p = PT::make_piece(turn, m_pt_iter);
        const DirectionEnum* const dirs = PT::get_attack_directions(p);
        if (dirs[1] == DIR_NA) { // FU or KY
            if (ST::to_rank(dst) == (turn == BLACK ? C::RANK_A : C::RANK_Z))
                return false;
            const Board<P>& b = m_state.get_board();
            if (m_pt_iter == C::FU) {
                if (b.has_pawn_in_file(ST::to_file(dst), turn))
                    return false;
                if (b.is_drop_pawn_mate_square(dst, turn))
                    return false;
            }
        } else if (dirs[2] == DIR_NA) { // KE
            static_assert(
                (DIR_SSW < DIR_NNW) && (DIR_SSE < DIR_NNW)
                && (DIR_NNW < DIR_NNE));
            if (dirs[0] >= DIR_NNW)
                return ST::to_rank(dst) > C::RANK_B;
            else
                return ST::to_rank(dst) < C::RANK_Y;
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

#endif // VSHOGI_COMMON_GENERATORS_DROP_HPP
