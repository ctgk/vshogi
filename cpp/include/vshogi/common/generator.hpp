#ifndef VSHOGI_COMMON_GENERATOR_HPP
#define VSHOGI_COMMON_GENERATOR_HPP

#include "vshogi/common/bitboard.hpp"
#include "vshogi/common/board.hpp"
#include "vshogi/common/color.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class Config>
class KingMoveGenerator
{
private:
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using StateType = State<Config>;
    using Square = typename Config::Square;
    using SHelper = Squares<Config>;
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    const Square m_src; //!< King square
    typename BitBoardType::SquareIterator m_iter;

public:
    KingMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_src(m_board.get_king_location(m_turn)),
          m_iter((BitBoardType::get_attacks_by(m_board[m_src], m_src)
                  & (~m_board.get_occupied(m_turn)))
                     .square_iterator())
    {
        increment_iterator_while_square_is_attacked();
    }
    KingMoveGenerator& operator++()
    {
        ++m_iter;
        increment_iterator_while_square_is_attacked();
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
        static const auto end_iter = KingMoveGenerator(m_state, true);
        return end_iter;
    }
    bool operator!=(const KingMoveGenerator& other) const
    {
        return m_iter != other.m_iter;
    }

private:
    KingMoveGenerator(const StateType& state, const bool)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_src(m_board.get_king_location(m_turn)),
          m_iter(BitBoardType().square_iterator())
    {
    }
    void increment_iterator_while_square_is_attacked()
    {
        while (!m_iter.is_end()) {
            if (m_board.is_square_attacked(~m_turn, *m_iter, m_src))
                ++m_iter;
            else
                break;
        }
    }
};

template <class Config>
class CheckKingMoveGenerator
{
private:
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using StateType = State<Config>;
    using Square = typename Config::Square;
    using SHelper = Squares<Config>;
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    const Square m_src; //!< King square
    typename BitBoardType::SquareIterator m_iter;

public:
    CheckKingMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_src(m_board.get_king_location(m_turn)), m_iter()
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        const auto checker_dir = SHelper::get_direction(m_src, enemy_king_sq);
        const auto checker_sq
            = m_board.find_attacker(m_turn, enemy_king_sq, checker_dir, m_src);
        if (checker_sq == SHelper::SQ_NA)
            return;
        const auto attacks
            = BitBoardType::get_attacks_by(m_board[m_src], m_src);
        const auto mask
            = (~m_board.get_occupied(m_turn))
              & (~BitBoardType::get_line_segment(checker_sq, enemy_king_sq));
        const auto dst_mask = (attacks & mask);
        m_iter = dst_mask.square_iterator();
        increment_iterator_while_square_is_attacked();
    }
    CheckKingMoveGenerator& operator++()
    {
        ++m_iter;
        increment_iterator_while_square_is_attacked();
        return *this;
    }
    MoveType operator*() const
    {
        return MoveType(*m_iter, m_src, false);
    }
    CheckKingMoveGenerator begin() const
    {
        return *this;
    }
    CheckKingMoveGenerator end() const
    {
        static const auto end_iter = CheckKingMoveGenerator(m_state, true);
        return end_iter;
    }
    bool operator!=(const CheckKingMoveGenerator& other) const
    {
        return m_iter != other.m_iter;
    }

private:
    CheckKingMoveGenerator(const StateType& state, const bool)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_src(m_board.get_king_location(m_turn)),
          m_iter(BitBoardType().square_iterator())
    {
    }
    void increment_iterator_while_square_is_attacked()
    {
        while (!m_iter.is_end()) {
            if (m_board.is_square_attacked(~m_turn, *m_iter, m_src))
                ++m_iter;
            else
                break;
        }
    }
};

