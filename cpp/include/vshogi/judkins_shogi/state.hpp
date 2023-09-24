#ifndef VSHOGI_JUDKINS_SHOGI_STATE_HPP
#define VSHOGI_JUDKINS_SHOGI_STATE_HPP

#include <string>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/judkins_shogi/board.hpp"
#include "vshogi/judkins_shogi/move.hpp"
#include "vshogi/judkins_shogi/stand.hpp"
#include "vshogi/state.hpp"

namespace vshogi::judkins_shogi
{

using State = vshogi::State<Board, BlackWhiteStands, Move>;

} // namespace vshogi::judkins_shogi

#endif // VSHOGI_JUDKINS_SHOGI_STATE_HPP
