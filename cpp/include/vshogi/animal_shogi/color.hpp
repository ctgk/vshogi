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

/**
 * @brief Get the current turn color from SFEN string.
 *
 * @param sfen String representing current game state.
 * e.g. "gle/1c1/1C1/ELG b - 1"
 * @return ColorEnum Color of current turn.
 */
inline ColorEnum get_current_turn(const std::string& sfen)
{
    const auto turn_location = sfen.find(' ', 0) + 1;
    const auto turn_char = sfen[turn_location];
    switch (turn_char) {
    case 'b':
        return BLACK;
    case 'w':
        return WHITE;
    default:
        break;
    }
    throw std::invalid_argument(
        "Failed to get current turn from SFEN: " + sfen);
}

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_COLOR_HPP
