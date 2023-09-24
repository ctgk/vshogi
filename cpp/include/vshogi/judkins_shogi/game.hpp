#ifndef VSHOGI_JUDKINS_SHOGI_GAME_HPP
#define VSHOGI_JUDKINS_SHOGI_GAME_HPP

#include "vshogi/game.hpp"
#include "vshogi/judkins_shogi/board.hpp"
#include "vshogi/judkins_shogi/move.hpp"
#include "vshogi/judkins_shogi/stand.hpp"
#include "vshogi/judkins_shogi/state.hpp"

namespace vshogi::judkins_shogi
{

using Game = vshogi::Game<State, Board, Stand, Move, Squares, Pieces>;

} // namespace vshogi::judkins_shogi

#endif // VSHOGI_JUDKINS_SHOGI_GAME_HPP
