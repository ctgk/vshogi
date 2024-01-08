#ifndef VSHOGI_MINISHOGI_SQUARES_HPP
#define VSHOGI_MINISHOGI_SQUARES_HPP

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"
#include "vshogi/minishogi/piece.hpp"
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

using Squares = vshogi::Squares<5, 5, SquareEnum, FileEnum, RankEnum, 8, 8, 7>;

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

template <>
template <>
inline const vshogi::minishogi::SquareEnum*
vshogi::minishogi::Squares::get_non_ranging_attacks_by(
    const vshogi::minishogi::BoardPieceTypeEnum& p,
    const vshogi::minishogi::SquareEnum& location)
{
    switch (p) {
    case vshogi::minishogi::B_FU:
        return non_ranging_attacks_array[0][location];
    case vshogi::minishogi::B_GI:
        return non_ranging_attacks_array[1][location];
    case vshogi::minishogi::B_KI:
    case vshogi::minishogi::B_TO:
    case vshogi::minishogi::B_NG:
        return non_ranging_attacks_array[2][location];
    case vshogi::minishogi::W_FU:
        return non_ranging_attacks_array[3][location];
    case vshogi::minishogi::W_GI:
        return non_ranging_attacks_array[4][location];
    case vshogi::minishogi::W_KI:
    case vshogi::minishogi::W_TO:
    case vshogi::minishogi::W_NG:
        return non_ranging_attacks_array[5][location];
    case vshogi::minishogi::B_OU:
    case vshogi::minishogi::W_OU:
        return non_ranging_attacks_array[6][location];
    default:
        return nullptr;
    }
}

template <>
inline void vshogi::minishogi::Squares::init_non_ranging_attacks_array()
{
    constexpr int size = sizeof(non_ranging_attacks_array)
                         / sizeof(non_ranging_attacks_array[0][0][0]);
    std::fill_n(
        &non_ranging_attacks_array[0][0][0], size, vshogi::minishogi::SQ_NA);

    // B_FU, B_GI, B_KI, W_FU, W_GI, W_KI, OU
    constexpr DirectionEnum piece_to_direction[7][9] = {
        // clang-format off
        {DIR_N, DIR_NA}, // B_FU
        {DIR_NW, DIR_N, DIR_NE, DIR_SW, DIR_SE, DIR_NA}, // B_GI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S, DIR_NA}, // B_KI
        {DIR_S, DIR_NA}, // W_FU
        {DIR_NW, DIR_NE, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // W_GI
        {DIR_N, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // W_KI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // OU
        // clang-format on
    };
    for (int i = 0; i < 7; ++i) {
        for (auto&& sq : square_array) {
            int index = 0;
            for (auto&& dir : piece_to_direction[i]) {
                if (dir == DIR_NA)
                    break;
                const auto dst = shift(sq, dir);
                if (dst == SQ_NA)
                    continue;
                non_ranging_attacks_array[i][sq][index++] = dst;
            }
        }
    }
}

} // namespace vshogi

#endif // VSHOGI_MINISHOGI_SQUARES_HPP
