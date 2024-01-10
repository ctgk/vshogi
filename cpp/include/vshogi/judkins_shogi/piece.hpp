#ifndef VSHOGI_JUDKINS_SHOGI_PIECE_HPP
#define VSHOGI_JUDKINS_SHOGI_PIECE_HPP

#include <cstdint>
#include <string>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"
#include "vshogi/pieces.hpp"

namespace vshogi::judkins_shogi
{

enum PieceTypeEnum : std::uint8_t
{
    FU = 0b0000, //!< Fu (Pawn)
    KE = 0b0001, //!< Kei (Knight)
    GI = 0b0010, //!< Gin (Silver)
    KA = 0b0011, //!< Kaku (Bishop)
    HI = 0b0100, //!< Hisha (Rook)
    KI = 0b0101, //!< Kin (Gold)
    OU = 0b0111, //!< Ou, Gyoku (King)

    TO = 0b1000 + FU, //!< Tokin (Promoted Pawn)
    NK = 0b1000 + KE, //!< Nari-Kei (Promoted Knight)
    NG = 0b1000 + GI, //!< Nari-Gin (Promoted Silver)
    UM = 0b1000 + KA, //!< Uma (Promoted Bishop)
    RY = 0b1000 + HI, //!< Ryu (Promoted Rook)

    NA = 0b1111, //!< NA
};

enum BoardPieceTypeEnum : std::uint8_t
{
    B_FU = FU, //!< Black Fu (Pawn)
    B_KE = KE, //!< Black Kei (Knight)
    B_GI = GI, //!< Black Gin (Silver)
    B_KA = KA, //!< Black Kaku (Bishop)
    B_HI = HI, //!< Black Hisha (Rook)
    B_KI = KI, //!< Black Kin (Gold)
    B_OU = OU, //!< Black Ou, Gyoku (King)
    B_TO = TO, //!< Black Tokin (Promoted Pawn)
    B_NK = NK, //!< Black Nari-Kei (Promoted Knight)
    B_NG = NG, //!< Black Nari-Gin (Promoted Silver)
    B_UM = UM, //!< Black Uma (Promoted Bishop)
    B_RY = RY, //!< Black Ryu (Promoted Rook)

    W_FU = 0b10000 + B_FU, //!< White Fu (Pawn)
    W_KE = 0b10000 + B_KE, //!< White Kei (Knight)
    W_GI = 0b10000 + B_GI, //!< White Gin (Silver)
    W_KA = 0b10000 + B_KA, //!< White Kaku (Bishop)
    W_HI = 0b10000 + B_HI, //!< White Hisha (Rook)
    W_KI = 0b10000 + B_KI, //!< White Kin (Gold)
    W_OU = 0b10000 + B_OU, //!< White Ou, Gyoku (King)
    W_TO = 0b10000 + B_TO, //!< White Tokin (Promoted Pawn)
    W_NK = 0b10000 + B_NK, //!< White Nari-Kei (Promoted Knight)
    W_NG = 0b10000 + B_NG, //!< White Nari-Gin (Promoted Silver)
    W_UM = 0b10000 + B_UM, //!< White Uma (Promoted Bishop)
    W_RY = 0b10000 + B_RY, //!< White Ryu (Promoted Rook)

    VOID = 0b11111, //!< Empty Square
};

using Pieces = vshogi::Pieces<PieceTypeEnum, BoardPieceTypeEnum, 12, 6, 3>;
static_assert(FU == Pieces::FU);
static_assert(OU == Pieces::OU);
static_assert(NA == Pieces::NA);
static_assert(B_OU == Pieces::B_OU);
static_assert(W_OU == Pieces::W_OU);
static_assert(VOID == Pieces::VOID);

} // namespace vshogi::judkins_shogi

namespace vshogi
{

template <>
inline const vshogi::judkins_shogi::PieceTypeEnum
    vshogi::judkins_shogi::Pieces::piece_array[]
    = {vshogi::judkins_shogi::FU,
       vshogi::judkins_shogi::KE,
       vshogi::judkins_shogi::GI,
       vshogi::judkins_shogi::KA,
       vshogi::judkins_shogi::HI,
       vshogi::judkins_shogi::KI,
       vshogi::judkins_shogi::OU,
       vshogi::judkins_shogi::TO,
       vshogi::judkins_shogi::NK,
       vshogi::judkins_shogi::NG,
       vshogi::judkins_shogi::UM,
       vshogi::judkins_shogi::RY};

template <>
inline const vshogi::judkins_shogi::PieceTypeEnum
    vshogi::judkins_shogi::Pieces::stand_piece_array[]
    = {vshogi::judkins_shogi::FU,
       vshogi::judkins_shogi::KE,
       vshogi::judkins_shogi::GI,
       vshogi::judkins_shogi::KA,
       vshogi::judkins_shogi::HI,
       vshogi::judkins_shogi::KI};

template <>
inline vshogi::judkins_shogi::PieceTypeEnum
vshogi::judkins_shogi::Pieces::to_piece_type(const char c)
{
    using namespace vshogi::judkins_shogi;
    switch (std::tolower(c)) {
    case 'p':
        return FU;
    case 'n':
        return KE;
    case 's':
        return GI;
    case 'b':
        return KA;
    case 'r':
        return HI;
    case 'g':
        return KI;
    case 'k':
        return OU;
    default:
        return NA;
    }
}

template <>
inline char vshogi::judkins_shogi::Pieces::to_char(
    const vshogi::judkins_shogi::PieceTypeEnum& p)
{
    using namespace vshogi::judkins_shogi;
    switch (p) {
    case FU:
        return 'p';
    case KE:
        return 'n';
    case GI:
        return 's';
    case KA:
        return 'b';
    case HI:
        return 'r';
    case KI:
        return 'g';
    case OU:
        return 'k';
    default:
        return '\0';
    }
}

template <>
inline constexpr bool vshogi::judkins_shogi::Pieces::is_promotable(
    const vshogi::judkins_shogi::PieceTypeEnum& p)
{
    return (p < vshogi::judkins_shogi::KI);
}

template <>
inline bool vshogi::judkins_shogi::Pieces::is_ranging_to(
    const vshogi::judkins_shogi::BoardPieceTypeEnum& p, const DirectionEnum& d)
{
    using namespace vshogi::judkins_shogi;
    const auto base = demote(to_piece_type(p));
    if (base == HI)
        return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
    if (base == KA)
        return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW) || (d == DIR_SE);
    return false;
}

template <>
inline int vshogi::judkins_shogi::Pieces::get_point(
    const vshogi::judkins_shogi::PieceTypeEnum& p)
{
    using namespace vshogi::judkins_shogi;
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

} // namespace vshogi

#endif // VSHOGI_JUDKINS_SHOGI_PIECE_HPP
