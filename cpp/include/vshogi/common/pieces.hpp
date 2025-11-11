#ifndef VSHOGI_PIECES_HPP
#define VSHOGI_PIECES_HPP

#include <cassert>
#include <cctype>
#include <string>

#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

struct FPTHelper
{
private:
    static const DirectionEnum attack_directions_table[29][9];

public:
    FPTHelper() = delete;

    static constexpr FullPieceTypes to_fpt(char c)
    {
        c = static_cast<char>(std::tolower(static_cast<int>(c)));
        switch (c) {
        case 'p':
            return PT_FU;
        case 'l':
            return PT_KY;
        case 'n':
            return PT_KE;
        case 's':
            return PT_GI;
        case 'b':
            return PT_KA;
        case 'r':
            return PT_HI;
        case 'g':
            return PT_KI;
        case 'k':
            return PT_OU;
        default:
            break;
        }
        return PT_NA;
    }
    static constexpr char to_char(const FullPieceTypes pt)
    {
        constexpr char table[] = {
            'p', //!< Fu (Pawn)
            'l', //!< Kyo (Lance)
            'n', //!< Kei (Knight)
            's', //!< Gin (Silver)
            'b', //!< Kaku (Bishop)
            'r', //!< Hisha (Rook)
            'g', //!< Kin (Gold)
            'k', //!< Ou, Gyoku (King)
            'p', //!< Tokin (Promoted Pawn)
            'l', //!< Nari-Kyo (Promoted Lance)
            'n', //!< Nari-Kei (Promoted Knight)
            's', //!< Nari-Gin (Promoted Silver)
            'b', //!< Uma (Promoted Bishop)
            'r', //!< Ryu (Promoted Rook)
            '?', //!< NA
        };
        return table[pt];
    }
    static const std::string&
    to_jpn(const FullPieceTypes pt, const bool single_char = true)
    {
        static const std::string single_char_table[] = {
            u8"\u6B69", //!< Fu (Pawn)
            u8"\u9999", //!< Kyo (Lance)
            u8"\u6842", //!< Kei (Knight)
            u8"\u9280", //!< Gin (Silver)
            u8"\u89D2", //!< Kaku (Bishop)
            u8"\u98DB", //!< Hisha (Rook)
            u8"\u91D1", //!< Kin (Gold)
            u8"\u7389", //!< Ou, Gyoku (King)
            u8"\u3068", //!< Tokin (Promoted Pawn)
            u8"\u674F", //!< Nari-Kyo (Promoted Lance)
            u8"\u572D", //!< Nari-Kei (Promoted Knight)
            u8"\u5168", //!< Nari-Gin (Promoted Silver)
            u8"\u99AC", //!< Uma (Promoted Bishop)
            u8"\u9F8D", //!< Ryu (Promoted Rook)
            u8"\uFF1F", //!< NA
        };
        static const std::string two_chars_table[] = {
            u8"\u6B69", //!< Fu (Pawn)
            u8"\u9999", //!< Kyo (Lance)
            u8"\u6842", //!< Kei (Knight)
            u8"\u9280", //!< Gin (Silver)
            u8"\u89D2", //!< Kaku (Bishop)
            u8"\u98DB", //!< Hisha (Rook)
            u8"\u91D1", //!< Kin (Gold)
            u8"\u7389", //!< Ou, Gyoku (King)
            u8"\u3068", //!< Tokin (Promoted Pawn)
            u8"\u6210\u9999", //!< Nari-Kyo (Promoted Lance)
            u8"\u6210\u6842", //!< Nari-Kei (Promoted Knight)
            u8"\u6210\u9280", //!< Nari-Gin (Promoted Silver)
            u8"\u99AC", //!< Uma (Promoted Bishop)
            u8"\u9F8D", //!< Ryu (Promoted Rook)
            u8"\uFF1F", //!< NA
        };
        if (single_char)
            return single_char_table[pt];
        return two_chars_table[pt];
    }
    static constexpr bool is_promotable(const FullPieceTypes pt)
    {
        return pt < PT_KI;
    }
    static constexpr bool is_promoted(const FullPieceTypes pt)
    {
        return pt > PT_OU;
    }
    static constexpr bool is_promotion_fully_superior(const FullPieceTypes pt)
    {
        return (pt == PT_FU) || (pt == PT_KA) || (pt == PT_HI);
    }
    static constexpr uint to_point(const FullPieceTypes pt)
    {
        constexpr uint table[] = {
            1, //!< Fu (Pawn)
            1, //!< Kyo (Lance)
            1, //!< Kei (Knight)
            1, //!< Gin (Silver)
            5, //!< Kaku (Bishop)
            5, //!< Hisha (Rook)
            1, //!< Kin (Gold)
            0, //!< Ou, Gyoku (King)
            1, //!< Tokin (Promoted Pawn)
            1, //!< Nari-Kyo (Promoted Lance)
            1, //!< Nari-Kei (Promoted Knight)
            1, //!< Nari-Gin (Promoted Silver)
            5, //!< Uma (Promoted Bishop)
            5, //!< Ryu (Promoted Rook)
            0, //!< NA
        };
        return table[pt];
    }

