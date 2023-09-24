#ifndef VSHOGI_MINISHOGI_BOARD_HPP
#define VSHOGI_MINISHOGI_BOARD_HPP

#include "vshogi/board.hpp"
#include "vshogi/minishogi/bitboard.hpp"
#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"

namespace vshogi::minishogi
{

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_BOARD_HPP
