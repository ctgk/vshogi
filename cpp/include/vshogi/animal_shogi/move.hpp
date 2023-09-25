#ifndef VSHOGI_ANIMAL_SHOGI_MOVE_HPP
#define VSHOGI_ANIMAL_SHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"
#include "vshogi/move.hpp"

namespace vshogi::animal_shogi
{

/**
 * @brief 8bit integer representing an Animal Shogi move.
 * @details
 *       ____ ****       destination square (12 squares)
 *       **** ____       source square (15 = 12 squares + 3 pieces to drop)
 * (MSB) xxxx xxxx (LSB)
 */
using Move = vshogi::Move<Squares, Pieces, std::uint8_t>;

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
constexpr int animal_shogi::Move::source_shift()
{
    return 4;
}
template <>
constexpr int animal_shogi::Move::promote_shift()
{
    return 8;
}
template <>
constexpr std::uint8_t animal_shogi::Move::destination_mask()
{
    return 0x0f;
}
template <>
constexpr std::uint8_t animal_shogi::Move::promote_mask()
{
    return 0x00;
}
template <>
constexpr std::uint8_t animal_shogi::Move::source_mask()
{
    return 0xf0;
}

template <>
inline int animal_shogi::Move::to_dlshogi_source_index() const
{
    if (is_drop())
        return 8 + static_cast<int>(source_piece());
    constexpr int diff_plus_4_to_dir_index[] = {0, 1, 2, 3, -1, 4, 5, 6, 7};
    return diff_plus_4_to_dir_index
        [static_cast<int>(source_square()) - static_cast<int>(destination())
         + 4];
}

template <>
constexpr int animal_shogi::Move::num_policy_per_square()
{
    return 8 + num_stand_piece_types;
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_MOVE_HPP
