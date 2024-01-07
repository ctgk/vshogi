#ifndef VSHOGI_MINISHOGI_PIECE_HPP
#define VSHOGI_MINISHOGI_PIECE_HPP

#include <cstdint>
#include <string>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"
#include "vshogi/pieces.hpp"

namespace vshogi::minishogi
{

enum PieceTypeEnum : std::uint8_t
{
    FU = 0b0000, //!< Fu (Pawn)
    GI = 0b0001, //!< Gin (Silver)
    KA = 0b0010, //!< Kaku (Bishop)
    HI = 0b0011, //!< Hisha (Rook)

    KI = 0b0100, //!< Kin (Gold)
    OU = 0b0111, //!< Ou, Gyoku (King)

    TO = 0b1000, //!< Tokin (Promoted Pawn)
    NG = 0b1001, //!< Nari-Gin (Promoted Silver)
    UM = 0b1010, //!< Uma (Promoted Bishop)
    RY = 0b1011, //!< Ryu (Promoted Rook)

    NA = 0b1111, //!< Not available
};

enum BoardPieceTypeEnum : std::uint8_t
{
    B_FU = FU, //!< Black Fu (Pawn)
    B_GI = GI, //!< Black Gin (Silver)
    B_KA = KA, //!< Black Kaku (Bishop)
    B_HI = HI, //!< Black Hisha (Rook)
    B_KI = KI, //!< Black Kin (Gold)
    B_OU = OU, //!< Black Ou, Gyoku (King)
    B_TO = TO, //!< Black Tokin (Promoted Pawn)
    B_NG = NG, //!< Black Nari-Gin (Promoted Silver)
    B_UM = UM, //!< Black Uma (Promoted Bishop)
    B_RY = RY, //!< Black Ryu (Promoted Rook)

    W_FU = 0b10000 + B_FU, //!< White Fu (Pawn)
    W_GI = 0b10000 + B_GI, //!< White Gin (Silver)
    W_KA = 0b10000 + B_KA, //!< White Kaku (Bishop)
    W_HI = 0b10000 + B_HI, //!< White Hisha (Rook)
    W_KI = 0b10000 + B_KI, //!< White Kin (Gold)
    W_OU = 0b10000 + B_OU, //!< White Ou, Gyoku (King)
    W_TO = 0b10000 + B_TO, //!< White Tokin (Promoted Pawn)
    W_NG = 0b10000 + B_NG, //!< White Nari-Gin (Promoted Silver)
    W_UM = 0b10000 + B_UM, //!< White Uma (Promoted Bishop)
    W_RY = 0b10000 + B_RY, //!< White Ryu (Promoted Rook)

    VOID = 0b11111, //!< Empty Square
};

using Pieces = vshogi::Pieces<PieceTypeEnum, BoardPieceTypeEnum, 10, 5, 3>;
static_assert(FU == Pieces::FU);
static_assert(OU == Pieces::OU);
static_assert(NA == Pieces::NA);
static_assert(B_OU == Pieces::B_OU);
static_assert(W_OU == Pieces::W_OU);
static_assert(VOID == Pieces::VOID);

} // namespace vshogi::minishogi

namespace vshogi
{

template <>
inline const vshogi::minishogi::PieceTypeEnum
    vshogi::minishogi::Pieces::piece_array[]
    = {vshogi::minishogi::FU,
       vshogi::minishogi::GI,
       vshogi::minishogi::KA,
       vshogi::minishogi::HI,
       vshogi::minishogi::KI,
       vshogi::minishogi::OU,
       vshogi::minishogi::TO,
       vshogi::minishogi::NG,
       vshogi::minishogi::UM,
       vshogi::minishogi::RY};

template <>
inline const vshogi::minishogi::PieceTypeEnum
    vshogi::minishogi::Pieces::stand_piece_array[]
    = {vshogi::minishogi::FU,
       vshogi::minishogi::GI,
       vshogi::minishogi::KA,
       vshogi::minishogi::HI,
       vshogi::minishogi::KI};

template <>
inline vshogi::minishogi::PieceTypeEnum
vshogi::minishogi::Pieces::to_piece_type(const char c)
{
    using namespace vshogi::minishogi;
    switch (std::tolower(c)) {
    case 'p':
        return FU;
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
inline constexpr bool vshogi::minishogi::Pieces::is_promotable(
    const vshogi::minishogi::PieceTypeEnum& p)
{
    return (p < vshogi::minishogi::KI);
}

template <>
inline bool vshogi::minishogi::Pieces::is_ranging_to(
    const vshogi::minishogi::BoardPieceTypeEnum& p, const DirectionEnum& d)
{
    using namespace vshogi::minishogi;
    const auto base = demote(to_piece_type(p));
    if (base == HI)
        return (d == DIR_N) || (d == DIR_W) || (d == DIR_E) || (d == DIR_S);
    if (base == KA)
        return (d == DIR_NW) || (d == DIR_NE) || (d == DIR_SW) || (d == DIR_SE);
    return false;
}

template <>
inline int
vshogi::minishogi::Pieces::get_point(const vshogi::minishogi::PieceTypeEnum& p)
{
    using namespace vshogi::minishogi;
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
inline void vshogi::minishogi::Pieces::append_sfen(
    const vshogi::minishogi::BoardPieceTypeEnum& p, std::string& out)
{
    using namespace vshogi::minishogi;
    const auto color = get_color(p);
    const auto promotion = is_promoted(p);
    const auto pt = demote(to_piece_type(p));
    char c;
    switch (pt) {
    case FU:
        c = 'p';
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

#endif // VSHOGI_MINISHOGI_PIECE_HPP
