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

template <class Parameters, bool Check = false>
class KingMoveGenerator
{
private:
    using C = Configuration<Parameters>;
    using SHelper = Squares<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;
    using SquareIterator = typename BitBoard<Parameters>::SquareIterator;
    using Square = typename C::Square;

private:
    const Square m_src; //!< King square
    SquareIterator m_iter;

    KingMoveGenerator() : m_src(C::SQ_NA), m_iter()
    {
    }

public:
    KingMoveGenerator(const StateType& state)
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
                          .compute_king_movable(~BitBoardType::get_line_segment(
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
    MoveType operator*() const
    {
        return MoveType(*m_iter, m_src, false);
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
    MoveType random_select()
    {
        MoveType out = operator*();
        operator++();
        for (uint ii = 2u; !is_end(); ++ii, operator++()) {
            const auto r = dist01(random_engine);
            if (static_cast<uint>(r * static_cast<float>(ii)) == 0u)
                out = operator*();
        }
        return out;
    }
};

template <class Parameters, bool Check = false>
class DropMoveGenerator
{
private:
    using C = Configuration<Parameters>;
    using SquareIterator = typename BitBoard<Parameters>::SquareIterator;
    using BitBoardType = BitBoard<Parameters>;
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;
    using StandType = Stand<Parameters>;
    using PieceType = typename C::PieceType;
    using PHelper = Pieces<Parameters>;

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const StandType& m_stand;
    SquareIterator m_sq_iter;
    PieceType m_pt_iter;

public:
    DropMoveGenerator(const StateType& state)
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
    MoveType operator*() const
    {
        return MoveType(*m_sq_iter, m_pt_iter);
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
    MoveType random_select()
    {
        MoveType out = operator*();
        operator++();
        for (uint ii = 2u; !is_end(); ++ii, operator++()) {
            const auto r = dist01(random_engine);
            if (static_cast<uint>(r * static_cast<float>(ii)) == 0u)
                out = operator*();
        }
        return out;
    }

private:
    DropMoveGenerator(const StateType& state, const PieceType pt)
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
                             BitBoardType::get_line_segment(
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

template <class Parameters>
class NoPromoMoveGenerator
{
private:
    using C = Configuration<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using BoardType = Board<Parameters>;
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;
    using Square = typename C::Square;
    using PHelper = Pieces<Parameters>;
    using SHelper = Squares<Parameters>;

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    const BitBoardType m_pinned;
    typename BitBoardType::SquareIterator m_src_iter;
    typename BitBoardType::SquareIterator m_dst_iter;

public:
    NoPromoMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(state.find_pinned()), m_src_iter(), m_dst_iter()
    {
        if (m_state.in_double_check())
            return;
        init_src_iter();
        while (!m_src_iter.is_end()) {
            init_dst_iter();
            if (m_dst_iter.is_end())
                ++m_src_iter;
            else
                break;
        }
    }
    NoPromoMoveGenerator(
        const StateType& state,
        const BitBoardType& src_mask,
        const BitBoardType& pinned)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(pinned), m_src_iter(), m_dst_iter()
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
    }
    NoPromoMoveGenerator& operator++()
    {
        ++m_dst_iter;
        if (!m_dst_iter.is_end())
            return *this;

        ++m_src_iter;
        while (!m_src_iter.is_end()) {
            init_dst_iter();
            if (m_dst_iter.is_end())
                ++m_src_iter;
            else
                return *this;
        }
        return *this;
    }
    MoveType operator*() const
    {
        return MoveType(*m_dst_iter, *m_src_iter, false);
    }
    NoPromoMoveGenerator begin() const
    {
        return *this;
    }
    NoPromoMoveGenerator end() const
    {
        static const auto end_iter
            = NoPromoMoveGenerator(m_state, BitBoardType(), BitBoardType());
        return end_iter;
    }
    bool operator!=(const NoPromoMoveGenerator& other) const
    {
        return (m_src_iter != other.m_src_iter)
               || (m_dst_iter != other.m_dst_iter);
    }
    bool is_end() const
    {
        return m_src_iter.is_end();
    }
    MoveType random_select()
    {
        return random_select_by_iterating_all();
    }

private:
    void init_src_iter()
    {
        const auto src_mask = m_board.get_occupied_by_non_promotable(m_turn);
        m_src_iter = src_mask.square_iterator();
    }
    void init_src_iter(const BitBoardType& src_mask)
    {
        m_src_iter = src_mask.square_iterator();
    }
    void init_dst_iter()
    {
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        auto movable
            = BitBoardType::get_attacks_by(p, src, m_board.get_occupied());
        const auto king_sq = m_board.get_king_location(m_turn);

        movable &= ~m_board.get_occupied(m_turn);
        if (!movable.any())
            goto ExitLabel;

        if (m_state.in_check()) {
            const auto checker_sq = m_state.get_checker_location();
            movable &= BitBoardType::get_line_segment(checker_sq, king_sq)
                           .set(checker_sq);
            if (!movable.any())
                goto ExitLabel;
        }
        if (m_pinned.is_one(src)) {
            const auto dir = SHelper::get_direction(src, king_sq);
            assert((dir < 8) || (dir == DIR_NA));
            movable &= BitBoardType::get_ray_to(king_sq, dir);
        }
    ExitLabel:
        m_dst_iter = movable.square_iterator();
    }
    MoveType random_select_by_iterating_all()
    {
        MoveType out = operator*();
        operator++();
        for (uint ii = 2u; !is_end(); ++ii, operator++()) {
            const auto r = dist01(random_engine);
            if (static_cast<uint>(r * static_cast<float>(ii)) == 0u)
                out = operator*();
        }
        return out;
    }
};

template <class Parameters>
class NonKingBoardMoveGenerator
{
private:
    using C = Configuration<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using BoardType = Board<Parameters>;
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;
    using Square = typename C::Square;
    using PHelper = Pieces<Parameters>;
    using SHelper = Squares<Parameters>;

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    const BitBoardType m_pinned;
    typename BitBoardType::SquareIterator m_src_iter;
    typename BitBoardType::SquareIterator m_dst_iter;
    bool m_promote;

public:
    NonKingBoardMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(m_state.find_pinned()), m_src_iter(), m_dst_iter(),
          m_promote(true)
    {
        if (m_state.in_double_check())
            return;
        init_no_check();
    }
    NonKingBoardMoveGenerator(
        const StateType& state, const BitBoardType& src_mask)
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
    NonKingBoardMoveGenerator& operator++()
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
    MoveType operator*() const
    {
        return MoveType(*m_dst_iter, *m_src_iter, m_promote);
    }
    NonKingBoardMoveGenerator begin()
    {
        return *this;
    }
    NonKingBoardMoveGenerator end()
    {
        static const auto end_iter
            = NonKingBoardMoveGenerator(m_state, BitBoardType());
        return end_iter;
    }
    bool operator!=(const NonKingBoardMoveGenerator& other) const
    {
        return (m_src_iter != other.m_src_iter)
               || (m_dst_iter != other.m_dst_iter)
               || (m_promote != other.m_promote);
    }
    bool is_end() const
    {
        return m_src_iter.is_end() && m_dst_iter.is_end();
    }
    MoveType random_select()
    {
        return random_select_by_iterating_all();
    }

private:
    NonKingBoardMoveGenerator(
        const StateType& state,
        const BitBoardType& src_mask,
        const BitBoardType& pinned)
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
    void init_src_iter(const BitBoardType& src_mask)
    {
        m_src_iter = src_mask.square_iterator();
    }
    void init_dst_iter()
    {
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        auto movable
            = BitBoardType::get_attacks_by(p, src, m_board.get_occupied());
        const auto king_sq = m_board.get_king_location(m_turn);

        movable &= ~m_board.get_occupied(m_turn);
        if (!movable.any())
            goto ExitLabel;

        if (m_state.in_check()) {
            const auto checker_sq = m_state.get_checker_location();
            movable &= BitBoardType::get_line_segment(checker_sq, king_sq)
                           .set(checker_sq);
            if (!movable.any())
                goto ExitLabel;
        }
        if (m_pinned.is_one(src)) {
            const auto dir = SHelper::get_direction(src, king_sq);
            assert((dir < 8) || (dir == DIR_NA));
            movable &= BitBoardType::get_ray_to(king_sq, dir);
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
        if (BitBoardType::get_attacks_by(p, *m_dst_iter).any())
            return;
        m_promote = true;
    }
    MoveType random_select_by_iterating_all()
    {
        MoveType out = operator*();
        operator++();
        for (uint ii = 2u; !is_end(); ++ii, operator++()) {
            const auto r = dist01(random_engine);
            if (static_cast<uint>(r * static_cast<float>(ii)) == 0u)
                out = operator*();
        }
        return out;
    }
};

template <class Parameters>
class BlockMoveGenerator
{
private:
    using C = Configuration<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using BoardType = Board<Parameters>;
    using MoveType = Move<Parameters>;
    using PHelper = Pieces<Parameters>;
    using SHelper = Squares<Parameters>;
    using StateType = State<Parameters>;
    using Square = typename C::Square;
    using SquareIterator = typename BitBoardType::SquareIterator;

    const BoardType& m_board;
    const ColorEnum m_turn;
    const BitBoardType m_not_pinned;
    SquareIterator m_dst_iter;
    SquareIterator m_src_iter;
    bool m_promote;

public:
    BlockMoveGenerator(const StateType& state)
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
            if (BitBoardType::get_attacks_by(p, dst).any()) {
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
    MoveType operator*() const
    {
        return MoveType(*m_dst_iter, *m_src_iter, m_promote);
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
    BlockMoveGenerator(const BoardType& b)
        : m_board(b), m_turn(), m_not_pinned(), m_dst_iter(), m_src_iter(),
          m_promote()
    {
    }
    void init_dst_iter(const Square& checker_sq)
    {
        const auto& king = m_board.get_king_location(m_turn);
        m_dst_iter = BitBoardType::get_line_segment(checker_sq, king)
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

template <class Parameters, bool Check>
class SoldierMoveGenerator
{
private:
    static_assert(Check);
    using C = Configuration<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using BoardType = Board<Parameters>;
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using PHelper = Pieces<Parameters>;
    using SHelper = Squares<Parameters>;

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    const BitBoardType m_pinned;
    const BitBoardType m_cover;
    typename BitBoardType::SquareIterator m_src_iter;
    typename BitBoardType::SquareIterator m_dst_iter;
    bool m_promote;
    BitBoardType m_dst_mask;

public:
    SoldierMoveGenerator(const StateType& state)
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
    MoveType operator*() const
    {
        return MoveType(*m_dst_iter, *m_src_iter, m_promote);
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
    SoldierMoveGenerator(const StateType& state, const BoardType& board)
        : m_state(state), m_turn(), m_board(board), m_pinned(), m_cover(),
          m_src_iter(), m_dst_iter(), m_promote(true), m_dst_mask()
    {
    }
    void init_src_iter()
    {
        const auto king_sq = m_board.get_king_location(m_turn);
        const auto target = m_board.get_king_location(~m_turn);
        assert(target != C::SQ_NA);
        const auto occ_pieces = m_board.get_occupied(m_turn).clear(king_sq);
        const auto occ_ranger = m_board.get_occupied_by_ranging(m_turn);
        const auto neighbor
            = BitBoardType::compute_2nd_neighbor_of(target, m_turn);
        const auto src_mask = occ_pieces & (occ_ranger | neighbor | m_cover);
        m_src_iter = src_mask.square_iterator();
    }
    void init_dst_mask()
    {
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        const auto king_sq = m_board.get_king_location(m_turn);

        m_dst_mask
            = BitBoardType::get_attacks_by(p, src, m_board.get_occupied());
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
        BitBoardType movable = m_dst_mask;
        update_mask_by_forcing_check(movable, p, src);
        if (movable.any()) {
            update_mask_by_nopromo(movable, p);
        }
        m_dst_iter = movable.square_iterator();
    }
    bool update_mask_by_promotion(BitBoardType& mask, const Square src)
    {
        assert(m_promote);
        if (!SHelper::in_promotion_zone(src, m_turn)) {
            mask &= BitBoardType::get_promotion_zone(m_turn);
            return true;
        }
        return false;
    }
    void update_mask_by_nopromo(BitBoardType& mask, const ColoredPiece p)
    {
        mask &= BitBoardType::compute_droppable(p);
    }
    void update_dst_mask_by_current_check(const Square king_sq)
    {
        if (m_state.in_check()) {
            const auto checker_sq = m_state.get_checker_location();
            assert(checker_sq != C::SQ_NA);
            m_dst_mask &= BitBoardType::get_line_segment(checker_sq, king_sq)
                              .set(checker_sq);
        }
    }
    void
    update_dst_mask_by_counter_check(const Square src, const Square king_sq)
    {
        if (m_pinned.is_one(src)) {
            const auto dir = SHelper::get_direction(src, king_sq);
            assert((dir < 8) || (dir == DIR_NA));
            m_dst_mask &= BitBoardType::get_ray_to(king_sq, dir);
        }
    }
    void update_mask_by_forcing_check(
        BitBoardType& mask, const ColoredPiece p, const Square& src)
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        auto pt = PHelper::to_piece_type(p);
        if (m_promote)
            pt = PHelper::promote_nocheck(pt);
        const auto atk = BitBoardType::get_attacks_by(
            PHelper::to_board_piece(~m_turn, pt),
            enemy_king_sq,
            m_board.get_occupied());
        if (m_cover.is_one(src)) {
            const auto dir = SHelper::get_direction(src, enemy_king_sq);
            mask &= atk | (~BitBoardType::get_ray_to(enemy_king_sq, dir));
        } else {
            mask &= atk;
        }
    }
    static BitBoardType compute_cover(const StateType& s)
    {
        if (s.in_double_check())
            return BitBoardType();
        return s.get_board().find_cover(s.get_turn());
    }
};

template <class Parameters>
class BoardMoveGenerator
{
private:
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;

private:
    KingMoveGenerator<Parameters> m_king_iter;
    NonKingBoardMoveGenerator<Parameters> m_board_iter;
    uint m_index; //!< 0: king, 1: board, 2: end

public:
    BoardMoveGenerator(const StateType& s)
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
    MoveType operator*() const
    {
        switch (m_index) {
        case 0u:
            return *m_king_iter;
        case 1u:
            return *m_board_iter;
        default:
            break;
        }
        return MoveType();
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
        const KingMoveGenerator<Parameters>& king_iter,
        const NonKingBoardMoveGenerator<Parameters>& board_iter,
        const uint index)
        : m_king_iter(king_iter), m_board_iter(board_iter), m_index(index)
    {
    }
};

template <class Parameters, bool Check = false>
class LegalMoveGenerator
{
private:
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;

private:
    KingMoveGenerator<Parameters, Check> m_king_iter;
    std::conditional_t<
        Check,
        SoldierMoveGenerator<Parameters, Check>,
        NonKingBoardMoveGenerator<Parameters>>
        m_board_iter;
    DropMoveGenerator<Parameters, Check> m_drop_iter;
    uint m_index; //!< 0: king, 1: board, 2: drop, 3: end

public:
    LegalMoveGenerator(const StateType& s)
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
    MoveType operator*() const
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
        return MoveType();
    }
    LegalMoveGenerator begin()
    {
        return *this;
    }
    LegalMoveGenerator end()
    {
        static const auto end_iter = LegalMoveGenerator<Parameters, Check>(
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
        const KingMoveGenerator<Parameters, Check>& king_iter,
        const std::conditional_t<
            Check,
            SoldierMoveGenerator<Parameters, Check>,
            NonKingBoardMoveGenerator<Parameters>>& board_iter,
        const DropMoveGenerator<Parameters, Check>& drop_iter,
        const uint index)
        : m_king_iter(king_iter), m_board_iter(board_iter),
          m_drop_iter(drop_iter), m_index(index)
    {
    }
};

template <class Parameters>
class CheckBoardMoveGenerator
{
private:
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;

private:
    KingMoveGenerator<Parameters, true> m_king_iter;
    SoldierMoveGenerator<Parameters, true> m_soldier_iter;
    uint m_index; //!< 0: king, 1: board, 2: end

public:
    CheckBoardMoveGenerator(const StateType& s)
        : m_king_iter(s), m_soldier_iter(s), m_index(0u)
    {
        if (m_king_iter.is_end()) {
            ++m_index;
            if (m_soldier_iter.is_end()) {
                ++m_index;
            }
        }
    }
    CheckBoardMoveGenerator& operator++()
    {
        switch (m_index) {
        case 0u:
            ++m_king_iter;
            if (m_king_iter.is_end()) {
                ++m_index;
                if (m_soldier_iter.is_end()) {
                    ++m_index;
                }
            }
            break;
        case 1u:
            ++m_soldier_iter;
            if (m_soldier_iter.is_end()) {
                ++m_index;
            }
            break;
        default:
            break;
        }
        return *this;
    }
    MoveType operator*() const
    {
        switch (m_index) {
        case 0u:
            return *m_king_iter;
        case 1u:
            return *m_soldier_iter;
        default:
            break;
        }
        return MoveType();
    }
    CheckBoardMoveGenerator begin()
    {
        return *this;
    }
    CheckBoardMoveGenerator end()
    {
        static const auto end_iter = CheckBoardMoveGenerator(
            m_king_iter.end(), m_soldier_iter.end(), 2u);
        return end_iter;
    }
    bool operator!=(const CheckBoardMoveGenerator& other) const
    {
        return (m_king_iter != other.m_king_iter)
               || (m_soldier_iter != other.m_soldier_iter)
               || (m_index != other.m_index);
    }
    bool is_end() const
    {
        return (m_index == 2u);
    }

private:
    CheckBoardMoveGenerator(
        const KingMoveGenerator<Parameters, true>& king_iter,
        const SoldierMoveGenerator<Parameters, true>& soldier_iter,
        const uint index)
        : m_king_iter(king_iter), m_soldier_iter(soldier_iter), m_index(index)
    {
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATOR_HPP
