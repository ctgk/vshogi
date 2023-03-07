#ifndef VSHOGI_ANIMAL_SHOGI_BOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BOARD_HPP

#include <cstdint>

#include "vshogi/animal_shogi/squares.hpp"

namespace vshogi::animal_shogi
{

enum BoardPieceTypeEnum
{
    VOID = 0,
    B_CH, //!< Black Chick (Pawn)
    B_EL, //!< Black Elephant (Limited Bishop)
    B_GI, //!< Black Giraffe (Limited Rook)
    B_LI, //!< Black Lion (King)
    B_HE, //!< Black Hen (Promoted Pawn)
    W_CH, //!< White Chick (Pawn)
    W_EL, //!< White Elephant (Limited Bishop)
    W_GI, //!< White Giraffe (Limited Rook)
    W_LI, //!< White Lion (King)
    W_HE, //!< White Hen (Promoted Pawn)
    NUM_BOARD_PIECE_TYPES,
};

// /**
//  * @brief Enumeration of board piece types.
//  * @details
//  *               *------------ Color
//  *               | *---------- Promotion
//  *               | |
//  *               v v
//  * (MSB) x x x x x x x x (LSB)
//  */
// enum BoardPieceTypeEnum
// {
//     VOID = 0b00000000, //!< Void (Empty square)
//     B_EL = 0b00000001, //!< Black Elephant (Limited Bishop)
//     B_GI = 0b00000010, //!< Black Giraffe (Limited Rook)
//     B_LI = 0b00000011, //!< Black Lion (King)
//     B_CH = 0b00000100, //!< Black Chick (Pawn)
//     B_HE = 0b00000101, //!< Black Hen (Promoted Pawn)
//     W_EL = 0b00001001, //!< White Elephant (Limited Bishop)
//     W_GI = 0b00001010, //!< White Giraffe (Limited Rook)
//     W_LI = 0b00001011, //!< White Lion (King)
//     W_CH = 0b00001100, //!< White Chick (Pawn)
//     W_HE = 0b00001101, //!< White Hen (Promoted Pawn)
// };

class Board
{
private:
    BoardPieceTypeEnum m_pieces[NUM_SQUARES];

    const char* set_sfen_rank(const char* const sfen_rank, const RankEnum rank);

public:
    Board()
        : m_pieces{
            // clang-format off
            W_GI, W_LI, W_EL,
            VOID, W_CH, VOID,
            VOID, B_CH, VOID,
            B_EL, B_LI, B_GI,
            // clang-format on
        }
    {
    }

    /**
     * @brief Set pieces on the board according to given SFEN string.
     *
     * @param sfen SFEN string. e.g. "gle/1c1/1C1/ELG b - 1"
     * @return const char* Remaining SFEN string. e.g. "b - 1"
     */
    const char* set_sfen(const char* const sfen);
    BoardPieceTypeEnum get_piece_at(const SquareEnum sq)
    {
        return m_pieces[sq];
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_BOARD_HPP
