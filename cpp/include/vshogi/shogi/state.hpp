#ifndef VSHOGI_SHOGI_STATE_HPP
#define VSHOGI_SHOGI_STATE_HPP

#include <vector>

#include "vshogi/shogi/board.hpp"
#include "vshogi/shogi/move.hpp"
#include "vshogi/shogi/stand.hpp"

namespace vshogi::shogi
{

class State
{
private:
    Board m_board;
    BlackWhiteStands m_stands;
    ColorEnum m_turn; //!< Player to move next.

    BitBoard m_piece_masks[num_colors];
    BitBoard m_attack_masks[num_colors];

public:
    State() : m_board(), m_stands(), m_turn(BLACK)
    {
        update_masks();
    }
    State(const std::string& sfen) : State()
    {
        set_sfen(sfen);
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
    State& apply(const Move move)
    {
        const auto dst = move.destination();
        auto moving = pop_piece_from_stand_or_board(move.source());
        const auto captured = to_piece_type(m_board[dst]);
        if (captured != NA)
            m_stands[m_turn].add(captured);
        if (move.promote())
            moving = promote(moving);
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
    std::string to_sfen() const
    {
        return m_board.to_sfen() + " " + to_sfen_turn() + " "
               + m_stands.to_sfen_holdings();
    }

private:
    void update_masks()
    {
        m_piece_masks[BLACK] = m_board.to_piece_mask(BLACK);
        m_piece_masks[WHITE] = m_board.to_piece_mask(WHITE);
        m_attack_masks[BLACK] = m_board.to_attack_mask(BLACK);
        m_attack_masks[WHITE] = m_board.to_attack_mask(WHITE);
    }
    BoardPieceTypeEnum pop_piece_from_stand_or_board(const MoveSourceEnum src)
    {
        if (is_square(src))
            return pop_piece_from_board(to_square(src));
        return pop_piece_from_stand(to_piece_type(src));
    }
    BoardPieceTypeEnum pop_piece_from_board(const SquareEnum src)
    {
        const auto out = m_board[src];
        m_board[src] = VOID;
        return out;
    }
    BoardPieceTypeEnum pop_piece_from_stand(const PieceTypeEnum src)
    {
        const auto out = to_board_piece(m_turn, src);
        m_stands[m_turn].subtract(src);
        return out;
    }

    bool is_legal_drop(const Move move) const
    {
        const auto p = to_piece_type(move.source());
        const auto dst = move.destination();
        return has_piece_on_turn_player_stand(p) && is_empty_square(dst)
               && has_movable_square_after_move(p, dst)
               && (move.promote() == false) && (!is_drop_pawn_mate(p, dst))
               && (!is_my_king_in_check_after_move(p, dst));
    }
    bool has_piece_on_turn_player_stand(const PieceTypeEnum p) const
    {
        return m_stands[m_turn].exist(p);
    }
    bool is_empty_square(const SquareEnum sq) const
    {
        return m_board[sq] == VOID;
    }
    bool has_movable_square_after_move(
        const BoardPieceTypeEnum p, const SquareEnum dst) const
    {
        return get_attacks_by(p, dst).any();
    }
    bool has_movable_square_after_move(
        const PieceTypeEnum p, const SquareEnum dst) const
    {
        return has_movable_square_after_move(to_board_piece(m_turn, p), dst);
    }
    bool is_drop_pawn_mate(const PieceTypeEnum p, const SquareEnum dst) const
    {
        if (p != FU)
            return false;
        const auto attacking_sq = shift(dst, (m_turn == BLACK) ? DIR_N : DIR_S);
        if (!is_opponent_king_sq(attacking_sq))
            return false;
        return !can_opponent_capture_piece_at(dst);
    }
    bool is_opponent_king_sq(const SquareEnum sq) const
    {
        return (m_board[sq] == to_board_piece(~m_turn, OU));
    }
    bool can_opponent_capture_piece_at(const SquareEnum target) const
    {
        const auto occupied = m_piece_masks[BLACK] | m_piece_masks[WHITE];
        for (auto src : square_array) {
            const auto p = m_board[src];
            if ((p == VOID) || (get_color(p) == m_turn))
                continue;
            const auto attacks = get_attacks_by(p, src, occupied);
            if (attacks.is_one(target)) {
                auto b = m_board;
                b[src] = VOID;
                b[target] = p;
                if (!b.in_check(~m_turn))
                    return true;
            }
        }
        return false;
    }
    bool is_my_king_in_check_after_move(
        const PieceTypeEnum p, const SquareEnum dst) const
    {
        auto b = m_board;
        b[dst] = to_board_piece(m_turn, p);
        return b.in_check(m_turn);
    }

    bool is_legal_board(const Move move) const
    {
        const auto src = to_square(move.source());
        const auto moving = m_board[src];
        const auto dst = move.destination();
        const auto promotion = move.promote();
        const auto attacks = get_attacks_by(
            moving, src, m_piece_masks[BLACK] | m_piece_masks[WHITE]);
        return (
            (moving != VOID) && (get_color(moving) == m_turn)
            && is_empty_or_opponent_square(dst) && attacks.is_one(dst)
            && is_valid_promotion(moving, dst, src, promotion)
            && (!is_my_king_in_check_after_move(dst, src)));
    }
    bool is_empty_or_opponent_square(const SquareEnum sq) const
    {
        return is_empty_square(sq) || (get_color(m_board[sq]) != m_turn);
    }
    bool is_valid_promotion(
        const BoardPieceTypeEnum p,
        const SquareEnum dst,
        const SquareEnum src,
        const bool promotion) const
    {
        if (promotion)
            return is_promotable(p)
                   && (is_in_promotion_zone(src) || is_in_promotion_zone(dst));
        return has_movable_square_after_move(p, dst);
    }
    bool is_in_promotion_zone(const RankEnum r) const
    {
        if (m_turn == BLACK)
            return (r == RANK1) || (r == RANK2) || (r == RANK3);
        return (r == RANK7) || (r == RANK8) || (r == RANK9);
    }
    bool is_in_promotion_zone(const SquareEnum sq) const
    {
        return is_in_promotion_zone(to_rank(sq));
    }
    bool is_my_king_in_check_after_move(
        const SquareEnum dst, const SquareEnum src) const
    {
        auto b = m_board;
        const auto tmp = b[src];
        b[src] = VOID;
        b[dst] = tmp;
        return b.in_check(m_turn);
    }

    void append_legal_moves_by_board_pieces(std::vector<Move>& out) const
    {
        for (auto src : square_array) {
            const auto piece = m_board[src];
            if ((piece == VOID) || (get_color(piece) != m_turn))
                continue;
            for (auto dst : square_array) {
                const auto m = Move(dst, src);
                if (is_legal_board(m))
                    out.emplace_back(m);
                if (is_in_promotion_zone(src) || is_in_promotion_zone(dst)) {
                    const auto promotion_move = Move(dst, src, true);
                    if (is_legal_board(promotion_move))
                        out.emplace_back(promotion_move);
                }
            }
        }
    }
    void append_legal_moves_by_stand_pieces(std::vector<Move>& out) const
    {
        const auto empty = ~(m_piece_masks[BLACK] | m_piece_masks[WHITE]);
        const auto stand = m_stands[m_turn];
        for (auto piece : stand_piece_array) {
            if (!stand.exist(piece))
                continue;
            for (auto dst : square_array) {
                if (!empty.is_one(dst))
                    continue;
                const auto m = Move(dst, piece);
                if (is_legal_drop(m))
                    out.emplace_back(m);
            }
        }
    }
    std::string to_sfen_turn() const
    {
        return (m_turn == BLACK) ? "b" : "w";
    }
};

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_STATE_HPP
