#ifndef VSHOGI_MINISHOGI_MOVE_HPP
#define VSHOGI_MINISHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/direction.hpp"
#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"
#include "vshogi/move.hpp"

namespace vshogi::minishogi
{

/**
 * @brief 16-bit integer representing a Minishogi move.
 * @details
 *       ________ ___*****       destination square (25-possibility)
 *       ________ __*_____       Promotion flag (2-possibility)
 *       ___***** ________       source square (30-possibility = 25-square + 5-drop)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 *
 */
using Move = vshogi::Move<Squares, Pieces, std::uint16_t>;

} // namespace vshogi::minishogi

namespace vshogi
{

template <>
constexpr int minishogi::Move::source_shift()
{
    return 8;
}
template <>
constexpr int minishogi::Move::promote_shift()
{
    return 5;
}
template <>
constexpr std::uint16_t minishogi::Move::destination_mask()
{
    return 0x001f;
}
template <>
constexpr std::uint16_t minishogi::Move::promote_mask()
{
    return 0x0020;
}
template <>
constexpr std::uint16_t minishogi::Move::source_mask()
{
    return 0x1f00;
}

} // namespace vshogi

#endif // VSHOGI_MINISHOGI_MOVE_HPP
