﻿/*
  Apery, a USI shogi playing engine derived from Stockfish, a UCI chess playing engine.
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2018 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad
  Copyright (C) 2011-2018 Hiraoka Takuya

  Apery is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Apery is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef APERY_PIECE_HPP
#define APERY_PIECE_HPP

#include "color.hpp"
#include "common.hpp"
#include "overloadEnumOperators.hpp"
#include <cassert>
#include <iostream>
#include <string>

enum PieceTypeEnum
{
    // Pro* は 元の 駒の種類に 8 を加算したもの。
    PTPromote = 8,
    Occupied = 0, // 各 PieceTypeEnum の or をとったもの。
    FU = 1,
    KY = 2,
    KE = 3,
    GI = 4,
    KA = 5,
    HI = 6,
    KI = 7,
    OU = 8,
    TO = PTPromote + FU,
    NY = PTPromote + KY,
    NK = PTPromote + KE,
    NG = PTPromote + GI,
    UM = PTPromote + KA,
    RY = PTPromote + HI,
    PieceTypeNum,
    GoldHorseDragon, // 単にtemnplate引数として使用

};
OverloadEnumOperators(PieceTypeEnum);

enum Piece
{
    // B* に 16 を加算することで、W* を表す。
    // Promoted を加算することで、成りを表す。
    // clang-format off
    Empty = 0, UnPromoted = 0, Promoted = 8,
    B_FU = 1, B_KY, B_KE, B_GI, B_KA, B_HI, B_KI, B_OU,
    B_TO, B_NY, B_NK, B_NG, B_UM, B_RY, // B_RY = 14
    W_FU = 17, W_KY, W_KE, W_GI, W_KA, W_HI, W_KI, W_OU,
    W_TO, W_NY, W_NK, W_NG, W_UM, W_RY,
    PieceNone, // PieceNone = 31  これを 32 にした方が多重配列のときに有利か。
    // clang-format on
};
OverloadEnumOperators(Piece);

inline Piece inverse(const Piece pc)
{
    return static_cast<Piece>(pc ^ 0x10);
}

// 持ち駒を表すときに使用する。
// todo: H_KI を H_HI の後ろに持っていき、PieceType との変換を簡単に出来るようにする。
enum HandPiece
{
    H_FU,
    H_KY,
    H_KE,
    H_GI,
    H_KI,
    H_KA,
    H_HI,
    HandPieceNum,
};
OverloadEnumOperators(HandPiece);

// p == Empty のとき、PieceType は OccuPied になってしまうので、
// Position::bbOf(pieceToPieceType(p)) とすると、
// Position::emptyBB() ではなく Position::occupiedBB() になってしまうので、
// 注意すること。出来れば修正したい。
inline PieceTypeEnum pieceToPieceType(const Piece p)
{
    return static_cast<PieceTypeEnum>(p & 15);
}

inline color::ColorEnum pieceToColor(const Piece p)
{
    assert(p != Empty);
    return static_cast<color::ColorEnum>(p >> 4);
}

inline Piece
colorAndPieceTypeToPiece(const color::ColorEnum c, const PieceTypeEnum pt)
{
    return static_cast<Piece>((c << 4) | pt);
}

const u32 IsSliderVal = 0x60646064;
// pc が遠隔駒であるか
inline bool isSlider(const Piece pc)
{
    return (IsSliderVal & (1 << pc)) != 0;
}
inline bool isSlider(const PieceTypeEnum pt)
{
    return (IsSliderVal & (1 << pt)) != 0;
}

const HandPiece PieceTypeToHandPieceTable[PieceTypeNum]
    = {HandPieceNum,
       H_FU,
       H_KY,
       H_KE,
       H_GI,
       H_KA,
       H_HI,
       H_KI,
       HandPieceNum,
       H_FU,
       H_KY,
       H_KE,
       H_GI,
       H_KA,
       H_HI};
inline HandPiece pieceTypeToHandPiece(const PieceTypeEnum pt)
{
    return PieceTypeToHandPieceTable[pt];
}

const PieceTypeEnum HandPieceToPieceTypeTable[HandPieceNum]
    = {FU, KY, KE, GI, KI, KA, HI};
inline PieceTypeEnum handPieceToPieceType(const HandPiece hp)
{
    return HandPieceToPieceTypeTable[hp];
}
inline Piece
colorAndHandPieceToPiece(const color::ColorEnum c, const HandPiece hp)
{
    return colorAndPieceTypeToPiece(c, handPieceToPieceType(hp));
}

#endif // #ifndef APERY_PIECE_HPP
