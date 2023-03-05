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
// xxxxxxxx xxxxxxxx xxxxxxxx xxx11111  piece::FU
// xxxxxxxx xxxxxxxx xxxxxxx1 11xxxxxx  piece::KY
// xxxxxxxx xxxxxxxx xxx111xx xxxxxxxx  piece::KE
// xxxxxxxx xxxxxxx1 11xxxxxx xxxxxxxx  piece::GI
// xxxxxxxx xxx111xx xxxxxxxx xxxxxxxx  piece::KI
// xxxxxxxx 11xxxxxx xxxxxxxx xxxxxxxx  piece::KA
// xxxxx11x xxxxxxxx xxxxxxxx xxxxxxxx  piece::HI
class PieceStand
{
public:
    PieceStand()
    {
    }
    explicit PieceStand(u32 v) : value_(v)
    {
    }
    u32 value() const
    {
        return value_;
    }
    template <piece::CapturedPieceTypeEnum HP>
    u32 numOf() const
    {
        return (
            HP == piece::C_FU   ? ((value() & HPawnMask) >> shift_bits_fu)
            : HP == piece::C_KY ? ((value() & HLanceMask) >> shift_bits_ky)
            : HP == piece::C_KE ? ((value() & HKnightMask) >> shift_bits_ke)
            : HP == piece::C_GI ? ((value() & HSilverMask) >> shift_bits_gi)
            : HP == piece::C_KI ? ((value() & HGoldMask) >> shift_bits_ki)
            : HP == piece::C_KA ? ((value() & HBishopMask) >> shift_bits_ka)
                                : ((value() /*& HRookMask*/) >> shift_bits_hi));
    }

    u32 numOf(const piece::CapturedPieceTypeEnum captured_piece_type) const
    {
        return (value() & HandPieceMask[captured_piece_type])
               >> HandPieceShiftBits[captured_piece_type];
    }
    // 2つの PieceStand 型変数の、同じ種類の駒の数を比較する必要があるため、
    // bool じゃなくて、u32 型でそのまま返す。
    template <piece::CapturedPieceTypeEnum HP>
    u32 exists() const
    {
        return (
            HP == piece::C_FU   ? (value() & HPawnMask)
            : HP == piece::C_KY ? (value() & HLanceMask)
            : HP == piece::C_KE ? (value() & HKnightMask)
            : HP == piece::C_GI ? (value() & HSilverMask)
            : HP == piece::C_KI ? (value() & HGoldMask)
            : HP == piece::C_KA ? (value() & HBishopMask)
                                : (value() & HRookMask));
    }
    u32 exists(const piece::CapturedPieceTypeEnum captured_piece_type) const
    {
        return value() & HandPieceMask[captured_piece_type];
    }
    u32 exceptPawnExists() const
    {
        return value() & HandPieceExceptPawnMask;
    }
    // num が int だけどまあ良いか。
    void orEqual(
        const int num, const piece::CapturedPieceTypeEnum captured_piece_type)
    {
        value_ |= num << HandPieceShiftBits[captured_piece_type];
    }
    void plusOne(const piece::CapturedPieceTypeEnum captured_piece_type)
    {
        value_ += HandPieceOne[captured_piece_type];
    }
    void minusOne(const piece::CapturedPieceTypeEnum captured_piece_type)
    {
        value_ -= HandPieceOne[captured_piece_type];
    }
    bool operator==(const PieceStand rhs) const
    {
        return this->value() == rhs.value();
    }
    bool operator!=(const PieceStand rhs) const
    {
        return this->value() != rhs.value();
    }
    // 手駒の優劣判定
    // 手駒が ref と同じか、勝っていれば true
    // 勝っている状態とは、全ての種類の手駒が、ref 以上の枚数があることを言う。
    bool isEqualOrSuperior(const PieceStand ref) const
    {
#if 0
        // 全ての駒が ref 以上の枚数なので、true
        return (ref.exists<piece::C_KE>() <= this->exists<piece::C_KE>()
                && ref.exists<piece::C_GI>() <= this->exists<piece::C_GI>()
                && ref.exists<piece::C_KI  >() <= this->exists<piece::C_KI  >()
                && ref.exists<piece::C_KA>() <= this->exists<piece::C_KA>()
                && ref.exists<piece::C_HI  >() <= this->exists<piece::C_HI  >());
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
    void setPP(const PieceStand us, const PieceStand them)
    {
        u32 mask = 0;
        if (them.exists<piece::C_FU>() == 0)
            mask |= HPawnMask;
        if (them.exists<piece::C_KY>() == 0)
            mask |= HLanceMask;
        if (them.exists<piece::C_KE>() == 0)
            mask |= HKnightMask;
        if (them.exists<piece::C_GI>() == 0)
            mask |= HSilverMask;
        if (them.exists<piece::C_KI>() == 0)
            mask |= HGoldMask;
        if (them.exists<piece::C_KA>() == 0)
            mask |= HBishopMask;
        if (them.exists<piece::C_HI>() == 0)
            mask |= HRookMask;

        // 相手が一枚も持っていない種類の持駒を一旦0にする
        value_ &= ~mask;

        // 相手が一枚も持っていない種類の持駒を設定する
        value_ |= (us.value() & mask);
    }

private:
    // clang-format off
    static const int shift_bits_fu =  0;
    static const int shift_bits_ky =  6;
    static const int shift_bits_ke = 10;
    static const int shift_bits_gi = 14;
    static const int shift_bits_ki = 18;
    static const int shift_bits_ka = 22;
    static const int shift_bits_hi = 25;
    static const u32 HPawnMask   = 0x1f << shift_bits_fu;
    static const u32 HLanceMask  = 0x7  << shift_bits_ky;
    static const u32 HKnightMask = 0x7  << shift_bits_ke;
    static const u32 HSilverMask = 0x7  << shift_bits_gi;
    static const u32 HGoldMask   = 0x7  << shift_bits_ki;
    static const u32 HBishopMask = 0x3  << shift_bits_ka;
    static const u32 HRookMask   = 0x3  << shift_bits_hi;
    // clang-format on
    static const u32 HandPieceExceptPawnMask
        = (HLanceMask | HKnightMask | HSilverMask | HGoldMask | HBishopMask
           | HRookMask);
    static const int HandPieceShiftBits[piece::NUM_CAPTURED_PIECE_TYPES];
    static const u32 HandPieceMask[piece::NUM_CAPTURED_PIECE_TYPES];
    // 特定の種類の持ち駒を 1 つ増やしたり減らしたりするときに使用するテーブル
    static const u32 HandPieceOne[piece::NUM_CAPTURED_PIECE_TYPES];
    static const u32 BorrowMask
        = ((HPawnMask + (1 << shift_bits_fu))
           | (HLanceMask + (1 << shift_bits_ky))
           | (HKnightMask + (1 << shift_bits_ke))
           | (HSilverMask + (1 << shift_bits_gi))
           | (HGoldMask + (1 << shift_bits_ki))
           | (HBishopMask + (1 << shift_bits_ka))
           | (HRookMask + (1 << shift_bits_hi)));

    u32 value_;
};

#endif // #ifndef APERY_HAND_HPP
