#include "vshogi/animal_shogi/state.hpp"

namespace vshogi::animal_shogi
{

State::State() : m_board(), m_piece_stand(), m_turn(BLACK)
{
}

State::State(const std::string&) : m_board(), m_piece_stand(), m_turn(BLACK)
{
}

State::State(const std::string&, const ColorEnum turn)
    : m_board(), m_piece_stand(), m_turn(turn)
{
}

} // namespace vshogi::animal_shogi
