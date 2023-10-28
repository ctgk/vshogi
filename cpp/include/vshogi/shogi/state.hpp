#ifndef VSHOGI_SHOGI_STATE_HPP
#define VSHOGI_SHOGI_STATE_HPP

#include <vector>

#include "vshogi/shogi/board.hpp"
#include "vshogi/shogi/move.hpp"
#include "vshogi/shogi/stand.hpp"
#include "vshogi/state.hpp"

namespace vshogi::shogi
{

using State = vshogi::State<Board, BlackWhiteStands, Move, 18>;

}

namespace vshogi
{

template <>
inline std::uint64_t vshogi::shogi::State::zobrist_board
    [shogi::Squares::num_squares]
    [num_colors * shogi::Pieces::num_piece_types + 1]
    = {};

template <>
inline std::uint64_t vshogi::shogi::State::zobrist_stand
    [num_colors][shogi::Pieces::num_stand_piece_types][19]
    = {};

} // namespace vshogi

#endif // VSHOGI_SHOGI_STATE_HPP
