#ifndef VSHOGI_PIECES_HPP
#define VSHOGI_PIECES_HPP

#include <string>

#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"

namespace vshogi
{

template <class Config>
struct Pieces
{
private:
    static constexpr uint num_piece_types = Config::num_piece_types;
    static constexpr uint num_colored_piece_types
        = Config::num_colored_piece_types;
    static constexpr uint num_stand_piece_types = Config::num_stand_piece_types;
    using PieceType = typename Config::PieceType;
    using ColoredPiece = typename Config::ColoredPiece;

    static const DirectionEnum attack_directions_table[2 * num_piece_types + 1]
                                                      [9];

public:
    static constexpr PieceType FU = static_cast<PieceType>(0); // NOLINT
    static constexpr PieceType OU // NOLINT
        = static_cast<PieceType>(num_stand_piece_types);
    static constexpr PieceType NA // NOLINT
        = static_cast<PieceType>(num_piece_types);
    static constexpr ColoredPiece VOID // NOLINT
        = static_cast<ColoredPiece>(num_colored_piece_types);

    Pieces() = delete;

    /**
     * @brief Get the color of a board piece.
     * @note Passing VOID returns WHITE.
     *
     * @param p Board piece.
     * @return constexpr ColorEnum Color of the board piece.
     */
    static constexpr ColorEnum get_color(const ColoredPiece& p)
    {
        return static_cast<ColorEnum>(p >= num_piece_types);
    }
    static constexpr PieceType to_piece_type(const ColoredPiece& p)
    {
        return (p < num_piece_types)
                   ? static_cast<PieceType>(p)
                   : static_cast<PieceType>(p - num_piece_types);
    }
    static PieceType to_piece_type(char c)
    {
        c = static_cast<char>(std::tolower(static_cast<int>(c)));
        const char* p = Config::piece_type_to_char;
        for (; *p != '\0'; ++p) {
            if (*p == c)
                return static_cast<PieceType>(p - Config::piece_type_to_char);
        }
        return NA;
    }
    static constexpr ColoredPiece
    to_board_piece(const ColorEnum& c, const PieceType& p)
    {
        if (p == NA)
            return VOID;
        return static_cast<ColoredPiece>(c * num_piece_types + p);
    }
    static constexpr char to_char(const PieceType& pt_demoted)
    {
        return Config::piece_type_to_char[pt_demoted];
    }

    static constexpr bool is_promotable(const PieceType& p)
    {
        return (p + 1 < num_stand_piece_types);
    }
    static constexpr bool is_promotable(const ColoredPiece& p)
    {
        return is_promotable(to_piece_type(p));
    }

    static constexpr bool is_promoted(const PieceType& pt)
    {
        return pt > num_stand_piece_types;
    }
    static constexpr bool is_promoted(const ColoredPiece& p)
    {
        return is_promoted(to_piece_type(p));
    }
    static bool is_ranging_to(const ColoredPiece& p, const DirectionEnum& d);
    static bool is_ranging_piece(const PieceType& pt);
    static bool is_ranging_piece(const ColoredPiece& p)
    {
        return is_ranging_piece(to_piece_type(p));
    }

    /**
     * @brief Promote a promotable piece.
     * @note If the piece is not promotable, the return may not be safe.
     *
     * @tparam T
     * @param p Piece to promote. Should be promotable.
     * @return constexpr T Promoted piece.
     */
    template <class T>
    static constexpr T promote_nocheck(const T& p)
    {
        return static_cast<T>(p + num_stand_piece_types + 1);
    }

    template <class T>
    static constexpr T demote(const T& p)
    {
        if (is_promoted(p))
            return demote_nocheck(p);
        return p;
    }
    template <class T>
    static constexpr T demote_nocheck(const T& p)
    {
        return static_cast<T>(p - num_stand_piece_types - 1);
    }

    static uint get_point(const PieceType& p);
    static uint get_point(const ColoredPiece& p)
    {
        return get_point(to_piece_type(p));
    }

    static void append_sfen(const ColoredPiece& p, std::string& out)
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
    static uint get_index(const ColoredPiece& p)
    {
        if (p == VOID)
            return 2 * num_piece_types;
        const PieceType pt = to_piece_type(p);
        const uint color_offset
            = static_cast<uint>(get_color(p) == WHITE) * num_piece_types;
        const uint promo_offset
            = static_cast<uint>(is_promoted(pt)) * (num_stand_piece_types + 1);
        const uint pt_index = (pt == OU) ? num_stand_piece_types
                                         : static_cast<uint>(demote(pt));
        return color_offset + promo_offset + pt_index;
    }
    static const DirectionEnum* get_attack_directions(const ColoredPiece& p)
    {
        return attack_directions_table[get_index(p)];
    }
    static void init_tables()
    {
    }

    static constexpr ColoredPiece B_OU = to_board_piece(BLACK, OU); // NOLINT
    static constexpr ColoredPiece W_OU = to_board_piece(WHITE, OU); // NOLINT
};

} // namespace vshogi

#endif // VSHOGI_PIECES_HPP
