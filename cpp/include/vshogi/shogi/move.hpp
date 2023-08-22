#ifndef VSHOGI_SHOGI_MOVE_HPP
#define VSHOGI_SHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/shogi/piece.hpp"
#include "vshogi/shogi/squares.hpp"

namespace vshogi::shogi
{

enum MoveSourceEnum
{
    // clang-format off
    MS_9A, MS_8A, MS_7A, MS_6A, MS_5A, MS_4A, MS_3A, MS_2A, MS_1A,
    MS_9B, MS_8B, MS_7B, MS_6B, MS_5B, MS_4B, MS_3B, MS_2B, MS_1B,
    MS_9C, MS_8C, MS_7C, MS_6C, MS_5C, MS_4C, MS_3C, MS_2C, MS_1C,
    MS_9D, MS_8D, MS_7D, MS_6D, MS_5D, MS_4D, MS_3D, MS_2D, MS_1D,
    MS_9E, MS_8E, MS_7E, MS_6E, MS_5E, MS_4E, MS_3E, MS_2E, MS_1E,
    MS_9F, MS_8F, MS_7F, MS_6F, MS_5F, MS_4F, MS_3F, MS_2F, MS_1F,
    MS_9G, MS_8G, MS_7G, MS_6G, MS_5G, MS_4G, MS_3G, MS_2G, MS_1G,
    MS_9H, MS_8H, MS_7H, MS_6H, MS_5H, MS_4H, MS_3H, MS_2H, MS_1H,
    MS_9I, MS_8I, MS_7I, MS_6I, MS_5I, MS_4I, MS_3I, MS_2I, MS_1I,
    MS_FU, MS_KY, MS_KE, MS_GI, MS_KA, MS_HI, MS_KI,
    // clang-format on
};

inline constexpr bool is_piece(const MoveSourceEnum src)
{
    return (src >= MS_FU);
}
inline constexpr bool is_square(const MoveSourceEnum src)
{
    return (src < MS_FU);
}
inline PieceTypeEnum to_piece_type(const MoveSourceEnum src)
{
    return static_cast<PieceTypeEnum>(src - MS_FU);
}
inline SquareEnum to_square(const MoveSourceEnum src)
{
    return static_cast<SquareEnum>(src);
}

class Move
{
private:
    /**
     * @brief 16-bit integer representing a move in shogi game.
     * @details
     *       ________ _*******       Destination square (81 possibilities)
     *       ________ *_______       Promotion flag (2 possibilities)
     *       _******* ________       Source square or piece (88 possibilities = 81 + 7)
     * (MSB) xxxxxxxx xxxxxxxx (LSB)
     */
    std::uint16_t m_value;

public:
    Move(const std::uint16_t hashed_value) : m_value(hashed_value & 0x7fff)
    {
    }
    Move(
        const SquareEnum dst,
        const MoveSourceEnum src,
        const bool promote = false)
        : m_value(static_cast<std::uint16_t>((src << 8) | (promote << 7) | dst))
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
        return static_cast<SquareEnum>(m_value & 0x007f);
    }
    MoveSourceEnum source() const
    {
        return static_cast<MoveSourceEnum>(m_value >> 8);
    }
    bool promote() const
    {
        return static_cast<bool>(m_value & 0x0080);
    }
    bool is_drop() const
    {
        return is_piece(source());
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
    int to_dlshogi_policy_index() const
    {
        const auto dst_index = static_cast<int>(destination());
        const auto src_index = to_dlshogi_source_index();
        return dst_index * num_policy_per_square() + src_index;
    }
    static constexpr int num_policy_per_square()
    {
        return 10 * 2 + 7; // 10-direction * 2-promotion + 7-drop-piece
    }

private:
    int to_dlshogi_source_index() const
    {
        if (is_drop())
            return 10 * 2 + static_cast<int>(to_piece_type(source()));

        const auto dst = destination();
        const auto src = to_square(source());
        const auto promo_offset = promote() ? 10 : 0;
        constexpr DirectionEnum directions[]
            = {DIR_NW,
               DIR_N,
               DIR_NE,
               DIR_W,
               DIR_E,
               DIR_SW,
               DIR_S,
               DIR_SE,
               DIR_SSW,
               DIR_SSE};
        for (auto d : directions) {
            SquareEnum sq = dst;
            for (int ii = 8; ii--;) {
                sq = shift(sq, d);
                if (sq == src)
                    return static_cast<int>(d) + promo_offset;
            }
        }
        return 0;
    }
};

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_MOVE_HPP
