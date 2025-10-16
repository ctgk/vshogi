#ifndef VSHOGI_COMMON_GENERATOR_HPP
#define VSHOGI_COMMON_GENERATOR_HPP

#include <cassert>

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/magic.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

/**
 * - source
 * - destination
 * - promotion
 * - piece type (optional)
 *
 * DropMoveGenerator<Check=false>
 * for (pt : piece_types)
 *     for (dst : destinations)
 *
 * DropMoveGenerator<Check=true>
 * for (pt : piece_types)
 *     for (dst : destinations)
 *
 * SoldierMoveGenerator<Check=false>
 * for (src : sources)
 *     for (dst : destinations)
 *         for (prm : promotions)
 *
 * SoldierMoveGenerator<Check=true>
 * for (src : sources)
 *     for (prm : promotions)
 *         for (dst : destinations)
 *
 * SoldierMoveGenerator<Check=true>
 * for (pt : piece_types)
 *     for (src : sources)
 *         for (dst : destinations)
 *
 * SoldierMoveGenerator<Check=true>
 * for (pt : piece_types)
 *     for (dst : destinations)
 *         for (src : sources)  # cannot cope with discovered check
 */

template <class P, bool Check = false>
class KingMoveGenerator
{
private:
    using C = Configuration<P>;
    using SHelper = Squares<P>;
    using SquareIterator = typename BitBoard<P>::SquareIterator;
    using Square = typename C::Square;

private:
    const Square m_src; //!< King square
    SquareIterator m_iter;

