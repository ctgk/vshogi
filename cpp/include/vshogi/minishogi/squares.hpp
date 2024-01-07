#ifndef VSHOGI_MINISHOGI_SQUARES_HPP
#define VSHOGI_MINISHOGI_SQUARES_HPP

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"
#include "vshogi/squares.hpp"

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
    NUM_SQ = 25,
};
enum FileEnum
{
    FILE1,
    FILE2,
    FILE3,
    FILE4,
    FILE5,
};
enum RankEnum
{
    RANK1,
    RANK2,
    RANK3,
    RANK4,
    RANK5,
};

using Squares = vshogi::Squares<5, 5, SquareEnum, FileEnum, RankEnum, 8, 8>;

} // namespace vshogi::minishogi

namespace vshogi
{

template <>
inline bool vshogi::minishogi::Squares::in_promotion_zone(
    const vshogi::minishogi::RankEnum& r, const ColorEnum& c)
{
    return (c == BLACK) ? (r == vshogi::minishogi::RANK1)
                        : (r == vshogi::minishogi::RANK5);
}

template <>
inline DirectionEnum
vshogi::minishogi::Squares::get_direction_for_diagonal_or_knight(
    const vshogi::minishogi::SquareEnum& dst,
    const vshogi::minishogi::SquareEnum& src)
{
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
        return (to_file(src) < vshogi::minishogi::FILE4) ? DIR_NW : DIR_NE;
    case -4:
    case -8:
    case -16:
        return DIR_SW;
    case -6:
    case -18:
    case -24:
        return DIR_SE;
    case -12:
        return (to_file(src) < vshogi::minishogi::FILE3) ? DIR_SW : DIR_SE;
    default:
        return DIR_NA;
    }
}

} // namespace vshogi

#endif // VSHOGI_MINISHOGI_SQUARES_HPP
