#ifndef VSHOGI_SHOGI_STATE_HPP
#define VSHOGI_SHOGI_STATE_HPP

#include <vector>

#include "vshogi/shogi/board.hpp"
#include "vshogi/shogi/move.hpp"
#include "vshogi/shogi/stand.hpp"
#include "vshogi/state.hpp"

namespace vshogi::shogi
{

using State = vshogi::State<Board, BlackWhiteStands, Move>;

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_STATE_HPP
