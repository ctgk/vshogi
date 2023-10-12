#ifndef VSHOGI_ANIMAL_SHOGI_BOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BOARD_HPP

#include "vshogi/animal_shogi/bitboard.hpp"
#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"
#include "vshogi/board.hpp"

namespace vshogi::animal_shogi
{

using Board = vshogi::Board<Squares, Pieces, BitBoard>;

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_BOARD_HPP