    /**
     * @brief Get valuation of a piece type.
     * @ref https://shogi.zukeran.org/2018/06/12/piece-value-1/
     *
     * @param pt Input
     * @return constexpr uint valuation of the input
     */
    static constexpr uint to_value(const FullPieceTypes pt)
    {
        constexpr uint table[] = {
            5, //!< Fu (Pawn)
            30, //!< Kyo (Lance)
            35, //!< Kei (Knight)
            55, //!< Gin (Silver)
            95, //!< Kaku (Bishop)
            100, //!< Hisha (Rook)
            60, //!< Kin (Gold)
            0, //!< Ou, Gyoku (King)
            60, //!< Tokin (Promoted Pawn)
            60, //!< Nari-Kyo (Promoted Lance)
            60, //!< Nari-Kei (Promoted Knight)
            60, //!< Nari-Gin (Promoted Silver)
            115, //!< Uma (Promoted Bishop)
            120, //!< Ryu (Promoted Rook)
            0, //!< NA
        };
        return table[pt];
    }

    static const DirectionEnum*
    get_attack_directions(const FullPieceTypes pt, const ColorEnum c)
    {
        if (pt == PT_NA)
            return attack_directions_table[28];
        return attack_directions_table[pt + static_cast<uint>(c) * 14u];
    }
    static bool is_ranging(const FullPieceTypes pt)
    {
        constexpr bool table[] = {
            false, //!< Fu (Pawn)
            true, //!< Kyo (Lance)
            false, //!< Kei (Knight)
            false, //!< Gin (Silver)
            true, //!< Kaku (Bishop)
            true, //!< Hisha (Rook)
            false, //!< Kin (Gold)
            false, //!< Ou, Gyoku (King)
            false, //!< Tokin (Promoted Pawn)
            false, //!< Nari-Kyo (Promoted Lance)
            false, //!< Nari-Kei (Promoted Knight)
            false, //!< Nari-Gin (Promoted Silver)
            true, //!< Uma (Promoted Bishop)
            true, //!< Ryu (Promoted Rook)
            false, //!< NA
        };
        return table[pt];
    }
    static bool is_ranging_to(const FullPieceTypes pt, const DirectionEnum d)
    {
        switch (pt) {
        case PT_KY:
            return (d == DIR_N);
        case PT_KA:
        case PT_UM:
            return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW)
                   || (d == DIR_SE);
        case PT_HI:
        case PT_RY:
            return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
        default:
            break;
        }
        return false;
    }
    static bool is_attacking_to(const FullPieceTypes pt, const DirectionEnum d)
    {
        switch (d) {
        case DIR_N:
            return (pt != PT_KE) && (pt != PT_KA);
        case DIR_NW:
        case DIR_NE:
            return !((pt < PT_GI) || (pt == PT_HI)); // FU, KY, KE, HI
        case DIR_W:
        case DIR_E:
        case DIR_S:
            return (pt > PT_KA) && (pt != PT_NA); // fu, ky, ke, gi, ka
        case DIR_SW:
        case DIR_SE:
            return (pt == PT_GI) || (pt == PT_KA) || (pt == PT_OU)
                   || (pt == PT_UM) || (pt == PT_RY);
        case DIR_NNW:
        case DIR_NNE:
            return (pt == PT_KE);

        default:
            break;
        }
        return false;
    }
};

inline const DirectionEnum FPTHelper::attack_directions_table[29][9] = {
    // clang-format off
    {DIR_N,                                                      DIR_NA}, // B_FU
    {DIR_N,                                                      DIR_NA}, // B_KY
    {DIR_NNW, DIR_NNE,                                           DIR_NA}, // B_KE
    {DIR_NW, DIR_N, DIR_NE, DIR_SW, DIR_SE,                      DIR_NA}, // B_GI
    {DIR_NW, DIR_NE, DIR_SW, DIR_SE,                             DIR_NA}, // B_KA
    {DIR_N, DIR_W, DIR_E, DIR_S,                                 DIR_NA}, // B_HI
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_KI
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_OU
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_TO
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NY
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NK
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S,                 DIR_NA}, // B_NG
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_UM
    {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_RY
    {DIR_S,                                                      DIR_NA}, // W_FU
    {DIR_S,                                                      DIR_NA}, // W_KY
    {DIR_SSE, DIR_SSW,                                           DIR_NA}, // W_KE
    {DIR_SE, DIR_S, DIR_SW, DIR_NE, DIR_NW,                      DIR_NA}, // W_GI
    {DIR_SE, DIR_SW, DIR_NE, DIR_NW,                             DIR_NA}, // W_KA
    {DIR_S, DIR_E, DIR_W, DIR_N,                                 DIR_NA}, // W_HI
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_KI
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_OU
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_TO
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NY
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NK
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_N,                 DIR_NA}, // W_NG
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_UM
    {DIR_SE, DIR_S, DIR_SW, DIR_E, DIR_W, DIR_NE, DIR_N, DIR_NW, DIR_NA}, // W_RY
    {                                                            DIR_NA}, // NA
    // clang-format on
};

