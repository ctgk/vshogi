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

template <>
inline int minishogi::Move::to_dlshogi_source_index() const
{
    if (is_drop())
        return minishogi::Squares::num_dlshogi_directions * 2
               + static_cast<int>(source_piece());
    const auto dst = destination();
    const auto src = source_square();
    const auto promo_offset
        = promote() ? minishogi::Squares::num_dlshogi_directions : 0;
    for (auto d : minishogi::Squares::dlshogi_direction_array) {
        SquareEnum sq = dst;
        for (int i = 4; i--;) {
            sq = minishogi::Squares::shift(sq, d);
            if (sq == src)
                return static_cast<int>(d) + promo_offset;
        }
    }
    return 0;
}

} // namespace vshogi

#endif // VSHOGI_MINISHOGI_MOVE_HPP
