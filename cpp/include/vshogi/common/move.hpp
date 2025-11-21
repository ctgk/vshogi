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
            (sfen[1] == '*')
                ? static_cast<uint>(PT::to_piece_type(sfen[0])) + C::num_squares
                : static_cast<uint>(ST::to_square(sfen)),
            ST::to_square(sfen + 2),
            sfen[4] == '+')
    {
    }
    std::uint16_t hash() const
    {
        return m_value;
    }
    void to_sfen(char sfen[5]) const
    {
        if (is_drop()) {
            sfen[0]
                = static_cast<char>(std::toupper(PT::to_char(source_piece())));
            sfen[1] = '*';
        } else {
            ST::to_sfen(sfen, source_square());
        }
        ST::to_sfen(sfen + 2, destination());
        sfen[4] = (promote()) ? '+' : '\0';
    }
    std::string to_sfen() const
    {
        std::string out("     ");
        to_sfen(out.data());
        return out;
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
    uint to_dlshogi_policy_index() const
    {
        const auto dst_index = static_cast<uint>(destination());
        const auto src_index = to_dlshogi_source_index();
        return dst_index * num_policy_per_square() + src_index;
    }
    static constexpr uint num_policy_per_square()
    {
        return 2 * C::num_dir_dl + C::num_stand_piece_types;
    }
    std::string destination_to_jpn(const Square dst_prev) const
    {
        const auto dst = destination();
        if (dst == dst_prev)
            return u8"\u540c";
        return ST::to_jpn(dst);
    }
    std::string promotion_to_jpn(const PieceType pt, const ColorEnum t) const
    {
        if (promote())
            return u8"\u6210";
        if (PT::is_promotable(pt)
            && (ST::in_promotion_zone(destination(), t)
                || ST::in_promotion_zone(source_square(), t)))
            return u8"\u4e0d\u6210";
        return u8"";
    }
    std::string promotion_to_eng(const PieceType pt, const ColorEnum t) const
    {
        if (promote())
            return "+";
        if (PT::is_promotable(pt)
            && (ST::in_promotion_zone(destination(), t)
                || ST::in_promotion_zone(source_square(), t)))
            return "=";
        return "";
    }

private:
    Move(const uint src, const Square dst, const bool promote = false)
        : m_value(static_cast<std::uint16_t>(
            (src << source_shift)
            | static_cast<uint>(promote << promotion_shift) | dst))
    {
    }
    uint to_dlshogi_source_index() const
    {
        if (is_drop())
            return C::num_dir_dl * 2 + static_cast<uint>(source_piece());
        const uint promo_offset = promote() ? C::num_dir_dl : 0U;
        const auto direction = ST::direction(destination(), source_square());
        return static_cast<uint>(direction) + promo_offset;
    }
};

} // namespace vshogi

#endif // VSHOGI_MOVE_HPP
