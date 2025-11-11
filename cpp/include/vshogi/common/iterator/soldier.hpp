#ifndef VSHOGI_COMMON_ITERATOR_SOLDIER_HPP
#define VSHOGI_COMMON_ITERATOR_SOLDIER_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/iterator/iterator.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/state.hpp"

namespace vshogi
{

/**
 * - source
 * - destination
 * - promotion
 * - piece type (optional)
 *
 * SoldierMoveIterator<LEGAL>
 * for (src : sources)
 *     for (dst : destinations)
 *         for (prm : promotions)
 *
 * SoldierMoveIterator<CHECK>
 * for (src : sources)
 *     for (prm : promotions)
 *         for (dst : destinations)
 *
 *
 * // Current implementation
 * SoldierMoveIterator<EVADE>
 * for (dst : destinations)
 *     for (src : sources) // needs to keep pinned map
 *         for (prm : promotions)
 *
 * // Possibly bad implementation
 * SoldierMoveIterator<EVADE>
 * for (src : sources) // too many sources
 *     for (dst : destinations)
 *         for (prm : promotions)
 */

template <class P, IterEnum IterType = IterEnum::LEGAL>
class SoldierMoveIterator;

template <class P>
class SoldierMoveIterator<P, IterEnum::LEGAL>
{
private:
    using C = Configuration<P>;
    using Square = typename C::Square;
    using PHelper = Pieces<P>;
    using SHelper = Squares<P>;

private:
    const State<P>& m_state;
    const ColorEnum m_turn;
    const Board<P>& m_board;
    const BitBoard<P> m_pinned;
    typename BitBoard<P>::BitboardSquareIterator m_src_iter;
    typename BitBoard<P>::BitboardSquareIterator m_dst_iter;
    bool m_promote;

public:
    SoldierMoveIterator(const State<P>& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(m_state.find_pinned()), m_src_iter(), m_dst_iter(),
          m_promote(true)
    {
        if (m_state.in_double_check())
            return;
        init_no_check();
    }
    SoldierMoveIterator(const State<P>& state, const BitBoard<P>& src_mask)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(state.find_pinned()), m_src_iter(), m_dst_iter(),
          m_promote(true)
    {
        if (m_state.in_double_check())
            return;
        init_src_iter(src_mask);
        while (!m_src_iter.is_end()) {
            init_dst_iter();
            if (m_dst_iter.is_end())
                ++m_src_iter;
            else
                break;
        }
        init_promote();
    }
    SoldierMoveIterator& operator++()
    {
        if (!m_promote) {
            const auto src = *m_src_iter;
            const auto dst = *m_dst_iter;
            const auto p = m_board[src];
            if (PHelper::is_promotable(p)
                && (SHelper::in_promotion_zone(src, m_turn)
                    || SHelper::in_promotion_zone(dst, m_turn))) {
                m_promote = true;
                return *this;
            }
        }

        m_promote = false;
        ++m_dst_iter;
        if (!m_dst_iter.is_end()) {
            init_promote();
            return *this;
        }

        ++m_src_iter;
        while (!m_src_iter.is_end()) {
            init_dst_iter();
            if (m_dst_iter.is_end())
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
    SoldierMoveIterator begin()
    {
        return *this;
    }
    SoldierMoveIterator end()
    {
        static const auto end_iter
            = SoldierMoveIterator(m_state, BitBoard<P>());
        return end_iter;
    }
    bool operator!=(const SoldierMoveIterator& other) const
    {
        return (m_src_iter != other.m_src_iter)
               || (m_dst_iter != other.m_dst_iter)
               || (m_promote != other.m_promote);
    }
    bool is_end() const
    {
        return m_src_iter.is_end() && m_dst_iter.is_end();
    }

private:
    SoldierMoveIterator(
        const State<P>& state,
        const BitBoard<P>& src_mask,
        const BitBoard<P>& pinned)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(pinned), m_src_iter(), m_dst_iter(), m_promote(true)
    {
        if (m_state.in_double_check())
            return;
        init_src_iter(src_mask);
        while (!m_src_iter.is_end()) {
            init_dst_iter();
            if (m_dst_iter.is_end())
                ++m_src_iter;
            else
                break;
        }
        init_promote();
    }
    void init_no_check()
    {
        init_src_iter();
        while (!m_src_iter.is_end()) {
            init_dst_iter();
            if (m_dst_iter.is_end())
                ++m_src_iter;
            else
                break;
        }
        init_promote();
    }
    void init_src_iter()
    {
        const auto king_sq = m_board.get_king_square(m_turn);
        const auto src_mask = m_board.get_occupied(m_turn).clear(king_sq);
        m_src_iter = src_mask.iterator();
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
            const auto checker_sq = m_state.get_checker_square();
            movable &= BitBoard<P>::get_line_segment(checker_sq, king_sq)
                           .set(checker_sq);
            if (!movable.any())
                goto ExitLabel;
        }
        if (m_pinned.is_one(src)) {
            const auto dir = SHelper::direction(king_sq, src);
            assert((dir < 8) || (dir == DIR_NA));
            movable &= BitBoard<P>::get_ray_to(king_sq, dir);
        }
    ExitLabel:
        m_dst_iter = movable.iterator();
    }
    void init_promote()
    {
        if (m_src_iter.is_end() || m_dst_iter.is_end()) {
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
};

template <class P>
class SoldierMoveIterator<P, IterEnum::CHECK>
{
private:
    using C = Configuration<P>;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using PHelper = Pieces<P>;
    using SHelper = Squares<P>;

private:
    const State<P>& m_state;
    const ColorEnum m_turn;
    const Board<P>& m_board;
    const BitBoard<P> m_pinned;
    const BitBoard<P> m_cover;
    typename BitBoard<P>::BitboardSquareIterator m_src_iter;
    typename BitBoard<P>::BitboardSquareIterator m_dst_iter;
    bool m_promote;
    BitBoard<P> m_dst_mask;

public:
    SoldierMoveIterator(const State<P>& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(state.find_pinned()), m_cover(compute_cover(state)),
          m_src_iter(), m_dst_iter(), m_promote(true), m_dst_mask()
    {
        if (state.in_double_check())
            return;
        init_src_iter();
        while (!m_src_iter.is_end()) {
            init_dst_mask();
            if (m_dst_mask.any()) {
                m_promote = false;
                init_dst_iter_nopromo();
                if (!m_dst_iter.is_end())
                    return;
                m_promote = true;
                init_dst_iter_promotion();
                if (!m_dst_iter.is_end())
                    return;
            }
            ++m_src_iter;
        }
    }
    SoldierMoveIterator& operator++()
    {
        ++m_dst_iter;
        if (!m_dst_iter.is_end())
            return *this;

        if (m_promote == false) {
            m_promote = true;
            init_dst_iter_promotion();
            if (!m_dst_iter.is_end())
                return *this;
        }
        ++m_src_iter;

        while (!m_src_iter.is_end()) {
            init_dst_mask();
            if (m_dst_mask.any()) {
                m_promote = false;
                init_dst_iter_nopromo();
                if (!m_dst_iter.is_end())
                    return *this;
                m_promote = true;
                init_dst_iter_promotion();
                if (!m_dst_iter.is_end())
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
    SoldierMoveIterator begin()
    {
        return *this;
    }
    SoldierMoveIterator end()
    {
        static const auto end_iter = SoldierMoveIterator(m_state, m_board);
        return end_iter;
    }
    bool operator!=(const SoldierMoveIterator& other) const
    {
        return (m_src_iter != other.m_src_iter)
               || (m_dst_iter != other.m_dst_iter)
               || (m_promote != other.m_promote);
    }
    bool is_end() const
    {
        return m_src_iter.is_end();
    }

private:
    SoldierMoveIterator(const State<P>& state, const Board<P>& board)
        : m_state(state), m_turn(), m_board(board), m_pinned(), m_cover(),
          m_src_iter(), m_dst_iter(), m_promote(true), m_dst_mask()
    {
    }
    void init_src_iter()
    {
        const auto king_sq = m_board.get_king_square(m_turn);
        const auto target = m_board.get_king_square(~m_turn);
        assert(target != C::SQ_NA);

        BitBoard<P> src_mask = m_cover;
        src_mask |= m_board.get_occupied_by_ranging(m_turn);
        src_mask |= BitBoard<P>::get_neighbor_2nd_at(target, m_turn);
        src_mask &= m_board.get_occupied(m_turn).clear(king_sq);
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
        if (!PHelper::is_promotable(p)) {
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
        if (!SHelper::in_promotion_zone(src, m_turn)) {
            mask &= BitBoard<P>::get_promotion_zone(m_turn);
            return true;
        }
        return false;
    }
    void update_mask_by_nopromo(BitBoard<P>& mask, const ColoredPiece p)
    {
        mask &= BitBoard<P>::compute_droppable(p);
    }
    void update_dst_mask_by_current_check(const Square king_sq)
    {
        if (m_state.in_check()) {
            const auto checker_sq = m_state.get_checker_square();
            assert(checker_sq != C::SQ_NA);
            m_dst_mask &= BitBoard<P>::get_line_segment(checker_sq, king_sq)
                              .set(checker_sq);
        }
    }
    void
    update_dst_mask_by_counter_check(const Square src, const Square king_sq)
    {
        if (m_pinned.is_one(src)) {
            const auto dir = SHelper::direction(king_sq, src);
            assert((dir < 8) || (dir == DIR_NA));
            m_dst_mask &= BitBoard<P>::get_ray_to(king_sq, dir);
        }
    }
    void update_mask_by_forcing_check(
        BitBoard<P>& mask, const ColoredPiece p, const Square& src)
    {
        const auto enemy_king_sq = m_board.get_king_square(~m_turn);
        auto pt = PHelper::to_piece_type(p);
        if (m_promote)
            pt = PHelper::promote_nocheck(pt);
        const auto atk = BitBoard<P>::get_attacks_by(
            PHelper::to_board_piece(~m_turn, pt),
            enemy_king_sq,
            m_board.get_occupied());
        if (m_cover.is_one(src)) {
            const auto dir = SHelper::direction(enemy_king_sq, src);
            mask &= atk | (~BitBoard<P>::get_ray_to(enemy_king_sq, dir));
        } else {
            mask &= atk;
        }
    }
    static BitBoard<P> compute_cover(const State<P>& s)
    {
        if (s.in_double_check())
            return BitBoard<P>();
        return s.get_board().find_cover(s.get_turn());
    }
};

template <class P>
class SoldierMoveIteratorEvade
{
private:
    using C = Configuration<P>;
    using S = Squares<P>;
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
    SoldierMoveIteratorEvade(const State<P>& state)
        : m_state{state}, m_src_mask{compute_src_mask(state)},
          m_dst_last{state.get_checker_square()},
          m_dst_iter{S::ray_from(
              state.get_king_square(),
              S::direction(state.get_king_square(), m_dst_last))},
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
    SoldierMoveIteratorEvade& operator++()
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
    SoldierMoveIteratorEvade begin()
    {
        return *this;
    }
    SoldierMoveIteratorEvade end()
    {
        static const auto end_iter
            = SoldierMoveIteratorEvade(m_state, C::SQ_NA);
        return end_iter;
    }
    bool operator!=(const SoldierMoveIteratorEvade& other) const
    {
        return (m_dst_iter != other.m_dst_iter) || (m_src != other.m_src)
               || (m_promote != other.m_promote);
    }
    bool is_end() const
    {
        return (m_dst_iter == nullptr);
    }

private:
    SoldierMoveIteratorEvade(const State<P>& state, const Square)
        : m_state{state}, m_src_mask{}, m_dst_last{}, m_dst_iter{},
          m_src_dir_iter{}, m_src{C::SQ_NA}, m_promote{}
    {
    }
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
        m_promote = Pieces<P>::is_promotable(p)
                    && (S::in_promotion_zone(dst, t)
                        || S::in_promotion_zone(m_src, t));
    }
    static BitBoard<P> compute_src_mask(const State<P>& state)
    {
        return ~state.find_pinned().set(state.get_king_square());
    }
};

/**
 * @brief Another implementation of `SoldierMoveIterator<P, EVADE>`.
 * This is as fast as (or as slow as `SoldierMoveIterator<P, EVADE>`)
 * using SquareIterator instead of array of squares.
 */
template <class P>
class SoldierMoveIterator<P, IterEnum::EVADE>
{
private:
    using C = Configuration<P>;
    using PHelper = Pieces<P>;
    using SHelper = Squares<P>;
    using Square = typename C::Square;
    using BitboardSquareIterator = typename BitBoard<P>::BitboardSquareIterator;

    const Board<P>& m_board;
    const ColorEnum m_turn;
    const BitBoard<P> m_not_pinned;
    BitboardSquareIterator m_dst_iter;
    BitboardSquareIterator m_src_iter;
    bool m_promote;

public:
    SoldierMoveIterator(const State<P>& state)
        : m_board(state.get_board()), m_turn(state.get_turn()),
          m_not_pinned(
              ~(state.find_pinned().set(m_board.get_king_square(m_turn)))),
          m_dst_iter(), m_src_iter(), m_promote()
    {
        if (state.in_double_check())
            return;
        if (!state.in_check())
            return;
        init_dst_iter(state.get_checker_square());
        while (!m_dst_iter.is_end()) {
            init_src_iter();
            if (m_src_iter.is_end())
                ++m_dst_iter;
            else
                break;
        }
        init_promote();
    }
    SoldierMoveIterator& operator++()
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
        if (!m_src_iter.is_end()) {
            init_promote();
            return *this;
        }

        ++m_dst_iter;
        while (!m_dst_iter.is_end()) {
            init_src_iter();
            if (m_src_iter.is_end())
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
    SoldierMoveIterator begin()
    {
        return *this;
    }
    SoldierMoveIterator end()
    {
        static const auto end_iter = SoldierMoveIterator(m_board);
        return end_iter;
    }
    bool operator!=(const SoldierMoveIterator& other) const
    {
        return (m_dst_iter != other.m_dst_iter)
               || (m_src_iter != other.m_src_iter)
               || (m_promote != other.m_promote);
    }
    bool is_end() const
    {
        return m_src_iter.is_end() && m_dst_iter.is_end();
    }

private:
    SoldierMoveIterator(const Board<P>& b)
        : m_board(b), m_turn(), m_not_pinned(), m_dst_iter(), m_src_iter(),
          m_promote()
    {
    }
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
        if (m_dst_iter.is_end() || m_src_iter.is_end()) {
            m_promote = false;
            return;
        }
        const Square dst = *m_dst_iter;
        const Square src = *m_src_iter;
        const auto& p = m_board[src];
        m_promote = PHelper::is_promotable(p)
                    && (SHelper::in_promotion_zone(dst, m_turn)
                        || SHelper::in_promotion_zone(src, m_turn));
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_ITERATOR_SOLDIER_HPP
