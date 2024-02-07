#ifndef VSHOGI_PIECES_HPP
#define VSHOGI_PIECES_HPP

#include <string>

#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"

namespace vshogi
{

template <
    class PieceType,
    class BoardPiece,
    uint NumPieceTypes,
    uint NumStandPieceTypes,
    uint PromotionBit>
struct Pieces
{
public:
    using PieceTypeEnum = PieceType;
    using BoardPieceTypeEnum = BoardPiece;

private:
    static constexpr uint color_bit = PromotionBit + 1;
    static constexpr uint promotion_mask = (1 << PromotionBit);
    static constexpr uint color_mask = (1 << color_bit);
    static constexpr uint piece_type_mask = (color_mask - 1);
    static DirectionEnum attack_directions_table[2 * NumPieceTypes + 1][9];

public:
    static constexpr PieceType FU = static_cast<PieceType>(0); // NOLINT
    static constexpr PieceType OU // NOLINT
        = static_cast<PieceType>(promotion_mask - 1);
    static constexpr PieceType NA // NOLINT
        = static_cast<PieceType>(color_mask - 1);
    static constexpr BoardPiece VOID // NOLINT
        = static_cast<BoardPiece>((color_mask << 1) - 1);

    Pieces() = delete;
    static const uint num_piece_types = NumPieceTypes;
    static const uint num_stand_piece_types = NumStandPieceTypes;

    static const PieceType piece_array[NumPieceTypes];
    static const PieceType stand_piece_array[NumStandPieceTypes];

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
    static char to_char(const PieceType& p);

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
    static bool is_ranging_piece(const PieceTypeEnum& pt);
    static bool is_ranging_piece(const BoardPieceTypeEnum& p)
    {
        return is_ranging_piece(to_piece_type(p));
    }

    template <class T>
    static constexpr T promote(const T& p)
    {
        return static_cast<T>(p | promotion_mask);
    }
    template <class T>
    static constexpr T demote(const T& p)
    {
        constexpr uint demotion_mask = (~promotion_mask);
        return static_cast<T>(p & demotion_mask);
    }

    static uint get_point(const PieceType& p);
    static uint get_point(const BoardPiece& p)
    {
        return get_point(to_piece_type(p));
    }

    static void append_sfen(const BoardPiece& p, std::string& out)
    {
        const auto color = get_color(p);
        const auto promotion = is_promoted(p);
        const auto pt = demote(to_piece_type(p));
        char c = to_char(pt);
        if (color == BLACK)
            c = static_cast<char>(std::toupper(static_cast<int>(c)));
        if (promotion)
            out += '+';
        out += c;
    }
    static uint get_index(const BoardPieceTypeEnum& p)
    {
        if (p == VOID)
            return 2 * NumPieceTypes;
        const PieceType pt = to_piece_type(p);
        const uint color_offset
            = static_cast<uint>(get_color(p) == WHITE) * NumPieceTypes;
        const uint promo_offset
            = static_cast<uint>(is_promoted(pt)) * (NumStandPieceTypes + 1);
        const uint pt_index
            = (pt == OU) ? NumStandPieceTypes : static_cast<uint>(demote(pt));
        return color_offset + promo_offset + pt_index;
    }
    static const DirectionEnum* get_attack_directions(const BoardPiece& p)
    {
        return attack_directions_table[get_index(p)];
    }
    static void init_tables()
    {
        std::fill_n(
            &attack_directions_table[0][0],
            sizeof(attack_directions_table)
                / sizeof(attack_directions_table[0][0]),
            DIR_NA);
        init_attack_directions_of_black();
        for (uint ii = NumPieceTypes; ii--;) {
            for (uint jj = 0; jj < 9; ++jj) {
                attack_directions_table[ii + NumPieceTypes][jj]
                    = rotate(attack_directions_table[ii][jj]);
            }
        }
    }
    static void init_attack_directions_of_black();

    static constexpr BoardPiece B_OU = to_board_piece(BLACK, OU); // NOLINT
    static constexpr BoardPiece W_OU = to_board_piece(WHITE, OU); // NOLINT
};

} // namespace vshogi

#endif // VSHOGI_PIECES_HPP
