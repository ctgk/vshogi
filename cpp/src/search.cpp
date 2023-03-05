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

#include "generateMoves.hpp"
#include "position.hpp"
#include "usi.hpp"

// 入玉勝ちかどうかを判定
bool nyugyoku(const Position& pos)
{
    // CSA ルールでは、一 から 六 の条件を全て満たすとき、入玉勝ち宣言が出来る。
    // 判定が高速に出来るものから順に判定していく事にする。

    // 一 宣言側の手番である。

    // この関数を呼び出すのは自分の手番のみとする。ponder では呼び出さない。

    // 六 宣言側の持ち時間が残っている。

    // 持ち時間が無ければ既に負けなので、何もチェックしない。

    // 五 宣言側の玉に王手がかかっていない。
    if (pos.inCheck())
        return false;

    const color::ColorEnum us = pos.turn();
    // 敵陣のマスク
    const Bitboard opponentsField
        = (us == color::BLACK ? inFrontMask<color::BLACK, Rank4>()
                              : inFrontMask<color::WHITE, Rank6>());

    // 二 宣言側の玉が敵陣三段目以内に入っている。
    if (!pos.bbOf(piece::OU, us).andIsAny(opponentsField))
        return false;

    // 四 宣言側の敵陣三段目以内の駒は、玉を除いて10枚以上存在する。
    const int ownPiecesCount = (pos.bbOf(us) & opponentsField).popCount() - 1;
    if (ownPiecesCount < 10)
        return false;

    // 三 宣言側が、大駒5点小駒1点で計算して
    //     先手の場合28点以上の持点がある。
    //     後手の場合27点以上の持点がある。
    //     点数の対象となるのは、宣言側の持駒と敵陣三段目以内に存在する玉を除く宣言側の駒のみである。
    const int ownBigPiecesCount
        = (pos.bbOf(piece::HI, piece::RY, piece::KA, piece::UM) & opponentsField
           & pos.bbOf(us))
              .popCount();
    const int ownSmallPiecesCount = ownPiecesCount - ownBigPiecesCount;
    const Hand hand = pos.hand(us);
    const int val = ownSmallPiecesCount + hand.numOf<piece::C_FU>()
                    + hand.numOf<piece::C_KY>() + hand.numOf<piece::C_KE>()
                    + hand.numOf<piece::C_GI>() + hand.numOf<piece::C_KI>()
                    + (ownBigPiecesCount + hand.numOf<piece::C_HI>()
                       + hand.numOf<piece::C_KA>())
                          * 5;
#if defined LAW_24
    if (val < 31)
        return false;
#else
    if (val < (us == color::BLACK ? 28 : 27))
        return false;
#endif

    return true;
}