template <class Parameters>
struct Pieces
{
private:
    using C = Configuration<Parameters>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;

public:
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
        return static_cast<ColorEnum>(p >= C::num_piece_types);
    }
    static constexpr PieceType to_piece_type(const ColoredPiece& p)
    {
        return (p < C::num_piece_types)
                   ? static_cast<PieceType>(p)
                   : static_cast<PieceType>(p - C::num_piece_types);
    }
    static PieceType to_piece_type(char c)
    {
        const FullPieceTypes fpt = FPTHelper::to_fpt(c);
        c = static_cast<char>(std::tolower(static_cast<int>(c)));
        const FullPieceTypes* ptr = C::piece_types.data();
        for (; *ptr != PT_NA; ++ptr) {
            if (*ptr == fpt)
                return static_cast<PieceType>(ptr - C::piece_types.data());
        }
        return C::NA;
    }
    static constexpr ColoredPiece
    to_board_piece(const ColorEnum& c, const PieceType& p)
    {
        if (p == C::NA)
            return C::VOID;
        return static_cast<ColoredPiece>(c * C::num_piece_types + p);
    }
    static constexpr ColoredPiece to_board_piece(const char c)
    {
        return to_board_piece(
            std::islower(static_cast<int>(c)) ? WHITE : BLACK,
            to_piece_type(c));
    }
    static constexpr char to_char(const PieceType& pt)
    {
        return FPTHelper::to_char(C::piece_types[pt]);
    }
    static const std::string& to_jpn(const PieceType& pt)
    {
        return FPTHelper::to_jpn(C::piece_types[pt]);
    }
    static const std::string&
    to_jpn(const PieceType& pt, const bool single_char)
    {
        return FPTHelper::to_jpn(C::piece_types[pt], single_char);
    }
    static const std::string to_eng(const PieceType& pt)
    {
        if (is_promoted(pt))
            return "+" + std::string(1, std::toupper(to_char(pt)));
        return std::string(1, std::toupper(to_char(pt)));
    }
    static const std::string to_eng(const ColoredPiece& p)
    {
        return to_eng(to_piece_type(p));
    }

    static constexpr bool is_promotable(const PieceType& p)
    {
        return (p + 1u < C::num_stand_piece_types);
    }
    static constexpr bool is_promotable(const ColoredPiece& p)
    {
        return is_promotable(to_piece_type(p));
    }

    static constexpr bool is_promoted(const PieceType& pt)
    {
        return pt > C::num_stand_piece_types;
    }
    static constexpr bool is_promoted(const ColoredPiece& p)
    {
        return is_promoted(to_piece_type(p));
    }
    static constexpr bool is_promotion_fully_superior(const PieceType& pt)
    {
        return FPTHelper::is_promotion_fully_superior(C::piece_types[pt]);
    }
    static constexpr bool is_promotion_fully_superior(const ColoredPiece& p)
    {
        return is_promotion_fully_superior(to_piece_type(p));
    }
    static bool is_ranging_to(const ColoredPiece& p, const DirectionEnum& d)
    {
        return FPTHelper::is_ranging_to(
            C::piece_types[to_piece_type(p)],
            (get_color(p) == BLACK) ? d : rotate(d));
    }
    static bool is_attacking_to(const ColoredPiece& p, const DirectionEnum& d)
    {
        return FPTHelper::is_attacking_to(
            C::piece_types[to_piece_type(p)],
            (get_color(p) == BLACK) ? d : rotate(d));
    }
    static bool is_ranging_piece(const PieceType& pt)
    {
        return FPTHelper::is_ranging(C::piece_types[pt]);
    }
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
        assert(is_promotable(p));
        return static_cast<T>(p + C::num_stand_piece_types + 1);
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
        return static_cast<T>(p - C::num_stand_piece_types - 1);
    }

    static uint get_point(const PieceType& p)
    {
        return FPTHelper::to_point(C::piece_types[p]);
    }
    static uint get_point(const ColoredPiece& p)
    {
        return get_point(to_piece_type(p));
    }
    static uint get_value(const PieceType& pt)
    {
        return FPTHelper::to_value(C::piece_types[pt]);
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
    static const DirectionEnum* get_attack_directions(const ColoredPiece& p)
    {
        assert(p != C::VOID);
        return FPTHelper::get_attack_directions(
            C::piece_types[to_piece_type(p)], get_color(p));
    }
    static void init_tables()
    {
    }
};

} // namespace vshogi

#endif // VSHOGI_PIECES_HPP
