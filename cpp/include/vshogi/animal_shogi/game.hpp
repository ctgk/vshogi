#ifndef VSHOGI_ANIMAL_SHOGI_GAME_HPP
#define VSHOGI_ANIMAL_SHOGI_GAME_HPP

#include <deque>
#include <string>

#include "vshogi/animal_shogi/state.hpp"

namespace vshogi::animal_shogi
{

struct Action
{
};

/**
 * @brief Pair of a state and action. The action turns the state to the next
 * one.
 *
 */
struct StateActionPair
{
    State state;
    Action action;
};

/**
 * @brief Animal Shogi game.
 *
 */
class Game
{
private:
    std::deque<StateActionPair> m_history;
    State m_current_state;

public:
    Game();
    Game(const std::string& sfen);
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_GAME_HPP
