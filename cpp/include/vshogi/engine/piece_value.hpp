#ifndef VSHOGI_ENGINE_PIECE_VALUE_HPP
#define VSHOGI_ENGINE_PIECE_VALUE_HPP

#include "vshogi/common/game.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi::engine
{

template <class Config>
inline float piece_value_func(const Game<Config>& g)
{
    using PHelper = Pieces<Config>;
    using PieceType = typename Config::PieceType;
    using Square = typename Config::Square;
    constexpr uint num_squares = Config::num_squares;
    constexpr uint num_stand_piece_types = Config::num_stand_piece_types;
    constexpr float scaler = static_cast<float>(Config::sum_piece_value);

    const ColorEnum turn = g.get_turn();
    const Board<Config>& board = g.get_board();
    const Stand<Config>& ally_stand = g.get_stand(turn);
    const Stand<Config>& enemy_stand = g.get_stand(~turn);
    float value = 0.f;
    for (Square sq : EnumIterator<Square, num_squares>()) {
        const auto p = board[sq];
        const auto pt = PHelper::to_piece_type(p);
        const auto c = PHelper::get_color(p);
        const auto v = static_cast<float>(PHelper::get_value(pt));
        value += (c == turn) ? v : -v;
    }
    for (PieceType pt : EnumIterator<PieceType, num_stand_piece_types>()) {
        const auto v = PHelper::get_value(pt);
        value += static_cast<float>(ally_stand.count(pt) * v);
        value -= static_cast<float>(enemy_stand.count(pt) * v);
    }
    return value / scaler;
}

} // namespace vshogi::engine

#endif // VSHOGI_ENGINE_PIECE_VALUE_HPP
