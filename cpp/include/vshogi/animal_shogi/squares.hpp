#ifndef VSHOGI_ANIMAL_SHOGI_SQUARES_HPP
#define VSHOGI_ANIMAL_SHOGI_SQUARES_HPP

#include "vshogi/direction.hpp"

namespace vshogi::animal_shogi
{

struct Squares
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
        SQ_A1, //!< First square.
        SQ_B1, SQ_C1,
        SQ_A2, SQ_B2, SQ_C2,
        SQ_A3, SQ_B3, SQ_C3,
        SQ_A4, SQ_B4,
        SQ_C4, //!< Last square.
        // clang-format on
        SQ_NA,
        NUM_SQ = 12,
    };
    static constexpr int num_squares = 12;
    static constexpr SquareEnum square_array[] = {
        SQ_A1,
        SQ_B1,
        SQ_C1,
        SQ_A2,
        SQ_B2,
        SQ_C2,
        SQ_A3,
        SQ_B3,
        SQ_C3,
        SQ_A4,
        SQ_B4,
        SQ_C4,
    };

    enum RankEnum
    {
        RANK1,
        RANK2,
        RANK3,
        RANK4,
    };
    static constexpr int num_ranks = 4;

    static inline RankEnum to_rank(const SquareEnum sq)
    {
        constexpr RankEnum table[] = {
            // clang-format off
        RANK1, RANK1, RANK1,
        RANK2, RANK2, RANK2,
        RANK3, RANK3, RANK3,
        RANK4, RANK4, RANK4,
            // clang-format on
        };
        return table[sq];
    }

    enum FileEnum
    {
        FILE_A,
        FILE_B,
        FILE_C,
    };
    static constexpr int num_files = 3;
    static inline FileEnum to_file(const SquareEnum sq)
    {
        constexpr FileEnum table[] = {
            // clang-format off
        FILE_A, FILE_B, FILE_C,
        FILE_A, FILE_B, FILE_C,
        FILE_A, FILE_B, FILE_C,
        FILE_A, FILE_B, FILE_C,
            // clang-format on
        };
        return table[sq];
    }

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
        constexpr int table[] = {-4, -3, -2, -1, 1, 2, 3, 4};
        return table[d];
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_SQUARES_HPP
