#include "vshogi/animal_shogi/game.hpp"

namespace vshogi::animal_shogi
{

Game::Game(const std::string& sfen)
    : m_history(), m_current_state(sfen), m_result(UNKNOWN)
{
    m_history.reserve(128);
}

} // namespace vshogi::animal_shogi
