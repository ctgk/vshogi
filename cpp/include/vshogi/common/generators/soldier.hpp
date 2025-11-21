#ifndef VSHOGI_COMMON_GENERATORS_SOLDIER_HPP
#define VSHOGI_COMMON_GENERATORS_SOLDIER_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/generators/gentype.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

/**
 * - source
 * - destination
 * - promotion
 * - piece type (optional)
 *
 * SoldierMoveGenerator<LEGAL>
 * for (src : sources)
 *     for (dst : destinations)
 *         for (prm : promotions)
 *
 * SoldierMoveGenerator<CHECK>
 * for (src : sources)
 *     for (prm : promotions)
 *         for (dst : destinations)
 *
 *
 * // Current implementation
 * SoldierMoveGenerator<EVADE>
 * for (dst : destinations)
 *     for (src : sources) // needs to keep pinned map
 *         for (prm : promotions)
 *
 * // Possibly bad implementation
 * SoldierMoveGenerator<EVADE>
 * for (src : sources) // too many sources
 *     for (dst : destinations)
 *         for (prm : promotions)
 */

template <class P, GenEnum GenType = GenEnum::LEGAL>
class SoldierMoveGenerator;

template <class P>
class SoldierMoveGenerator<P, GenEnum::LEGAL>
{
private:
    using C = Configuration<P>;
    using Square = typename C::Square;
    using PT = PieceTraits<P>;
    using ST = SquareTraits<P>;

private:
    const State<P>& m_state;
    const ColorEnum m_turn;
    const Board<P>& m_board;
    const BitBoard<P> m_pinned;
    typename BitBoard<P>::Iterator m_src_iter;
    typename BitBoard<P>::Iterator m_dst_iter;
    bool m_promote;

public:
    SoldierMoveGenerator(const State<P>& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(m_state.find_pinned()), m_src_iter(), m_dst_iter(),
          m_promote(true)
    {
        if (m_state.in_double_check())
            return;
        init_no_check();
    }
    SoldierMoveGenerator(const State<P>& state, const Move<P>& move)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(m_state.find_pinned()), m_src_iter(), m_dst_iter(),
          m_promote(true)
    {
        if (m_state.in_double_check())
            return;
        init_no_check(move.source_square(), move.destination(), move.promote());
    }
    SoldierMoveGenerator(const State<P>& state, const BitBoard<P>& src_mask)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(state.find_pinned()), m_src_iter(), m_dst_iter(),
          m_promote(true)
    {
        if (m_state.in_double_check())
            return;
        init_src_iter(src_mask);
        while (m_src_iter) {
            init_dst_iter();
            if (m_dst_iter)
                break;
            ++m_src_iter;
        }
        init_promote();
    }
    SoldierMoveGenerator& operator++()
    {
        if (!m_promote) {
            const auto src = *m_src_iter;
            const auto dst = *m_dst_iter;
            const auto p = m_board[src];
            if (PT::is_promotable(p)
                && (ST::in_promotion_zone(src, m_turn)
                    || ST::in_promotion_zone(dst, m_turn))) {
                m_promote = true;
                return *this;
            }
        }

        m_promote = false;
        ++m_dst_iter;
        if (m_dst_iter) {
            init_promote();
            return *this;
        }

        ++m_src_iter;
        while (m_src_iter) {
            init_dst_iter();
            if (!m_dst_iter)
                ++m_src_iter;
            else {
                init_promote();
                return *this;
            }
        }

        m_promote = true;
        return *this;
    }
    Move<P> operator*() const
    {
        return Move<P>(*m_src_iter, *m_dst_iter, m_promote);
    }
    operator bool() const
    {
        return m_src_iter; // && m_dst_iter;
    }

private:
    void init_no_check()
    {
        init_src_iter();
        while (m_src_iter) {
            init_dst_iter();
            if (m_dst_iter)
                break;
            ++m_src_iter;
        }
        init_promote();
    }
    void init_no_check(
        const Square src_begin,
        const Square dst_begin,
        const bool promote_begin)
    {
        init_src_iter(src_begin);
        while (m_src_iter) {
            init_dst_iter(dst_begin);
            if (m_dst_iter)
                break;
            ++m_src_iter;
        }
        init_promote(promote_begin);
    }
    void init_src_iter()
    {
        const auto king_sq = m_board.get_king_square(m_turn);
        auto src_mask = m_board.get_occupied(m_turn);
        src_mask.clear(king_sq);
        m_src_iter = src_mask.iterator();
    }
    void init_src_iter(const Square begin)
    {
        init_src_iter();
        while (m_src_iter) {
            if (*m_src_iter < begin)
                ++m_src_iter;
            else
                break;
        }
    }
    void init_src_iter(const BitBoard<P>& src_mask)
    {
        m_src_iter = src_mask.iterator();
    }
    void init_dst_iter()
    {
        const auto src = *m_src_iter;
        const auto king_sq = m_board.get_king_square(m_turn);
        auto movable = m_board.get_attacks_by_nocheck(src);
        movable &= ~m_board.get_occupied(m_turn);
        if (!movable.any())
            goto ExitLabel;

        if (m_state.in_check()) {
            const auto checker_sq = m_state.find_checker_square();
            movable &= BitBoard<P>::get_line_segment(checker_sq, king_sq)
                           .set(checker_sq);
            if (!movable.any())
                goto ExitLabel;
        }
        if (m_pinned.is_one(src)) {
            const auto dir = ST::direction(king_sq, src);
            assert((dir < 8) || (dir == DIR_NA));
            movable &= BitBoard<P>::get_ray_to(king_sq, dir);
        }
    ExitLabel:
        m_dst_iter = movable.iterator();
    }
    void init_dst_iter(const Square begin)
    {
        init_dst_iter();
        while (m_dst_iter) {
            if (*m_dst_iter < begin)
                ++m_dst_iter;
            else
                break;
        }
    }
    void init_promote()
    {
        if (!m_src_iter || !m_dst_iter) {
            m_promote = true;
            return;
        }

        m_promote = false;
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        if (BitBoard<P>::get_attacks_by(p, *m_dst_iter).any())
            return;
        m_promote = true;
    }
    void init_promote(const bool begin)
    {
        init_promote();
        if (m_promote < begin)
            operator++();
    }
};

template <class P>
class SoldierMoveGenerator<P, GenEnum::CHECK>
{
private:
    using C = Configuration<P>;
    using Piece = typename C::Piece;
    using Square = typename C::Square;
    using PT = PieceTraits<P>;
    using ST = SquareTraits<P>;

private:
    const State<P>& m_state;
    const ColorEnum m_turn;
    const Board<P>& m_board;
    const BitBoard<P> m_pinned;
    const BitBoard<P> m_cover;
    typename BitBoard<P>::Iterator m_src_iter;
    typename BitBoard<P>::Iterator m_dst_iter;
    bool m_promote;
    BitBoard<P> m_dst_mask;

public:
    SoldierMoveGenerator(const State<P>& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(state.find_pinned()), m_cover(compute_cover(state)),
          m_src_iter(), m_dst_iter(), m_promote(true), m_dst_mask()
    {
        if (state.in_double_check())
            return;
        init_src_iter();
        while (m_src_iter) {
            init_dst_mask();
            if (m_dst_mask.any()) {
                m_promote = false;
                init_dst_iter_nopromo();
                if (m_dst_iter)
                    return;
                m_promote = true;
                init_dst_iter_promotion();
                if (m_dst_iter)
                    return;
            }
            ++m_src_iter;
        }
    }
    SoldierMoveGenerator& operator++()
    {
        ++m_dst_iter;
        if (m_dst_iter)
            return *this;

        if (m_promote == false) {
            m_promote = true;
            init_dst_iter_promotion();
            if (m_dst_iter)
                return *this;
        }
        ++m_src_iter;

        while (m_src_iter) {
            init_dst_mask();
            if (m_dst_mask.any()) {
                m_promote = false;
                init_dst_iter_nopromo();
                if (m_dst_iter)
                    return *this;
                m_promote = true;
                init_dst_iter_promotion();
                if (m_dst_iter)
                    return *this;
            }
            ++m_src_iter;
        }
        return *this;
    }
    Move<P> operator*() const
    {
        return Move<P>(*m_src_iter, *m_dst_iter, m_promote);
    }
    operator bool() const
    {
        return m_src_iter;
    }

private:
    void init_src_iter()
    {
        const auto target = m_board.get_king_square(~m_turn);
        assert(target != C::SQ_NA);

        BitBoard<P> src_mask = m_cover;
        for (auto pt : C::piece_type_iterator()) {
            if (pt == C::OU)
                continue;
            const auto p = PT::make_piece(m_turn, pt);
            src_mask |= m_board.get_occupied(pt)
                        & BitBoard<P>::get_neighbor_2nd(target, p);
        }
        src_mask &= m_board.get_occupied(m_turn);
        m_src_iter = src_mask.iterator();
    }
    void init_dst_mask()
    {
        const auto src = *m_src_iter;
        const auto king_sq = m_board.get_king_square(m_turn);
        m_dst_mask = m_board.get_attacks_by_nocheck(src);
        m_dst_mask &= ~m_board.get_occupied(m_turn);
        update_dst_mask_by_current_check(king_sq);
        update_dst_mask_by_counter_check(src, king_sq);
    }
    void init_dst_iter_promotion()
    {
        assert(m_promote);
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        if (!PT::is_promotable(p)) {
            return;
        }
        auto movable = m_dst_mask;
        if (update_mask_by_promotion(movable, src)) {
            if (!movable.any()) {
                return;
            }
        }
        update_mask_by_forcing_check(movable, p, src);
        m_dst_iter = movable.iterator();
    }
    void init_dst_iter_nopromo()
    {
        assert(!m_promote);
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        BitBoard<P> movable = m_dst_mask;
        update_mask_by_forcing_check(movable, p, src);
        if (movable.any()) {
            update_mask_by_nopromo(movable, p);
        }
        m_dst_iter = movable.iterator();
    }
    bool update_mask_by_promotion(BitBoard<P>& mask, const Square src)
    {
        assert(m_promote);
        if (!ST::in_promotion_zone(src, m_turn)) {
            mask &= BitBoard<P>::get_promotion_zone(m_turn);
            return true;
        }
        return false;
    }
    void update_mask_by_nopromo(BitBoard<P>& mask, const Piece p)
    {
        mask &= BitBoard<P>::compute_droppable(p);
    }
    void update_dst_mask_by_current_check(const Square king_sq)
    {
        if (m_state.in_check()) {
            const auto checker_sq = m_state.find_checker_square();
            assert(checker_sq != C::SQ_NA);
            m_dst_mask &= BitBoard<P>::get_line_segment(checker_sq, king_sq)
                              .set(checker_sq);
        }
    }
    void
    update_dst_mask_by_counter_check(const Square src, const Square king_sq)
    {
        if (m_pinned.is_one(src)) {
            const auto dir = ST::direction(king_sq, src);
            assert((dir < 8) || (dir == DIR_NA));
            m_dst_mask &= BitBoard<P>::get_ray_to(king_sq, dir);
        }
    }
    void update_mask_by_forcing_check(
        BitBoard<P>& mask, const Piece p, const Square& src)
    {
        const auto enemy_king_sq = m_board.get_king_square(~m_turn);
        auto pt = PT::to_piece_type(p);
        if (m_promote)
            pt = PT::promote_nocheck(pt);
        const auto atk = BitBoard<P>::get_attacks_by(
            PT::make_piece(~m_turn, pt), enemy_king_sq, m_board.get_occupied());
        if (m_cover.is_one(src)) {
            const auto dir = ST::direction(enemy_king_sq, src);
            mask &= atk | (~BitBoard<P>::get_ray_to(enemy_king_sq, dir));
        } else {
            mask &= atk;
        }
    }
    static BitBoard<P> compute_cover(const State<P>& s)
    {
        if (s.in_double_check())
            return BitBoard<P>();
        return s.get_board()
            .find_cover(s.get_turn())
            .clear(s.get_king_square());
    }
};

template <class P>
class SoldierMoveGeneratorEvade
{
private:
    using C = Configuration<P>;
    using ST = SquareTraits<P>;
    using PT = PieceTraits<P>;
    using Square = typename C::Square;
    using DirIter = EnumIterator<DirectionEnum, C::num_dir>;

