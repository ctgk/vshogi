#ifndef VSHOGI_MINISHOGI_STATE_HPP
#define VSHOGI_MINISHOGI_STATE_HPP

#include "vshogi/color.hpp"
#include "vshogi/minishogi/board.hpp"
#include "vshogi/minishogi/move.hpp"
#include "vshogi/minishogi/stand.hpp"
#include "vshogi/state.hpp"

namespace vshogi::minishogi
{

using State = vshogi::State<Board, BlackWhiteStands, Move>;

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_STATE_HPP
