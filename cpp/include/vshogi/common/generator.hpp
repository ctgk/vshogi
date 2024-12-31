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
                m_iter = (state.compute_king_movable()
                          & (~BitBoardType::get_line_segment(
                              checker_sq, enemy_king_sq)))
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

template <class Parameters>
class DropMoveGenerator
{
private:
    using C = Configuration<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using BoardType = Board<Parameters>;
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;
    using StandType = Stand<Parameters>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using File = typename C::File;
    using PHelper = Pieces<Parameters>;
    using SHelper = Squares<Parameters>;
    static constexpr uint num_dir = C::num_dir;
    static constexpr uint num_stand_piece_types = C::num_stand_piece_types;

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    const StandType& m_stand;
    typename BitBoardType::SquareIterator m_sq_iter;
    PieceType m_pt_iter;

public:
    DropMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_stand(state.get_stand(m_turn)), m_sq_iter{}, m_pt_iter{}
    {
        if (state.in_double_check()
            || (state.in_check()
                && !PHelper::is_ranging_piece(
                    m_board[state.get_checker_location()]))) {
            m_pt_iter = static_cast<PieceType>(num_stand_piece_types);
            return;
        }
        increment_piece_type_unless_in_stand();
        if (m_pt_iter == num_stand_piece_types)
            return;
        init_sq_iter();
        increment_iterators_unless_legal();
    }
    DropMoveGenerator& operator++()
    {
        ++m_sq_iter;
        increment_iterators_unless_legal();
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
            m_state, static_cast<PieceType>(num_stand_piece_types));
        return end_iter;
    }
    bool operator!=(const DropMoveGenerator& other) const
    {
        return (m_sq_iter != other.m_sq_iter) || (m_pt_iter != other.m_pt_iter);
    }
    bool is_end() const
    {
        return m_sq_iter.is_end() && (m_pt_iter == num_stand_piece_types);
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
    DropMoveGenerator& increment_to_end()
    {
        m_sq_iter = BitBoardType().square_iterator();
        m_pt_iter = static_cast<PieceType>(num_stand_piece_types);
        return *this;
    }

private:
    DropMoveGenerator(const StateType& state, const PieceType pt)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_stand(state.get_stand(m_turn)),
          m_sq_iter(BitBoardType().square_iterator()), m_pt_iter(pt)
    {
    }
    void init_sq_iter()
    {
        if (m_state.in_check())
            m_sq_iter = BitBoardType::get_line_segment(
                            m_board.get_king_location(m_turn),
                            m_state.get_checker_location())
                            .square_iterator();
        else
            m_sq_iter = (~m_board.get_occupied()).square_iterator();
    }
    void increment_iterators_unless_legal()
    {
        while (m_pt_iter < num_stand_piece_types) {
            increment_piece_type_unless_in_stand();
            if (m_pt_iter == num_stand_piece_types) {
                m_sq_iter = BitBoardType().square_iterator();
                break;
            }
            increment_square();

            if (m_sq_iter.is_end()) {
                m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
                if (m_pt_iter == num_stand_piece_types)
                    break;
                init_sq_iter();
            } else {
                break;
            }
        }
    }
    void increment_square()
    {
        if (m_pt_iter == C::FU)
            increment_square_for_pawn_unless_legal();
        else
            increment_square_unless_legal();
    }
    void increment_piece_type_unless_in_stand()
    {
        while ((m_pt_iter < num_stand_piece_types)
               && !m_stand.exist(m_pt_iter)) {
            m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
        }
    }
    void increment_square_for_pawn_unless_legal()
    {
        const auto p = PHelper::to_board_piece(m_turn, m_pt_iter);
        while (!m_sq_iter.is_end()) {
            const auto attacks = BitBoardType::get_attacks_by(p, *m_sq_iter);
            if (!attacks.any()) {
                ++m_sq_iter;
                continue;
            }
            const auto f = SHelper::to_file(*m_sq_iter);
            if (m_board.has_pawn_in_file(f, m_turn)) {
                do {
                    ++m_sq_iter;
                }
                while (SHelper::to_file(*m_sq_iter) == f);
                continue;
            }
            if (m_board.is_drop_pawn_mate(*m_sq_iter, m_turn)) {
                ++m_sq_iter;
                continue;
            }
            break;
        }
    }
    void increment_square_unless_legal()
    {
        const auto p = PHelper::to_board_piece(m_turn, m_pt_iter);
        while (!m_sq_iter.is_end()) {
            const auto attacks = BitBoardType::get_attacks_by(p, *m_sq_iter);
            if (attacks.any())
                break;
            ++m_sq_iter;
        }
    }
};