    KingMoveGenerator() : m_src(C::SQ_NA), m_iter()
    {
    }

public:
    KingMoveGenerator(const State<P>& state)
        : m_src(state.get_board().get_king_location(state.get_turn())), m_iter()
    {
        if constexpr (Check) {
            const auto t = state.get_turn();
            const auto& b = state.get_board();
            const auto enemy_king_sq = b.get_king_location(~t);
            const auto checker_dir
                = SHelper::get_direction(m_src, enemy_king_sq);
            const auto checker_sq
                = b.find_ranging_attacker(t, enemy_king_sq, checker_dir, m_src);
            if (checker_sq == C::SQ_NA) {
                return;
            } else {
                m_iter
                    = state
                          .compute_king_movable(~BitBoard<P>::get_line_segment(
                              checker_sq, enemy_king_sq))
                          .square_iterator();
            }
        } else {
            m_iter = state.compute_king_movable().square_iterator();
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
    KingMoveGenerator begin() const
    {
        return *this;
    }
    KingMoveGenerator end() const
    {
        static const auto end_iter = KingMoveGenerator();
        return end_iter;
    }
    bool operator!=(const KingMoveGenerator& other) const
    {
        return m_iter != other.m_iter;
    }
    bool is_end() const
    {
        return m_iter.is_end();
    }
};

template <class P, bool Check = false>
class DropMoveGenerator
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
    DropMoveGenerator(const State<P>& state)
        : m_state(state), m_turn(state.get_turn()),
          m_stand(state.get_stand(m_turn)), m_sq_iter{}, m_pt_iter{}
    {
        if (state.in_double_check()
            || (state.in_check()
                && !PHelper::is_ranging_piece(
                    state.get_board()[state.get_checker_location()]))) {
            m_pt_iter = static_cast<PieceType>(C::num_stand_piece_types);
            return;
        }
        increment_piece_type_unless_in_stand();
        if (m_pt_iter == C::num_stand_piece_types)
            return;
        init_sq_iter();
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
    DropMoveGenerator begin()
    {
        return *this;
    }
    DropMoveGenerator end()
    {
        static const auto end_iter = DropMoveGenerator(
            m_state, static_cast<PieceType>(C::num_stand_piece_types));
        return end_iter;
    }
    bool operator!=(const DropMoveGenerator& other) const
    {
        return (m_sq_iter != other.m_sq_iter) || (m_pt_iter != other.m_pt_iter);
    }
    bool is_end() const
    {
        return m_sq_iter.is_end() && (m_pt_iter == C::num_stand_piece_types);
    }

private:
    DropMoveGenerator(const State<P>& state, const PieceType pt)
        : m_state(state), m_turn(state.get_turn()),
          m_stand(state.get_stand(m_turn)), m_sq_iter(), m_pt_iter(pt)
    {
    }
    void init_sq_iter()
    {
        const auto& b = m_state.get_board();
        const auto p = PHelper::to_board_piece(m_turn, m_pt_iter);
        if (m_state.in_check()) {
            m_sq_iter = b.template compute_droppable<Check>(
                             p,
                             BitBoard<P>::get_line_segment(
                                 m_state.get_checker_location(),
                                 b.get_king_location(m_turn)))
                            .square_iterator();
        } else {
            m_sq_iter
                = b.template compute_droppable<Check>(p).square_iterator();
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

template <class P, bool Check>
class SoldierMoveGenerator;

template <class P>
class SoldierMoveGenerator<P, false>
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
    typename BitBoard<P>::SquareIterator m_src_iter;
    typename BitBoard<P>::SquareIterator m_dst_iter;
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
    SoldierMoveGenerator(const State<P>& state, const BitBoard<P>& src_mask)
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
    SoldierMoveGenerator& operator++()
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
    SoldierMoveGenerator begin()
    {
        return *this;
    }
    SoldierMoveGenerator end()
    {
        static const auto end_iter
            = SoldierMoveGenerator(m_state, BitBoard<P>());
        return end_iter;
    }
    bool operator!=(const SoldierMoveGenerator& other) const
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
    SoldierMoveGenerator(
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
        const auto king_sq = m_board.get_king_location(m_turn);
        const auto src_mask = m_board.get_occupied(m_turn).clear(king_sq);
        m_src_iter = src_mask.square_iterator();
    }
    void init_src_iter(const BitBoard<P>& src_mask)
    {
        m_src_iter = src_mask.square_iterator();
    }
    void init_dst_iter()
    {
        const auto src = *m_src_iter;
        const auto king_sq = m_board.get_king_location(m_turn);
        auto movable = m_board.get_attacks_by_nocheck(src);
        movable &= ~m_board.get_occupied(m_turn);
        if (!movable.any())
            goto ExitLabel;

        if (m_state.in_check()) {
            const auto checker_sq = m_state.get_checker_location();
            movable &= BitBoard<P>::get_line_segment(checker_sq, king_sq)
                           .set(checker_sq);
            if (!movable.any())
                goto ExitLabel;
        }
        if (m_pinned.is_one(src)) {
            const auto dir = SHelper::get_direction(src, king_sq);
            assert((dir < 8) || (dir == DIR_NA));
            movable &= BitBoard<P>::get_ray_to(king_sq, dir);
        }
    ExitLabel:
        m_dst_iter = movable.square_iterator();
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
class SoldierMoveGenerator<P, true>
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
    typename BitBoard<P>::SquareIterator m_src_iter;
    typename BitBoard<P>::SquareIterator m_dst_iter;
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
    SoldierMoveGenerator& operator++()
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
    SoldierMoveGenerator begin()
    {
        return *this;
    }
    SoldierMoveGenerator end()
    {
        static const auto end_iter = SoldierMoveGenerator(m_state, m_board);
        return end_iter;
    }
    bool operator!=(const SoldierMoveGenerator& other) const
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
    SoldierMoveGenerator(const State<P>& state, const Board<P>& board)
        : m_state(state), m_turn(), m_board(board), m_pinned(), m_cover(),
          m_src_iter(), m_dst_iter(), m_promote(true), m_dst_mask()
    {
    }
    void init_src_iter()
    {
        const auto king_sq = m_board.get_king_location(m_turn);
        const auto target = m_board.get_king_location(~m_turn);
        assert(target != C::SQ_NA);

        BitBoard<P> src_mask = m_cover;
        src_mask |= m_board.get_occupied_by_ranging(m_turn);
        src_mask |= BitBoard<P>::get_neighbor_2nd_at(target, m_turn);
        src_mask &= m_board.get_occupied(m_turn).clear(king_sq);
        m_src_iter = src_mask.square_iterator();
    }
    void init_dst_mask()
    {
        const auto src = *m_src_iter;
        const auto king_sq = m_board.get_king_location(m_turn);
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
        m_dst_iter = movable.square_iterator();
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
        m_dst_iter = movable.square_iterator();
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
            const auto checker_sq = m_state.get_checker_location();
            assert(checker_sq != C::SQ_NA);
            m_dst_mask &= BitBoard<P>::get_line_segment(checker_sq, king_sq)
                              .set(checker_sq);
        }
    }
    void
    update_dst_mask_by_counter_check(const Square src, const Square king_sq)
    {
        if (m_pinned.is_one(src)) {
            const auto dir = SHelper::get_direction(src, king_sq);
            assert((dir < 8) || (dir == DIR_NA));
            m_dst_mask &= BitBoard<P>::get_ray_to(king_sq, dir);
        }
    }
    void update_mask_by_forcing_check(
        BitBoard<P>& mask, const ColoredPiece p, const Square& src)
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        auto pt = PHelper::to_piece_type(p);
        if (m_promote)
            pt = PHelper::promote_nocheck(pt);
        const auto atk = BitBoard<P>::get_attacks_by(
            PHelper::to_board_piece(~m_turn, pt),
            enemy_king_sq,
            m_board.get_occupied());
        if (m_cover.is_one(src)) {
            const auto dir = SHelper::get_direction(src, enemy_king_sq);
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
class SoldierMoveGeneratorTSD
{
    using C = Configuration<P>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using PHelper = Pieces<P>;

private:
    const ColorEnum m_turn;
    const Board<P>& m_board;
    const BitBoard<P> m_pinned;
    const BitBoard<P> m_cover;
    PieceType m_piece_type;
    ColoredPiece m_colored_piece;
    typename BitBoard<P>::SquareIterator m_src_iter;
    typename BitBoard<P>::SquareIterator m_dst_iter;
    BitBoard<P> m_dst_mask;

public:
    SoldierMoveGeneratorTSD(const State<P>& s)
        : m_turn(s.get_turn()), m_board(s.get_board()),
          m_pinned(s.find_pinned()),
          m_cover(m_board.find_cover(m_turn)), m_piece_type{},
          m_colored_piece{}, m_src_iter{}, m_dst_iter{}, m_dst_mask{}
    {
    }

private:
    void increment_destination()
    {
        ++m_dst_iter;
        if (!m_dst_iter.is_end())
            return;
        increment_source();
    }
    void increment_source()
    {
        ++m_src_iter;
        if (!m_src_iter.is_end()) {
            m_dst_iter
                = (m_dst_mask & m_board.get_attacks_by_nocheck(*m_src_iter))
                      .square_iterator();
            if (!m_dst_iter.is_end())
                return;
        }
        increment_piece_type();
    }
    void increment_piece_type()
    {
        ++m_piece_type;
        if (m_piece_type == C::NA)
            return;
        // todo
    }
};

template <class P>
class BlockMoveGenerator
{
private:
    using C = Configuration<P>;
    using PHelper = Pieces<P>;
    using SHelper = Squares<P>;
    using Square = typename C::Square;
    using SquareIterator = typename BitBoard<P>::SquareIterator;

    const Board<P>& m_board;
    const ColorEnum m_turn;
    const BitBoard<P> m_not_pinned;
    SquareIterator m_dst_iter;
    SquareIterator m_src_iter;
    bool m_promote;

public:
    BlockMoveGenerator(const State<P>& state)
        : m_board(state.get_board()), m_turn(state.get_turn()),
          m_not_pinned(
              ~(state.find_pinned().set(m_board.get_king_location(m_turn)))),
          m_dst_iter(), m_src_iter(), m_promote()
    {
        if (state.in_double_check())
            return;
        if (!state.in_check())
            return;
        init_dst_iter(state.get_checker_location());
        while (!m_dst_iter.is_end()) {
            init_src_iter();
            if (m_src_iter.is_end())
                ++m_dst_iter;
            else
                break;
        }
        init_promote();
    }
    BlockMoveGenerator& operator++()
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
    BlockMoveGenerator begin()
    {
        return *this;
    }
    BlockMoveGenerator end()
    {
        static const auto end_iter = BlockMoveGenerator(m_board);
        return end_iter;
    }
    bool operator!=(const BlockMoveGenerator& other) const
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
    BlockMoveGenerator(const Board<P>& b)
        : m_board(b), m_turn(), m_not_pinned(), m_dst_iter(), m_src_iter(),
          m_promote()
    {
    }
    void init_dst_iter(const Square& checker_sq)
    {
        const auto& king = m_board.get_king_location(m_turn);
        m_dst_iter = BitBoard<P>::get_line_segment(checker_sq, king)
                         .set(checker_sq)
                         .square_iterator();
    }
    void init_src_iter()
    {
        const Square dst = *m_dst_iter;
        m_src_iter = m_board.compute_movable_to(dst, m_turn, m_not_pinned)
                         .square_iterator();
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

template <class P, bool Check = false>
class BoardMoveGenerator
{
private:
    KingMoveGenerator<P, Check> m_king_iter;
    SoldierMoveGenerator<P, Check> m_board_iter;
    uint m_index; //!< 0: king, 1: board, 2: end

public:
    BoardMoveGenerator(const State<P>& s)
        : m_king_iter(s), m_board_iter(s), m_index(0u)
    {
        if (m_king_iter.is_end()) {
            ++m_index;
            if (m_board_iter.is_end()) {
                ++m_index;
            }
        }
    }
    BoardMoveGenerator& operator++()
    {
        switch (m_index) {
        case 0u:
            ++m_king_iter;
            if (m_king_iter.is_end()) {
                ++m_index;
                if (m_board_iter.is_end()) {
                    ++m_index;
                }
            }
            break;
        case 1u:
            ++m_board_iter;
            if (m_board_iter.is_end()) {
                ++m_index;
            }
            break;
        default:
            break;
        }
        return *this;
    }
    Move<P> operator*() const
    {
        switch (m_index) {
        case 0u:
            return *m_king_iter;
        case 1u:
            return *m_board_iter;
        default:
            break;
        }
        return Move<P>();
    }
    BoardMoveGenerator begin()
    {
        return *this;
    }
    BoardMoveGenerator end()
    {
        static const auto end_iter
            = BoardMoveGenerator(m_king_iter.end(), m_board_iter.end(), 2u);
        return end_iter;
    }
    bool operator!=(const BoardMoveGenerator& other) const
    {
        return (m_king_iter != other.m_king_iter)
               || (m_board_iter != other.m_board_iter)
               || (m_index != other.m_index);
    }
    bool is_end() const
    {
        return (m_index == 2u);
    }

private:
    BoardMoveGenerator(
        const KingMoveGenerator<P, Check>& king_iter,
        const SoldierMoveGenerator<P, Check>& board_iter,
        const uint index)
        : m_king_iter(king_iter), m_board_iter(board_iter), m_index(index)
    {
    }
};

template <class P, bool Check = false>
class LegalMoveGenerator
{
private:
    KingMoveGenerator<P, Check> m_king_iter;
    SoldierMoveGenerator<P, Check> m_board_iter;
    DropMoveGenerator<P, Check> m_drop_iter;
    uint m_index; //!< 0: king, 1: board, 2: drop, 3: end

public:
    LegalMoveGenerator(const State<P>& s)
        : m_king_iter(s), m_board_iter(s), m_drop_iter(s), m_index(0u)
    {
        if (m_king_iter.is_end()) {
            ++m_index;
            if (m_board_iter.is_end()) {
                ++m_index;
                if (m_drop_iter.is_end())
                    ++m_index;
            }
        }
    }
    LegalMoveGenerator& operator++()
    {
        switch (m_index) {
        case 0u:
            ++m_king_iter;
            if (m_king_iter.is_end()) {
                ++m_index;
                if (m_board_iter.is_end()) {
                    ++m_index;
                    if (m_drop_iter.is_end())
                        ++m_index;
                }
            }
            break;
        case 1u:
            ++m_board_iter;
            if (m_board_iter.is_end()) {
                ++m_index;
                if (m_drop_iter.is_end())
                    ++m_index;
            }
            break;
        case 2u:
            ++m_drop_iter;
            if (m_drop_iter.is_end())
                ++m_index;
            break;
        default:
            break;
        }
        return *this;
    }
    Move<P> operator*() const
    {
        switch (m_index) {
        case 0u:
            return *m_king_iter;
        case 1u:
            return *m_board_iter;
        case 2u:
            return *m_drop_iter;
        default:
            break;
        }
        return Move<P>();
    }
    LegalMoveGenerator begin()
    {
        return *this;
    }
    LegalMoveGenerator end()
    {
        static const auto end_iter = LegalMoveGenerator<P, Check>(
            m_king_iter.end(), m_board_iter.end(), m_drop_iter.end(), 3u);
        return end_iter;
    }
    bool operator!=(const LegalMoveGenerator& other) const
    {
        return (m_king_iter != other.m_king_iter)
               || (m_board_iter != other.m_board_iter)
               || (m_drop_iter != other.m_drop_iter)
               || (m_index != other.m_index);
    }
    bool is_end() const
    {
        return (m_index == 3u);
    }

private:
    LegalMoveGenerator(
        const KingMoveGenerator<P, Check>& king_iter,
        const SoldierMoveGenerator<P, Check>& board_iter,
        const DropMoveGenerator<P, Check>& drop_iter,
        const uint index)
        : m_king_iter(king_iter), m_board_iter(board_iter),
          m_drop_iter(drop_iter), m_index(index)
    {
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATOR_HPP
