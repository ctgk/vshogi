#ifndef VSHOGI_JUDKINS_SHOGI_MOVE_HPP
#define VSHOGI_JUDKINS_SHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/direction.hpp"
#include "vshogi/judkins_shogi/piece.hpp"
#include "vshogi/judkins_shogi/squares.hpp"
#include "vshogi/move.hpp"

namespace vshogi::judkins_shogi
{

/**
 * @brief 16-bit integer representing a move in Judkins-Shogi game.
 * @details
 *       ________ __******       Destination square (36 possibilities)
 *       ________ _*______       Promotion flag (2 possibilities)
 *       __****** ________       Source square or piece (42 possibilities = 36 + 6)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 */
using Move = vshogi::Move<Squares, Pieces, std::uint16_t>;

} // namespace vshogi::judkins_shogi

namespace vshogi
{

template <>
constexpr int judkins_shogi::Move::source_shift()
{
    return 8;
}
template <>
constexpr int judkins_shogi::Move::promote_shift()
{
    return 6;
}
template <>
constexpr std::uint16_t judkins_shogi::Move::destination_mask()
{
    return 0x003f;
}
template <>
constexpr std::uint16_t judkins_shogi::Move::promote_mask()
{
    return 0x0040;
}
template <>
constexpr std::uint16_t judkins_shogi::Move::source_mask()
{
    return 0x3f00;
}

} // namespace vshogi

#endif // VSHOGI_JUDKINS_SHOGI_MOVE_HPP
