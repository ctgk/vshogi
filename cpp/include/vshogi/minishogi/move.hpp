#ifndef VSHOGI_MINISHOGI_MOVE_HPP
#define VSHOGI_MINISHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"

namespace vshogi::minishogi
{

enum MoveSourceEnum
{
    // clang-format off
    MS_51 = 20, MS_41 = 15, MS_31 = 10, MS_21 = 5, MS_11 = 0,
    MS_52 = 21, MS_42 = 16, MS_32 = 11, MS_22 = 6, MS_12 = 1,
    MS_53 = 22, MS_43 = 17, MS_33 = 12, MS_23 = 7, MS_13 = 2,
    MS_54 = 23, MS_44 = 18, MS_34 = 13, MS_24 = 8, MS_14 = 3,
    MS_55 = 24, MS_45 = 19, MS_35 = 14, MS_25 = 9, MS_15 = 4,
    MS_FU = 25, MS_GI = 26, MS_KA = 27, MS_HI = 28, MS_KI = 29,
    // clang-format on
};

inline PieceTypeEnum to_piece_type(const MoveSourceEnum src)
{
    return (src < MS_FU) ? NA : static_cast<PieceTypeEnum>(src - MS_FU);
}

inline SquareEnum to_square(const MoveSourceEnum src)
{
    return static_cast<SquareEnum>(src);
}

inline constexpr bool is_drop(const MoveSourceEnum src)
{
    return (src >= MS_FU);
}

/**
 * @brief 16-bit integer representing a Minishogi move.
 * @details
 *       ________ ___*****       destination square (25-possibility)
 *       ________ __*_____       Promotion flag (2-possibility)
 *       ___***** ________       source square (30-possibility = 25-square + 5-drop)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 *
 */
class Move
{
private:
    const std::uint16_t m_value;

public:
    Move(const std::uint16_t hashed_value)
        : m_value(hashed_value & 0b0001111100111111)
    {
    }
    Move(
        const SquareEnum dst,
        const MoveSourceEnum src,
        const bool promote = false)
        : m_value(static_cast<std::uint16_t>((src << 8) | (promote << 5) | dst))
    {
    }
    Move(const SquareEnum dst, const SquareEnum src, const bool promote = false)
        : Move(dst, static_cast<MoveSourceEnum>(src), promote)
    {
    }
    Move(const SquareEnum dst, const PieceTypeEnum src)
        : Move(
            dst,
            static_cast<MoveSourceEnum>(
                static_cast<int>(src) + static_cast<int>(MS_FU)))
    {
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
        return static_cast<SquareEnum>(m_value & 0b011111);
    }
    MoveSourceEnum source() const
    {
        return static_cast<MoveSourceEnum>(m_value >> 8);
    }
    bool promote() const
    {
        return static_cast<bool>(m_value & 0b00100000);
    }
    bool is_drop() const
    {
        return source() >= MS_FU;
    }
    std::uint16_t hash() const
    {
        return m_value;
    }
    Move rotate() const
    {
        const auto dst_rotated = static_cast<SquareEnum>(
            num_squares - 1 - static_cast<int>(destination()));
        const auto src_rotated
            = is_drop() ? source()
                        : static_cast<MoveSourceEnum>(
                            num_squares - 1 - static_cast<int>(source()));
        return Move(dst_rotated, src_rotated, promote());
    }
};

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_MOVE_HPP