template <class Config>
class DropMoveGenerator
{
private:
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using StateType = State<Config>;
    using StandType = Stand<Config>;
    using PieceType = typename Config::PieceType;
    using ColoredPiece = typename Config::ColoredPiece;
    using Square = typename Config::Square;
    using File = typename Config::File;
    using PHelper = Pieces<Config>;
    using SHelper = Squares<Config>;
    static constexpr uint num_dir = Config::num_dir;
    static constexpr uint num_stand_piece_types = Config::num_stand_piece_types;
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

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
        if (m_pt_iter == PHelper::FU)
            increment_square_for_pawn_unless_legal();
        else
            increment_square_unless_legal();
    }
    void increment_piece_type_unless_in_stand()
    {
        while (!m_stand.exist(m_pt_iter)
               && (m_pt_iter < num_stand_piece_types)) {
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
            if (has_pawn_in_file(SHelper::to_file(*m_sq_iter), p)) {
                ++m_sq_iter;
                continue;
            }
            if (is_drop_pawn_mate(*m_sq_iter, p)) {
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
    bool has_pawn_in_file(const File f, const ColoredPiece pawn) const
    {
        for (auto sq : SHelper::file_to_square_array[f]) {
            if (pawn == m_board[sq])
                return true;
        }
        return false;
    }
    bool is_drop_pawn_mate(const Square dst, const ColoredPiece pawn) const
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        if (SHelper::get_non_ranging_attacks_by(pawn, dst)[0] != enemy_king_sq)
            return false;

        // if enemy king can move away from the attack, then return false.
        const auto enemy_mask = m_board.get_occupied(~m_turn);
        const auto enemy_king_movable
            = BitBoardType::get_attacks_by(
                  m_board[enemy_king_sq], enemy_king_sq)
              & (~enemy_mask);
        for (auto sq : enemy_king_movable.square_iterator()) {
            if (m_board.is_square_attacked(m_turn, sq, enemy_king_sq))
                continue;
            return false;
        }

        // if enemy can capture the pawn, then return false.
        const auto enemy_king_dir = (m_turn == BLACK) ? DIR_N : DIR_S;
        for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
            if (dir == enemy_king_dir)
                continue;
            const auto src_next = m_board.find_attacker(~m_turn, dst, dir);
            const auto attacking_the_pawn = (src_next != SHelper::SQ_NA);
            if (attacking_the_pawn) {
                const auto discovered_dir
                    = SHelper::get_direction(src_next, enemy_king_sq);
                const auto discovered_attacker_sq = m_board.find_attacker(
                    m_turn, enemy_king_sq, discovered_dir, src_next);
                const auto pinned = (discovered_attacker_sq != SHelper::SQ_NA);
                if (!pinned)
                    return false;
            }
        }

        return true;
    }
};

