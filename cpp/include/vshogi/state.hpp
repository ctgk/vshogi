#ifndef VSHOGI_STATE_HPP
#define VSHOGI_STATE_HPP

#include <string>
#include <type_traits>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi
{

template <class Board, class Stands, class Move>
class State
{
    static_assert(std::is_same<
                  typename Board::PiecesType,
                  typename Move::PiecesType>::value);
    static_assert(std::is_same<
                  typename Board::SquaresType,
                  typename Move::SquaresType>::value);

private:
    using BitBoard = typename Board::BitBoardType;
    using Squares = typename Board::SquaresType;
    using Pieces = typename Board::PiecesType;
    using Stand = typename Stands::StandType;

public:
    using BoardType = Board;
    using BitBoardType = typename Board::BitBoardType;
    using SquaresType = typename Board::SquaresType;
    using PiecesType = typename Board::PiecesType;
    using StandType = typename Stands::StandType;
    using MoveType = Move;
    using PieceTypeEnum = typename Pieces::PieceTypeEnum;
    using BoardPieceTypeEnum = typename Pieces::BoardPieceTypeEnum;
    using RankEnum = typename Squares::RankEnum;
    using SquareEnum = typename Squares::SquareEnum;

private:
    Board m_board;
    Stands m_stands;
    ColorEnum m_turn; //!< Player to make a move in the current state.

    BitBoard m_piece_masks[num_colors];

public:
    State() : m_board(), m_stands(), m_turn(BLACK)
    {
        update_masks();
    }
    State(const std::string& sfen) : m_board(), m_stands(), m_turn()
    {
        set_sfen(sfen);
    }
    const Board& get_board() const
    {
        return m_board;
    }
    const Stand& get_stand(const ColorEnum c) const
    {
        return m_stands[c];
    }
    ColorEnum get_turn() const
    {
        return m_turn;
    }
    void set_sfen(const std::string& sfen)
    {
        auto s = sfen.c_str();
        s = m_board.set_sfen(s);
        {
            (*s == 'b') ? m_turn = BLACK : m_turn = WHITE;
            s += 2;
        }
        m_stands.set_sfen_holdings(s);
        update_masks();
    }
    std::string to_sfen() const
    {
        return m_board.to_sfen() + " " + to_sfen_turn() + " "
               + m_stands.to_sfen_holdings();
    }
    State& apply(const Move move)
    {
        const auto dst = move.destination();
        auto moving = pop_piece_from_stand_or_board(move);
        const auto captured = Pieces::to_piece_type(m_board[dst]);
        if (captured != Pieces::NA)
            m_stands[m_turn].add(captured);
        if (move.promote())
            moving = Pieces::promote(moving);
        m_board[dst] = moving;
        m_turn = ~m_turn;
        update_masks();
        return *this;
    }
    bool is_legal(const Move move) const
    {
        if (move.is_drop())
            return is_legal_drop(move);
        return is_legal_board(move);
    }
    std::vector<Move> get_legal_moves() const
    {
        auto out = std::vector<Move>();
        append_legal_moves_by_board_pieces(out);
        append_legal_moves_by_stand_pieces(out);
        return out;
    }

private:
    void update_masks()
    {
        m_piece_masks[BLACK] = m_board.to_piece_mask(BLACK);
        m_piece_masks[WHITE] = m_board.to_piece_mask(WHITE);
    }
    std::string to_sfen_turn() const
    {
        return (m_turn == BLACK) ? "b" : "w";
    }
    BoardPieceTypeEnum pop_piece_from_stand_or_board(const Move move)
    {
        if (move.is_drop()) {
            const auto src = move.source_piece();
            const auto out = Pieces::to_board_piece(m_turn, src);
            m_stands[m_turn].subtract(src);
            return out;
        } else {
            const auto src = move.source_square();
            const auto out = m_board[src];
            m_board[src] = Pieces::VOID;
            return out;
        }
    }
    void append_legal_moves_by_stand_pieces(std::vector<Move>& out) const
    {
        const auto occupied = (m_piece_masks[BLACK] | m_piece_masks[WHITE]);
        const auto stand = m_stands[m_turn];
        for (auto piece : Pieces::stand_piece_array) {
            if (!stand.exist(piece))
                continue;
            for (auto dst : Squares::square_array) {
                if (occupied.is_one(dst))
                    continue;
                const auto m = Move(dst, piece);
                if (is_legal_drop(m))
                    out.emplace_back(m);
            }
        }
    }
    void append_legal_moves_by_board_pieces(std::vector<Move>& out) const
    {
        for (auto src : Squares::square_array) {
            const auto piece = m_board[src];
            if (m_board.is_empty(src) || (Pieces::get_color(piece) != m_turn))
                continue;
            for (auto dst : Squares::square_array) {
                const auto m = Move(dst, src);
                if (is_legal_board(m))
                    out.emplace_back(m);
                if (Squares::is_promotion_zone(dst, m_turn)
                    || Squares::is_promotion_zone(src, m_turn)) {
                    const auto promotion_move = Move(dst, src, true);
                    if (is_legal_board(promotion_move))
                        out.emplace_back(promotion_move);
                }
            }
        }
    }

private:
    bool is_legal_drop(const Move move) const
    {
        const auto p = move.source_piece();
        const auto dst = move.destination();
        return (
            has_piece_on_turn_player_stand(p) && m_board.is_empty(dst)
            && has_movable_square_after_move(
                Pieces::to_board_piece(m_turn, p), dst)
            && (!is_drop_pawn_on_pawn_existing_file(p, dst))
            && (!is_drop_pawn_mate(p, dst))
            && (!king_in_check_after_move(move, m_turn)));
    }
    bool has_piece_on_turn_player_stand(const PieceTypeEnum p) const
    {
        return m_stands[m_turn].exist(p);
    }
    bool is_drop_pawn_on_pawn_existing_file(
        const PieceTypeEnum p, const SquareEnum dst) const
    {
        if (p != Pieces::FU)
            return false;
        const auto file_dst = Squares::to_file(dst);
        for (auto sq : Squares::squares_in_file[file_dst]) {
            if (m_board[sq] == Pieces::to_board_piece(m_turn, Pieces::FU))
                return true;
        }
        return false;
    }
    bool is_drop_pawn_mate(const PieceTypeEnum p, const SquareEnum dst) const
    {
        if (p != Pieces::FU)
            return false;

        // Note that `has_movable_square_after_drop` precedes this method.
        const auto attacking_sq
            = Squares::shift(dst, (m_turn == BLACK) ? DIR_N : DIR_S);
        const auto attacked = m_board[attacking_sq];
        if (attacked != Pieces::to_board_piece(~m_turn, Pieces::OU))
            return false;
        return !can_opponent_capture_piece_at(dst);
    }
    bool can_opponent_capture_piece_at(const SquareEnum target) const
    {
        const auto occupied = m_piece_masks[BLACK] | m_piece_masks[WHITE];
        for (auto src : Squares::square_array) {
            const auto p = m_board[src];
            if ((p == Pieces::VOID) || (Pieces::get_color(p) == m_turn))
                continue;
            const auto attacks = BitBoard::get_attacks_by(p, src, occupied);
            if (attacks.is_one(target)) {
                auto b = m_board;
                b[src] = Pieces::VOID;
                b[target] = p;
                if (!b.in_check(~m_turn))
                    return true;
            }
        }
        return false;
    }

private:
    bool is_legal_board(const Move move) const
    {
        const auto src = move.source_square();
        const auto moving = m_board[src];
        const auto dst = move.destination();
        return (
            (Pieces::get_color(moving) == m_turn) // No VOID check on purpose
            && is_empty_or_opponent_square(dst)
            && BitBoard::get_attacks_by(
                   moving, src, m_piece_masks[BLACK] | m_piece_masks[WHITE])
                   .is_one(dst) // VOID should return false here.
            && is_valid_promotion(move)
            && has_movable_square_after_move(
                move.promote() ? Pieces::promote(moving) : moving, dst)
            && !king_in_check_after_move(move, m_turn));
    }
    bool is_empty_or_opponent_square(const SquareEnum sq) const
    {
        return (
            m_board.is_empty(sq) || (Pieces::get_color(m_board[sq]) != m_turn));
    }
    bool has_movable_square_after_move(
        const BoardPieceTypeEnum p, const SquareEnum dst) const
    {
        return BitBoard::get_attacks_by(p, dst).any();
    }
    bool king_in_check_after_move(const Move m, const ColorEnum c) const
    {
        const auto dst = m.destination();
        auto b = m_board;
        if (m.is_drop()) {
            b[dst] = Pieces::to_board_piece(m_turn, m.source_piece());
        } else {
            const auto src = m.source_square();
            const auto tmp = b[src];
            b[src] = Pieces::VOID;
            b[dst] = tmp;
        }
        return b.in_check(c);
    }

private:
    bool is_valid_promotion(const Move move) const
    {
        if (!move.promote())
            return true;
        if (move.is_drop())
            return false;
        const auto p = Pieces::to_piece_type(m_board[move.source_square()]);
        if (!Pieces::is_promotable(p))
            return false;

        const auto dst = move.destination();
        const auto src = move.source_square();
        return Squares::is_promotion_zone(dst, m_turn)
               || Squares::is_promotion_zone(src, m_turn);
    }
};

} // namespace vshogi

#endif // VSHOGI_STATE_HPP
