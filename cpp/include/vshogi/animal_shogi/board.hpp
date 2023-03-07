#ifndef VSHOGI_ANIMAL_SHOGI_BOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BOARD_HPP

#include <cstdint>

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
enum BoardSquareEnum
{
    // clang-format off
    A1, B1, C1,
    A2, B2, C2,
    A3, B3, C3,
    A4, B4, C4,
    // clang-format on
    NUM_BOARD_SQUARES,
};

class Board
{
public:
    constexpr static std::uint8_t num_squares = 12;

private:
    BoardPieceTypeEnum m_pieces[num_squares];

public:
    Board()
        : m_pieces{
            // clang-format off
            W_GI, W_LI, W_EL,
            VOID, W_CH, VOID,
            VOID, B_CH, VOID,
            B_EL, B_LI, B_GI
            // clang-format on
        }
    {
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_BOARD_HPP
