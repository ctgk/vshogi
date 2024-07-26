#ifndef VSHOGI_MOVE_HPP
#define VSHOGI_MOVE_HPP

#include <cctype> // std::toupper

#include "vshogi/common/direction.hpp"
#include "vshogi/common/pieces.hpp"
#include "vshogi/common/squares.hpp"
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
template <class Config>
class Move
{
private:
    static constexpr auto num_squares = Config::num_squares;
    static constexpr auto num_stand_piece_types = Config::num_stand_piece_types;
    static constexpr auto num_dir_dl = Config::num_dir_dl;
    using SquaresHelper = Squares<Config>;
    using PiecesHelper = Pieces<Config>;
    using Square = typename Config::Square;
    using PieceType = typename Config::PieceType;

    using Int = std::uint16_t;
    static constexpr uint msb_destination = 6u;
    static constexpr uint msb_promotion = 7u;
    static constexpr uint msb_source = 14u;
    static constexpr uint source_shift = msb_promotion + 1u;
    static constexpr uint promotion_shift = msb_destination + 1u;
    static constexpr Int destination_mask
        = static_cast<Int>((1U << (msb_destination + 1U)) - 1U);
    static constexpr Int promotion_mask = static_cast<Int>(
        (1U << (msb_promotion + 1U)) - 1U - destination_mask);
    static constexpr Int source_mask = static_cast<Int>(
        (1U << (msb_source + 1U)) - 1U - destination_mask - promotion_mask);

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
        const Square dst, const Square src, const bool promote = false) noexcept
        : Move(dst, static_cast<uint>(src), promote)
    {
    }
    Move(const Square dst, const PieceType src) noexcept
        : Move(dst, static_cast<uint>(src) + num_squares)
    {
    }
    explicit Move(const char usi[5])
        : Move(
            SquaresHelper::to_square(usi + 2),
            (usi[1] == '*')
                ? static_cast<uint>(PiecesHelper::to_piece_type(usi[0]))
                      + num_squares
                : static_cast<uint>(SquaresHelper::to_square(usi)),
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
                std::toupper(PiecesHelper::to_char(source_piece())));
            usi[1] = '*';
        } else {
            SquaresHelper::to_usi(usi, source_square());
        }
        SquaresHelper::to_usi(usi + 2, destination());
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
    Square destination() const
    {
        return static_cast<Square>(m_value & destination_mask);
    }
    Square source_square() const
    {
        return static_cast<Square>(m_value >> source_shift);
    }
    PieceType source_piece() const
    {
        return static_cast<PieceType>(
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
        const auto dst_hflipped = SquaresHelper::hflip(destination());
        if (is_drop())
            return Move(dst_hflipped, source_piece());
        const auto src_hflipped = SquaresHelper::hflip(source_square());
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
        return 2 * num_dir_dl + num_stand_piece_types;
    }

private:
    Move(const Square dst, const uint src, const bool promote = false)
        : m_value(static_cast<Int>(
            (src << source_shift)
            | static_cast<uint>(promote << promotion_shift) | dst))
    {
    }
    static constexpr Square rotate_square(const Square sq)
    {
        return static_cast<Square>(
            static_cast<int>(num_squares) - 1 - static_cast<int>(sq));
    }
    uint to_dlshogi_source_index() const
    {
        if (is_drop())
            return num_dir_dl * 2 + static_cast<uint>(source_piece());
        const uint promo_offset = promote() ? num_dir_dl : 0U;
        const auto direction
            = SquaresHelper::get_direction(source_square(), destination());
        return static_cast<uint>(direction) + promo_offset;
    }
};

} // namespace vshogi

#endif // VSHOGI_MOVE_HPP
