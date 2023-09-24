#ifndef VSHOGI_ANIMAL_SHOGI_STATE_HPP
#define VSHOGI_ANIMAL_SHOGI_STATE_HPP

#include "vshogi/animal_shogi/board.hpp"
#include "vshogi/animal_shogi/move.hpp"
#include "vshogi/animal_shogi/stand.hpp"
#include "vshogi/color.hpp"
#include "vshogi/state.hpp"

namespace vshogi::animal_shogi
{

using State = vshogi::State<Board, BlackWhiteStands, Move>;

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
inline animal_shogi::State&
animal_shogi::State::apply(const animal_shogi::Move move)
{
    const auto dst = move.destination();
    auto moving = pop_piece_from_stand_or_board(move);
    const auto captured = Pieces::to_piece_type(m_board[dst]);
    if (captured != Pieces::NA)
        m_stands[m_turn].add(captured);
    moving = animal_shogi::internal::promote_if_possible(moving, move);
    m_board[dst] = moving;
    m_turn = ~m_turn;
    update_masks();
    return *this;
}

template <>
inline bool
animal_shogi::State::is_legal_drop(const animal_shogi::Move move) const
{
    const auto p = move.source_piece();
    const auto dst = move.destination();
    return (has_piece_on_turn_player_stand(p) && m_board.is_empty(dst));
}

template <>
inline bool
animal_shogi::State::is_legal_board(const animal_shogi::Move move) const
{
    const auto src = move.source_square();
    const auto moving = m_board[src];
    const auto dst = move.destination();
    return (
        (Pieces::get_color(moving) == m_turn) // No VOID check on purpose
        && is_empty_or_opponent_square(dst)
        && BitBoard::get_attacks_by(moving, src)
               .is_one(dst) // VOID should return false here.
    );
}

template <>
inline void animal_shogi::State::append_legal_moves_by_board_pieces(
    std::vector<animal_shogi::Move>& out) const
{
    constexpr DirectionEnum direction_array[]
        = {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE};
    for (auto src : Squares::square_array) {
        const auto piece = m_board[src];
        if (m_board.is_empty(src) || (Pieces::get_color(piece) != m_turn))
            continue;
        for (auto dir : direction_array) {
            const auto dst = static_cast<Squares::SquareEnum>(
                src + Squares::direction_to_delta(dir));
            const auto m = animal_shogi::Move(dst, src);
            if (is_legal_board(m))
                out.emplace_back(m);
        }
    }
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_STATE_HPP
