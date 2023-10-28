#ifndef VSHOGI_MINISHOGI_STATE_HPP
#define VSHOGI_MINISHOGI_STATE_HPP

#include "vshogi/color.hpp"
#include "vshogi/minishogi/board.hpp"
#include "vshogi/minishogi/move.hpp"
#include "vshogi/minishogi/stand.hpp"
#include "vshogi/state.hpp"

namespace vshogi::minishogi
{

using State = vshogi::State<Board, BlackWhiteStands, Move, 2>;

}

namespace vshogi
{

template <>
inline std::uint64_t minishogi::State::zobrist_board
    [minishogi::Squares::num_squares]
    [num_colors * minishogi::Pieces::num_piece_types + 1]
    = {};

template <>
inline std::uint64_t
    minishogi::State::zobrist_stand[num_colors]
                                   [minishogi::Pieces::num_stand_piece_types][3]
    = {};

} // namespace vshogi

#endif // VSHOGI_MINISHOGI_STATE_HPP
