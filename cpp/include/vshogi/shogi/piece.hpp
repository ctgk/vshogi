#ifndef VSHOGI_SHOGI_PIECE_HPP
#define VSHOGI_SHOGI_PIECE_HPP

#include <cctype>
#include <cstdint>
#include <string>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"
#include "vshogi/pieces.hpp"

namespace vshogi::shogi
{

enum PieceTypeEnum : std::uint8_t
{
    FU = 0b0000, //!< Fu (Pawn)
    KY = 0b0001, //!< Kyo (Lance)
    KE = 0b0010, //!< Kei (Knight)
    GI = 0b0011, //!< Gin (Silver)
    KA = 0b0100, //!< Kaku (Bishop)
    HI = 0b0101, //!< Hisha (Rook)
    KI = 0b0110, //!< Kin (Gold)
    OU = 0b0111, //!< Ou, Gyoku (King)

    TO = 0b1000, //!< Tokin (Promoted Pawn)
    NY = 0b1001, //!< Nari-Kyo (Promoted Lance)
    NK = 0b1010, //!< Nari-Kei (Promoted Knight)
    NG = 0b1011, //!< Nari-Gin (Promoted Silver)
    UM = 0b1100, //!< Uma (Promoted Bishop)
    RY = 0b1101, //!< Ryu (Promoted Rook)

    NA = 0b1111, //!< NA
};

enum BoardPieceTypeEnum : std::uint8_t
{
    B_FU = 0b00000, //!< Black Fu (Pawn)
    B_KY = 0b00001, //!< Black Kyo (Lance)
    B_KE = 0b00010, //!< Black Kei (Knight)
    B_GI = 0b00011, //!< Black Gin (Silver)
    B_KA = 0b00100, //!< Black Kaku (Bishop)
    B_HI = 0b00101, //!< Black Hisha (Rook)
    B_KI = 0b00110, //!< Black Kin (Gold)
    B_OU = 0b00111, //!< Black Ou, Gyoku (King)
    B_TO = 0b01000, //!< Black Tokin (Promoted Pawn)
    B_NY = 0b01001, //!< Black Nari-Kyo (Promoted Lance)
    B_NK = 0b01010, //!< Black Nari-Kei (Promoted Knight)
    B_NG = 0b01011, //!< Black Nari-Gin (Promoted Silver)
    B_UM = 0b01100, //!< Black Uma (Promoted Bishop)
    B_RY = 0b01101, //!< Black Ryu (Promoted Rook)

    W_FU = 0b10000, //!< White Fu (Pawn)
    W_KY = 0b10001, //!< White Kyo (Lance)
    W_KE = 0b10010, //!< White Kei (Knight)
    W_GI = 0b10011, //!< White Gin (Silver)
    W_KA = 0b10100, //!< White Kaku (Bishop)
    W_HI = 0b10101, //!< White Hisha (Rook)
    W_KI = 0b10110, //!< White Kin (Gold)
    W_OU = 0b10111, //!< White Ou, Gyoku (King)
    W_TO = 0b11000, //!< White Tokin (Promoted Pawn)
    W_NY = 0b11001, //!< White Nari-Kyo (Promoted Lance)
    W_NK = 0b11010, //!< White Nari-Kei (Promoted Knight)
    W_NG = 0b11011, //!< White Nari-Gin (Promoted Silver)
    W_UM = 0b11100, //!< White Uma (Promoted Bishop)
    W_RY = 0b11101, //!< White Ryu (Promoted Rook)

    VOID = 0b11111, //!< Empty Square
};

using Pieces = vshogi::Pieces<PieceTypeEnum, BoardPieceTypeEnum, 14, 7, 3>;
static_assert(FU == Pieces::FU);
static_assert(OU == Pieces::OU);
static_assert(NA == Pieces::NA);
static_assert(B_OU == Pieces::B_OU);
static_assert(W_OU == Pieces::W_OU);
static_assert(VOID == Pieces::VOID);

} // namespace vshogi::shogi

namespace vshogi
{

template <>
inline const vshogi::shogi::PieceTypeEnum vshogi::shogi::Pieces::piece_array[]
    = {vshogi::shogi::FU,
       vshogi::shogi::KY,
       vshogi::shogi::KE,
       vshogi::shogi::GI,
       vshogi::shogi::KA,
       vshogi::shogi::HI,
       vshogi::shogi::KI,
       vshogi::shogi::OU,
       vshogi::shogi::TO,
       vshogi::shogi::NY,
       vshogi::shogi::NK,
       vshogi::shogi::NG,
       vshogi::shogi::UM,
       vshogi::shogi::RY};

template <>
inline const vshogi::shogi::PieceTypeEnum
    vshogi::shogi::Pieces::stand_piece_array[]
    = {vshogi::shogi::FU,
       vshogi::shogi::KY,
       vshogi::shogi::KE,
       vshogi::shogi::GI,
       vshogi::shogi::KA,
       vshogi::shogi::HI,
       vshogi::shogi::KI};

template <>
inline vshogi::shogi::PieceTypeEnum
vshogi::shogi::Pieces::to_piece_type(const char c)
{
    switch (std::tolower(c)) {
    case 'p':
        return vshogi::shogi::FU;
    case 'l':
        return vshogi::shogi::KY;
    case 'n':
        return vshogi::shogi::KE;
    case 's':
        return vshogi::shogi::GI;
    case 'b':
        return vshogi::shogi::KA;
    case 'r':
        return vshogi::shogi::HI;
    case 'g':
        return vshogi::shogi::KI;
    case 'k':
        return vshogi::shogi::OU;
    default:
        return vshogi::shogi::NA;
    }
}

template <>
inline constexpr bool
vshogi::shogi::Pieces::is_promotable(const vshogi::shogi::PieceTypeEnum& p)
{
    return (p < vshogi::shogi::KI);
}

template <>
inline bool vshogi::shogi::Pieces::is_ranging_to(
    const vshogi::shogi::BoardPieceTypeEnum& p, const DirectionEnum& d)
{
    using namespace vshogi::shogi;
    if (p == B_KY)
        return (d == DIR_N);
    if (p == W_KY)
        return (d == DIR_S);
    const auto base = demote(to_piece_type(p));
    if (base == HI)
        return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
    if (base == KA)
        return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW) || (d == DIR_SE);
    return false;
}

template <>
inline int
vshogi::shogi::Pieces::get_point(const vshogi::shogi::PieceTypeEnum& p)
{
    using namespace vshogi::shogi;
    switch (demote(p)) {
    case OU:
        return 0;
    case KA:
    case HI:
        return 5;
    default:
        return 1;
    }
}

template <>
inline void vshogi::shogi::Pieces::append_sfen(
    const vshogi::shogi::BoardPieceTypeEnum& p, std::string& out)
{
    using namespace vshogi::shogi;
    const auto color = get_color(p);
    const auto promotion = is_promoted(p);
    const auto pt = demote(to_piece_type(p));
    char c;
    switch (pt) {
    case FU:
        c = 'p';
        break;
    case KY:
        c = 'l';
        break;
    case KE:
        c = 'n';
        break;
    case GI:
        c = 's';
        break;
    case KA:
        c = 'b';
        break;
    case HI:
        c = 'r';
        break;
    case KI:
        c = 'g';
        break;
    case OU:
        c = 'k';
        break;
    default:
        c = ' ';
        break;
    }
    if (color == BLACK)
        c = static_cast<char>(std::toupper(static_cast<int>(c)));
    if (promotion)
        out += '+';
    out += c;
}

} // namespace vshogi

#endif // VSHOGI_SHOGI_PIECE_HPP
