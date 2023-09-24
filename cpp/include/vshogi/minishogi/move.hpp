#ifndef VSHOGI_MINISHOGI_MOVE_HPP
#define VSHOGI_MINISHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/direction.hpp"
#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"
#include "vshogi/move.hpp"

namespace vshogi::minishogi
{

/**
 * @brief 16-bit integer representing a Minishogi move.
 * @details
 *       ________ ___*****       destination square (25-possibility)
 *       ________ __*_____       Promotion flag (2-possibility)
 *       ___***** ________       source square (30-possibility = 25-square + 5-drop)
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 *
 */
using Move = vshogi::Move<Squares, Pieces, std::uint16_t>;

} // namespace vshogi::minishogi

namespace vshogi
{

template <>
inline minishogi::Move::Move(const std::uint16_t value)
    : m_value(value & 0x1f3f)
{
}

template <>
inline minishogi::Move::Move(
    const SquareEnum dst, const SquareEnum src, const bool promote)
    : m_value(static_cast<std::uint16_t>((src << 8) | (promote << 5) | dst))
{
}

template <>
inline minishogi::Move::Move(const SquareEnum dst, const PieceTypeEnum src)
    : m_value(static_cast<std::uint16_t>(
        ((static_cast<int>(src) + num_squares) << 8) | dst))
{
}

template <>
inline minishogi::Squares::SquareEnum minishogi::Move::destination() const
{
    return static_cast<SquareEnum>(m_value & 0x1f);
}

template <>
template <>
inline minishogi::Squares::SquareEnum minishogi::Move::source<>() const
{
    return static_cast<SquareEnum>(m_value >> 8);
}

template <>
template <>
inline minishogi::Pieces::PieceTypeEnum minishogi::Move::source<>() const
{
    return static_cast<PieceTypeEnum>((m_value >> 8) - num_squares);
}

template <>
inline bool minishogi::Move::promote() const
{
    return static_cast<bool>(m_value & 0x0020);
}

template <>
inline bool minishogi::Move::is_drop() const
{
    return (m_value >> 8) >= num_squares;
}

template <>
constexpr int minishogi::Move::num_policy_per_square()
{
    return 2 * 8 + num_stand_piece_types;
}

template <>
inline int minishogi::Move::to_dlshogi_source_index() const
{
    if (is_drop())
        return 8 * 2 + static_cast<int>(source<PieceTypeEnum>());
    const auto dst = destination();
    const auto src = source<SquareEnum>();
    const auto promotion = promote();
    constexpr DirectionEnum directions[]
        = {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE};
    for (auto d : directions) {
        SquareEnum sq = dst;
        for (int i = 4; i--;) {
            sq = minishogi::Squares::shift(sq, d);
            if (sq == src)
                return static_cast<int>(d) + (promotion ? 8 : 0);
        }
    }
    return 0;
}

} // namespace vshogi

#endif // VSHOGI_MINISHOGI_MOVE_HPP
