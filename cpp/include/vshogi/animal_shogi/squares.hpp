#ifndef VSHOGI_ANIMAL_SHOGI_SQUARES_HPP
#define VSHOGI_ANIMAL_SHOGI_SQUARES_HPP

namespace vshogi::animal_shogi
{

/**
 * @brief Enumeration of board squares.
 * @details
 *     A   B   C
 *   *---*---*---*
 * 1 | 0 | 1 | 2 |
 *   *---*---*---*
 * 2 | 3 | 4 | 5 |
 *   *---*---*---*
 * 3 | 6 | 7 | 8 |
 *   *---*---*---*
 * 4 | 9 | 10| 11|
 *   *---*---*---*
 */
enum SquareEnum
{
    // clang-format off
    SQ_A1, SQ_B1, SQ_C1,
    SQ_A2, SQ_B2, SQ_C2,
    SQ_A3, SQ_B3, SQ_C3,
    SQ_A4, SQ_B4, SQ_C4,
    // clang-format on
    NUM_SQUARES,
};

enum RankEnum
{
    RANK1,
    RANK2,
    RANK3,
    RANK4,
    NUM_RANKS,
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_SQUARES_HPP
