#ifndef VSHOGI_SHOGI_PIECE_HPP
#define VSHOGI_SHOGI_PIECE_HPP

#include <cctype>
#include <cstdint>
#include <string>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi::shogi
{

struct Pieces
{
    Pieces() = delete;
    enum PieceTypeEnum : std::uint8_t
    {
        FU = 0b0000, //!< Fu (Pawn)
        KY = 0b0001, //!< Kyo (Lance)
        KE = 0b0010, //!< Kei (Knight)
        GI = 0b0011, //!< Gin (Silver)
        KA = 0b0100, //!< Kaku (Bishop)
        HI = 0b0101, //!< Hisha (Rook)
        KI = 0b0110, //!< Kin (Gold)
        OU = 0b0111, //!< Ou, Gyoku (King)

        TO = 0b1000, //!< Tokin (Promoted Pawn)
        NY = 0b1001, //!< Nari-Kyo (Promoted Lance)
        NK = 0b1010, //!< Nari-Kei (Promoted Knight)
        NG = 0b1011, //!< Nari-Gin (Promoted Silver)
        UM = 0b1100, //!< Uma (Promoted Bishop)
        RY = 0b1101, //!< Ryu (Promoted Rook)

        NA = 0b1111, //!< NA
    };
    static constexpr PieceTypeEnum stand_piece_array[]
        = {FU, KY, KE, GI, KA, HI, KI};
    static constexpr int num_stand_piece_types
        = sizeof(stand_piece_array) / sizeof(stand_piece_array[0]);
    static constexpr PieceTypeEnum piece_array[]
        = {FU, KY, KE, GI, KA, HI, KI, OU, TO, NY, NK, NG, UM, RY};
    static constexpr int num_piece_types
        = sizeof(piece_array) / sizeof(piece_array[0]);

    enum BoardPieceTypeEnum : std::uint8_t
    {
        B_FU = 0b00000, //!< Black Fu (Pawn)
        B_KY = 0b00001, //!< Black Kyo (Lance)
        B_KE = 0b00010, //!< Black Kei (Knight)
        B_GI = 0b00011, //!< Black Gin (Silver)
        B_KA = 0b00100, //!< Black Kaku (Bishop)
        B_HI = 0b00101, //!< Black Hisha (Rook)
        B_KI = 0b00110, //!< Black Kin (Gold)
        B_OU = 0b00111, //!< Black Ou, Gyoku (King)
        B_TO = 0b01000, //!< Black Tokin (Promoted Pawn)
        B_NY = 0b01001, //!< Black Nari-Kyo (Promoted Lance)
        B_NK = 0b01010, //!< Black Nari-Kei (Promoted Knight)
        B_NG = 0b01011, //!< Black Nari-Gin (Promoted Silver)
        B_UM = 0b01100, //!< Black Uma (Promoted Bishop)
        B_RY = 0b01101, //!< Black Ryu (Promoted Rook)

        W_FU = 0b10000, //!< White Fu (Pawn)
        W_KY = 0b10001, //!< White Kyo (Lance)
        W_KE = 0b10010, //!< White Kei (Knight)
        W_GI = 0b10011, //!< White Gin (Silver)
        W_KA = 0b10100, //!< White Kaku (Bishop)
        W_HI = 0b10101, //!< White Hisha (Rook)
        W_KI = 0b10110, //!< White Kin (Gold)
        W_OU = 0b10111, //!< White Ou, Gyoku (King)
        W_TO = 0b11000, //!< White Tokin (Promoted Pawn)
        W_NY = 0b11001, //!< White Nari-Kyo (Promoted Lance)
        W_NK = 0b11010, //!< White Nari-Kei (Promoted Knight)
        W_NG = 0b11011, //!< White Nari-Gin (Promoted Silver)
        W_UM = 0b11100, //!< White Uma (Promoted Bishop)
        W_RY = 0b11101, //!< White Ryu (Promoted Rook)

        VOID = 0b11111, //!< Empty Square
    };

    /**
     * @brief Get the color of a board piece. Note that passing VOID returns WHITE.
     *
     * @param p Board piece.
     * @return constexpr ColorEnum Color of the board piece.
     */
    static constexpr ColorEnum get_color(const BoardPieceTypeEnum p)
    {
        return static_cast<ColorEnum>(p >> 4);
    }

    static constexpr bool is_promotable(const PieceTypeEnum p)
    {
        return (p < 0b0110);
    }
    static constexpr bool is_promotable(const BoardPieceTypeEnum p)
    {
        return ((p & 0b01111) < 0b0110);
    }

    static constexpr bool is_promoted(const PieceTypeEnum p)
    {
        return static_cast<bool>(p >> 3);
    }
    static constexpr bool is_promoted(const BoardPieceTypeEnum p)
    {
        return static_cast<bool>((p & 0x0f) >> 3);
    }

    static constexpr PieceTypeEnum promote(const PieceTypeEnum p)
    {
        return static_cast<PieceTypeEnum>(p | 0b01000);
    }
    static constexpr BoardPieceTypeEnum promote(const BoardPieceTypeEnum p)
    {
        return static_cast<BoardPieceTypeEnum>(p | 0b01000);
    }

    static constexpr PieceTypeEnum demote(const PieceTypeEnum p)
    {
        return static_cast<PieceTypeEnum>(p & 0b0111);
    }
    static constexpr BoardPieceTypeEnum demote(const BoardPieceTypeEnum p)
    {
        return static_cast<BoardPieceTypeEnum>(p & 0b10111);
    }

    static constexpr PieceTypeEnum to_piece_type(const BoardPieceTypeEnum p)
    {
        return static_cast<PieceTypeEnum>(p & 0x0f);
    }
    static inline PieceTypeEnum to_piece_type(const char c)
    {
        switch (std::tolower(c)) {
        case 'p':
            return FU;
        case 'l':
            return KY;
        case 'n':
            return KE;
        case 's':
            return GI;
        case 'b':
            return KA;
        case 'r':
            return HI;
        case 'g':
            return KI;
        case 'k':
            return OU;
        default:
            return NA;
        }
    }

    static constexpr BoardPieceTypeEnum
    to_board_piece(const ColorEnum c, const PieceTypeEnum p)
    {
        if (p == NA)
            return VOID;
        return static_cast<BoardPieceTypeEnum>((c << 4) | p);
    }

    static inline bool
    is_ranging_to(const BoardPieceTypeEnum p, const DirectionEnum d)
    {
        if (p == B_KY)
            return (d == DIR_N);
        if (p == W_KY)
            return (d == DIR_S);
        const auto base = demote(to_piece_type(p));
        if (base == HI)
            return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
        if (base == KA)
            return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW)
                   || (d == DIR_SE);
        return false;
    }

    static int get_point(const PieceTypeEnum p)
    {
        const auto pt = demote(p);
        if (pt == OU)
            return 0;
        if ((pt == HI) || (pt == KA))
            return 5;
        return 1;
    }
    static int get_point(const BoardPieceTypeEnum p)
    {
        return get_point(to_piece_type(p));
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
        case KY:
            c = 'l';
            break;
        case KE:
            c = 'n';
            break;
        case GI:
            c = 's';
            break;
        case KA:
            c = 'b';
            break;
        case HI:
            c = 'r';
            break;
        case KI:
            c = 'g';
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

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_PIECE_HPP
