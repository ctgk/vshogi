#ifndef VSHOGI_MINISHOGI_PIECE_HPP
#define VSHOGI_MINISHOGI_PIECE_HPP

#include <cstdint>
#include <string>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi::minishogi
{

struct Pieces
{
    Pieces() = delete;

    enum PieceTypeEnum : std::uint8_t
    {
        FU = 0b0000, //!< Fu (Pawn)
        GI = 0b0001, //!< Gin (Silver)
        KA = 0b0010, //!< Kaku (Bishop)
        HI = 0b0011, //!< Hisha (Rook)

        KI = 0b0100, //!< Kin (Gold)
        OU = 0b0101, //!< Ou, Gyoku (King)

        TO = 0b1000, //!< Tokin (Promoted Pawn)
        NG = 0b1001, //!< Nari-Gin (Promoted Silver)
        UM = 0b1010, //!< Uma (Promoted Bishop)
        RY = 0b1011, //!< Ryu (Promoted Rook)

        NA = 0b1111, //!< Not available
    };
    static constexpr PieceTypeEnum piece_array[]
        = {FU, GI, KA, HI, KI, OU, TO, NG, UM, RY};
    static constexpr PieceTypeEnum stand_piece_array[] = {FU, GI, KA, HI, KI};
    static constexpr int num_stand_piece_types
        = sizeof(stand_piece_array) / sizeof(stand_piece_array[0]);

    static inline bool is_promotable(const PieceTypeEnum p)
    {
        return (p < 0b0100);
    }

    static inline PieceTypeEnum promote(const PieceTypeEnum p)
    {
        return is_promotable(p) ? static_cast<PieceTypeEnum>(p | 0b1000) : p;
    }

    static inline PieceTypeEnum demote(const PieceTypeEnum p)
    {
        return (p == NA) ? NA : static_cast<PieceTypeEnum>(p & 0b0111);
    }

    enum BoardPieceTypeEnum : std::uint8_t
    {
        B_FU = 0b00000, //!< Black Fu (Pawn)
        B_GI = 0b00001, //!< Black Gin (Silver)
        B_KA = 0b00010, //!< Black Kaku (Bishop)
        B_HI = 0b00011, //!< Black Hisha (Rook)
        B_KI = 0b00100, //!< Black Kin (Gold)
        B_OU = 0b00101, //!< Black Ou, Gyoku (King)
        B_TO = 0b01000, //!< Black Tokin (Promoted Pawn)
        B_NG = 0b01001, //!< Black Nari-Gin (Promoted Silver)
        B_UM = 0b01010, //!< Black Uma (Promoted Bishop)
        B_RY = 0b01011, //!< Black Ryu (Promoted Rook)

        W_FU = 0b10000, //!< White Fu (Pawn)
        W_GI = 0b10001, //!< White Gin (Silver)
        W_KA = 0b10010, //!< White Kaku (Bishop)
        W_HI = 0b10011, //!< White Hisha (Rook)
        W_KI = 0b10100, //!< White Kin (Gold)
        W_OU = 0b10101, //!< White Ou, Gyoku (King)
        W_TO = 0b11000, //!< White Tokin (Promoted Pawn)
        W_NG = 0b11001, //!< White Nari-Gin (Promoted Silver)
        W_UM = 0b11010, //!< White Uma (Promoted Bishop)
        W_RY = 0b11011, //!< White Ryu (Promoted Rook)

        VOID = 0b11111, //!< Empty Square.
    };

    /**
     * @brief Get the color of the board piece. Note that passing VOID returns WHITE.
     *
     * @param p Board piece.
     * @return ColorEnum Color of the board piece.
     */
    static constexpr ColorEnum get_color(const BoardPieceTypeEnum p)
    {
        return static_cast<ColorEnum>(p >> 4);
    }

    static constexpr PieceTypeEnum to_piece_type(const BoardPieceTypeEnum p)
    {
        return static_cast<PieceTypeEnum>(p & 0x0f);
    }

    static inline BoardPieceTypeEnum
    to_board_piece(const ColorEnum c, const PieceTypeEnum p)
    {
        if (p == NA)
            return VOID;
        return static_cast<BoardPieceTypeEnum>((c << 4) | p);
    }

    static constexpr bool is_promoted(const PieceTypeEnum p)
    {
        return static_cast<bool>(p & 0b01000);
    }

    static constexpr bool is_promoted(const BoardPieceTypeEnum p)
    {
        return static_cast<bool>(p & 0b01000);
    }

    static bool is_promotable(const BoardPieceTypeEnum p)
    {
        return (to_piece_type(p) < 0b0100);
    }

    static BoardPieceTypeEnum promote(const BoardPieceTypeEnum p)
    {
        return is_promotable(p) ? static_cast<BoardPieceTypeEnum>(p | 0b1000)
                                : p;
    }

    static inline bool
    is_ranging_to(const BoardPieceTypeEnum p, const DirectionEnum d)
    {
        const auto base = demote(to_piece_type(p));
        if (base == HI)
            return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
        if (base == KA)
            return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW)
                   || (d == DIR_SE);
        return false;
    }

    static void append_sfen(const BoardPieceTypeEnum p, std::string& out)
    {
        const auto color = get_color(p);
        const auto promotion = is_promoted(p);
        const auto pt = demote(to_piece_type(p));
        char c;
        switch (pt) {
        case FU:
            c = 'p';
            break;
        case GI:
            c = 's';
            break;
        case KI:
            c = 'g';
            break;
        case KA:
            c = 'b';
            break;
        case HI:
            c = 'r';
            break;
        case OU:
            c = 'k';
            break;
        default:
            c = ' ';
            break;
        }
        if (color == BLACK)
            c = static_cast<char>(std::toupper(static_cast<int>(c)));
        if (promotion)
            out += '+';
        out += c;
    }
};

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_PIECE_HPP
