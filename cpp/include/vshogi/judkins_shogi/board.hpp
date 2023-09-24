#ifndef VSHOGI_JUDKINS_SHOGI_BOARD_HPP
#define VSHOGI_JUDKINS_SHOGI_BOARD_HPP

#include <string>

#include "vshogi/board.hpp"
#include "vshogi/judkins_shogi/bitboard.hpp"
#include "vshogi/judkins_shogi/piece.hpp"
#include "vshogi/judkins_shogi/squares.hpp"

namespace vshogi::judkins_shogi
{

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

}

#endif // VSHOGI_JUDKINS_SHOGI_BOARD_HPP
