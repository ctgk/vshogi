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

    PieceTypeEnum pop_piece_from_board(const SquareEnum sq)
    {
        return m_board.pop_piece_at(sq);
    }
    void add_captured_piece_to_stand(const PieceTypeEnum)
    {
    }

    BoardPieceTypeEnum pop_piece_from_stand_or_board(const MoveSourceEnum)
    {
        return VOID;
    }
    void place_piece_on_board(const BoardPieceTypeEnum)
    {
    }

public:
    State();
    State(const std::string& sfen);

    ColorEnum get_turn() const
    {
        return m_turn;
    }
    void apply_move(const Move move);
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
