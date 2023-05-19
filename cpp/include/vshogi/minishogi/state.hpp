#ifndef VSHOGI_MINISHOGI_STATE_HPP
#define VSHOGI_MINISHOGI_STATE_HPP

#include <string>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/minishogi/board.hpp"
#include "vshogi/minishogi/move.hpp"
#include "vshogi/minishogi/stand.hpp"

namespace vshogi::minishogi
{

class State
{
private:
    Board m_board;
    BlackWhiteStands m_stands;
    ColorEnum m_turn; //! Player to move next.

    BitBoard m_piece_masks[num_colors];
    BitBoard m_attack_masks[num_colors];

    void update_masks()
    {
        m_piece_masks[BLACK] = m_board.to_piece_mask(BLACK);
        m_piece_masks[WHITE] = m_board.to_piece_mask(WHITE);
        m_attack_masks[BLACK] = m_board.to_attack_mask(BLACK);
        m_attack_masks[WHITE] = m_board.to_attack_mask(WHITE);
    }

    BoardPieceTypeEnum pop_piece_from_stand_or_board(const MoveSourceEnum src)
    {
        if (src < num_squares) {
            const auto s = to_square(src);
            const auto out = m_board[s];
            m_board[s] = VOID;
            return out;
        } else {
            const auto s = to_piece_type(src);
            const auto out = to_board_piece(m_turn, s);
            m_stands[m_turn].subtract(s);
            return out;
        }
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
    bool
    is_checkmate_by_pawn_drop(const PieceTypeEnum p, const SquareEnum sq) const
    {
        if (p != FU)
            return false;

        // Note that `has_movable_square_after_drop` precedes this method.
        const auto attacking_sq = shift(sq, (m_turn == BLACK) ? DIR_N : DIR_S);
        const auto attacked = m_board[attacking_sq];
        if (attacked != to_board_piece(~m_turn, OU))
            return false;
        {
            auto b = m_board;
            b[attacking_sq] = VOID;
            const auto attack_mask = b.to_attack_mask(~m_turn);
            if (attack_mask.is_one(sq))
                return false; // Opponent can capture my pawn next turn.
        }

        const auto opponent_king_attacks
            = get_attacks_by(attacked, attacking_sq);
        const auto opponent_king_movable = opponent_king_attacks
                                           & (~m_piece_masks[~m_turn])
                                           & (~m_attack_masks[m_turn]);
        const auto opponent_king_is_alive = opponent_king_movable.any();
        return !opponent_king_is_alive;
    }
    bool is_legal_drop(const Move move) const
    {
        const auto p = to_piece_type(move.source());
        const auto dst = move.destination();
        return (
            has_piece_on_turn_player_stand(p) && is_empty_square(dst)
            && has_movable_square_after_move(p, dst) && is_valid_promotion(move)
            && (!is_checkmate_by_pawn_drop(p, dst))
            && (!is_my_king_in_check_after_move(move)));
    }
    bool is_empty_or_opponent_square(const SquareEnum sq) const
    {
        const auto p = m_board[sq];
        return (p == VOID) || (get_color(p) == (~m_turn));
    }
    bool is_my_king_in_check_after_move(const Move m) const
    {
        const auto dst = m.destination();
        auto b = m_board;
        if (m.is_drop()) {
            b[dst] = to_board_piece(m_turn, to_piece_type(m.source()));
        } else {
            const auto src = to_square(m.source());
            const auto tmp = b[src];
            b[src] = VOID;
            b[dst] = tmp;
        }
        return b.in_check(m_turn);
    }
    bool is_valid_promotion(const Move move) const
    {
        if (!move.promote())
            return true;
        if (move.is_drop())
            return false;
        const auto p = to_piece_type(m_board[to_square(move.source())]);
        if (!is_promotable(p))
            return false;

        const auto dst = to_rank(move.destination());
        const auto src = to_rank(to_square(move.source()));
        const auto r = (m_turn == BLACK) ? RANK1 : RANK5;
        return (dst == r) || (src == r);
    }

    bool is_legal_board(const Move move) const
    {
        const auto src = to_square(move.source());
        const auto moving = m_board[src];
        const auto dst = move.destination();
        return (
            (get_color(moving) == m_turn) // No VOID check on purpose
            && is_empty_or_opponent_square(dst)
            && get_attacks_by(
                   moving, src, m_piece_masks[m_turn] | m_piece_masks[~m_turn])
                   .is_one(dst) // VOID has no 1 in it.
            && is_valid_promotion(move)
            && has_movable_square_after_move(
                move.promote() ? promote(moving) : moving, dst)
            && !is_my_king_in_check_after_move(move));
    }
    bool is_promotion_zone(const RankEnum r) const
    {
        if (m_turn == BLACK)
            return r == RANK1;
        return r == RANK5;
    }
    bool is_promotion_zone(const SquareEnum sq) const
    {
        return is_promotion_zone(to_rank(sq));
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
                if (is_promotion_zone(dst)) {
                    const auto promotion_move = Move(dst, src, true);
                    if (is_legal_board(promotion_move))
                        out.emplace_back(promotion_move);
                }
            }
        }
    }
    void append_legal_moves_by_stand_pieces(std::vector<Move>& out) const
    {
        const auto empty = ~m_board.to_piece_mask();
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
};

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_STATE_HPP
