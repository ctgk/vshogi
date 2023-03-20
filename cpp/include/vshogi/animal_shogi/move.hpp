#ifndef VSHOGI_ANIMAL_SHOGI_MOVE_HPP
#define VSHOGI_ANIMAL_SHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"

namespace vshogi::animal_shogi
{

enum MoveSourceEnum : std::uint8_t
{
    // clang-format off
    MS_A1, MS_B1, MS_C1,
    MS_A2, MS_B2, MS_C2,
    MS_A3, MS_B3, MS_C3,
    MS_A4, MS_B4, MS_C4,
    MS_CH, MS_EL, MS_GI, // dropping
    // clang-format on
};

inline PieceTypeEnum to_piece_type(const MoveSourceEnum source)
{
    constexpr PieceTypeEnum table[] = {
        // clang-format off
        NA, NA, NA,
        NA, NA, NA,
        NA, NA, NA,
        NA, NA, NA,
        CH, EL, GI,
    };
    // clang-format on
    return table[source];
}

inline SquareEnum to_square(const MoveSourceEnum source)
{
    return static_cast<SquareEnum>(source);
}

inline bool is_drop(const MoveSourceEnum source)
{
    constexpr bool table[] = {
        // clang-format off
        false, false, false,
        false, false, false,
        false, false, false,
        false, false, false,
        true, true, true,
    };
    // clang-format on
    return table[source];
}

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
    Move(const std::uint8_t hashed_value) : m_value(hashed_value)
    {
    }
    Move(const SquareEnum destination, const MoveSourceEnum source)
        : m_value(static_cast<std::uint8_t>((source << 4) | destination))
    {
    }
    Move(const SquareEnum destination, const SquareEnum source)
        : Move(destination, static_cast<MoveSourceEnum>(source))
    {
    }
    Move(const SquareEnum destination, const PieceTypeEnum source)
        : Move(
            destination,
            static_cast<MoveSourceEnum>(
                (source < LI) ? source + 12 : destination))
    {
        static_assert(static_cast<int>(MS_CH) - static_cast<int>(CH) == 12);
    }
    bool operator==(const Move& other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const Move& other) const
    {
        return m_value != other.m_value;
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
    std::uint8_t hash() const
    {
        return m_value;
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_MOVE_HPP
