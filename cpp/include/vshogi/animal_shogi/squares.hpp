#ifndef VSHOGI_ANIMAL_SHOGI_SQUARES_HPP
#define VSHOGI_ANIMAL_SHOGI_SQUARES_HPP

#include "vshogi/direction.hpp"
#include "vshogi/squares.hpp"

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
    SQ_NA,
    NUM_SQ = 12,
};
enum FileEnum
{
    FILE_A,
    FILE_B,
    FILE_C,
};
enum RankEnum
{
    RANK1,
    RANK2,
    RANK3,
    RANK4,
};

using Squares = vshogi::Squares<3, 4, SquareEnum, FileEnum, RankEnum, 8, 8, 7>;

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
constexpr vshogi::animal_shogi::SquareEnum
vshogi::animal_shogi::Squares::to_square(
    const vshogi::animal_shogi::FileEnum& f,
    const vshogi::animal_shogi::RankEnum& r)
{
    return static_cast<vshogi::animal_shogi::SquareEnum>(r * num_files + f);
}

template <>
inline vshogi::animal_shogi::SquareEnum
vshogi::animal_shogi::Squares::to_square(const char usi[2])
{
    return to_square(
        static_cast<vshogi::animal_shogi::FileEnum>(usi[0] - 'a'),
        static_cast<vshogi::animal_shogi::RankEnum>(usi[1] - '1'));
}

template <>
inline bool vshogi::animal_shogi::Squares::in_promotion_zone(
    const vshogi::animal_shogi::RankEnum& r, const ColorEnum& c)
{
    return (c == BLACK) ? (r == vshogi::animal_shogi::RANK1)
                        : (r == vshogi::animal_shogi::RANK4);
}

template <>
inline void vshogi::animal_shogi::Squares::init_non_ranging_attacks_array()
{
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_SQUARES_HPP
