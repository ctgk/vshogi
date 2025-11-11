#ifndef VSHOGI_ENGINE_PIECE_VALUE_HPP
#define VSHOGI_ENGINE_PIECE_VALUE_HPP

#include "vshogi/common/game.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/stand.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi::engine
{

template <class Parameters>
inline float piece_value_func(const Game<Parameters>& g)
{
    using C = Configuration<Parameters>;
    using PHelper = Pieces<Parameters>;
    using PieceType = typename C::PieceType;
    using Square = typename C::Square;
    constexpr float scaler = static_cast<float>(C::sum_piece_value);

    const ColorEnum turn = g.get_turn();
    const Board<Parameters>& board = g.get_board();
    const Stand<Parameters>& ally_stand = g.get_stand(turn);
    const Stand<Parameters>& enemy_stand = g.get_stand(~turn);
    float value = 0.f;
    for (Square sq : C::square_iterator()) {
        const auto p = board[sq];
        const auto pt = PHelper::to_piece_type(p);
        const auto c = PHelper::get_color(p);
        const auto v = static_cast<float>(PHelper::get_value(pt));
        value += (c == turn) ? v : -v;
    }
    for (PieceType pt : C::stand_piece_type_iterator()) {
        const auto v = PHelper::get_value(pt);
        value += static_cast<float>(ally_stand.count(pt) * v);
        value -= static_cast<float>(enemy_stand.count(pt) * v);
    }
    return value / scaler;
}

} // namespace vshogi::engine

#endif // VSHOGI_ENGINE_PIECE_VALUE_HPP
