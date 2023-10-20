#ifndef VSHOGI_ANIMAL_SHOGI_PIECE_HPP
#define VSHOGI_ANIMAL_SHOGI_PIECE_HPP

#include <cstdint>
#include <string>

#include "vshogi/color.hpp"

namespace vshogi::animal_shogi
{

struct Pieces
{
    Pieces() = delete;
    enum PieceTypeEnum : std::uint8_t
    {
        CH = 0b0000, //!< Chick (Pawn)
        EL = 0b0001, //!< Elephant (Limited Bishop)
        GI = 0b0010, //!< Giraffe (Limited Rook)
        LI = 0b0011, //!< Lion (King)
        HE = 0b0100, //!< Hen (Promoted Pawn)
        NA = 0b0111, // Not available.

        OU = LI,
    };
    static constexpr PieceTypeEnum stand_piece_array[] = {CH, EL, GI};
    static constexpr int num_stand_piece_types
        = sizeof(stand_piece_array) / sizeof(stand_piece_array[0]);
    static constexpr PieceTypeEnum piece_array[] = {CH, EL, GI, LI, HE};

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

    /**
 * @brief Return color of the board piece. Note that VOID returns WHITE.
 *
 * @param p Board piece
 * @return ColorEnum Color of the piece.
 */
    static inline ColorEnum get_color(const BoardPieceTypeEnum p)
    {
        return static_cast<ColorEnum>(p >> 3);
    }

    static inline PieceTypeEnum to_piece_type(const BoardPieceTypeEnum p)
    {
        return static_cast<PieceTypeEnum>(p & 0x07);
    }

    static inline BoardPieceTypeEnum
    to_board_piece(const ColorEnum c, const PieceTypeEnum p)
    {
        if (p == NA)
            return VOID;
        return static_cast<BoardPieceTypeEnum>((c << 3) | p);
    }

    static void append_sfen(const BoardPieceTypeEnum p, std::string& out)
    {
        switch (p) {
        case B_CH:
            out += 'C';
            break;
        case B_EL:
            out += 'E';
            break;
        case B_GI:
            out += 'G';
            break;
        case B_LI:
            out += 'L';
            break;
        case B_HE:
            out += 'H';
            break;
        case W_CH:
            out += 'c';
            break;
        case W_EL:
            out += 'e';
            break;
        case W_GI:
            out += 'g';
            break;
        case W_LI:
            out += 'l';
            break;
        case W_HE:
            out += 'h';
            break;
        default:
            break;
        }
    }
};

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_PIECE_HPP
