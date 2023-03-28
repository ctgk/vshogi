#ifndef VSHOGI_MINISHOGI_PIECE_HPP
#define VSHOGI_MINISHOGI_PIECE_HPP

#include <cstdint>

#include "vshogi/color.hpp"

namespace vshogi::minishogi
{

enum PieceTypeEnum : std::uint8_t
{
    FU = 0b0000, //!< Fu (Pawn)
    GI = 0b0001, //!< Gin (Silver)
    KA = 0b0010, //!< Kaku (Bishop)
    HI = 0b0011, //!< Hisha (Rook)

    KI = 0b0100, //!< Kin (Gold)
    OU = 0b0101, //!< Ou, Gyoku (King)

    TO = 0b1000, //!< Tokin (Promoted Pawn)
    NG = 0b1001, //!< Nari-Gin (Promoted Silver)
    UM = 0b1010, //!< Uma (Promoted Bishop)
    RY = 0b1011, //!< Ryu (Promoted Rook)

    NA = 0b1111, //!< Not available
};
constexpr PieceTypeEnum stand_piece_array[] = {FU, GI, KI, KA, HI};

inline bool is_promotable(const PieceTypeEnum p)
{
    return (p < 0b0100);
}

inline PieceTypeEnum promote(const PieceTypeEnum p)
{
    return is_promotable(p) ? static_cast<PieceTypeEnum>(p | 0b1000) : p;
}

inline PieceTypeEnum unpromote(const PieceTypeEnum p)
{
    return (p == NA) ? NA : static_cast<PieceTypeEnum>(p & 0b0111);
}

enum BoardPieceTypeEnum : std::uint8_t
{
    B_FU = 0b00000, //!< Black Fu (Pawn)
    B_GI = 0b00001, //!< Black Gin (Silver)
    B_KA = 0b00010, //!< Black Kaku (Bishop)
    B_HI = 0b00011, //!< Black Hisha (Rook)
    B_KI = 0b00100, //!< Black Kin (Gold)
    B_OU = 0b00101, //!< Black Ou, Gyoku (King)
    B_TO = 0b01000, //!< Black Tokin (Promoted Pawn)
    B_NG = 0b01001, //!< Black Nari-Gin (Promoted Silver)
    B_UM = 0b01010, //!< Black Uma (Promoted Bishop)
    B_RY = 0b01011, //!< Black Ryu (Promoted Rook)

    W_FU = 0b10000, //!< White Fu (Pawn)
    W_GI = 0b10001, //!< White Gin (Silver)
    W_KA = 0b10010, //!< White Kaku (Bishop)
    W_HI = 0b10011, //!< White Hisha (Rook)
    W_KI = 0b10100, //!< White Kin (Gold)
    W_OU = 0b10101, //!< White Ou, Gyoku (King)
    W_TO = 0b11000, //!< White Tokin (Promoted Pawn)
    W_NG = 0b11001, //!< White Nari-Gin (Promoted Silver)
    W_UM = 0b11010, //!< White Uma (Promoted Bishop)
    W_RY = 0b11011, //!< White Ryu (Promoted Rook)

    VOID = 0b11111, //!< Empty Square.
};

/**
 * @brief Get the color of the board piece. Note that passing VOID returns WHITE.
 *
 * @param p Board piece.
 * @return ColorEnum Color of the board piece.
 */
inline constexpr ColorEnum get_color(const BoardPieceTypeEnum p)
{
    return static_cast<ColorEnum>(p >> 4);
}

inline constexpr PieceTypeEnum to_piece_type(const BoardPieceTypeEnum p)
{
    return static_cast<PieceTypeEnum>(p & 0x0f);
}

inline constexpr BoardPieceTypeEnum
to_board_piece(const ColorEnum c, const PieceTypeEnum p)
{
    if (p == NA)
        return VOID;
    return static_cast<BoardPieceTypeEnum>((c << 4) | p);
}

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_PIECE_HPP
