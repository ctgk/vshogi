#ifndef VSHOGI_PIECES_HPP
#define VSHOGI_PIECES_HPP

#include <string>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi
{

template <
    class PieceType,
    class BoardPiece,
    int NumPieceTypes,
    int NumStandPieceTypes,
    int PromotionBit>
struct Pieces
{
public:
    using PieceTypeEnum = PieceType;
    using BoardPieceTypeEnum = BoardPiece;

private:
    static constexpr int color_bit = PromotionBit + 1;
    static constexpr int promotion_mask = (1 << PromotionBit);
    static constexpr int color_mask = (1 << color_bit);
    static constexpr int piece_type_mask = (color_mask - 1);

public:
    static constexpr PieceType FU = static_cast<PieceType>(0); // NOLINT
    static constexpr PieceType OU // NOLINT
        = static_cast<PieceType>(promotion_mask - 1);
    static constexpr PieceType NA // NOLINT
        = static_cast<PieceType>(color_mask - 1);
    static constexpr BoardPiece VOID // NOLINT
        = static_cast<BoardPiece>((color_mask << 1) - 1);

    Pieces() = delete;
    static const int num_piece_types = NumPieceTypes;
    static const int num_stand_piece_types = NumStandPieceTypes;

    static const PieceType
        piece_array[static_cast<unsigned int>(NumPieceTypes)];
    static const PieceType
        stand_piece_array[static_cast<unsigned int>(NumStandPieceTypes)];

    /**
     * @brief Get the color of a board piece.
     * @note Passing VOID returns WHITE.
     *
     * @param p Board piece.
     * @return constexpr ColorEnum Color of the board piece.
     */
    static constexpr ColorEnum get_color(const BoardPiece& p)
    {
        return static_cast<ColorEnum>(p >> color_bit);
    }
    static constexpr PieceType to_piece_type(const BoardPiece& p)
    {
        return static_cast<PieceType>(p & piece_type_mask);
    }
    static PieceType to_piece_type(const char c);
    static constexpr BoardPiece
    to_board_piece(const ColorEnum& c, const PieceType& p)
    {
        if (p == NA)
            return VOID;
        return static_cast<BoardPiece>((c << color_bit) | p);
    }

    static constexpr bool is_promotable(const PieceType& p);
    static constexpr bool is_promotable(const BoardPiece& p)
    {
        return is_promotable(to_piece_type(p));
    }

    template <class T>
    static constexpr bool is_promoted(const T& p)
    {
        return static_cast<bool>(p & promotion_mask);
    }
    static constexpr bool is_promoted(const BoardPiece& p)
    {
        return is_promoted(to_piece_type(p));
    }
    static bool is_ranging_to(const BoardPiece& p, const DirectionEnum& d);

    template <class T>
    static constexpr T promote(const T& p)
    {
        return static_cast<T>(p | promotion_mask);
    }
    template <class T>
    static constexpr T demote(const T& p)
    {
        constexpr int demotion_mask = (~promotion_mask);
        return static_cast<T>(p & demotion_mask);
    }

    static int get_point(const PieceType& p);
    static int get_point(const BoardPiece& p)
    {
        return get_point(to_piece_type(p));
    }

    static void append_sfen(const BoardPiece& p, std::string& out);

    static constexpr BoardPiece B_OU = to_board_piece(BLACK, OU); // NOLINT
    static constexpr BoardPiece W_OU = to_board_piece(WHITE, OU); // NOLINT
};

} // namespace vshogi

#endif // VSHOGI_PIECES_HPP
