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
     * @brief Check if a move is applicable or not.
     * @details Unlike ordinary Shogi, the followings are legal:
     * - Two Chicks in one file.
     * - Checkmate by dropping a Chick.
     * - Dropping a Chick on the final rank (, which will never promote to Hen)
     * - Perpetual check.
     *
     * cf. https://en.wikipedia.org/wiki/D%C5%8Dbutsu_sh%C5%8Dgi#Play
     *
     * @param move
     * @return true The move is applicable.
     * @return false The move is not applicable
     */
    bool is_applicable(const Move move) const
    {
        const auto src = move.source();
        const auto dst = move.destination();
        if (move.is_drop()) {
            const auto piece = to_piece_type(src);
            return (
                m_two_piece_stands[m_turn].exist(piece)
                && (m_board.get_piece_at(dst) == VOID));
        } else {
            const auto src_sq = to_square(src);
            const auto piece = m_board.get_piece_at(src_sq);
            return (
                (to_color(piece) == m_turn)
                && get_attacks_by(piece, src_sq).is_one(dst)
                && is_empty_or_opponent_piece_on_square(dst));
        }
    }

    /**
     * @brief Apply an applicable move to the state. This method does not check
     * if a move is applicable or not, check it by `is_move_applicable` method.
     *
     * @param [in] move Applicable move.
     * @param [out] moved Piece moved.
     * @param [out] captured Piece captured.
     * @return State&
     */
    State& apply(
        const Move move,
        PieceTypeEnum* const moved = nullptr,
        PieceTypeEnum* const captured = nullptr)
    {
        const auto dst = move.destination();
        const auto captured_piece = pop_piece_from_board(dst);
        add_captured_piece_to_stand(captured_piece);
        const BoardPieceTypeEnum moving_piece
            = pop_piece_from_stand_or_board(move.source());
        place_piece_on_board(dst, moving_piece);
        change_turn();
        if (moved != nullptr)
            *moved = to_piece_type(moving_piece);
        if (captured != nullptr)
            *captured = captured_piece;
        return *this;
    }

    /**
     * @brief Hash current state in 7 bytes.
     * @details
     * (MSB) <None: 1byte> <Turn + Stand: 1byte> <Board: 6byte> (LSB)
     * - First 1 byte: None
     * - Second 1 bytes: Turn + Pieces on stand of the turn player.
     *      - _____x__ : Turn
     *      - xx_xx_xx : Pieces on stand
     * - Last 6 bytes: Board (4bit x 12squares = 48bit = 6byte)
     * @return std::uint64_t
     */
    std::uint64_t hash() const
    {
        return (
            (static_cast<std::uint64_t>(m_turn) << 50)
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
        m_two_piece_stands[m_turn].add(p);
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
            const auto p = to_piece_type(src);
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
    bool is_empty_or_opponent_piece_on_square(const SquareEnum sq) const
    {
        const auto p = m_board.get_piece_at(sq);
        if (p == VOID)
            return true;
        return to_color(p) != m_turn;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
