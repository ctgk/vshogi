#ifndef VSHOGI_COLOR_HPP
#define VSHOGI_COLOR_HPP

#include <cstdint>

#include "vshogi/common/utils.hpp"

namespace vshogi
{

/**
 * @brief Enumeration of colors in Animal Shogi.
 *
 */
enum ColorEnum : std::uint8_t
{
    BLACK = 0, //!< First player's color.
    WHITE = 1, //!< Second player's color.
};
constexpr uint num_colors = 2;
constexpr ColorEnum color_array[] = {BLACK, WHITE};

/**
 * @brief Return opposite color of the input color.
 *
 * @param c Input color.
 * @return ColorEnum Opposite of the input color.
 */
inline constexpr ColorEnum opposite(const ColorEnum c)
{
    return static_cast<ColorEnum>(static_cast<std::uint8_t>(c) ^ 1);
}

/**
 * @brief Return opposite color of the input color.
 *
 * @param c Input color
 * @return ColorEnum Opposite of the input color.
 */
inline constexpr ColorEnum operator~(const ColorEnum c)
{
    return opposite(c);
}

} // namespace vshogi

#endif // VSHOGI_COLOR_HPP
