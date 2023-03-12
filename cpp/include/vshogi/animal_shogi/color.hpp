#ifndef VSHOGI_ANIMAL_SHOGI_COLOR_HPP
#define VSHOGI_ANIMAL_SHOGI_COLOR_HPP

#include <cstdint>
#include <stdexcept>

namespace vshogi::animal_shogi
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
constexpr int num_colors = 2;

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

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_COLOR_HPP
