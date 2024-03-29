#ifndef VSHOGI_MINISHOGI_GAME_HPP
#define VSHOGI_MINISHOGI_GAME_HPP

#include <string>
#include <utility>
#include <vector>

#include "vshogi/game.hpp"
#include "vshogi/minishogi/move.hpp"
#include "vshogi/minishogi/state.hpp"
#include "vshogi/result.hpp"

namespace vshogi::minishogi
{

using Game = vshogi::Game<State, Board, Stand, Move, Squares, Pieces>;

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_GAME_HPP
