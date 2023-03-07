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

    BoardPieceTypeEnum pop_piece_from_stand_or_board(const MoveSourceEnum src)
    {
        if (is_drop(src)) {
            const auto p = to_captured(src);
            m_two_piece_stands[m_turn].subtract(p);
            return to_board_piece(m_turn, p);
        }
        return m_board.get_piece_at(static_cast<SquareEnum>(src));
    }
    void place_piece_on_board(const SquareEnum sq, const BoardPieceTypeEnum p)
    {
        m_board.place_piece_at(sq, p);
    }
    void change_turn()
    {
        m_turn = opposite(m_turn);
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
