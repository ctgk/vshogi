/*
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

enum ColoredPieceEnum
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
OverloadEnumOperators(ColoredPieceEnum);

inline ColoredPieceEnum inverse(const ColoredPieceEnum pc)
{
    return static_cast<ColoredPieceEnum>(pc ^ 0x10);
}

// 持ち駒を表すときに使用する。
// todo: C_KI を C_HI の後ろに持っていき、PieceType との変換を簡単に出来るようにする。
enum CapturedPieceTypeEnum
{
    C_FU,
    C_KY,
    C_KE,
    C_GI,
    C_KI,
    C_KA,
    C_HI,
    NUM_CAPTURED_PIECE_TYPES,
};
OverloadEnumOperators(CapturedPieceTypeEnum);

// p == Empty のとき、PieceType は OccuPied になってしまうので、
// Position::bbOf(to_piece_type(p)) とすると、
// Position::emptyBB() ではなく Position::occupiedBB() になってしまうので、
// 注意すること。出来れば修正したい。
inline PieceTypeEnum to_piece_type(const ColoredPieceEnum p)
{
    return static_cast<PieceTypeEnum>(p & 15);
}

inline color::ColorEnum get_color(const ColoredPieceEnum p)
{
    assert(p != Empty);
    return static_cast<color::ColorEnum>(p >> 4);
}

inline ColoredPieceEnum
to_colored_piece(const color::ColorEnum c, const PieceTypeEnum pt)
{
    return static_cast<ColoredPieceEnum>((c << 4) | pt);
}

inline CapturedPieceTypeEnum to_captured_piece_type(const PieceTypeEnum pt)
{
    constexpr CapturedPieceTypeEnum table[PieceTypeNum]
        = {NUM_CAPTURED_PIECE_TYPES,
           C_FU,
           C_KY,
           C_KE,
           C_GI,
           C_KA,
           C_HI,
           C_KI,
           NUM_CAPTURED_PIECE_TYPES,
           C_FU,
           C_KY,
           C_KE,
           C_GI,
           C_KA,
           C_HI};
    return table[pt];
}

inline PieceTypeEnum to_piece_type(const CapturedPieceTypeEnum hp)
{
    constexpr PieceTypeEnum table[NUM_CAPTURED_PIECE_TYPES]
        = {FU, KY, KE, GI, KI, KA, HI};
    return table[hp];
}
inline ColoredPieceEnum
to_colored_piece(const color::ColorEnum c, const CapturedPieceTypeEnum hp)
{
    return to_colored_piece(c, to_piece_type(hp));
}

#endif // #ifndef APERY_PIECE_HPP
