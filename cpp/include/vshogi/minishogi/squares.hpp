#ifndef VSHOGI_MINISHOGI_SQUARES_HPP
#define VSHOGI_MINISHOGI_SQUARES_HPP

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi::minishogi
{

struct Squares
{
    Squares() = delete;
    /**
     * @brief Enumeration of board squares following SFEN.
     * @details
     *   5   4   3   2   1
     * +---+---+---+---+---+
     * | 0 | 1 | 2 | 3 | 4 | a
     * +---+---+---+---+---+
     * | 5 | 6 | 7 | 8 | 9 | b
     * +---+---+---+---+---+
     * | 10| 11| 12| 13| 14| c
     * +---+---+---+---+---+
     * | 15| 16| 17| 18| 19| d
     * +---+---+---+---+---+
     * | 20| 21| 22| 23| 24| e
     * +---+---+---+---+---+
     */
    enum SquareEnum
    {
        // clang-format off
        SQ_5A =  0, SQ_4A =  1, SQ_3A =  2, SQ_2A =  3, SQ_1A =  4,
        SQ_5B =  5, SQ_4B =  6, SQ_3B =  7, SQ_2B =  8, SQ_1B =  9,
        SQ_5C = 10, SQ_4C = 11, SQ_3C = 12, SQ_2C = 13, SQ_1C = 14,
        SQ_5D = 15, SQ_4D = 16, SQ_3D = 17, SQ_2D = 18, SQ_1D = 19,
        SQ_5E = 20, SQ_4E = 21, SQ_3E = 22, SQ_2E = 23, SQ_1E = 24,
        // clang-format on
        SQ_NA,
        NUM_SQ = 25,
    };
    static constexpr int num_squares = 25;
    static constexpr SquareEnum square_array[] = {
        SQ_5A, SQ_4A, SQ_3A, SQ_2A, SQ_1A, SQ_5B, SQ_4B, SQ_3B, SQ_2B,
        SQ_1B, SQ_5C, SQ_4C, SQ_3C, SQ_2C, SQ_1C, SQ_5D, SQ_4D, SQ_3D,
        SQ_2D, SQ_1D, SQ_5E, SQ_4E, SQ_3E, SQ_2E, SQ_1E,
    };

    enum RankEnum
    {
        RANK1,
        RANK2,
        RANK3,
        RANK4,
        RANK5,
    };
    static constexpr int num_ranks = 5;

    static constexpr RankEnum to_rank(const SquareEnum sq)
    {
        return static_cast<RankEnum>(sq / 5);
    }

    static constexpr bool is_edge(const RankEnum r)
    {
        return (r == RANK1) || (r == RANK5);
    }

    enum FileEnum
    {
        FILE1,
        FILE2,
        FILE3,
        FILE4,
        FILE5,
    };
    static constexpr int num_files = 5;
    static constexpr SquareEnum squares_in_file[5][5] = {
        {SQ_1A, SQ_1B, SQ_1C, SQ_1D, SQ_1E},
        {SQ_2A, SQ_2B, SQ_2C, SQ_2D, SQ_2E},
        {SQ_3A, SQ_3B, SQ_3C, SQ_3D, SQ_3E},
        {SQ_4A, SQ_4B, SQ_4C, SQ_4D, SQ_4E},
        {SQ_5A, SQ_5B, SQ_5C, SQ_5D, SQ_5E},
    };

    static constexpr FileEnum to_file(const SquareEnum sq)
    {
        return static_cast<FileEnum>(4 - sq % 5);
    }

    static constexpr bool is_edge(const FileEnum f)
    {
        return (f == FILE1) || (f == FILE5);
    }
    static constexpr bool is_promotion_zone(const RankEnum r, const ColorEnum c)
    {
        return (c == BLACK) ? (r <= RANK1) : (r >= RANK5);
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
    };
    constexpr static int num_dlshogi_directions
        = sizeof(dlshogi_direction_array) / sizeof(dlshogi_direction_array[0]);
    static constexpr int direction_to_delta(const DirectionEnum d)
    {
        constexpr int table[] = {-6, -5, -4, -1, 1, 4, 5, 6};
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
        case DIR_NW:
        case DIR_W:
        case DIR_SW:
            return (f == FILE5) ? SQ_NA : out;
        case DIR_NE:
        case DIR_E:
        case DIR_SE:
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
        case 6:
        case 18:
        case 24:
            return DIR_NW;
        case 4:
        case 8:
        case 16:
            return DIR_NE;
        case 12:
            return (Squares::to_file(src) < Squares::FILE4) ? DIR_NW : DIR_NE;
        case -4:
        case -8:
        case -16:
            return DIR_SW;
        case -6:
        case -18:
        case -24:
            return DIR_SE;
        case -12:
            return (Squares::to_file(src) < Squares::FILE3) ? DIR_SW : DIR_SE;
        default:
            return DIR_NA;
        }
    }
};

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_SQUARES_HPP
