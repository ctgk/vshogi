#ifndef VSHOGI_ANIMAL_SHOGI_STATE_HPP
#define VSHOGI_ANIMAL_SHOGI_STATE_HPP

#include <string>

#include "vshogi/animal_shogi/board.hpp"
#include "vshogi/animal_shogi/color.hpp"
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

    ColorEnum get_turn() const
    {
        return m_turn;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
