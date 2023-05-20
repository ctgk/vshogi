#ifndef VSHOGI_ANIMAL_SHOGI_STATE_HPP
#define VSHOGI_ANIMAL_SHOGI_STATE_HPP

#include <string>
#include <vector>

#include "vshogi/animal_shogi/board.hpp"
#include "vshogi/animal_shogi/move.hpp"
#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/stand.hpp"
#include "vshogi/color.hpp"

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
    BlackWhiteStands m_black_white_stands;
    ColorEnum m_turn; //!< Player to move next.

public:
    State() : m_board(), m_black_white_stands(), m_turn(BLACK)
    {
    }
    State(const std::string& sfen) : m_board(), m_black_white_stands(), m_turn()
    {
        auto s = sfen.c_str();
        s = m_board.set_sfen(s);
        {
            (*s == 'b') ? m_turn = BLACK : m_turn = WHITE;
            ++s;
            ++s;
        }
        m_black_white_stands.set_sfen_holdings(s);
    }
    void set_hash(std::uint64_t value)
    {
        m_board.set_hash(value);
        m_turn = static_cast<ColorEnum>(static_cast<bool>(value & (1UL << 50)));

        auto& turn_stand = m_black_white_stands[m_turn];
        auto& oppo_stand = m_black_white_stands[~m_turn];
        turn_stand.set_value(static_cast<std::uint8_t>(value >> 48));
        oppo_stand.add(
            CH,
            static_cast<std::uint8_t>(
                2 - m_board.count(CH) - m_board.count(HE)
                - turn_stand.count(CH)));
        oppo_stand.add(
            EL,
            static_cast<std::uint8_t>(
                2 - m_board.count(EL) - turn_stand.count(EL)));
        oppo_stand.add(
            GI,
            static_cast<std::uint8_t>(
                2 - m_board.count(GI) - turn_stand.count(GI)));
    }
    std::string to_sfen() const
    {
        auto out = m_board.to_sfen();
        out += ' ';
        out += m_turn == BLACK ? 'b' : 'w';
        out += ' ';
        m_black_white_stands.append_to_sfen(out);
        return out;
    }

    const Board& get_board() const
    {
        return m_board;
    }
    const Stand& get_stand(const ColorEnum c) const
    {
        return m_black_white_stands[c];
    }
    ColorEnum get_turn() const
    {
        return m_turn;
    }

    /**
     * @brief Check if a move is legal or not.
     * @details Unlike ordinary Shogi, the followings are legal:
     * - Two Chicks in one file.
     * - Checkmate by dropping a Chick.
     * - Dropping a Chick on the final rank (, which will never promote to Hen)
     * - Perpetual check.
     *
     * cf. https://en.wikipedia.org/wiki/D%C5%8Dbutsu_sh%C5%8Dgi#Play
     *
     * @param move
     * @return true The move is legal.
     * @return false The move is not legal
     */
    bool is_legal(const Move move) const
    {
        const auto src = move.source();
        const auto dst = move.destination();
        if (move.is_drop()) {
            const auto piece = to_piece_type(src);
            return (
                m_black_white_stands[m_turn].exist(piece)
                && (m_board[dst] == VOID));
        } else {
            const auto src_sq = to_square(src);
            const auto piece = m_board[src_sq];
            return (
                (to_color(piece) == m_turn)
                && get_attacks_by(piece, src_sq).is_one(dst)
                && is_empty_or_opponent_piece_on_square(dst));
        }
    }

    std::vector<Move> get_legal_moves() const
    {
        auto out = std::vector<Move>();
        append_legal_moves_by_board_pieces(out);
        append_legal_moves_by_stand_pieces(out);
        return out;
    }

    /**
     * @brief Apply an legal move to the state. This method does not check
     * if a move is legal or not, check it by `is_move_legal` method.
     *
     * @param [in] move legal move.
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
        m_board[dst] = moving_piece;
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
                   m_black_white_stands[m_turn].get_value())
               << 48)
            + m_board.hash());
    }

private:
    PieceTypeEnum pop_piece_from_board(const SquareEnum sq)
    {
        const auto p = m_board[sq];
        m_board[sq] = VOID;
        return to_piece_type(p);
    }
    void add_captured_piece_to_stand(const PieceTypeEnum p)
    {
        m_black_white_stands[m_turn].add(p);
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
            m_black_white_stands[m_turn].subtract(p);
            return to_board_piece(m_turn, p);
        } else {
            const auto sq = static_cast<SquareEnum>(src);
            const auto p = m_board[sq];
            m_board[sq] = VOID;
            return promote(p, sq);
        }
    }
    void change_turn()
    {
        m_turn = opposite(m_turn);
    }
    bool is_empty_or_opponent_piece_on_square(const SquareEnum sq) const
    {
        const auto p = m_board[sq];
        if (p == VOID)
            return true;
        return to_color(p) != m_turn;
    }
    void append_legal_moves_by_board_pieces(std::vector<Move>& move_list) const
    {
        const auto empty_or_opponent_piece = ~m_board.to_piece_mask(m_turn);
        for (auto src : square_array) {
            const auto piece = m_board[src];
            if ((piece == VOID) || (to_color(piece) != m_turn))
                continue;
            const auto attacking = get_attacks_by(piece, src);
            const auto movable = attacking & empty_or_opponent_piece;
            for (auto dst : square_array) {
                if (movable.is_one(dst)) {
                    move_list.emplace_back(Move(dst, src));
                }
            }
        }
    }
    void append_legal_moves_by_stand_pieces(std::vector<Move>& move_list) const
    {
        const auto empty = ~m_board.to_piece_mask();
        const auto stand = m_black_white_stands[m_turn];
        for (auto piece : stand_piece_array) {
            if (!stand.exist(piece))
                continue;
            for (auto dst : square_array) {
                if (empty.is_one(dst)) {
                    move_list.emplace_back(Move(dst, piece));
                }
            }
        }
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
