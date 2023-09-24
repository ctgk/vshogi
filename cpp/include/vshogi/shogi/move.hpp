#ifndef VSHOGI_SHOGI_MOVE_HPP
#define VSHOGI_SHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/direction.hpp"
#include "vshogi/move.hpp"
#include "vshogi/shogi/piece.hpp"
#include "vshogi/shogi/squares.hpp"

namespace vshogi::shogi
{

/**
 * @brief 16-bit integer representing a move in shogi game.
 * @details
 *       ________ _*******       Destination square (81 possibilities)
 *       ________ *_______       Promotion flag (2 possibilities)
 *       _******* ________       Source square or piece (88 possibilities = 81 + 7)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 */
using Move = vshogi::Move<Squares, Pieces, std::uint16_t>;

} // namespace vshogi::shogi

namespace vshogi
{

template <>
inline shogi::Move::Move(const std::uint16_t value) : m_value(value & 0x7fff)
{
}

template <>
inline shogi::Move::Move(
    const SquareEnum dst, const SquareEnum src, const bool promote)
    : m_value(static_cast<std::uint16_t>((src << 8) | (promote << 7) | dst))
{
}

template <>
inline shogi::Move::Move(const SquareEnum dst, const PieceTypeEnum src)
    : m_value(static_cast<std::uint16_t>(
        ((static_cast<int>(src) + num_squares) << 8) | dst))
{
}

template <>
inline shogi::Squares::SquareEnum shogi::Move::destination() const
{
    return static_cast<SquareEnum>(m_value & 0x007f);
}

template <>
template <>
inline shogi::Squares::SquareEnum shogi::Move::source<>() const
{
    return static_cast<SquareEnum>(m_value >> 8);
}

template <>
template <>
inline shogi::Pieces::PieceTypeEnum shogi::Move::source<>() const
{
    return static_cast<PieceTypeEnum>((m_value >> 8) - num_squares);
}

template <>
inline bool shogi::Move::promote() const
{
    return static_cast<bool>(m_value & 0x0080);
}

template <>
inline bool shogi::Move::is_drop() const
{
    return (m_value >> 8) >= num_squares;
}

template <>
constexpr int shogi::Move::num_policy_per_square()
{
    return 2 * 10 + num_stand_piece_types;
}

template <>
inline int shogi::Move::to_dlshogi_source_index() const
{
    if (is_drop())
        return 10 * 2 + static_cast<int>(source<PieceTypeEnum>());

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
    for (auto d : directions) {
        SquareEnum sq = dst;
        for (int ii = 8; ii--;) {
            sq = shogi::Squares::shift(sq, d);
            if (sq == src)
                return static_cast<int>(d) + promo_offset;
        }
    }
    return 0;
}

} // namespace vshogi

#endif // VSHOGI_SHOGI_MOVE_HPP