    const State<P>& m_state;
    const BitBoard<P> m_src_mask;
    const Square m_dst_last; // inclusive
    const Square* m_dst_iter; //!< outer loop
    DirIter m_src_dir_iter; //!< inner loop
    Square m_src;
    bool m_promote; //!< most inner loop

public:
    SoldierMoveGeneratorEvade(const State<P>& state)
        : m_state{state}, m_src_mask{compute_src_mask(state)},
          m_dst_last{state.find_checker_square()},
          m_dst_iter{
              ST::ray_from(state.get_king_square(), state.get_checker_dir())},
          m_src_dir_iter{}, m_src{C::SQ_NA}, m_promote{}
    {
        assert(state.in_check());
        if (state.in_double_check()) {
            m_dst_iter = nullptr;
            return;
        }
        while (true) {
            init_src_dir_iter();
            if (!m_src_dir_iter.is_end())
                break;

            if (*m_dst_iter == m_dst_last) {
                m_dst_iter = nullptr;
                break;
            }
            ++m_dst_iter;
        }
        init_promote();
    }
    Move<P> operator*() const
    {
        assert(m_dst_iter != nullptr);
        return Move<P>(m_src, *m_dst_iter, m_promote);
    }
    SoldierMoveGeneratorEvade& operator++()
    {
        if (m_promote) {
            const auto p = m_state.get_board()[m_src];
            if (BitBoard<P>::get_attacks_by(p, *m_dst_iter).any()) {
                m_promote = false;
                return *this;
            }
        }

        ++m_src_dir_iter;
        find_src_or_increment_src_dir_iter();
        if (!m_src_dir_iter.is_end()) {
            init_promote();
            return *this;
        }

        while (true) {
            if (*m_dst_iter == m_dst_last) {
                m_dst_iter = nullptr;
                break;
            }
            ++m_dst_iter;
            init_src_dir_iter();
            if (!m_src_dir_iter.is_end())
                break;
        }
        init_promote();
        return *this;
    }
    operator bool() const
    {
        return m_dst_iter;
    }

private:
    void init_src_dir_iter()
    {
        m_src_dir_iter.reset();
        find_src_or_increment_src_dir_iter();
    }
    void find_src_or_increment_src_dir_iter()
    {
        const auto t = m_state.get_turn();
        const auto& b = m_state.get_board();
        while (!m_src_dir_iter.is_end()) {
            const auto src = b.find_attacker(t, *m_dst_iter, *m_src_dir_iter);
            if (m_src_mask.is_one(src)) {
                m_src = src;
                return;
            }
            ++m_src_dir_iter;
        }
        m_src = C::SQ_NA;
    }
    void init_promote()
    {
        if ((m_dst_iter == nullptr) || (m_src == C::SQ_NA)) {
            m_promote = false;
            return;
        }
        const auto dst = *m_dst_iter;
        const auto t = m_state.get_turn();
        const auto p = m_state.get_board()[m_src];
        m_promote = PT::is_promotable(p)
                    && (ST::in_promotion_zone(dst, t)
                        || ST::in_promotion_zone(m_src, t));
    }
    static BitBoard<P> compute_src_mask(const State<P>& state)
    {
        return ~state.find_pinned().set(state.get_king_square());
    }
};

/**
 * @brief Another implementation of `SoldierMoveGenerator<P, EVADE>`.
 * This is as fast as (or as slow as `SoldierMoveGenerator<P, EVADE>`)
 * using SquareIterator instead of array of squares.
 */
template <class P>
class SoldierMoveGenerator<P, GenEnum::EVADE>
{
private:
    using C = Configuration<P>;
    using PT = PieceTraits<P>;
    using ST = SquareTraits<P>;
    using Square = typename C::Square;

