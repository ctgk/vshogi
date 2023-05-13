#ifndef VSHOGI_MINISHOGI_SQUARES_HPP
#define VSHOGI_MINISHOGI_SQUARES_HPP

#include "vshogi/direction.hpp"

namespace vshogi::minishogi
{

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
};
constexpr int num_squares = 25;
constexpr SquareEnum square_array[] = {
    SQ_5A, SQ_4A, SQ_3A, SQ_2A, SQ_1A, SQ_5B, SQ_4B, SQ_3B, SQ_2B,
    SQ_1B, SQ_5C, SQ_4C, SQ_3C, SQ_2C, SQ_1C, SQ_5D, SQ_4D, SQ_3D,
    SQ_2D, SQ_1D, SQ_5E, SQ_4E, SQ_3E, SQ_2E, SQ_1E,
};

constexpr int direction_to_delta(const DirectionEnum d)
{
    constexpr int table[] = {-6, -5, -4, -1, 1, 4, 5, 6};
    return table[d];
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

inline SquareEnum shift(const SquareEnum sq, const DirectionEnum d)
{
    const auto r = to_rank(sq);
    const auto f = to_file(sq);
    if (r == RANK1 && (d == DIR_NW || d == DIR_N || d == DIR_NE))
        return sq;
    if (r == RANK5 && (d == DIR_SW || d == DIR_S || d == DIR_SE))
        return sq;
    if (f == FILE1 && (d == DIR_NE || d == DIR_E || d == DIR_SE))
        return sq;
    if (f == FILE5 && (d == DIR_NW || d == DIR_W || d == DIR_SW))
        return sq;
    return static_cast<SquareEnum>(
        static_cast<int>(sq) + direction_to_delta(d));
}

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_SQUARES_HPP
