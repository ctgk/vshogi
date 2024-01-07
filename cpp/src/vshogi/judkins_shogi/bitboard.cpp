#include "vshogi/judkins_shogi/bitboard.hpp"

namespace vshogi::judkins_shogi
{

/**
 * @brief Table of attacks by non-ranging pieces:
 * B_FU, B_KE, B_GI, B_KI, W_FU, W_KE, W_GI, W_KI, and OU.
 */
static SquareEnum non_ranging_attacks[9][Squares::num_squares][8];

static SquareEnum ranging_squares_to[Squares::num_squares]
                                    [Squares::num_directions][6];

void init_non_ranging_attacks_table()
{
    constexpr int size
        = sizeof(non_ranging_attacks) / sizeof(non_ranging_attacks[0][0][0]);
    std::fill_n(&non_ranging_attacks[0][0][0], size, Squares::SQ_NA);

    constexpr BoardPieceTypeEnum non_ranging_pieces[]
        = {B_FU, B_KE, B_GI, B_KI, W_FU, W_KE, W_GI, W_KI, B_OU};
    constexpr int num
        = sizeof(non_ranging_pieces) / sizeof(non_ranging_pieces[0]);
    for (int ii = 0; ii < num; ++ii) {
        const auto p = non_ranging_pieces[ii];
        for (auto& src : Squares::square_array) {
            const auto attacks = BitBoard::get_attacks_by(p, src);
            int index = 0;
            for (auto& dst : Squares::square_array) {
                if (attacks.is_one(dst))
                    non_ranging_attacks[ii][src][index++] = dst;
            }
        }
    }
}

void init_ranging_squares_table()
{
    constexpr int size
        = sizeof(ranging_squares_to) / sizeof(ranging_squares_to[0][0][0]);
    std::fill_n(&ranging_squares_to[0][0][0], size, Squares::SQ_NA);

    for (auto& src : Squares::square_array) {
        for (auto& dir : Squares::direction_array) {
            auto dst = src;
            int index = 0;
            while (true) {
                dst = Squares::shift(dst, dir);
                if (dst == Squares::SQ_NA)
                    break;
                ranging_squares_to[src][dir][index++] = dst;
                if (dir > DIR_SE)
                    break;
            }
        }
    }
}

const SquareEnum* BitBoard::get_attacks_by_non_ranging(
    const BoardPieceTypeEnum& piece, const SquareEnum& location)
{
    switch (piece) {
    case B_FU:
        return non_ranging_attacks[0][location];
    case B_KE:
        return non_ranging_attacks[1][location];
    case B_GI:
        return non_ranging_attacks[2][location];
    case B_KI:
    case B_TO:
    case B_NK:
    case B_NG:
        return non_ranging_attacks[3][location];
    case W_FU:
        return non_ranging_attacks[4][location];
    case W_KE:
        return non_ranging_attacks[5][location];
    case W_GI:
        return non_ranging_attacks[6][location];
    case W_KI:
    case W_TO:
    case W_NK:
    case W_NG:
        return non_ranging_attacks[7][location];
    case B_OU:
    case W_OU:
        return non_ranging_attacks[8][location];
    default:
        return nullptr;
    }
}

const SquareEnum* BitBoard::get_squares_along(
    const DirectionEnum& direction, const SquareEnum& location)
{
    if ((direction == DIR_NA) || (location == Squares::SQ_NA))
        return nullptr;
    return ranging_squares_to[location][direction];
}

} // namespace vshogi::judkins_shogi
