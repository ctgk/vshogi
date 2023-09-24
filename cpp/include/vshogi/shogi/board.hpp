#ifndef VSHOGI_SHOGI_BOARD_HPP
#define VSHOGI_SHOGI_BOARD_HPP

#include "vshogi/board.hpp"
#include "vshogi/shogi/bitboard.hpp"
#include "vshogi/shogi/piece.hpp"
#include "vshogi/shogi/squares.hpp"

namespace vshogi::shogi
{

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_BOARD_HPP
