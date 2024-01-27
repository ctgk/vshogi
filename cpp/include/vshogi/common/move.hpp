#ifndef VSHOGI_MOVE_HPP
#define VSHOGI_MOVE_HPP

#include <cctype> // std::toupper

#include "vshogi/common/direction.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <
    class Int,
    class Squares,
    class Pieces,
    uint SourceMSB,
    uint PromotionMSB,
    uint DestinationMSB>
class Move
{
public:
    using SquaresType = Squares;
    using PiecesType = Pieces;
    using SquareEnum = typename Squares::SquareEnum;
    using PieceTypeEnum = typename Pieces::PieceTypeEnum;

private:
    static constexpr auto num_squares = Squares::num_squares;
    static constexpr auto num_stand_piece_types = Pieces::num_stand_piece_types;

    static constexpr uint source_shift = PromotionMSB + 1;
    static constexpr uint promotion_shift = DestinationMSB + 1;
    static constexpr Int destination_mask
        = static_cast<Int>((1U << (DestinationMSB + 1U)) - 1U);
    static constexpr Int promotion_mask
        = static_cast<Int>((1U << (PromotionMSB + 1U)) - 1U - destination_mask);
    static constexpr Int source_mask = static_cast<Int>(
        (1U << (SourceMSB + 1U)) - 1U - destination_mask - promotion_mask);

private:
    /**
     * @brief N-bit integer representing a move in a game.
    */
    Int m_value;

public:
    Move() : m_value()
    {
    }
    Move(const Int value)
        : m_value(value & (source_mask | promotion_mask | destination_mask))
    {
    }
    Move(
        const SquareEnum dst,
        const SquareEnum src,
        const bool promote = false) noexcept
        : Move(dst, static_cast<uint>(src), promote)
    {
    }
    Move(const SquareEnum dst, const PieceTypeEnum src) noexcept
        : Move(dst, static_cast<uint>(src) + num_squares)
    {
    }
    explicit Move(const char usi[5])
        : Move(
            Squares::to_square(usi + 2),
            (usi[1] == '*')
                ? static_cast<uint>(Pieces::to_piece_type(usi[0])) + num_squares
                : static_cast<uint>(Squares::to_square(usi)),
            usi[4] == '+')
    {
    }
    Int hash() const
    {
        return m_value;
    }
    void to_usi(char usi[5]) const
    {
        if (is_drop()) {
            usi[0] = static_cast<char>(
                std::toupper(Pieces::to_char(source_piece())));
            usi[1] = '*';
        } else {
            Squares::to_usi(usi, source_square());
        }
        Squares::to_usi(usi + 2, destination());
        usi[4] = (promote()) ? '+' : '\0';
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
        return static_cast<SquareEnum>(m_value & destination_mask);
    }
    SquareEnum source_square() const
    {
        return static_cast<SquareEnum>(m_value >> source_shift);
    }
    PieceTypeEnum source_piece() const
    {
        return static_cast<PieceTypeEnum>(
            static_cast<int>(m_value >> source_shift)
            - static_cast<int>(num_squares));
    }
    bool promote() const
    {
        return static_cast<bool>(m_value & promotion_mask);
    }
    bool is_drop() const
    {
        return static_cast<uint>(m_value >> source_shift) >= num_squares;
    }
    Move rotate() const
    {
        const auto dst_rotated = rotate_square(destination());
        if (is_drop())
            return Move(dst_rotated, source_piece());
        const auto src_rotated = rotate_square(source_square());
        return Move(dst_rotated, src_rotated, promote());
    }
    Move hflip() const
    {
        const auto dst_hflipped = Squares::hflip(destination());
        if (is_drop())
            return Move(dst_hflipped, source_piece());
        const auto src_hflipped = Squares::hflip(source_square());
        return Move(dst_hflipped, src_hflipped, promote());
    }
    uint to_dlshogi_policy_index() const
    {
        const auto dst_index = static_cast<uint>(destination());
        const auto src_index = to_dlshogi_source_index();
        return dst_index * num_policy_per_square() + src_index;
    }
    static constexpr uint num_policy_per_square()
    {
        return 2 * Squares::num_directions_dlshogi + num_stand_piece_types;
    }

private:
    Move(const SquareEnum dst, const uint src, const bool promote = false)
        : m_value(static_cast<Int>(
            (src << source_shift)
            | static_cast<uint>(promote << promotion_shift) | dst))
    {
    }
    static constexpr SquareEnum rotate_square(const SquareEnum sq)
    {
        return static_cast<SquareEnum>(
            static_cast<int>(num_squares) - 1 - static_cast<int>(sq));
    }
    uint to_dlshogi_source_index() const
    {
        if (is_drop())
            return Squares::num_directions_dlshogi * 2
                   + static_cast<uint>(source_piece());
        const uint promo_offset
            = promote() ? Squares::num_directions_dlshogi : 0U;
        const auto direction
            = Squares::get_direction(source_square(), destination());
        return static_cast<uint>(direction) + promo_offset;
    }
};

} // namespace vshogi

#endif // VSHOGI_MOVE_HPP
