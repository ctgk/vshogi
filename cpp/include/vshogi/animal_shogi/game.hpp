#ifndef VSHOGI_ANIMAL_SHOGI_GAME_HPP
#define VSHOGI_ANIMAL_SHOGI_GAME_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"
#include "vshogi/animal_shogi/state.hpp"
#include "vshogi/game.hpp"
#include "vshogi/result.hpp"

namespace vshogi::animal_shogi
{

using Game = vshogi::Game<State>;

namespace internal
{

inline ResultEnum move_result(
    const Move move,
    const Pieces::BoardPieceTypeEnum moving,
    const Pieces::BoardPieceTypeEnum captured)
{
    if (captured == Pieces::B_LI)
        return WHITE_WIN;
    if (captured == Pieces::W_LI)
        return BLACK_WIN;
    const auto r_dst = Squares::to_rank(move.destination());
    if ((moving == Pieces::B_LI) && (r_dst == Squares::RANK1))
        return BLACK_WIN;
    if ((moving == Pieces::W_LI) && (r_dst == Squares::RANK4))
        return WHITE_WIN;
    return ONGOING;
}

} // namespace internal

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
inline animal_shogi::Game&
animal_shogi::Game::apply(const animal_shogi::Move move)
{
    m_record.emplace_back(std::make_pair(m_current_state.to_sfen(), move));
    if (!move.is_drop()) {
        const auto moving = get_board()[move.source_square()];
        const auto captured = get_board()[move.destination()];
        m_result = animal_shogi::internal::move_result(move, moving, captured);
    }
    m_current_state.apply(move);
    update_result();
    return *this;
}

template <>
inline void animal_shogi::Game::to_feature_map(float* const data) const
{
    constexpr int num_squares = ranks() * files();
    constexpr int sp_types = stand_piece_types();
    constexpr int bp_types = board_piece_types();
    constexpr int unpromoted_piece_types = sp_types + 1; // + OU
    constexpr int ch_half = sp_types + bp_types;
    constexpr int ch = feature_channels();

    const auto turn = get_turn();
    const auto stand_curr = get_stand(turn);
    const auto stand_next = get_stand(~turn);
    const auto board = get_board();

    for (int k = sp_types; k--;) {
        const auto p = animal_shogi::Pieces::stand_piece_array[k];
        const auto num_curr = static_cast<float>(stand_curr.count(p));
        const auto num_next = static_cast<float>(stand_next.count(p));
        for (int i = num_squares; i--;) {
            data[i * ch + k] = num_curr;
            data[i * ch + k + ch_half] = num_next;
        }
    }
    for (int i = num_squares; i--;) {
        const auto sq = static_cast<typename animal_shogi::Squares::SquareEnum>(
            (turn == BLACK) ? i : (num_squares - 1 - i));
        const auto board_piece = board[sq];
        for (int k = bp_types; k--;) {
            data[i * ch + k + sp_types] = 0.f;
            data[i * ch + k + sp_types + ch_half] = 0.f;
        }
        if (board.is_empty(sq))
            continue;
        const auto color = animal_shogi::Pieces::get_color(board_piece);
        const auto piece_type
            = animal_shogi::Pieces::to_piece_type(board_piece);
        auto k = static_cast<int>(piece_type);
        k += (turn == color) ? 0 : ch_half;
        data[i * ch + k + sp_types] = 1.f;
    }
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_GAME_HPP
