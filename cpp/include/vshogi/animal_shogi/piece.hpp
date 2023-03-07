#ifndef VSHOGI_ANIMAL_SHOGI_PIECE_HPP
#define VSHOGI_ANIMAL_SHOGI_PIECE_HPP

#include <cstdint>

#include "vshogi/animal_shogi/color.hpp"

namespace vshogi::animal_shogi
{

enum PieceTypeEnum : std::uint8_t
{
    CH = 0b0000, //!< Chick (Pawn)
    EL = 0b0001, //!< Elephant (Limited Bishop)
    GI = 0b0010, //!< Giraffe (Limited Rook)
    LI = 0b0011, //!< Lion (King)
    HE = 0b0100, //!< Hen (Promoted Pawn)
    NA = 0b0111, // Not available.
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
    VOID = 0b1111, //!< Empty square.
};

enum CapturedPieceTypeEnum : std::uint8_t
{
    C_CH = 0, // Captured Chick (Pawn)
    C_EL = 1, // Captured Elephant (limited Bishop)
    C_GI = 2, // Captured Giraffe (limited Rook)
    C_NA = 3, // Not available.
};

/**
 * @brief Return color of the board piece. Note that VOID returns WHITE.
 *
 * @param p Board piece
 * @return ColorEnum Color of the piece.
 */
inline ColorEnum to_color(const BoardPieceTypeEnum p)
{
    return static_cast<ColorEnum>(p >> 3);
}

inline PieceTypeEnum to_piece_type(const BoardPieceTypeEnum p)
{
    return static_cast<PieceTypeEnum>(p & 0x07);
}

inline BoardPieceTypeEnum
to_board_piece(const ColorEnum c, const PieceTypeEnum p)
{
    if (p == NA)
        return VOID;
    return static_cast<BoardPieceTypeEnum>((c << 3) | p);
}

inline BoardPieceTypeEnum
to_board_piece(const ColorEnum c, const CapturedPieceTypeEnum p)
{
    if (p == C_NA)
        return VOID;
    return static_cast<BoardPieceTypeEnum>((c << 3) | p);
}

inline CapturedPieceTypeEnum to_captured(const PieceTypeEnum p)
{
    return static_cast<CapturedPieceTypeEnum>(p & 0x3);
}

inline CapturedPieceTypeEnum to_captured(const BoardPieceTypeEnum p)
{
    return static_cast<CapturedPieceTypeEnum>(p & 0x3);
}

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_PIECE_HPP
