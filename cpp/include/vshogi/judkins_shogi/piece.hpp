#ifndef VSHOGI_JUDKINS_SHOGI_PIECE_HPP
#define VSHOGI_JUDKINS_SHOGI_PIECE_HPP

#include <cstdint>
#include <string>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi::judkins_shogi
{

struct Pieces
{
    Pieces() = delete;
    enum PieceTypeEnum : std::uint8_t
    {
        FU = 0b0000, //!< Fu (Pawn)
        KE = 0b0001, //!< Kei (Knight)
        GI = 0b0010, //!< Gin (Silver)
        KA = 0b0011, //!< Kaku (Bishop)
        HI = 0b0100, //!< Hisha (Rook)
        KI = 0b0101, //!< Kin (Gold)
        OU = 0b0110, //!< Ou, Gyoku (King)

        TO = 0b1000 + FU, //!< Tokin (Promoted Pawn)
        NK = 0b1000 + KE, //!< Nari-Kei (Promoted Knight)
        NG = 0b1000 + GI, //!< Nari-Gin (Promoted Silver)
        UM = 0b1000 + KA, //!< Uma (Promoted Bishop)
        RY = 0b1000 + HI, //!< Ryu (Promoted Rook)

        NA = 0b1111, //!< NA
    };

    enum BoardPieceTypeEnum : std::uint8_t
    {
        B_FU = FU, //!< Black Fu (Pawn)
        B_KE = KE, //!< Black Kei (Knight)
        B_GI = GI, //!< Black Gin (Silver)
        B_KA = KA, //!< Black Kaku (Bishop)
        B_HI = HI, //!< Black Hisha (Rook)
        B_KI = KI, //!< Black Kin (Gold)
        B_OU = OU, //!< Black Ou, Gyoku (King)
        B_TO = TO, //!< Black Tokin (Promoted Pawn)
        B_NK = NK, //!< Black Nari-Kei (Promoted Knight)
        B_NG = NG, //!< Black Nari-Gin (Promoted Silver)
        B_UM = UM, //!< Black Uma (Promoted Bishop)
        B_RY = RY, //!< Black Ryu (Promoted Rook)

        W_FU = 0b10000 + B_FU, //!< White Fu (Pawn)
        W_KE = 0b10000 + B_KE, //!< White Kei (Knight)
        W_GI = 0b10000 + B_GI, //!< White Gin (Silver)
        W_KA = 0b10000 + B_KA, //!< White Kaku (Bishop)
        W_HI = 0b10000 + B_HI, //!< White Hisha (Rook)
        W_KI = 0b10000 + B_KI, //!< White Kin (Gold)
        W_OU = 0b10000 + B_OU, //!< White Ou, Gyoku (King)
        W_TO = 0b10000 + B_TO, //!< White Tokin (Promoted Pawn)
        W_NK = 0b10000 + B_NK, //!< White Nari-Kei (Promoted Knight)
        W_NG = 0b10000 + B_NG, //!< White Nari-Gin (Promoted Silver)
        W_UM = 0b10000 + B_UM, //!< White Uma (Promoted Bishop)
        W_RY = 0b10000 + B_RY, //!< White Ryu (Promoted Rook)

        VOID = 0b11111, //!< Empty Square
    };
    static constexpr PieceTypeEnum stand_piece_array[]
        = {FU, KE, GI, KA, HI, KI};
    static constexpr int num_stand_piece_types
        = sizeof(stand_piece_array) / sizeof(stand_piece_array[0]);
    static constexpr PieceTypeEnum piece_array[]
        = {FU, KE, GI, KA, HI, KI, OU, TO, NK, NG, UM, RY};
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
    static constexpr bool is_promotable(const PieceTypeEnum p)
    {
        return (p < KI);
    }
    static constexpr bool is_promotable(const BoardPieceTypeEnum p)
    {
        return is_promotable(to_piece_type(p));
    }
    static constexpr bool is_promoted(const PieceTypeEnum p)
    {
        return static_cast<bool>(p >> 3);
    }
    static constexpr bool is_promoted(const BoardPieceTypeEnum p)
    {
        return is_promoted(to_piece_type(p));
    }
    static constexpr PieceTypeEnum promote(const PieceTypeEnum p)
    {
        if (is_promotable(p))
            return static_cast<PieceTypeEnum>(p | 0b01000);
        return p;
    }
    static constexpr BoardPieceTypeEnum promote(const BoardPieceTypeEnum p)
    {
        if (is_promotable(p))
            return static_cast<BoardPieceTypeEnum>(p | 0b01000);
        return p;
    }
    static constexpr PieceTypeEnum demote(const PieceTypeEnum p)
    {
        return static_cast<PieceTypeEnum>(p & 0b0111);
    }
    static constexpr BoardPieceTypeEnum demote(const BoardPieceTypeEnum p)
    {
        return static_cast<BoardPieceTypeEnum>(p & 0b10111);
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
    static inline void append_sfen(const BoardPieceTypeEnum p, std::string& out)
    {
        const auto color = get_color(p);
        const auto promotion = is_promoted(p);
        const auto pt = demote(to_piece_type(p));
        char c;
        switch (pt) {
        case FU:
            c = 'p';
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

} // namespace vshogi::judkins_shogi

#endif // VSHOGI_JUDKINS_SHOGI_PIECE_HPP
