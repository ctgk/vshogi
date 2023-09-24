#ifndef VSHOGI_ANIMAL_SHOGI_MOVE_HPP
#define VSHOGI_ANIMAL_SHOGI_MOVE_HPP

#include <cstdint>

#include "vshogi/animal_shogi/piece.hpp"
#include "vshogi/animal_shogi/squares.hpp"
#include "vshogi/move.hpp"

namespace vshogi::animal_shogi
{

/**
 * @brief 8bit integer representing an Animal Shogi move.
 * @details
 *       ____ ****       destination square (12 squares)
 *       **** ____       source square (15 = 12 squares + 3 pieces to drop)
 * (MSB) xxxx xxxx (LSB)
 */
using Move = vshogi::Move<Squares, Pieces, std::uint8_t>;

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
inline animal_shogi::Move::Move(const std::uint8_t value) : m_value(value)
{
}

template <>
inline animal_shogi::Move::Move(
    const SquareEnum dst, const SquareEnum src, const bool)
    : m_value(static_cast<std::uint8_t>((src << 4) | dst))
{
}

template <>
inline animal_shogi::Move::Move(const SquareEnum dst, const PieceTypeEnum src)
    : m_value(static_cast<std::uint8_t>(((src + num_squares) << 4) | dst))
{
}

template <>
inline animal_shogi::Squares::SquareEnum animal_shogi::Move::destination() const
{
    return static_cast<SquareEnum>(m_value & 0x0f);
}

template <>
template <>
inline animal_shogi::Squares::SquareEnum animal_shogi::Move::source<>() const
{
    return static_cast<SquareEnum>(m_value >> 4);
}

template <>
template <>
inline animal_shogi::Pieces::PieceTypeEnum animal_shogi::Move::source<>() const
{
    return static_cast<PieceTypeEnum>((m_value >> 4) - num_squares);
}

template <>
inline bool animal_shogi::Move::promote() const
{
    return false;
}

template <>
inline bool animal_shogi::Move::is_drop() const
{
    return (m_value >> 4) >= num_squares;
}

template <>
inline int animal_shogi::Move::to_dlshogi_source_index() const
{
    if (is_drop())
        return 8 + static_cast<int>(source<PieceTypeEnum>());
    constexpr int diff_plus_4_to_dir_index[] = {0, 1, 2, 3, -1, 4, 5, 6, 7};
    return diff_plus_4_to_dir_index
        [static_cast<int>(source<SquareEnum>())
         - static_cast<int>(destination()) + 4];
}

template <>
constexpr int animal_shogi::Move::num_policy_per_square()
{
    return 8 + 3;
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_MOVE_HPP