template <class Config>
class CheckDropMoveGenerator
{
private:
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using StateType = State<Config>;
    using StandType = Stand<Config>;
    using PieceType = typename Config::PieceType;
    using ColoredPiece = typename Config::ColoredPiece;
    using Square = typename Config::Square;
    using File = typename Config::File;
    using PHelper = Pieces<Config>;
    using SHelper = Squares<Config>;
    static constexpr uint num_dir = Config::num_dir;
    static constexpr uint num_stand_piece_types = Config::num_stand_piece_types;
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

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
            } else {
                break;
            }
        }
    }
    void increment_square()
    {
        if (m_pt_iter == PHelper::FU)
            increment_square_for_pawn_unless_legal();
        else
            increment_square_unless_legal();
    }
    void increment_piece_type_unless_in_stand()
    {
        while (!m_stand.exist(m_pt_iter)
               && (m_pt_iter < num_stand_piece_types)) {
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
            if (has_pawn_in_file(SHelper::to_file(*m_sq_iter), p)) {
                ++m_sq_iter;
                continue;
            }
            if (is_drop_pawn_mate(*m_sq_iter, p)) {
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
    bool has_pawn_in_file(const File f, const ColoredPiece pawn) const
    {
        for (auto sq : SHelper::file_to_square_array[f]) {
            if (pawn == m_board[sq])
                return true;
        }
        return false;
    }
    bool is_drop_pawn_mate(const Square dst, const ColoredPiece pawn) const
    {
        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        if (SHelper::get_non_ranging_attacks_by(pawn, dst)[0] != enemy_king_sq)
            return false;

        // if enemy king can move away from the attack, then return false.
        const auto enemy_mask = m_board.get_occupied(~m_turn);
        const auto enemy_king_movable
            = BitBoardType::get_attacks_by(
                  m_board[enemy_king_sq], enemy_king_sq)
              & (~enemy_mask);
        for (auto sq : enemy_king_movable.square_iterator()) {
            if (m_board.is_square_attacked(m_turn, sq, enemy_king_sq))
                continue;
            return false;
        }

        // if enemy can capture the pawn, then return false.
        const auto enemy_king_dir = (m_turn == BLACK) ? DIR_N : DIR_S;
        for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
            if (dir == enemy_king_dir)
                continue;
            const auto src_next = m_board.find_attacker(~m_turn, dst, dir);
            const auto attacking_the_pawn = (src_next != SHelper::SQ_NA);
            if (attacking_the_pawn) {
                const auto discovered_dir
                    = SHelper::get_direction(src_next, enemy_king_sq);
                const auto discovered_attacker_sq = m_board.find_attacker(
                    m_turn, enemy_king_sq, discovered_dir, src_next);
                const auto pinned = (discovered_attacker_sq != SHelper::SQ_NA);
                if (!pinned)
                    return false;
            }
        }

        return true;
    }
};

template <class Config>
class NonKingBoardMoveGenerator
{
private:
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using StateType = State<Config>;
    using Square = typename Config::Square;
    using PHelper = Pieces<Config>;
    using SHelper = Squares<Config>;
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    typename BitBoardType::SquareIterator m_src_iter;
    typename BitBoardType::SquareIterator m_dst_iter;
    bool m_promote;

public:
    NonKingBoardMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_src_iter(), m_dst_iter(), m_promote(true)
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
        static const auto end_iter = NonKingBoardMoveGenerator(m_state, true);
        return end_iter;
    }
    bool operator!=(const NonKingBoardMoveGenerator& other) const
    {
        return (m_src_iter != other.m_src_iter)
               || (m_dst_iter != other.m_dst_iter)
               || (m_promote != other.m_promote);
    }

private:
    NonKingBoardMoveGenerator(const StateType& state, const bool promote)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_src_iter(), m_dst_iter(), m_promote(promote)
    {
    }
    void init_src_iter()
    {
        const auto king_sq = m_board.get_king_location(m_turn);
        const auto src_mask = m_board.get_occupied(m_turn).clear(king_sq);
        m_src_iter = src_mask.square_iterator();
    }
    void init_dst_iter()
    {
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        auto movable
            = BitBoardType::get_attacks_by(p, src, m_board.get_occupied());
        movable &= ~m_board.get_occupied(m_turn);
        const auto king_sq = m_board.get_king_location(m_turn);
        if (m_state.in_check()) {
            const auto checker_sq = m_state.get_checker_location();
            movable &= BitBoardType::get_line_segment(checker_sq, king_sq)
                           .set(checker_sq);
        }
        const auto src_dir_from_king = SHelper::get_direction(src, king_sq);
        const auto hidden_attacker_sq
            = m_board.find_attacker(~m_turn, king_sq, src_dir_from_king, src);
        if (hidden_attacker_sq != SHelper::SQ_NA) {
            movable
                &= BitBoardType::get_line_segment(hidden_attacker_sq, king_sq)
                       .set(hidden_attacker_sq);
        }
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
};

template <class Config>
class CheckNonKingBoardMoveGenerator
{
private:
    using BitBoardType = BitBoard<Config>;
    using BoardType = Board<Config>;
    using MoveType = Move<Config>;
    using StateType = State<Config>;
    using Square = typename Config::Square;
    using PHelper = Pieces<Config>;
    using SHelper = Squares<Config>;
    static constexpr auto SQ_NA = SHelper::SQ_NA; // NOLINT

private:
    const StateType& m_state;
    const ColorEnum m_turn;
    const BoardType& m_board;
    typename BitBoardType::SquareIterator m_src_iter;
    bool m_promote;
    typename BitBoardType::SquareIterator m_dst_iter;

public:
    CheckNonKingBoardMoveGenerator(const StateType& state)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_src_iter(), m_promote(true), m_dst_iter()
    {
        if (m_state.in_double_check())
            return;
        init_src_iter();
        if (m_src_iter.is_end())
            return;

        init_promote();
        while (!m_src_iter.is_end()) {
            init_dst_iter();
            if (m_dst_iter.is_end()) {
                if (m_promote) {
                    ++m_src_iter;
                    m_promote = false;
                } else {
                    m_promote = true;
                }
            } else
                return;
        }
        m_promote = true;
    }
    CheckNonKingBoardMoveGenerator& operator++()
    {
        ++m_dst_iter;
        if (!m_dst_iter.is_end())
            return *this;

        if (m_promote == false) {
            m_promote = true;
            init_dst_iter();
            if (!m_dst_iter.is_end())
                return *this;
        }

        m_promote = false;
        while (!m_src_iter.is_end()) {
            ++m_src_iter;
            if (m_src_iter.is_end()) {
                m_promote = true;
                m_dst_iter = BitBoardType().square_iterator();
                return *this;
            }
            m_promote = false;
            init_dst_iter();
            if (!m_dst_iter.is_end())
                return *this;
            m_promote = true;
            init_dst_iter();
            if (!m_dst_iter.is_end())
                return *this;
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

private:
    CheckNonKingBoardMoveGenerator(const StateType& state, const bool promote)
        : m_state(state), m_turn(state.get_turn()), m_board(state.get_board()),
          m_src_iter(), m_promote(promote), m_dst_iter()
    {
    }
    void init_src_iter()
    {
        const auto king_sq = m_board.get_king_location(m_turn);
        const auto src_mask = m_board.get_occupied(m_turn).clear(king_sq);
        m_src_iter = src_mask.square_iterator();
    }
    void init_dst_iter()
    {
        m_dst_iter = BitBoardType().square_iterator();
        const auto src = *m_src_iter;
        const auto p = m_board[src];
        if (m_promote && (!PHelper::is_promotable(p)))
            return;

        auto movable
            = BitBoardType::get_attacks_by(p, src, m_board.get_occupied());
        movable &= ~m_board.get_occupied(m_turn);
        if (!movable.any())
            return;

        if (m_promote && !SHelper::in_promotion_zone(src, m_turn)) {
            movable &= BitBoardType::get_promotion_zone(m_turn);
            if (!movable.any())
                return;
        }

        const auto king_sq = m_board.get_king_location(m_turn);
        if (m_state.in_check()) {
            const auto checker_sq = m_state.get_checker_location();
            movable &= BitBoardType::get_line_segment(checker_sq, king_sq)
                           .set(checker_sq);
            if (!movable.any())
                return;
        }

        const auto src_dir_from_king = SHelper::get_direction(src, king_sq);
        const auto hidden_attacker_sq
            = m_board.find_attacker(~m_turn, king_sq, src_dir_from_king, src);
        if (hidden_attacker_sq != SHelper::SQ_NA) {
            movable
                &= BitBoardType::get_line_segment(hidden_attacker_sq, king_sq)
                       .set(hidden_attacker_sq);
            if (!movable.any())
                return;
        }

        const auto enemy_king_sq = m_board.get_king_location(~m_turn);
        auto pt = PHelper::to_piece_type(p);
        if (m_promote)
            pt = PHelper::promote_nocheck(pt);
        const auto discovered_checker_sq = m_board.find_attacker(
            m_turn,
            enemy_king_sq,
            SHelper::get_direction(src, enemy_king_sq),
            src);
        if (discovered_checker_sq == SQ_NA) // check by moving piece.
            movable &= BitBoardType::get_attacks_by(
                PHelper::to_board_piece(~m_turn, pt),
                enemy_king_sq,
                m_board.get_occupied());
        else { // check by moving piece or a discovered piece.
            movable
                &= (BitBoardType::get_attacks_by(
                        PHelper::to_board_piece(~m_turn, pt),
                        enemy_king_sq,
                        m_board.get_occupied())
                    | (~BitBoardType::get_line_segment(
                        discovered_checker_sq, enemy_king_sq)));
        }
        if (!movable.any())
            return;
        m_dst_iter = movable.square_iterator();
    }
    void init_promote()
    {
        m_promote = false;
    }
};

} // namespace vshogi

#endif // VSHOGI_COMMON_GENERATOR_HPP
