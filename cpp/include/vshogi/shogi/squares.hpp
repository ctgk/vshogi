#ifndef VSHOGI_SHOGI_SQUARES_HPP
#define VSHOGI_SHOGI_SQUARES_HPP

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi::shogi
{

struct Squares
{
    Squares() = delete;
    /**
     * @brief Enumeration of board squares following SFEN.
     * @details
     *   9   8   7   6   5   4   3   2   1
     * +---+---+---+---+---+---+---+---+---+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | a
     * +---+---+---+---+---+---+---+---+---+
     * | 9 | 10| 11| 12| 13| 14| 15| 16| 17| b
     * +---+---+---+---+---+---+---+---+---+
     * | 18| 19| 20| 21| 22| 23| 24| 25| 26| c
     * +---+---+---+---+---+---+---+---+---+
     * | 27| 28| 29| 30| 31| 32| 33| 34| 35| d
     * +---+---+---+---+---+---+---+---+---+
     * | 36| 37| 38| 39| 40| 41| 42| 43| 44| e
     * +---+---+---+---+---+---+---+---+---+
     * | 45| 46| 47| 48| 49| 50| 51| 52| 53| f
     * +---+---+---+---+---+---+---+---+---+
     * | 54| 55| 56| 57| 58| 59| 60| 61| 62| g
     * +---+---+---+---+---+---+---+---+---+
     * | 63| 64| 65| 66| 67| 68| 69| 70| 71| h
     * +---+---+---+---+---+---+---+---+---+
     * | 72| 73| 74| 75| 76| 77| 78| 79| 80| i
     * +---+---+---+---+---+---+---+---+---+
     */
    enum SquareEnum
    {
        // clang-format off
    SQ_9A, SQ_8A, SQ_7A, SQ_6A, SQ_5A, SQ_4A, SQ_3A, SQ_2A, SQ_1A,
    SQ_9B, SQ_8B, SQ_7B, SQ_6B, SQ_5B, SQ_4B, SQ_3B, SQ_2B, SQ_1B,
    SQ_9C, SQ_8C, SQ_7C, SQ_6C, SQ_5C, SQ_4C, SQ_3C, SQ_2C, SQ_1C,
    SQ_9D, SQ_8D, SQ_7D, SQ_6D, SQ_5D, SQ_4D, SQ_3D, SQ_2D, SQ_1D,
    SQ_9E, SQ_8E, SQ_7E, SQ_6E, SQ_5E, SQ_4E, SQ_3E, SQ_2E, SQ_1E,
    SQ_9F, SQ_8F, SQ_7F, SQ_6F, SQ_5F, SQ_4F, SQ_3F, SQ_2F, SQ_1F,
    SQ_9G, SQ_8G, SQ_7G, SQ_6G, SQ_5G, SQ_4G, SQ_3G, SQ_2G, SQ_1G,
    SQ_9H, SQ_8H, SQ_7H, SQ_6H, SQ_5H, SQ_4H, SQ_3H, SQ_2H, SQ_1H,
    SQ_9I, SQ_8I, SQ_7I, SQ_6I, SQ_5I, SQ_4I, SQ_3I, SQ_2I, SQ_1I,
        // clang-format on
        SQ_NA,
        NUM_SQ = 81,

        // clang-format off
    A9 = SQ_9A, A8 = SQ_8A, A7 = SQ_7A, A6 = SQ_6A, A5 = SQ_5A, A4 = SQ_4A, A3 = SQ_3A, A2 = SQ_2A, A1 = SQ_1A,
    B9 = SQ_9B, B8 = SQ_8B, B7 = SQ_7B, B6 = SQ_6B, B5 = SQ_5B, B4 = SQ_4B, B3 = SQ_3B, B2 = SQ_2B, B1 = SQ_1B,
    C9 = SQ_9C, C8 = SQ_8C, C7 = SQ_7C, C6 = SQ_6C, C5 = SQ_5C, C4 = SQ_4C, C3 = SQ_3C, C2 = SQ_2C, C1 = SQ_1C,
    D9 = SQ_9D, D8 = SQ_8D, D7 = SQ_7D, D6 = SQ_6D, D5 = SQ_5D, D4 = SQ_4D, D3 = SQ_3D, D2 = SQ_2D, D1 = SQ_1D,
    E9 = SQ_9E, E8 = SQ_8E, E7 = SQ_7E, E6 = SQ_6E, E5 = SQ_5E, E4 = SQ_4E, E3 = SQ_3E, E2 = SQ_2E, E1 = SQ_1E,
    F9 = SQ_9F, F8 = SQ_8F, F7 = SQ_7F, F6 = SQ_6F, F5 = SQ_5F, F4 = SQ_4F, F3 = SQ_3F, F2 = SQ_2F, F1 = SQ_1F,
    G9 = SQ_9G, G8 = SQ_8G, G7 = SQ_7G, G6 = SQ_6G, G5 = SQ_5G, G4 = SQ_4G, G3 = SQ_3G, G2 = SQ_2G, G1 = SQ_1G,
    H9 = SQ_9H, H8 = SQ_8H, H7 = SQ_7H, H6 = SQ_6H, H5 = SQ_5H, H4 = SQ_4H, H3 = SQ_3H, H2 = SQ_2H, H1 = SQ_1H,
    I9 = SQ_9I, I8 = SQ_8I, I7 = SQ_7I, I6 = SQ_6I, I5 = SQ_5I, I4 = SQ_4I, I3 = SQ_3I, I2 = SQ_2I, I1 = SQ_1I,
        // clang-format on
    };
    static constexpr int num_squares = 81;
    static constexpr SquareEnum square_array[] = {
        // clang-format off
    SQ_9A, SQ_8A, SQ_7A, SQ_6A, SQ_5A, SQ_4A, SQ_3A, SQ_2A, SQ_1A,
    SQ_9B, SQ_8B, SQ_7B, SQ_6B, SQ_5B, SQ_4B, SQ_3B, SQ_2B, SQ_1B,
    SQ_9C, SQ_8C, SQ_7C, SQ_6C, SQ_5C, SQ_4C, SQ_3C, SQ_2C, SQ_1C,
    SQ_9D, SQ_8D, SQ_7D, SQ_6D, SQ_5D, SQ_4D, SQ_3D, SQ_2D, SQ_1D,
    SQ_9E, SQ_8E, SQ_7E, SQ_6E, SQ_5E, SQ_4E, SQ_3E, SQ_2E, SQ_1E,
    SQ_9F, SQ_8F, SQ_7F, SQ_6F, SQ_5F, SQ_4F, SQ_3F, SQ_2F, SQ_1F,
    SQ_9G, SQ_8G, SQ_7G, SQ_6G, SQ_5G, SQ_4G, SQ_3G, SQ_2G, SQ_1G,
    SQ_9H, SQ_8H, SQ_7H, SQ_6H, SQ_5H, SQ_4H, SQ_3H, SQ_2H, SQ_1H,
    SQ_9I, SQ_8I, SQ_7I, SQ_6I, SQ_5I, SQ_4I, SQ_3I, SQ_2I, SQ_1I,
        // clang-format on
    };

    enum RankEnum
    {
        RANK1,
        RANK2,
        RANK3,
        RANK4,
        RANK5,
        RANK6,
        RANK7,
        RANK8,
        RANK9,
    };
    static constexpr int num_ranks = 9;
    enum FileEnum
    {
        FILE1,
        FILE2,
        FILE3,
        FILE4,
        FILE5,
        FILE6,
        FILE7,
        FILE8,
        FILE9,
    };
    static constexpr int num_files = 9;
    static constexpr SquareEnum squares_in_file[9][9] = {
        {SQ_1A, SQ_1B, SQ_1C, SQ_1D, SQ_1E, SQ_1F, SQ_1G, SQ_1H, SQ_1I},
        {SQ_2A, SQ_2B, SQ_2C, SQ_2D, SQ_2E, SQ_2F, SQ_2G, SQ_2H, SQ_2I},
        {SQ_3A, SQ_3B, SQ_3C, SQ_3D, SQ_3E, SQ_3F, SQ_3G, SQ_3H, SQ_3I},
        {SQ_4A, SQ_4B, SQ_4C, SQ_4D, SQ_4E, SQ_4F, SQ_4G, SQ_4H, SQ_4I},
        {SQ_5A, SQ_5B, SQ_5C, SQ_5D, SQ_5E, SQ_5F, SQ_5G, SQ_5H, SQ_5I},
        {SQ_6A, SQ_6B, SQ_6C, SQ_6D, SQ_6E, SQ_6F, SQ_6G, SQ_6H, SQ_6I},
        {SQ_7A, SQ_7B, SQ_7C, SQ_7D, SQ_7E, SQ_7F, SQ_7G, SQ_7H, SQ_7I},
        {SQ_8A, SQ_8B, SQ_8C, SQ_8D, SQ_8E, SQ_8F, SQ_8G, SQ_8H, SQ_8I},
        {SQ_9A, SQ_9B, SQ_9C, SQ_9D, SQ_9E, SQ_9F, SQ_9G, SQ_9H, SQ_9I},
    };

    static constexpr RankEnum to_rank(const SquareEnum sq)
    {
        return static_cast<RankEnum>(sq / 9);
    }
    static constexpr FileEnum to_file(const SquareEnum sq)
    {
        return static_cast<FileEnum>(8 - sq % 9);
    }
    static bool is_edge(const RankEnum r)
    {
        return ((r == RANK1) || (r == RANK9));
    }
    static bool is_edge(const FileEnum f)
    {
        return ((f == FILE1) || (f == FILE9));
    }
    static bool is_edge(const SquareEnum sq)
    {
        const auto r = to_rank(sq);
        const auto f = to_file(sq);
        return (is_edge(r) || is_edge(f));
    }
    static constexpr bool is_promotion_zone(const RankEnum r, const ColorEnum c)
    {
        return (c == BLACK) ? (r <= RANK3) : (r >= RANK7);
    }
    static constexpr bool
    is_promotion_zone(const SquareEnum sq, const ColorEnum c)
    {
        return is_promotion_zone(to_rank(sq), c);
    }

    constexpr static DirectionEnum direction_array[] = {
        DIR_NW,
        DIR_N,
        DIR_NE,
        DIR_W,
        DIR_E,
        DIR_SW,
        DIR_S,
        DIR_SE,
        DIR_SSW,
        DIR_SSE,
        DIR_NNW,
        DIR_NNE,
    };
    constexpr static DirectionEnum dlshogi_direction_array[] = {
        DIR_NW,
        DIR_N,
        DIR_NE,
        DIR_W,
        DIR_E,
        DIR_SW,
        DIR_S,
        DIR_SE,
        DIR_SSW,
        DIR_SSE,
    };
    constexpr static int num_dlshogi_directions
        = sizeof(dlshogi_direction_array) / sizeof(dlshogi_direction_array[0]);
    constexpr static int direction_to_delta(const DirectionEnum d)
    {
        constexpr int table[]
            = {-10, -9, -8, -1, 1, 8, 9, 10, 17, 19, -19, -17};
        return table[d];
    }
    static constexpr SquareEnum
    shift(const SquareEnum sq, const DirectionEnum d)
    {
        if (d == DIR_NA)
            return SQ_NA;
        const auto f = to_file(sq);
        const auto out = static_cast<SquareEnum>(
            static_cast<int>(sq) + direction_to_delta(d));
        if ((out < 0) || (out >= num_squares))
            return SQ_NA;
        switch (d) {
        case DIR_NNW:
        case DIR_NW:
        case DIR_W:
        case DIR_SW:
        case DIR_SSW:
            return (f == FILE9) ? SQ_NA : out;
        case DIR_NNE:
        case DIR_NE:
        case DIR_E:
        case DIR_SE:
        case DIR_SSE:
            return (f == FILE1) ? SQ_NA : out;
        default:
            break;
        }
        return out;
    }
    static constexpr DirectionEnum
    get_direction(const SquareEnum dst, const SquareEnum src)
    {
        if ((dst == SQ_NA) || (src == SQ_NA))
            return DIR_NA;
        if (to_file(dst) == to_file(src))
            return (src < dst) ? DIR_S : DIR_N;
        if (to_rank(dst) == to_rank(src))
            return (src < dst) ? DIR_E : DIR_W;
        switch (static_cast<int>(src - dst)) {
        case 10:
        case 20:
        case 30:
        // case 40:
        case 50:
        case 60:
        case 70:
        case 80:
            return DIR_NW;
        case 8:
        case 16:
        case 24:
        case 32:
        // case 40:
        case 48:
        case 56:
        case 64:
            return DIR_NE;
        case 40:
            return (Squares::to_file(src) < Squares::FILE6) ? DIR_NW : DIR_NE;
        case -8:
        case -16:
        case -24:
        case -32:
        // case -40:
        case -48:
        case -56:
        case -64:
            return DIR_SW;
        case -10:
        case -20:
        case -30:
        // case -40:
        case -50:
        case -60:
        case -70:
        case -80:
            return DIR_SE;
        case -40:
            return (Squares::to_file(src) < Squares::FILE5) ? DIR_SW : DIR_SE;
        case -17:
            return DIR_SSW;
        case -19:
            return DIR_SSE;
        default:
            return DIR_NA;
        }
    }
};

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_SQUARES_HPP
