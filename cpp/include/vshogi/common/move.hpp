#ifndef VSHOGI_MOVE_HPP
#define VSHOGI_MOVE_HPP

#include <cctype> // std::toupper

#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

/**
 * @brief 16-bit representation of a move.
 * @details
 *       fedcba98 76543210
 *       ________ _*******       Destination square
 *       ________ *_______       Promotion flag (2 possibilities)
 *       _******* ________       Source square or piece
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 */
template <class Parameters>
class Move
{
private:
    using C = Configuration<Parameters>;
    using ST = SquareTraits<Parameters>;
    using PT = PieceTraits<Parameters>;
    using Square = typename C::Square;
    using PieceType = typename C::PieceType;

    static constexpr uint source_shift = 8u;
    static constexpr uint promotion_shift = 7u;
    static constexpr std::uint16_t dst_mask = 0x007fu;
    static constexpr std::uint16_t prm_mask = 0x0080u;
    static constexpr std::uint16_t src_mask = 0x7f00u;
    static constexpr std::uint16_t full_mask = 0x7fffu;

private:
    /**
     * @brief 16-bit integer representing a move in a game.
    */
    std::uint16_t m_value;

    Move(const uint src, const Square dst, const bool promote = false);

public:
    Move() : m_value()
    {
    }
    Move(const std::uint16_t value) : m_value(value & full_mask)
    {
    }
    Move(
        const Square src, const Square dst, const bool promote = false) noexcept
        : Move(static_cast<uint>(src), dst, promote)
    {
    }
    Move(const PieceType src, const Square dst) noexcept
        : Move(static_cast<uint>(src) + C::num_squares, dst)
    {
    }
    explicit Move(const char sfen[5])
        : Move(
              (sfen[1] == '*') ? static_cast<uint>(PT::to_piece_type(sfen[0]))
                                     + C::num_squares
                               : static_cast<uint>(to_square(sfen)),
              to_square(sfen + 2),
              sfen[4] == '+')
    {
    }
    std::uint16_t hash() const
    {
        return m_value;
    }
    bool operator==(const Move& other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const Move& other) const
    {
        return m_value != other.m_value;
    }
    Square destination() const
    {
        return static_cast<Square>(m_value & dst_mask);
    }
    Square source_square() const
    {
        return static_cast<Square>(m_value >> source_shift);
    }
    PieceType source_piece() const
    {
        return static_cast<PieceType>(
            static_cast<uint>(m_value >> source_shift) - C::num_squares);
    }
    bool promote() const
    {
        return static_cast<bool>(m_value & prm_mask);
    }
    bool is_drop() const
    {
        return static_cast<uint>(m_value >> source_shift) >= C::num_squares;
    }
    Move rotate() const
    {
        const auto dst_rotated = ST::rotate(destination());
        if (is_drop())
            return Move(source_piece(), dst_rotated);
        const auto src_rotated = ST::rotate(source_square());
        return Move(src_rotated, dst_rotated, promote());
    }
    Move hflip() const
    {
        const auto dst_hflipped = ST::hflip(destination());
        if (is_drop())
            return Move(source_piece(), dst_hflipped);
        const auto src_hflipped = ST::hflip(source_square());
        return Move(src_hflipped, dst_hflipped, promote());
    }
    uint to_dlshogi_policy_index(const ColorEnum& by_side) const;
    static constexpr uint num_policy_per_square()
    {
        return 2 * C::num_dir_dl + C::num_stand_piece_types;
    }

private:
    static Square to_square(const char sfen[2]);
    uint to_dlshogi_source_index(const ColorEnum& by_side) const;
};

template <class P>
Move<P>::Move(const uint src, const Square dst, const bool promote)
    : m_value(static_cast<std::uint16_t>(
          (src << source_shift) | static_cast<uint>(promote << promotion_shift)
          | dst))
{
}

template <class P>
typename Configuration<P>::Square Move<P>::to_square(const char sfen[2])
{
    return ST::to_square(
        static_cast<typename C::File>(sfen[0] - '1'),
        static_cast<typename C::Rank>(sfen[1] - 'a'));
}

template <class P>
uint Move<P>::to_dlshogi_policy_index(const ColorEnum& by_side) const
{
    auto dst = destination();
    if (by_side == WHITE)
        dst = ST::rotate(dst);
    return dst * num_policy_per_square() + to_dlshogi_source_index(by_side);
}

template <class P>
uint Move<P>::to_dlshogi_source_index(const ColorEnum& by_side) const
{
    if (is_drop())
        return C::num_dir_dl * 2u + source_piece();
    const uint promo_offset = promote() ? C::num_dir_dl : 0u;
    auto dir = ST::direction(destination(), source_square());
    if (by_side == WHITE)
        dir = vshogi::rotate(dir);
    return dir + promo_offset;
}

} // namespace vshogi

#endif // VSHOGI_MOVE_HPP
