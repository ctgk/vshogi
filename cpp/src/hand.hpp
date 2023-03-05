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

#ifndef APERY_HAND_HPP
#define APERY_HAND_HPP

#include "common.hpp"
#include "piece.hpp"

// 手駒
// 手駒の状態 (32bit に pack する)
// 手駒の優劣判定を高速に行う為に各駒の間を1bit空ける。
// xxxxxxxx xxxxxxxx xxxxxxxx xxx11111  FU
// xxxxxxxx xxxxxxxx xxxxxxx1 11xxxxxx  KY
// xxxxxxxx xxxxxxxx xxx111xx xxxxxxxx  KE
// xxxxxxxx xxxxxxx1 11xxxxxx xxxxxxxx  GI
// xxxxxxxx xxx111xx xxxxxxxx xxxxxxxx  KI
// xxxxxxxx 11xxxxxx xxxxxxxx xxxxxxxx  KA
// xxxxx11x xxxxxxxx xxxxxxxx xxxxxxxx  HI
class Hand
{
public:
    Hand()
    {
    }
    explicit Hand(u32 v) : value_(v)
    {
    }
    u32 value() const
    {
        return value_;
    }
    template <HandPiece HP>
    u32 numOf() const
    {
        return (
            HP == H_FU   ? ((value() & HPawnMask) >> HPawnShiftBits)
            : HP == H_KY ? ((value() & HLanceMask) >> HLanceShiftBits)
            : HP == H_KE ? ((value() & HKnightMask) >> HKnightShiftBits)
            : HP == H_GI ? ((value() & HSilverMask) >> HSilverShiftBits)
            : HP == H_KI ? ((value() & HGoldMask) >> HGoldShiftBits)
            : HP == H_KA ? ((value() & HBishopMask) >> HBishopShiftBits)
                         : ((value() /*& HRookMask*/) >> HRookShiftBits));
    }

    u32 numOf(const HandPiece handPiece) const
    {
        return (value() & HandPieceMask[handPiece])
               >> HandPieceShiftBits[handPiece];
    }
    // 2つの Hand 型変数の、同じ種類の駒の数を比較する必要があるため、
    // bool じゃなくて、u32 型でそのまま返す。
    template <HandPiece HP>
    u32 exists() const
    {
        return (
            HP == H_FU   ? (value() & HPawnMask)
            : HP == H_KY ? (value() & HLanceMask)
            : HP == H_KE ? (value() & HKnightMask)
            : HP == H_GI ? (value() & HSilverMask)
            : HP == H_KI ? (value() & HGoldMask)
            : HP == H_KA ? (value() & HBishopMask)
                         : (value() & HRookMask));
    }
    u32 exists(const HandPiece handPiece) const
    {
        return value() & HandPieceMask[handPiece];
    }
    u32 exceptPawnExists() const
    {
        return value() & HandPieceExceptPawnMask;
    }
    // num が int だけどまあ良いか。
    void orEqual(const int num, const HandPiece handPiece)
    {
        value_ |= num << HandPieceShiftBits[handPiece];
    }
    void plusOne(const HandPiece handPiece)
    {
        value_ += HandPieceOne[handPiece];
    }
    void minusOne(const HandPiece handPiece)
    {
        value_ -= HandPieceOne[handPiece];
    }
    bool operator==(const Hand rhs) const
    {
        return this->value() == rhs.value();
    }
    bool operator!=(const Hand rhs) const
    {
        return this->value() != rhs.value();
    }
    // 手駒の優劣判定
    // 手駒が ref と同じか、勝っていれば true
    // 勝っている状態とは、全ての種類の手駒が、ref 以上の枚数があることを言う。
    bool isEqualOrSuperior(const Hand ref) const
    {
#if 0
        // 全ての駒が ref 以上の枚数なので、true
        return (ref.exists<H_KE>() <= this->exists<H_KE>()
                && ref.exists<H_GI>() <= this->exists<H_GI>()
                && ref.exists<H_KI  >() <= this->exists<H_KI  >()
                && ref.exists<H_KA>() <= this->exists<H_KA>()
                && ref.exists<H_HI  >() <= this->exists<H_HI  >());
#else
        // こちらは、同じ意味でより高速
        // ref の方がどれか一つでも多くの枚数の駒を持っていれば、Borrow の位置のビットが立つ。
        return ((this->value() - ref.value()) & BorrowMask) == 0;
#endif
    }
    // 証明駒用メソッド
    void set(const u32 value)
    {
        value_ = value;
    }
    // 相手が一枚も持っていない種類の持駒を加算
    void setPP(const Hand us, const Hand them)
    {
        u32 mask = 0;
        if (them.exists<H_FU>() == 0)
            mask |= HPawnMask;
        if (them.exists<H_KY>() == 0)
            mask |= HLanceMask;
        if (them.exists<H_KE>() == 0)
            mask |= HKnightMask;
        if (them.exists<H_GI>() == 0)
            mask |= HSilverMask;
        if (them.exists<H_KI>() == 0)
            mask |= HGoldMask;
        if (them.exists<H_KA>() == 0)
            mask |= HBishopMask;
        if (them.exists<H_HI>() == 0)
            mask |= HRookMask;

        // 相手が一枚も持っていない種類の持駒を一旦0にする
        value_ &= ~mask;

        // 相手が一枚も持っていない種類の持駒を設定する
        value_ |= (us.value() & mask);
    }

private:
    // clang-format off
    static const int HPawnShiftBits   =  0;
    static const int HLanceShiftBits  =  6;
    static const int HKnightShiftBits = 10;
    static const int HSilverShiftBits = 14;
    static const int HGoldShiftBits   = 18;
    static const int HBishopShiftBits = 22;
    static const int HRookShiftBits   = 25;
    static const u32 HPawnMask   = 0x1f << HPawnShiftBits;
    static const u32 HLanceMask  = 0x7  << HLanceShiftBits;
    static const u32 HKnightMask = 0x7  << HKnightShiftBits;
    static const u32 HSilverMask = 0x7  << HSilverShiftBits;
    static const u32 HGoldMask   = 0x7  << HGoldShiftBits;
    static const u32 HBishopMask = 0x3  << HBishopShiftBits;
    static const u32 HRookMask   = 0x3  << HRookShiftBits;
    // clang-format on
    static const u32 HandPieceExceptPawnMask
        = (HLanceMask | HKnightMask | HSilverMask | HGoldMask | HBishopMask
           | HRookMask);
    static const int HandPieceShiftBits[HandPieceNum];
    static const u32 HandPieceMask[HandPieceNum];
    // 特定の種類の持ち駒を 1 つ増やしたり減らしたりするときに使用するテーブル
    static const u32 HandPieceOne[HandPieceNum];
    static const u32 BorrowMask
        = ((HPawnMask + (1 << HPawnShiftBits))
           | (HLanceMask + (1 << HLanceShiftBits))
           | (HKnightMask + (1 << HKnightShiftBits))
           | (HSilverMask + (1 << HSilverShiftBits))
           | (HGoldMask + (1 << HGoldShiftBits))
           | (HBishopMask + (1 << HBishopShiftBits))
           | (HRookMask + (1 << HRookShiftBits)));

    u32 value_;
};

#endif // #ifndef APERY_HAND_HPP
