#ifndef VSHOGI_ANIMAL_SHOGI_PIECE_HPP
#define VSHOGI_ANIMAL_SHOGI_PIECE_HPP

namespace vshogi::animal_shogi
{

enum PieceTypeEnum
{
    CH, //!< Chick (Pawn)
    EL, //!< Elephant (Limited Bishop)
    GI, //!< Giraffe (Limited Rook)
    LI, //!< Lion (King)
    HE, //!< Hen (Promoted Pawn)
};

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

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_PIECE_HPP
