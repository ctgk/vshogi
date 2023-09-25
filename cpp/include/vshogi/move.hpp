#ifndef VSHOGI_MOVE_HPP
#define VSHOGI_MOVE_HPP

namespace vshogi
{

template <class Squares, class Pieces, class Int>
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

private:
    /**
     * @brief N-bit integer representing a move in a game.
    */
    Int m_value;

public:
    Move(const Int value)
        : m_value(value & (source_mask() | promote_mask() | destination_mask()))
    {
    }
    Move(const SquareEnum dst, const SquareEnum src, const bool promote = false)
        : Move(dst, static_cast<int>(src), promote)
    {
    }
    Move(const SquareEnum dst, const PieceTypeEnum src)
        : Move(dst, static_cast<int>(src) + num_squares)
    {
    }
    Int hash() const
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
    SquareEnum destination() const
    {
        return static_cast<SquareEnum>(m_value & destination_mask());
    }
    SquareEnum source_square() const
    {
        return static_cast<SquareEnum>(m_value >> source_shift());
    }
    PieceTypeEnum source_piece() const
    {
        return static_cast<PieceTypeEnum>(
            (m_value >> source_shift()) - num_squares);
    }
    bool promote() const
    {
        return static_cast<bool>(m_value & promote_mask());
    }
    bool is_drop() const
    {
        return (m_value >> source_shift()) >= num_squares;
    }
    Move rotate() const
    {
        const auto dst_rotated = rotate_square(destination());
        if (is_drop())
            return Move(dst_rotated, source_piece());
        const auto src_rotated = rotate_square(source_square());
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
        return 2 * Squares::num_dlshogi_directions + num_stand_piece_types;
    }

private:
    static constexpr int source_shift();
    static constexpr int promote_shift();
    static constexpr Int destination_mask();
    static constexpr Int source_mask();
    static constexpr Int promote_mask();
    Move(const SquareEnum dst, const int src, const bool promote = false)
        : m_value(static_cast<Int>(
            (src << source_shift()) | (promote << promote_shift()) | dst))
    {
    }
    static constexpr SquareEnum rotate_square(const SquareEnum sq)
    {
        return static_cast<SquareEnum>(num_squares - 1 - static_cast<int>(sq));
    }
    int to_dlshogi_source_index() const;
};

} // namespace vshogi

#endif // VSHOGI_MOVE_HPP
