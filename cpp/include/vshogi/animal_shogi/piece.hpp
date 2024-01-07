#ifndef VSHOGI_ANIMAL_SHOGI_PIECE_HPP
#define VSHOGI_ANIMAL_SHOGI_PIECE_HPP

#include <cctype>
#include <cstdint>
#include <string>

#include "vshogi/color.hpp"
#include "vshogi/pieces.hpp"

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

    OU = LI,
};

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

using Pieces = vshogi::Pieces<PieceTypeEnum, BoardPieceTypeEnum, 5, 3, 2>;

} // namespace vshogi::animal_shogi

namespace vshogi
{

template <>
inline const vshogi::animal_shogi::PieceTypeEnum
    vshogi::animal_shogi::Pieces::piece_array[]
    = {vshogi::animal_shogi::CH,
       vshogi::animal_shogi::EL,
       vshogi::animal_shogi::GI,
       vshogi::animal_shogi::LI,
       vshogi::animal_shogi::HE};

template <>
inline const vshogi::animal_shogi::PieceTypeEnum
    vshogi::animal_shogi::Pieces::stand_piece_array[]
    = {vshogi::animal_shogi::CH,
       vshogi::animal_shogi::EL,
       vshogi::animal_shogi::GI};

template <>
inline vshogi::animal_shogi::PieceTypeEnum
vshogi::animal_shogi::Pieces::to_piece_type(const char c)
{
    using namespace vshogi::animal_shogi;
    switch (std::tolower(c)) {
    case 'c':
        return CH;
    case 'e':
        return EL;
    case 'g':
        return GI;
    case 'h':
        return HE;
    case 'l':
        return LI;
    default:
        return NA;
    }
}

template <>
inline void vshogi::animal_shogi::Pieces::append_sfen(
    const vshogi::animal_shogi::BoardPieceTypeEnum& p, std::string& out)
{
    switch (p) {
    case vshogi::animal_shogi::B_CH:
        out += 'C';
        break;
    case vshogi::animal_shogi::B_EL:
        out += 'E';
        break;
    case vshogi::animal_shogi::B_GI:
        out += 'G';
        break;
    case vshogi::animal_shogi::B_LI:
        out += 'L';
        break;
    case vshogi::animal_shogi::B_HE:
        out += 'H';
        break;
    case vshogi::animal_shogi::W_CH:
        out += 'c';
        break;
    case vshogi::animal_shogi::W_EL:
        out += 'e';
        break;
    case vshogi::animal_shogi::W_GI:
        out += 'g';
        break;
    case vshogi::animal_shogi::W_LI:
        out += 'l';
        break;
    case vshogi::animal_shogi::W_HE:
        out += 'h';
        break;
    default:
        break;
    }
}

} // namespace vshogi

#endif // VSHOGI_ANIMAL_SHOGI_PIECE_HPP
