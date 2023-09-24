#ifndef VSHOGI_SHOGI_GAME_HPP
#define VSHOGI_SHOGI_GAME_HPP

#include <string>
#include <utility>
#include <vector>

#include "vshogi/game.hpp"
#include "vshogi/result.hpp"
#include "vshogi/shogi/move.hpp"
#include "vshogi/shogi/state.hpp"

namespace vshogi::shogi
{

using Game = vshogi::Game<State, Board, Stand, Move, Squares, Pieces>;

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_GAME_HPP