    const Board<P>& m_board;
    const ColorEnum m_turn;
    const BitBoard<P> m_not_pinned;
    typename BitBoard<P>::Iterator m_dst_iter;
    typename BitBoard<P>::Iterator m_src_iter;
    bool m_promote;

public:
    SoldierMoveGenerator(const State<P>& state)
        : m_board(state.get_board()), m_turn(state.get_turn()),
          m_not_pinned(
              ~(state.find_pinned().set(m_board.get_king_square(m_turn)))),
          m_dst_iter(), m_src_iter(), m_promote()
    {
        if (state.in_double_check())
            return;
        if (!state.in_check())
            return;
        init_dst_iter(state.find_checker_square());
        while (m_dst_iter) {
            init_src_iter();
            if (m_src_iter)
                break;
            ++m_dst_iter;
        }
        init_promote();
    }
    SoldierMoveGenerator& operator++()
    {
        if (m_promote) {
            const auto dst = *m_dst_iter;
            const auto& p = m_board[*m_src_iter];
            if (BitBoard<P>::get_attacks_by(p, dst).any()) {
                m_promote = false;
                return *this;
            }
        }

        ++m_src_iter;
        if (m_src_iter) {
            init_promote();
            return *this;
        }

        ++m_dst_iter;
        while (m_dst_iter) {
            init_src_iter();
            if (!m_src_iter)
                ++m_dst_iter;
            else
                break;
        }
        init_promote();
        return *this;
    }
    Move<P> operator*() const
    {
        return Move<P>(*m_src_iter, *m_dst_iter, m_promote);
    }
    operator bool() const
    {
        return m_src_iter; // && m_dst_iter
    }

private:
    void init_dst_iter(const Square& checker_sq)
    {
        const auto& king = m_board.get_king_square(m_turn);
        m_dst_iter = BitBoard<P>::get_line_segment(checker_sq, king)
                         .set(checker_sq)
                         .iterator();
    }
    void init_src_iter()
    {
        const Square dst = *m_dst_iter;
        m_src_iter
            = m_board.compute_movable_to(dst, m_turn, m_not_pinned).iterator();
    }
    void init_promote()
    {
        if (!m_dst_iter || !m_src_iter) {
            m_promote = false;
            return;
        }
        const Square dst = *m_dst_iter;
        const Square src = *m_src_iter;
        const auto& p = m_board[src];
        m_promote = PT::is_promotable(p)
                    && (ST::in_promotion_zone(dst, m_turn)
                        || ST::in_promotion_zone(src, m_turn));
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATORS_SOLDIER_HPP