template <class Parameters>
class CheckDropMoveGenerator
{
private:
    using C = Configuration<Parameters>;
    using BitBoardType = BitBoard<Parameters>;
    using BoardType = Board<Parameters>;
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;
    using StandType = Stand<Parameters>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using File = typename C::File;
    using PHelper = Pieces<Parameters>;
    using SHelper = Squares<Parameters>;
    static constexpr uint num_dir = C::num_dir;
    static constexpr uint num_stand_piece_types = C::num_stand_piece_types;

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    const StandType& m_stand;
    typename BitBoardType::SquareIterator m_sq_iter;
    PieceType m_pt_iter;

public:
    CheckDropMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_stand(state.get_stand(m_turn)), m_sq_iter{}, m_pt_iter{}
    {
        if (state.in_double_check()
            || (state.in_check()
                && !PHelper::is_ranging_piece(
                    m_board[state.get_checker_location()]))) {
            m_pt_iter = static_cast<PieceType>(num_stand_piece_types);
            return;
        }
        increment_piece_type_unless_in_stand();
        if (m_pt_iter == num_stand_piece_types)
            return;
        init_sq_iter();
        increment_iterators_unless_legal();
    }
    CheckDropMoveGenerator& operator++()
    {
        ++m_sq_iter;
        increment_iterators_unless_legal();
        return *this;
    }
    MoveType operator*() const
    {
        return MoveType(*m_sq_iter, m_pt_iter);
    }
    CheckDropMoveGenerator begin()
    {
        return *this;
    }
    CheckDropMoveGenerator end()
    {
        static const auto end_iter = CheckDropMoveGenerator(
            m_state, static_cast<PieceType>(num_stand_piece_types));
        return end_iter;
    }
    bool operator!=(const CheckDropMoveGenerator& other) const
    {
        return (m_sq_iter != other.m_sq_iter) || (m_pt_iter != other.m_pt_iter);
    }
    bool is_end() const
    {
        return m_sq_iter.is_end() && (m_pt_iter == num_stand_piece_types);
    }

private:
    CheckDropMoveGenerator(const StateType& state, const PieceType pt)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_stand(state.get_stand(m_turn)),
          m_sq_iter(BitBoardType().square_iterator()), m_pt_iter(pt)
    {
    }
    void init_sq_iter()
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        const auto occupied = m_board.get_occupied();
        const auto dst_mask = BitBoardType::get_attacks_by(
                                  PHelper::to_board_piece(~m_turn, m_pt_iter),
                                  enemy_king_sq,
                                  occupied)
                              & (~occupied);
        if (m_state.in_check())
            m_sq_iter = (BitBoardType::get_line_segment(
                             m_board.get_king_location(m_turn),
                             m_state.get_checker_location())
                         & dst_mask)
                            .square_iterator();
        else
            m_sq_iter = dst_mask.square_iterator();
    }
    void increment_iterators_unless_legal()
    {
        while (m_pt_iter < num_stand_piece_types) {
            const auto before = m_pt_iter;
            increment_piece_type_unless_in_stand();
            if (m_pt_iter == num_stand_piece_types) {
                m_sq_iter = BitBoardType().square_iterator();
                break;
            } else if (m_pt_iter != before) {
                init_sq_iter();
            }
            increment_square();

            if (m_sq_iter.is_end()) {
                m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
                if (m_pt_iter == num_stand_piece_types)
                    break;
                init_sq_iter();
            } else {
                break;
            }
        }
    }
    void increment_square()
    {
        if (m_pt_iter == C::FU)
            increment_square_for_pawn_unless_legal();
        else
            increment_square_unless_legal();
    }
    void increment_piece_type_unless_in_stand()
    {
        while ((m_pt_iter < num_stand_piece_types)
               && !m_stand.exist(m_pt_iter)) {
            m_pt_iter = static_cast<PieceType>(m_pt_iter + 1);
        }
    }
    void increment_square_for_pawn_unless_legal()
    {
        const auto p = PHelper::to_board_piece(m_turn, m_pt_iter);
        while (!m_sq_iter.is_end()) {
            const auto attacks = BitBoardType::get_attacks_by(p, *m_sq_iter);
            if (!attacks.any()) {
                ++m_sq_iter;
                continue;
            }
            if (m_board.has_pawn_in_file(
                    SHelper::to_file(*m_sq_iter), m_turn)) {
                ++m_sq_iter;
                continue;
            }
            if (m_board.is_drop_pawn_mate(*m_sq_iter, m_turn)) {
                ++m_sq_iter;
                continue;
            }
            break;
        }
    }
    void increment_square_unless_legal()
    {
        const auto p = PHelper::to_board_piece(m_turn, m_pt_iter);
        while (!m_sq_iter.is_end()) {
            const auto attacks = BitBoardType::get_attacks_by(p, *m_sq_iter);
            if (attacks.any())
                break;
            ++m_sq_iter;
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
          m_pinned(), m_src_iter(), m_dst_iter()
    {
        if (m_state.in_double_check())
            return;
        m_pinned = m_board.find_pinned(m_turn);
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
          m_pinned(m_board.find_pinned(m_turn)), m_src_iter(), m_dst_iter(),
          m_promote(true)
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
        init_promote();
    }
    NonKingBoardMoveGenerator(
        const StateType& state, const BitBoardType& src_mask)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(m_board.find_pinned(m_turn)), m_src_iter(), m_dst_iter(),
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
class CheckNonKingBoardMoveGenerator
{
private:
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
    typename BitBoardType::SquareIterator m_src_iter;
    typename BitBoardType::SquareIterator m_dst_iter;
    bool m_promote;
    BitBoardType m_dst_mask;
    Square m_discovered_checker_sq;

public:
    CheckNonKingBoardMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(m_board.find_pinned(m_turn)), m_src_iter(), m_dst_iter(),
          m_promote(true), m_dst_mask(), m_discovered_checker_sq(C::SQ_NA)
    {
        if (m_state.in_double_check())
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
    CheckNonKingBoardMoveGenerator& operator++()
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
    CheckNonKingBoardMoveGenerator begin()
    {
        return *this;
    }
    CheckNonKingBoardMoveGenerator end()
    {
        static const auto end_iter
            = CheckNonKingBoardMoveGenerator(m_state, true);
        return end_iter;
    }
    bool operator!=(const CheckNonKingBoardMoveGenerator& other) const
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
    CheckNonKingBoardMoveGenerator(const StateType& state, const bool promote)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_pinned(), m_src_iter(), m_dst_iter(), m_promote(promote),
          m_dst_mask(), m_discovered_checker_sq(C::SQ_NA)
    {
    }
    void init_src_iter()
    {
        const auto king_sq = m_board.get_king_location(m_turn);
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        assert(enemy_king_sq != C::SQ_NA);
        const auto non_king_occupancy
            = m_board.get_occupied(m_turn).clear(king_sq);
        const auto ranging_occupancy = m_board.get_occupied_by_ranging(m_turn);
        const auto eight_dir_mask
            = Magic<Parameters>::get_adjacent_attack(enemy_king_sq)
              | Magic<Parameters>::get_diagonal_attack(enemy_king_sq);
        const auto second_neighbor
            = BitBoardType::compute_2nd_neighbor_of(enemy_king_sq, m_turn);
        const auto src_mask
            = non_king_occupancy
              & (eight_dir_mask | ranging_occupancy | second_neighbor);
        m_src_iter = src_mask.square_iterator();
    }
    void init_dst_mask()
    {
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        const auto king_sq = m_board.get_king_location(m_turn);
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);

        m_dst_mask
            = BitBoardType::get_attacks_by(p, src, m_board.get_occupied());
        m_dst_mask &= ~m_board.get_occupied(m_turn);
        update_dst_mask_by_current_check(king_sq);
        update_dst_mask_by_counter_check(src, king_sq);
        m_discovered_checker_sq = m_board.find_ranging_attacker(
            m_turn,
            enemy_king_sq,
            SHelper::get_direction(src, enemy_king_sq),
            src);
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
        update_mask_by_forcing_check(movable, p);
        m_dst_iter = movable.square_iterator();
    }
    void init_dst_iter_nopromo()
    {
        assert(!m_promote);
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        auto movable = m_dst_mask;
        update_mask_by_nopromo(movable, p);
        if (!movable.any()) {
            return;
        }
        update_mask_by_forcing_check(movable, p);
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
        const auto dirs = PHelper::get_attack_directions(p);
        if (dirs[1] == DIR_NA) {
            mask &= ~BitBoardType::from_rank(
                (dirs[0] == DIR_N) ? C::RANK_A : C::RANK_Z);
        } else if (dirs[1] > DIR_SE) {
            if (dirs[1] >= DIR_NNW)
                mask &= ~BitBoardType::
                            template from_rank<C::RANK_A, C::RANK_B>();
            else
                mask &= ~BitBoardType::
                            template from_rank<C::RANK_Y, C::RANK_Z>();
        }
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
    void update_mask_by_forcing_check(BitBoardType& mask, const ColoredPiece p)
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        auto pt = PHelper::to_piece_type(p);
        if (m_promote)
            pt = PHelper::promote_nocheck(pt);
        if (m_discovered_checker_sq == C::SQ_NA) // check by moving piece.
            mask &= BitBoardType::get_attacks_by(
                PHelper::to_board_piece(~m_turn, pt),
                enemy_king_sq,
                m_board.get_occupied());
        else { // check by moving piece or a discovered piece.
            mask
                &= (BitBoardType::get_attacks_by(
                        PHelper::to_board_piece(~m_turn, pt),
                        enemy_king_sq,
                        m_board.get_occupied())
                    | (~BitBoardType::get_line_segment(
                        m_discovered_checker_sq, enemy_king_sq)));
        }
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

template <class Parameters>
class LegalMoveGenerator
{
private:
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;

private:
    KingMoveGenerator<Parameters> m_king_iter;
    NonKingBoardMoveGenerator<Parameters> m_board_iter;
    DropMoveGenerator<Parameters> m_drop_iter;
    uint m_index; //!< 0: king, 1: board, 2: drop, 3: end

public:
    LegalMoveGenerator(const StateType& s, const bool& include_drop = true)
        : m_king_iter(s), m_board_iter(s), m_drop_iter(s), m_index(0u)
    {
        if (!include_drop)
            m_drop_iter.increment_to_end();
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
        static const auto end_iter = LegalMoveGenerator(
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
        const KingMoveGenerator<Parameters>& king_iter,
        const NonKingBoardMoveGenerator<Parameters>& board_iter,
        const DropMoveGenerator<Parameters>& drop_iter,
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
    CheckNonKingBoardMoveGenerator<Parameters> m_board_iter;
    uint m_index; //!< 0: king, 1: board, 2: end

public:
    CheckBoardMoveGenerator(const StateType& s)
        : m_king_iter(s), m_board_iter(s), m_index(0u)
    {
        if (m_king_iter.is_end()) {
            ++m_index;
            if (m_board_iter.is_end()) {
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
    CheckBoardMoveGenerator begin()
    {
        return *this;
    }
    CheckBoardMoveGenerator end()
    {
        static const auto end_iter = CheckBoardMoveGenerator(
            m_king_iter.end(), m_board_iter.end(), 2u);
        return end_iter;
    }
    bool operator!=(const CheckBoardMoveGenerator& other) const
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
    CheckBoardMoveGenerator(
        const KingMoveGenerator<Parameters, true>& king_iter,
        const CheckNonKingBoardMoveGenerator<Parameters>& board_iter,
        const uint index)
        : m_king_iter(king_iter), m_board_iter(board_iter), m_index(index)
    {
    }
};

template <class Parameters>
class CheckMoveGenerator
{
private:
    using MoveType = Move<Parameters>;
    using StateType = State<Parameters>;

private:
    KingMoveGenerator<Parameters, true> m_king_iter;
    CheckNonKingBoardMoveGenerator<Parameters> m_board_iter;
    CheckDropMoveGenerator<Parameters> m_drop_iter;
    uint m_index; //!< 0: king, 1: board, 2: drop, 3: end

public:
    CheckMoveGenerator(const StateType& s)
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
    CheckMoveGenerator& operator++()
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
    CheckMoveGenerator begin()
    {
        return *this;
    }
    CheckMoveGenerator end()
    {
        static const auto end_iter = CheckMoveGenerator(
            m_king_iter.end(), m_board_iter.end(), m_drop_iter.end(), 3u);
        return end_iter;
    }
    bool operator!=(const CheckMoveGenerator& other) const
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
    CheckMoveGenerator(
        const KingMoveGenerator<Parameters, true>& king_iter,
        const CheckNonKingBoardMoveGenerator<Parameters>& board_iter,
        const CheckDropMoveGenerator<Parameters>& drop_iter,
        const uint index)
        : m_king_iter(king_iter), m_board_iter(board_iter),
          m_drop_iter(drop_iter), m_index(index)
    {
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATOR_HPP
