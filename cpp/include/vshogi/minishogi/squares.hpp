#ifndef VSHOGI_MINISHOGI_SQUARES_HPP
#define VSHOGI_MINISHOGI_SQUARES_HPP

namespace vshogi::minishogi
{

/**
 * @brief Enumeration of board squares.
 * @details
 *   5   4   3   2   1
 * +---+---+---+---+---+
 * | 0 | 1 | 2 | 3 | 4 | 1
 * +---+---+---+---+---+
 * | 5 | 6 | 7 | 8 | 9 | 2
 * +---+---+---+---+---+
 * | 10| 11| 12| 13| 14| 3
 * +---+---+---+---+---+
 * | 15| 16| 17| 18| 19| 4
 * +---+---+---+---+---+
 * | 20| 21| 22| 23| 24| 5
 * +---+---+---+---+---+
 */
enum SquareEnum
{
    // clang-format off
    SQ_51 =  0, SQ_41 =  1, SQ_31 =  2, SQ_21 =  3, SQ_11 =  4,
    SQ_52 =  5, SQ_42 =  6, SQ_32 =  7, SQ_22 =  8, SQ_12 =  9,
    SQ_53 = 10, SQ_43 = 11, SQ_33 = 12, SQ_23 = 13, SQ_13 = 14,
    SQ_54 = 15, SQ_44 = 16, SQ_34 = 17, SQ_24 = 18, SQ_14 = 19,
    SQ_55 = 20, SQ_45 = 21, SQ_35 = 22, SQ_25 = 23, SQ_15 = 24,
    // clang-format on
    SQ_NA,
};
constexpr int num_squares = 25;
constexpr SquareEnum square_array[] = {
    SQ_51, SQ_41, SQ_31, SQ_21, SQ_11, SQ_52, SQ_42, SQ_32, SQ_22,
    SQ_12, SQ_53, SQ_43, SQ_33, SQ_23, SQ_13, SQ_54, SQ_44, SQ_34,
    SQ_24, SQ_14, SQ_55, SQ_45, SQ_35, SQ_25, SQ_15,
};

enum DirectionEnum
{
    // clang-format off
    DIR_NW = 0,  DIR_N = 1, DIR_NE = 2,
     DIR_W = 3,              DIR_E = 4,
    DIR_SW = 5,  DIR_S = 6, DIR_SE = 7,
    // clang-format on
};
constexpr int square_value_diffs[] = {-6, -5, -4, -1, 1, 4, 5, 6};

inline SquareEnum shift(const SquareEnum sq, const DirectionEnum d)
{
    return static_cast<SquareEnum>(
        static_cast<int>(sq) + square_value_diffs[d]);
}

enum RankEnum
{
    RANK1,
    RANK2,
    RANK3,
    RANK4,
    RANK5,
};
constexpr int num_ranks = 5;

inline constexpr RankEnum to_rank(const SquareEnum sq)
{
    return static_cast<RankEnum>(sq / 5);
}

inline constexpr bool is_edge(const RankEnum r)
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
constexpr int num_files = 5;

inline constexpr FileEnum to_file(const SquareEnum sq)
{
    return static_cast<FileEnum>(4 - sq % 5);
}

inline constexpr bool is_edge(const FileEnum f)
{
    return (f == FILE1) || (f == FILE5);
}

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_SQUARES_HPP
