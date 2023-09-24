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
    Move(const Int value);
    Move(
        const SquareEnum dst, const SquareEnum src, const bool promote = false);
    Move(const SquareEnum dst, const PieceTypeEnum src);
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
    SquareEnum destination() const;
    template <class T>
    T source() const;
    SquareEnum source_square() const
    {
        return source<SquareEnum>();
    }
    PieceTypeEnum source_piece() const
    {
        return source<PieceTypeEnum>();
    }
    bool promote() const;
    bool is_drop() const;
    Move rotate() const
    {
        const auto dst_rotated = rotate_square(destination());
        if (is_drop())
            return Move(dst_rotated, source<PieceTypeEnum>());
        const auto src_rotated = rotate_square(source<SquareEnum>());
        return Move(dst_rotated, src_rotated, promote());
    }
    int to_dlshogi_policy_index() const
    {
        const auto dst_index = static_cast<int>(destination());
        const auto src_index = to_dlshogi_source_index();
        return dst_index * num_policy_per_square() + src_index;
    }
    static constexpr int num_policy_per_square();

private:
    static constexpr SquareEnum rotate_square(const SquareEnum sq)
    {
        return static_cast<SquareEnum>(num_squares - 1 - static_cast<int>(sq));
    }
    int to_dlshogi_source_index() const;
};

} // namespace vshogi

#endif // VSHOGI_MOVE_HPP
