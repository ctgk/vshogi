#ifndef VSHOGI_JUDKINS_SHOGI_MOVE_HPP
#define VSHOGI_JUDKINS_SHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/direction.hpp"
#include "vshogi/judkins_shogi/piece.hpp"
#include "vshogi/judkins_shogi/squares.hpp"
#include "vshogi/move.hpp"

namespace vshogi::judkins_shogi
{

/**
 * @brief 16-bit integer representing a move in Judkins-Shogi game.
 * @details
 *       ________ __******       Destination square (36 possibilities)
 *       ________ _*______       Promotion flag (2 possibilities)
 *       __****** ________       Source square or piece (42 possibilities = 36 + 6)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 */
using Move = vshogi::Move<Squares, Pieces, std::uint16_t>;

} // namespace vshogi::judkins_shogi

namespace vshogi
{

template <>
inline judkins_shogi::Move::Move(const std::uint16_t value)
    : m_value(value & 0x3f7f)
{
}

template <>
inline judkins_shogi::Move::Move(
    const SquareEnum dst, const SquareEnum src, const bool promote)
    : m_value(static_cast<std::uint16_t>((src << 8) | (promote << 6) | dst))
{
}

template <>
inline judkins_shogi::Move::Move(const SquareEnum dst, const PieceTypeEnum src)
    : m_value(static_cast<std::uint16_t>(((src + num_squares) << 8) | dst))
{
}

template <>
inline judkins_shogi::Squares::SquareEnum
judkins_shogi::Move::destination() const
{
    return static_cast<SquareEnum>(m_value & 0x003f);
}

template <>
template <>
inline judkins_shogi::Squares::SquareEnum judkins_shogi::Move::source<>() const
{
    return static_cast<SquareEnum>(m_value >> 8);
}

template <>
template <>
inline judkins_shogi::Pieces::PieceTypeEnum
judkins_shogi::Move::source<>() const
{
    return static_cast<PieceTypeEnum>((m_value >> 8) - num_squares);
}

template <>
inline bool judkins_shogi::Move::promote() const
{
    return static_cast<bool>(m_value & 0x0040);
}

template <>
inline bool judkins_shogi::Move::is_drop() const
{
    return (m_value >> 8) >= num_squares;
}

template <>
constexpr int judkins_shogi::Move::num_policy_per_square()
{
    return 2 * 10 + 6;
}

template <>
inline int judkins_shogi::Move::to_dlshogi_source_index() const
{
    if (is_drop())
        return 2 * 10 + static_cast<int>(source<PieceTypeEnum>());
    const auto dst = destination();
    const auto src = source<SquareEnum>();
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
    constexpr int max_range = 5;
    for (auto d : directions) {
        SquareEnum sq = dst;
        for (int ii = max_range; ii--;) {
            sq = judkins_shogi::Squares::shift(sq, d);
            if (sq == src)
                return static_cast<int>(d) + promo_offset;
        }
    }
    return 0;
}

} // namespace vshogi

#endif // VSHOGI_JUDKINS_SHOGI_MOVE_HPP
