#ifndef VSHOGI_ANIMAL_SHOGI_PIECE_HPP
#define VSHOGI_ANIMAL_SHOGI_PIECE_HPP

#include <cstdint>

namespace vshogi::animal_shogi
{

enum PieceTypeEnum : std::uint8_t
{
    CH = 0b0000, //!< Chick (Pawn)
    EL = 0b0001, //!< Elephant (Limited Bishop)
    GI = 0b0010, //!< Giraffe (Limited Rook)
    LI = 0b0011, //!< Lion (King)
    HE = 0b0100, //!< Hen (Promoted Pawn)
    NA = 0xff, // Not available.
};

/**
 * @brief Enumeration of pieces on board (plus VOID).
 * @details
 *            *--------- Color
 *            |*-------- Promotion
 *            ||
 *            vv
 * (MSB) xxxx xxxx (LSB)
 *
 */
enum BoardPieceTypeEnum : std::uint8_t
{
    B_CH = 0b0000, //!< Black Chick (Pawn)
    B_EL = 0b0001, //!< Black Elephant (Limited Bishop)
    B_GI = 0b0010, //!< Black Giraffe (Limited Rook)
    B_LI = 0b0011, //!< Black Lion (King)
    B_HE = 0b0100, //!< Black Hen (Promoted Pawn)
    W_CH = 0b1000, //!< White Chick (Pawn)
    W_EL = 0b1001, //!< White Elephant (Limited Bishop)
    W_GI = 0b1010, //!< White Giraffe (Limited Rook)
    W_LI = 0b1011, //!< White Lion (King)
    W_HE = 0b1100, //!< White Hen (Promoted Pawn)
    VOID = 0xff,
};

enum CapturedPieceTypeEnum : std::uint8_t
{
    C_CH = 0, // Captured Chick (Pawn)
    C_EL = 1, // Captured Elephant (limited Bishop)
    C_GI = 2, // Captured Giraffe (limited Rook)
    C_NA = 0xff, // Not available.
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_PIECE_HPP
