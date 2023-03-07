#ifndef VSHOGI_ANIMAL_SHOGI_MOVE_HPP
#define VSHOGI_ANIMAL_SHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/animal_shogi/squares.hpp"

namespace vshogi::animal_shogi
{

enum MoveSourceEnum
{
    // clang-format off
    MS_A1, MS_B1, MS_C1,
    MS_A2, MS_B2, MS_C2,
    MS_A3, MS_B3, MS_C3,
    MS_A4, MS_B4, MS_C4,
    MS_CH, MS_EL, MS_GI, // dropping
    // clang-format on
};

/**
 * @brief 8bit integer representing an Animal Shogi move.
 * @details
 *       ____ ****       destination square (12 squares)
 *       **** ____       source square (15 = 12 squares + 3 pieces to drop)
 * (MSB) xxxx xxxx (LSB)
 */
class Move
{
private:
    std::uint8_t m_value;

public:
    Move(const SquareEnum destination, const MoveSourceEnum source)
        : m_value(static_cast<std::uint8_t>((source << 4) | destination))
    {
    }
    SquareEnum destination() const
    {
        return static_cast<SquareEnum>(m_value & 0xf);
    }
    MoveSourceEnum source() const
    {
        return static_cast<MoveSourceEnum>(m_value >> 4);
    }
    bool is_drop() const
    {
        return source() >= MS_CH;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_MOVE_HPP
