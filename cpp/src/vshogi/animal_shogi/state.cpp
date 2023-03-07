#include "vshogi/animal_shogi/state.hpp"

namespace vshogi::animal_shogi
{

State::State() : m_board(), m_two_piece_stands(), m_turn(BLACK)
{
}

State::State(const std::string& sfen)
    : m_board(), m_two_piece_stands(), m_turn(get_current_turn(sfen))
{
}

} // namespace vshogi::animal_shogi
