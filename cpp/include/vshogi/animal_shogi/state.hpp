#ifndef VSHOGI_ANIMAL_SHOGI_STATE_HPP
#define VSHOGI_ANIMAL_SHOGI_STATE_HPP

#include "vshogi/animal_shogi/board.hpp"
#include "vshogi/animal_shogi/move.hpp"
#include "vshogi/animal_shogi/stand.hpp"
#include "vshogi/color.hpp"
#include "vshogi/state.hpp"

namespace vshogi::animal_shogi
{

using State = vshogi::State<Board, BlackWhiteStands, Move, 2>;

namespace internal
{

inline Pieces::BoardPieceTypeEnum
promote_if_possible(const Pieces::BoardPieceTypeEnum p, const Move move)
{
    if (move.is_drop())
        return p;
    if (Pieces::to_piece_type(p) != Pieces::CH)
        return p;
    const auto r_src = Squares::to_rank(move.source_square());
    const auto r_dst = Squares::to_rank(move.destination());
    const auto c = Pieces::get_color(p);
    if (c == BLACK) {
        if ((r_src == Squares::RANK2) && (r_dst == Squares::RANK1))
            return Pieces::to_board_piece(c, Pieces::HE);
        else
            return p;
    } else {
        if ((r_src == Squares::RANK3) && (r_dst == Squares::RANK4))
            return Pieces::to_board_piece(c, Pieces::HE);
        else
            return p;
    }
}

} // namespace internal

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
inline std::uint64_t animal_shogi::State::zobrist_board
    [animal_shogi::Squares::num_squares]
    [num_colors * animal_shogi::Pieces::num_piece_types + 1]
    = {};

template <>
inline std::uint64_t animal_shogi::State::zobrist_stand
    [num_colors][animal_shogi::Pieces::num_stand_piece_types][3]
    = {};

template <>
inline animal_shogi::State& animal_shogi::State::apply(
    const animal_shogi::Move& move, std::uint64_t* const hash)
{
    const auto dst = move.destination();
    auto moving = pop_piece_from_stand_or_board(move, hash);
    const auto captured = Pieces::to_piece_type(m_board[dst]);
    if ((captured != Pieces::NA) && (captured != Pieces::LI))
        add_captured_to_stand(Pieces::demote(captured), hash);
    moving = animal_shogi::internal::promote_if_possible(moving, move);
    place_piece_at(dst, moving, hash);
    m_turn = ~m_turn;
    return *this;
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
