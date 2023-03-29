#ifndef VSHOGI_MINISHOGI_SQUARES_HPP
#define VSHOGI_MINISHOGI_SQUARES_HPP

namespace vshogi::minishogi
{

/**
 * @brief Enumeration of board squares.
 * @details
 *   5   4   3   2   1
 * +---+---+---+---+---+
 * | 20| 15| 10| 5 | 0 | 1
 * +---+---+---+---+---+
 * | 21| 16| 11| 6 | 1 | 2
 * +---+---+---+---+---+
 * | 22| 17| 12| 7 | 2 | 3
 * +---+---+---+---+---+
 * | 23| 18| 13| 8 | 3 | 4
 * +---+---+---+---+---+
 * | 24| 19| 14| 9 | 4 | 5
 * +---+---+---+---+---+
 */
enum SquareEnum
{
    // clang-format off
    SQ_51 = 20, SQ_41 = 15, SQ_31 = 10, SQ_21 = 5, SQ_11 = 0,
    SQ_52 = 21, SQ_42 = 16, SQ_32 = 11, SQ_22 = 6, SQ_12 = 1,
    SQ_53 = 22, SQ_43 = 17, SQ_33 = 12, SQ_23 = 7, SQ_13 = 2,
    SQ_54 = 23, SQ_44 = 18, SQ_34 = 13, SQ_24 = 8, SQ_14 = 3,
    SQ_55 = 24, SQ_45 = 19, SQ_35 = 14, SQ_25 = 9, SQ_15 = 4,
    // clang-format on
};
constexpr int num_squares = 25;
constexpr SquareEnum square_array[] = {
    SQ_11, SQ_12, SQ_13, SQ_14, SQ_15, SQ_21, SQ_22, SQ_23, SQ_24,
    SQ_25, SQ_31, SQ_32, SQ_33, SQ_34, SQ_35, SQ_41, SQ_42, SQ_43,
    SQ_44, SQ_45, SQ_51, SQ_52, SQ_53, SQ_54, SQ_55,
};

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
    return static_cast<RankEnum>(sq % 5);
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
    return static_cast<FileEnum>(sq / 5);
}

inline constexpr bool is_edge(const FileEnum f)
{
    return (f == FILE1) || (f == FILE5);
}

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_SQUARES_HPP
