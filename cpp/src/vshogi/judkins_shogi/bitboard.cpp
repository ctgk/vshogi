#include "vshogi/judkins_shogi/bitboard.hpp"

namespace vshogi::judkins_shogi
{

/**
 * @brief Table of attacks by non-ranging pieces:
 * B_FU, B_KE, B_GI, B_KI, W_FU, W_KE, W_GI, W_KI, and OU.
 */
static Squares::SquareEnum non_ranging_attacks[9][Squares::num_squares][8];

void init_non_ranging_attacks_table()
{
    constexpr int size
        = sizeof(non_ranging_attacks) / sizeof(non_ranging_attacks[0][0][0]);
    std::fill_n(&non_ranging_attacks[0][0][0], size, Squares::SQ_NA);

    constexpr Pieces::BoardPieceTypeEnum non_ranging_pieces[]
        = {Pieces::B_FU,
           Pieces::B_KE,
           Pieces::B_GI,
           Pieces::B_KI,
           Pieces::W_FU,
           Pieces::W_KE,
           Pieces::W_GI,
           Pieces::W_KI,
           Pieces::B_OU};
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

const Squares::SquareEnum* BitBoard::get_attacks_by_non_ranging(
    const Pieces::BoardPieceTypeEnum& piece,
    const Squares::SquareEnum& location)
{
    switch (piece) {
    case Pieces::B_FU:
        return non_ranging_attacks[0][location];
    case Pieces::B_KE:
        return non_ranging_attacks[1][location];
    case Pieces::B_GI:
        return non_ranging_attacks[2][location];
    case Pieces::B_KI:
    case Pieces::B_TO:
    case Pieces::B_NK:
    case Pieces::B_NG:
        return non_ranging_attacks[3][location];
    case Pieces::W_FU:
        return non_ranging_attacks[4][location];
    case Pieces::W_KE:
        return non_ranging_attacks[5][location];
    case Pieces::W_GI:
        return non_ranging_attacks[6][location];
    case Pieces::W_KI:
    case Pieces::W_TO:
    case Pieces::W_NK:
    case Pieces::W_NG:
        return non_ranging_attacks[7][location];
    case Pieces::B_OU:
    case Pieces::W_OU:
        return non_ranging_attacks[8][location];
    default:
        return nullptr;
    }
}

} // namespace vshogi::judkins_shogi
