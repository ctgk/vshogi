#ifndef VSHOGI_JUDKINS_SHOGI_SQUARES_HPP
#define VSHOGI_JUDKINS_SHOGI_SQUARES_HPP

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi::judkins_shogi
{

struct Squares
{
    Squares() = delete;

    /**
    * @brief Enumeration of board squares following SFEN.
    * @details
    *   6   5   4   3   2   1
    * +---+---+---+---+---+---+
    * | 0 | 1 | 2 | 3 | 4 | 5 | a
    * +---+---+---+---+---+---+
    * | 6 | 7 | 8 | 9 | 10| 11| b
    * +---+---+---+---+---+---+
    * | 12| 13| 14| 15| 16| 17| c
    * +---+---+---+---+---+---+
    * | 18| 19| 20| 21| 22| 23| d
    * +---+---+---+---+---+---+
    * | 24| 25| 26| 27| 28| 29| e
    * +---+---+---+---+---+---+
    * | 30| 31| 32| 33| 34| 35| f
    * +---+---+---+---+---+---+
    */
    enum SquareEnum
    {
        // clang-format off
        SQ_6A, SQ_5A, SQ_4A, SQ_3A, SQ_2A, SQ_1A,
        SQ_6B, SQ_5B, SQ_4B, SQ_3B, SQ_2B, SQ_1B,
        SQ_6C, SQ_5C, SQ_4C, SQ_3C, SQ_2C, SQ_1C,
        SQ_6D, SQ_5D, SQ_4D, SQ_3D, SQ_2D, SQ_1D,
        SQ_6E, SQ_5E, SQ_4E, SQ_3E, SQ_2E, SQ_1E,
        SQ_6F, SQ_5F, SQ_4F, SQ_3F, SQ_2F, SQ_1F,
        // clang-format on
        SQ_NA,
        NUM_SQ = 36,

        // clang-format off
        A6 = SQ_6A, A5 = SQ_5A, A4 = SQ_4A, A3 = SQ_3A, A2 = SQ_2A, A1 = SQ_1A,
        B6 = SQ_6B, B5 = SQ_5B, B4 = SQ_4B, B3 = SQ_3B, B2 = SQ_2B, B1 = SQ_1B,
        C6 = SQ_6C, C5 = SQ_5C, C4 = SQ_4C, C3 = SQ_3C, C2 = SQ_2C, C1 = SQ_1C,
        D6 = SQ_6D, D5 = SQ_5D, D4 = SQ_4D, D3 = SQ_3D, D2 = SQ_2D, D1 = SQ_1D,
        E6 = SQ_6E, E5 = SQ_5E, E4 = SQ_4E, E3 = SQ_3E, E2 = SQ_2E, E1 = SQ_1E,
        F6 = SQ_6F, F5 = SQ_5F, F4 = SQ_4F, F3 = SQ_3F, F2 = SQ_2F, F1 = SQ_1F,
        // clang-format on
    };
    static constexpr int num_squares = 36;
    static constexpr SquareEnum square_array[] = {
        // clang-format off
        SQ_6A, SQ_5A, SQ_4A, SQ_3A, SQ_2A, SQ_1A,
        SQ_6B, SQ_5B, SQ_4B, SQ_3B, SQ_2B, SQ_1B,
        SQ_6C, SQ_5C, SQ_4C, SQ_3C, SQ_2C, SQ_1C,
        SQ_6D, SQ_5D, SQ_4D, SQ_3D, SQ_2D, SQ_1D,
        SQ_6E, SQ_5E, SQ_4E, SQ_3E, SQ_2E, SQ_1E,
        SQ_6F, SQ_5F, SQ_4F, SQ_3F, SQ_2F, SQ_1F,
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
    };
    static constexpr int num_ranks = 6;
    enum FileEnum
    {
        FILE1,
        FILE2,
        FILE3,
        FILE4,
        FILE5,
        FILE6,
    };
    static constexpr int num_files = 6;
    static constexpr SquareEnum squares_in_file[6][6] = {
        {SQ_1A, SQ_1B, SQ_1C, SQ_1D, SQ_1E, SQ_1F},
        {SQ_2A, SQ_2B, SQ_2C, SQ_2D, SQ_2E, SQ_2F},
        {SQ_3A, SQ_3B, SQ_3C, SQ_3D, SQ_3E, SQ_3F},
        {SQ_4A, SQ_4B, SQ_4C, SQ_4D, SQ_4E, SQ_4F},
        {SQ_5A, SQ_5B, SQ_5C, SQ_5D, SQ_5E, SQ_5F},
        {SQ_6A, SQ_6B, SQ_6C, SQ_6D, SQ_6E, SQ_6F},
    };

    static constexpr RankEnum to_rank(const SquareEnum sq)
    {
        return static_cast<RankEnum>(sq / 6);
    }
    static constexpr FileEnum to_file(const SquareEnum sq)
    {
        return static_cast<FileEnum>(5 - sq % 6);
    }
    static bool is_edge(const RankEnum r)
    {
        return ((r == RANK1) || (r == RANK6));
    }
    static bool is_edge(const FileEnum f)
    {
        return ((f == FILE1) || (f == FILE6));
    }
    static bool is_edge(const SquareEnum sq)
    {
        const auto r = to_rank(sq);
        const auto f = to_file(sq);
        return (is_edge(r) || is_edge(f));
    }
    static bool is_promotion_zone(const RankEnum r, const ColorEnum c)
    {
        return (c == BLACK) ? (r <= RANK2) : (r >= RANK5);
    }
    static bool is_promotion_zone(const SquareEnum sq, const ColorEnum c)
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
    static constexpr int direction_to_delta(const DirectionEnum d)
    {
        constexpr int table[] = {-7, -6, -5, -1, 1, 5, 6, 7, 11, 13, -13, -11};
        return table[d];
    }
    static SquareEnum shift(const SquareEnum sq, const DirectionEnum d)
    {
        if (d == DIR_NA)
            return SQ_NA;
        const auto r = to_rank(sq);
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
            return (f == FILE6) ? SQ_NA : out;
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
        if (to_file(dst) == to_file(src))
            return (src < dst) ? DIR_S : DIR_N;
        if (to_rank(dst) == to_rank(src))
            return (src < dst) ? DIR_E : DIR_W;
        switch (static_cast<int>(src - dst)) {
        case 7:
        case 14:
        case 21:
        case 28:
        case 35:
            return DIR_NW;
        case 5:
        case 10:
        case 15:
        case 20:
        case 25:
            return DIR_NE;
        case -5:
        case -10:
        case -15:
        case -20:
        case -25:
            return DIR_SW;
        case -7:
        case -14:
        case -21:
        case -28:
        case -35:
            return DIR_SE;
        case -11:
            return DIR_SSW;
        case -13:
            return DIR_SSE;
        default:
            return DIR_NA;
        }
    }
};

} // namespace vshogi::judkins_shogi

#endif // VSHOGI_JUDKINS_SHOGI_SQUARES_HPP
