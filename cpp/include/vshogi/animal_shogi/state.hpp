#ifndef VSHOGI_ANIMAL_SHOGI_STATE_HPP
#define VSHOGI_ANIMAL_SHOGI_STATE_HPP

#include <string>

#include "vshogi/animal_shogi/color.hpp"
#include "vshogi/animal_shogi/piece_stand.hpp"

namespace vshogi::animal_shogi
{

class Board
{
};

class State
{
private:
    Board m_board;
    PieceStand m_piece_stand[2];
    ColorEnum m_turn; //!< Player to move next.

public:
    State();
    State(const std::string& sfen);
    State(const std::string& sfen, const ColorEnum turn);

    ColorEnum get_turn() const
    {
        return m_turn;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
