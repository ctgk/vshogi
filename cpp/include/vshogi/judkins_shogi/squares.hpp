#ifndef VSHOGI_JUDKINS_SHOGI_SQUARES_HPP
#define VSHOGI_JUDKINS_SHOGI_SQUARES_HPP

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"
#include "vshogi/squares.hpp"

namespace vshogi::judkins_shogi
{

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
enum RankEnum
{
    RANK1,
    RANK2,
    RANK3,
    RANK4,
    RANK5,
    RANK6,
};
enum FileEnum
{
    FILE1,
    FILE2,
    FILE3,
    FILE4,
    FILE5,
    FILE6,
};

using Squares = vshogi::Squares<6, 6, SquareEnum, FileEnum, RankEnum, 12, 10>;

} // namespace vshogi::judkins_shogi

namespace vshogi
{

template <>
inline bool vshogi::judkins_shogi::Squares::in_promotion_zone(
    const vshogi::judkins_shogi::RankEnum& r, const ColorEnum& c)
{
    return (c == BLACK) ? (r <= vshogi::judkins_shogi::RANK2)
                        : (r >= vshogi::judkins_shogi::RANK5);
}

template <>
inline DirectionEnum
vshogi::judkins_shogi::Squares::get_direction_for_diagonal_or_knight(
    const SquareEnum& dst, const SquareEnum& src)
{
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
    case 11:
        return DIR_NNE;
    case 13:
        return DIR_NNW;
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

} // namespace vshogi

#endif // VSHOGI_JUDKINS_SHOGI_SQUARES_HPP
