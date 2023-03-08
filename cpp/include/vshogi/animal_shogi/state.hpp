#ifndef VSHOGI_ANIMAL_SHOGI_STATE_HPP
#define VSHOGI_ANIMAL_SHOGI_STATE_HPP

#include <string>

#include "vshogi/animal_shogi/board.hpp"
#include "vshogi/animal_shogi/color.hpp"
#include "vshogi/animal_shogi/move.hpp"
#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/piece_stand.hpp"

namespace vshogi::animal_shogi
{

/**
 * @brief SFEN string representing default initial state of Animal Shogi.
 *
 */
constexpr char default_initial_sfen[] = "gle/1c1/1C1/ELG b - 1";

class State
{
private:
    Board m_board;
    TwoPieceStands m_two_piece_stands;
    ColorEnum m_turn; //!< Player to move next.

public:
    State();
    State(const std::string& sfen);

    const Board& get_board() const
    {
        return m_board;
    }
    const PieceStand& get_piece_stand(const ColorEnum c) const
    {
        return m_two_piece_stands[c];
    }
    ColorEnum get_turn() const
    {
        return m_turn;
    }

    /**
     * @brief Check if a move is legal or not.
     *
     * @param move
     * @return true
     * @return false
     */
    bool is_move_legal(const Move move) const
    {
        const auto dst = move.destination();
        if (move.is_drop()) {
            const auto piece = to_captured(move.source());
            return (
                m_two_piece_stands[m_turn].exist(piece)
                && (m_board.get_piece_at(dst) == VOID)
                && is_movable_after_drop(piece, dst));
        } else {
            const auto src = to_square(move.source());
            const auto piece = m_board.get_piece_at(src);
            return (
                (to_color(piece) == m_turn)
                && get_attacks_by(piece, src).is_one(dst)
                && is_empty_or_opponent_piece_on_square(dst)
                && !is_check_after_move(piece, dst));
        }
    }
    State& apply_move(const Move move)
    {
        const auto dst = move.destination();

        const PieceTypeEnum captured_piece = pop_piece_from_board(dst);
        add_captured_piece_to_stand(captured_piece);

        const BoardPieceTypeEnum moving_piece
            = pop_piece_from_stand_or_board(move.source());
        place_piece_on_board(dst, moving_piece);
        change_turn();
        return *this;
    }

    /**
     * @brief hash current state.
     * @details
     * (MSB) <Turn + alpha: 1byte> <Stand: 1byte> <Board: 1byte> (LSB)
     * - First 1 byte: Turn (+ alpha)
     *     - <Turn: 1bit> <Other: 7bit>
     * - Second 1 bytes: Pieces on stand of the turn player.
     * - Last 6 bytes: Board (4bit x 12squares = 48bit = 6byte)
     * @return std::uint64_t
     */
    std::uint64_t hash() const
    {
        return (
            (static_cast<std::uint64_t>(m_turn) << 63)
            + (static_cast<std::uint64_t>(
                   m_two_piece_stands[m_turn].get_value())
               << 48)
            + m_board.hash());
    }

private:
    PieceTypeEnum pop_piece_from_board(const SquareEnum sq)
    {
        return m_board.pop_piece_at(sq);
    }
    void add_captured_piece_to_stand(const PieceTypeEnum p)
    {
        const auto captured = to_captured(p);
        if (captured != C_NA) {
            m_two_piece_stands[m_turn].add(captured);
        }
    }
    BoardPieceTypeEnum
    promote(const BoardPieceTypeEnum p, const SquareEnum source)
    {
        if (to_piece_type(p) != CH)
            return p;
        const auto r = to_rank(source);
        if ((m_turn == BLACK) && (r == RANK2))
            return B_HE;
        if ((m_turn == WHITE) && (r == RANK3))
            return W_HE;
        return p;
    }

    BoardPieceTypeEnum pop_piece_from_stand_or_board(const MoveSourceEnum src)
    {
        if (is_drop(src)) {
            const auto p = to_captured(src);
            m_two_piece_stands[m_turn].subtract(p);
            return to_board_piece(m_turn, p);
        } else {
            const auto sq = static_cast<SquareEnum>(src);
            const auto p = m_board.get_piece_at(sq);
            m_board.pop_piece_at(sq);
            return promote(p, sq);
        }
    }
    void place_piece_on_board(const SquareEnum sq, const BoardPieceTypeEnum p)
    {
        m_board.place_piece_at(sq, p);
    }
    void change_turn()
    {
        m_turn = opposite(m_turn);
    }
    bool is_movable_after_drop(
        const CapturedPieceTypeEnum p, const SquareEnum destination) const
    {
        if (p == C_CH) {
            const auto r = to_rank(destination);
            return (m_turn == BLACK) ? (r != RANK1) : (r != RANK4);
        }
        return true;
    }
    bool is_empty_or_opponent_piece_on_square(const SquareEnum sq) const
    {
        const auto p = m_board.get_piece_at(sq);
        if (p == VOID)
            return true;
        return to_color(p) != m_turn;
    }
    bool is_check_after_move(
        const BoardPieceTypeEnum moving_piece,
        const SquareEnum destination) const
    {
        if (to_piece_type(moving_piece) != LI)
            return false; // there is no discovered attacks in Animal Shogi.
        const auto attacks_by_opponent
            = m_board.to_attack_mask(opposite(to_color(moving_piece)));
        return attacks_by_opponent.is_one(destination);
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
