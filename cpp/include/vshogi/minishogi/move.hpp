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
    MS_5A = SQ_5A, MS_4A = SQ_4A, MS_3A = SQ_3A, MS_2A = SQ_2A, MS_1A = SQ_1A,
    MS_5B = SQ_5B, MS_4B = SQ_4B, MS_3B = SQ_3B, MS_2B = SQ_2B, MS_1B = SQ_1B,
    MS_5C = SQ_5C, MS_4C = SQ_4C, MS_3C = SQ_3C, MS_2C = SQ_2C, MS_1C = SQ_1C,
    MS_5D = SQ_5D, MS_4D = SQ_4D, MS_3D = SQ_3D, MS_2D = SQ_2D, MS_1D = SQ_1D,
    MS_5E = SQ_5E, MS_4E = SQ_4E, MS_3E = SQ_3E, MS_2E = SQ_2E, MS_1E = SQ_1E,
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
