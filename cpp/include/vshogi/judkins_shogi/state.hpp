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

using State = vshogi::State<Board, BlackWhiteStands, Move, 2>;

}

namespace vshogi
{

template <>
inline std::uint64_t judkins_shogi::State::zobrist_board
    [judkins_shogi::Squares::num_squares]
    [num_colors * judkins_shogi::Pieces::num_piece_types + 1]
    = {};

template <>
inline std::uint64_t judkins_shogi::State::zobrist_stand
    [num_colors][judkins_shogi::Pieces::num_stand_piece_types][3]
    = {};

} // namespace vshogi

#endif // VSHOGI_JUDKINS_SHOGI_STATE_HPP
