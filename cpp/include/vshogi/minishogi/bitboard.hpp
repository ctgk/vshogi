#ifndef VSHOGI_MINISHOGI_BITBOARD_HPP
#define VSHOGI_MINISHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/color.hpp"
#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"

namespace vshogi::minishogi
{

enum DirectionEnum
{
    // clang-format off
    NW = 0,  N = 1, NE = 2,
     W = 3,          E = 4,
    SW = 5,  S = 6, SE = 7,
    // clang-format on
};

/**
 * @brief N-bit binary board representing ON and OFF of each square.
 * @details
 *              +--------------------------------- SQ_55
 *              |                     +----------- SQ_21
 *              |                     |+---------- SQ_15
 *              |                     ||  +------- SQ_12
 *              |                     ||  |+------ SQ_11
 *              |                     ||  ||
 *              v                     vv  vv
 * (MSB) xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx (LSB)
 */
class BitBoard
{
private:
    std::uint32_t m_value;

public:
    constexpr BitBoard() : m_value(0U)
    {
    }
    constexpr BitBoard(const std::uint32_t v) : m_value(v & 0x01ffffff)
    {
    }
    constexpr static BitBoard from_square(const SquareEnum sq)
    {
        return BitBoard(static_cast<std::uint32_t>(1 << static_cast<int>(sq)));
    }
    constexpr BitBoard operator|(const BitBoard other) const
    {
        return BitBoard(m_value | other.m_value);
    }
    BitBoard& operator|=(const BitBoard other)
    {
        m_value = static_cast<std::uint32_t>(m_value | other.m_value);
        return *this;
    }
    constexpr BitBoard operator&(const BitBoard other) const
    {
        return BitBoard(m_value & other.m_value);
    }
    constexpr BitBoard operator~() const
    {
        return BitBoard(static_cast<std::uint32_t>(~m_value));
    }
    bool operator==(const BitBoard& other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const BitBoard other) const
    {
        return m_value != other.m_value;
    }
    constexpr BitBoard shift_above() const;
    constexpr BitBoard shift_below() const;
    constexpr BitBoard shift_right() const
    {
        return BitBoard(static_cast<std::uint32_t>(m_value >> 5));
    }
    constexpr BitBoard shift_left() const
    {
        return BitBoard(static_cast<std::uint32_t>(m_value << 5));
    }
    template <DirectionEnum D>
    constexpr BitBoard shift() const
    {
        if constexpr (D == NW)
            return shift_above().shift_left();
        else if constexpr (D == N)
            return shift_above();
        else if constexpr (D == NE)
            return shift_above().shift_right();
        else if constexpr (D == W)
            return shift_left();
        else if constexpr (D == E)
            return shift_right();
        else if constexpr (D == SW)
            return shift_below().shift_left();
        else if constexpr (D == S)
            return shift_below();
        else if constexpr (D == SE)
            return shift_below().shift_right();
        else
            return *this;
    }
    constexpr BitBoard rotate() const
    {
        std::uint32_t v = m_value;
        v = (v & 0xffff0000) >> 16 | (v & 0x0000ffff) << 16;
        v = (v & 0xff00ff00) >> 8 | (v & 0x00ff00ff) << 8;
        v = (v & 0xf0f0f0f0) >> 4 | (v & 0x0f0f0f0f) << 4;
        v = (v & 0xcccccccc) >> 2 | (v & 0x33333333) << 2;
        v = (v & 0xaaaaaaaa) >> 1 | (v & 0x55555555) << 1;
        v = (v >> 7);
        return BitBoard(v);
    }
    bool any() const
    {
        return m_value > 0;
    }
    bool is_one(const SquareEnum sq) const
    {
        return static_cast<bool>(
            (1U << static_cast<std::uint32_t>(sq)) & m_value);
    }
    int hamming_weight() const
    {
        // https://en.wikipedia.org/wiki/Hamming_weight
        constexpr std::uint32_t m1 = 0x55555555;
        constexpr std::uint32_t m2 = 0x33333333;
        constexpr std::uint32_t m4 = 0x0f0f0f0f;
        constexpr std::uint32_t h01 = 0x01010101;
        auto x = m_value;
        x -= (x >> 1U) & m1;
        x = (x & m2) + ((x >> 2) & m2);
        x = (x + (x >> 4)) & m4;
        x += x >> 8;
        x += x >> 16;
        return x & 0x7f;
    }
    constexpr BitBoard expand_adjacently() const
    {
        return *this | shift<N>() | shift<E>() | shift<S>() | shift<W>();
    }
    constexpr BitBoard expand_diagonally() const
    {
        return *this | shift<NW>() | shift<NE>() | shift<SW>() | shift<SE>();
    }
    constexpr BitBoard expand() const
    {
        return expand_adjacently() | expand_diagonally();
    }
    template <DirectionEnum D>
    static BitBoard
    ranging_attacks_to(const SquareEnum sq, const BitBoard occupied)
    {
        const auto base = BitBoard::from_square(sq);
        auto attacks = base;
        for (int i = 4; i--;) {
            const auto expansion = attacks.shift<D>() & (~attacks);
            if (!expansion.any())
                break; // reached the end of the board.
            else if ((expansion & occupied).any()) {
                attacks |= expansion;
                break; // reached a piece.
            } else {
                attacks |= expansion; // continue.
            }
        }
        return attacks & (~base);
    }
    static BitBoard
    ranging_attacks_to_diagonal(const SquareEnum sq, const BitBoard occupied)
    {
        return ranging_attacks_to<NW>(sq, occupied)
               | ranging_attacks_to<NE>(sq, occupied)
               | ranging_attacks_to<SW>(sq, occupied)
               | ranging_attacks_to<SE>(sq, occupied);
    }
    static BitBoard
    ranging_attacks_to_adjacent(const SquareEnum sq, const BitBoard occupied)
    {
        return ranging_attacks_to<N>(sq, occupied)
               | ranging_attacks_to<E>(sq, occupied)
               | ranging_attacks_to<W>(sq, occupied)
               | ranging_attacks_to<S>(sq, occupied);
    }
};

constexpr BitBoard bb_11 = BitBoard::from_square(SQ_11);
constexpr BitBoard bb_12 = BitBoard::from_square(SQ_12);
constexpr BitBoard bb_13 = BitBoard::from_square(SQ_13);
constexpr BitBoard bb_14 = BitBoard::from_square(SQ_14);
constexpr BitBoard bb_15 = BitBoard::from_square(SQ_15);
constexpr BitBoard bb_21 = BitBoard::from_square(SQ_21);
constexpr BitBoard bb_22 = BitBoard::from_square(SQ_22);
constexpr BitBoard bb_23 = BitBoard::from_square(SQ_23);
constexpr BitBoard bb_24 = BitBoard::from_square(SQ_24);
constexpr BitBoard bb_25 = BitBoard::from_square(SQ_25);
constexpr BitBoard bb_31 = BitBoard::from_square(SQ_31);
constexpr BitBoard bb_32 = BitBoard::from_square(SQ_32);
constexpr BitBoard bb_33 = BitBoard::from_square(SQ_33);
constexpr BitBoard bb_34 = BitBoard::from_square(SQ_34);
constexpr BitBoard bb_35 = BitBoard::from_square(SQ_35);
constexpr BitBoard bb_41 = BitBoard::from_square(SQ_41);
constexpr BitBoard bb_42 = BitBoard::from_square(SQ_42);
constexpr BitBoard bb_43 = BitBoard::from_square(SQ_43);
constexpr BitBoard bb_44 = BitBoard::from_square(SQ_44);
constexpr BitBoard bb_45 = BitBoard::from_square(SQ_45);
constexpr BitBoard bb_51 = BitBoard::from_square(SQ_51);
constexpr BitBoard bb_52 = BitBoard::from_square(SQ_52);
constexpr BitBoard bb_53 = BitBoard::from_square(SQ_53);
constexpr BitBoard bb_54 = BitBoard::from_square(SQ_54);
constexpr BitBoard bb_55 = BitBoard::from_square(SQ_55);

constexpr BitBoard bb_r1 = bb_11 | bb_21 | bb_31 | bb_41 | bb_51;
constexpr BitBoard bb_r2 = bb_12 | bb_22 | bb_32 | bb_42 | bb_52;
constexpr BitBoard bb_r3 = bb_13 | bb_23 | bb_33 | bb_43 | bb_53;
constexpr BitBoard bb_r4 = bb_14 | bb_24 | bb_34 | bb_44 | bb_54;
constexpr BitBoard bb_r5 = bb_15 | bb_25 | bb_35 | bb_45 | bb_55;
constexpr BitBoard rank_bbs[] = {bb_r1, bb_r2, bb_r3, bb_r4, bb_r5};

constexpr BitBoard bb_f1 = bb_11 | bb_12 | bb_13 | bb_14 | bb_15;
constexpr BitBoard bb_f2 = bb_21 | bb_22 | bb_23 | bb_24 | bb_25;
constexpr BitBoard bb_f3 = bb_31 | bb_32 | bb_33 | bb_34 | bb_35;
constexpr BitBoard bb_f4 = bb_41 | bb_42 | bb_43 | bb_44 | bb_45;
constexpr BitBoard bb_f5 = bb_51 | bb_52 | bb_53 | bb_54 | bb_55;
constexpr BitBoard file_bbs[] = {bb_f1, bb_f2, bb_f3, bb_f4, bb_f5};

constexpr BitBoard BitBoard::shift_above() const
{
    return BitBoard(
        static_cast<std::uint32_t>((m_value & (~bb_r1.m_value)) >> 1));
}

constexpr BitBoard BitBoard::shift_below() const
{
    return BitBoard(
        static_cast<std::uint32_t>((m_value & (~bb_r5.m_value)) << 1));
}

namespace internal
{

constexpr BitBoard attacks_by_bgi_sq33 = bb_22 | bb_24 | bb_32 | bb_42 | bb_44;
constexpr BitBoard attacks_by_bgi_sq34 = attacks_by_bgi_sq33.shift_below();
constexpr BitBoard attacks_by_bgi_sq35 = attacks_by_bgi_sq34.shift_below();
constexpr BitBoard attacks_by_bgi_sq32 = attacks_by_bgi_sq33.shift_above();
constexpr BitBoard attacks_by_bgi_sq31 = attacks_by_bgi_sq32.shift_below();
constexpr BitBoard attacks_by_bgi_sq21 = attacks_by_bgi_sq31.shift_right();
constexpr BitBoard attacks_by_bgi_sq22 = attacks_by_bgi_sq32.shift_right();
constexpr BitBoard attacks_by_bgi_sq23 = attacks_by_bgi_sq33.shift_right();
constexpr BitBoard attacks_by_bgi_sq24 = attacks_by_bgi_sq34.shift_right();
constexpr BitBoard attacks_by_bgi_sq25 = attacks_by_bgi_sq35.shift_right();
constexpr BitBoard attacks_by_bgi_sq11 = attacks_by_bgi_sq21.shift_right();
constexpr BitBoard attacks_by_bgi_sq12 = attacks_by_bgi_sq22.shift_right();
constexpr BitBoard attacks_by_bgi_sq13 = attacks_by_bgi_sq23.shift_right();
constexpr BitBoard attacks_by_bgi_sq14 = attacks_by_bgi_sq24.shift_right();
constexpr BitBoard attacks_by_bgi_sq15 = attacks_by_bgi_sq25.shift_right();
constexpr BitBoard attacks_by_bgi_sq41 = attacks_by_bgi_sq31.shift_left();
constexpr BitBoard attacks_by_bgi_sq42 = attacks_by_bgi_sq32.shift_left();
constexpr BitBoard attacks_by_bgi_sq43 = attacks_by_bgi_sq33.shift_left();
constexpr BitBoard attacks_by_bgi_sq44 = attacks_by_bgi_sq34.shift_left();
constexpr BitBoard attacks_by_bgi_sq45 = attacks_by_bgi_sq35.shift_left();
constexpr BitBoard attacks_by_bgi_sq51 = attacks_by_bgi_sq41.shift_left();
constexpr BitBoard attacks_by_bgi_sq52 = attacks_by_bgi_sq42.shift_left();
constexpr BitBoard attacks_by_bgi_sq53 = attacks_by_bgi_sq43.shift_left();
constexpr BitBoard attacks_by_bgi_sq54 = attacks_by_bgi_sq44.shift_left();
constexpr BitBoard attacks_by_bgi_sq55 = attacks_by_bgi_sq45.shift_left();

constexpr BitBoard attacks_by_wgi_sq33 = bb_22 | bb_24 | bb_34 | bb_42 | bb_44;
constexpr BitBoard attacks_by_wgi_sq34 = attacks_by_wgi_sq33.shift_below();
constexpr BitBoard attacks_by_wgi_sq35 = attacks_by_wgi_sq34.shift_below();
constexpr BitBoard attacks_by_wgi_sq32 = attacks_by_wgi_sq33.shift_above();
constexpr BitBoard attacks_by_wgi_sq31 = attacks_by_wgi_sq32.shift_below();
constexpr BitBoard attacks_by_wgi_sq21 = attacks_by_wgi_sq31.shift_right();
constexpr BitBoard attacks_by_wgi_sq22 = attacks_by_wgi_sq32.shift_right();
constexpr BitBoard attacks_by_wgi_sq23 = attacks_by_wgi_sq33.shift_right();
constexpr BitBoard attacks_by_wgi_sq24 = attacks_by_wgi_sq34.shift_right();
constexpr BitBoard attacks_by_wgi_sq25 = attacks_by_wgi_sq35.shift_right();
constexpr BitBoard attacks_by_wgi_sq11 = attacks_by_wgi_sq21.shift_right();
constexpr BitBoard attacks_by_wgi_sq12 = attacks_by_wgi_sq22.shift_right();
constexpr BitBoard attacks_by_wgi_sq13 = attacks_by_wgi_sq23.shift_right();
constexpr BitBoard attacks_by_wgi_sq14 = attacks_by_wgi_sq24.shift_right();
constexpr BitBoard attacks_by_wgi_sq15 = attacks_by_wgi_sq25.shift_right();
constexpr BitBoard attacks_by_wgi_sq41 = attacks_by_wgi_sq31.shift_left();
constexpr BitBoard attacks_by_wgi_sq42 = attacks_by_wgi_sq32.shift_left();
constexpr BitBoard attacks_by_wgi_sq43 = attacks_by_wgi_sq33.shift_left();
constexpr BitBoard attacks_by_wgi_sq44 = attacks_by_wgi_sq34.shift_left();
constexpr BitBoard attacks_by_wgi_sq45 = attacks_by_wgi_sq35.shift_left();
constexpr BitBoard attacks_by_wgi_sq51 = attacks_by_wgi_sq41.shift_left();
constexpr BitBoard attacks_by_wgi_sq52 = attacks_by_wgi_sq42.shift_left();
constexpr BitBoard attacks_by_wgi_sq53 = attacks_by_wgi_sq43.shift_left();
constexpr BitBoard attacks_by_wgi_sq54 = attacks_by_wgi_sq44.shift_left();
constexpr BitBoard attacks_by_wgi_sq55 = attacks_by_wgi_sq45.shift_left();

constexpr BitBoard attacks_by_bki_sq33
    = bb_22 | bb_23 | bb_32 | bb_34 | bb_42 | bb_43;
constexpr BitBoard attacks_by_bki_sq32 = attacks_by_bki_sq33.shift_above();
constexpr BitBoard attacks_by_bki_sq31 = attacks_by_bki_sq32.shift_above();
constexpr BitBoard attacks_by_bki_sq34 = attacks_by_bki_sq33.shift_below();
constexpr BitBoard attacks_by_bki_sq35 = attacks_by_bki_sq34.shift_below();
constexpr BitBoard attacks_by_bki_sq21 = attacks_by_bki_sq31.shift_right();
constexpr BitBoard attacks_by_bki_sq22 = attacks_by_bki_sq32.shift_right();
constexpr BitBoard attacks_by_bki_sq23 = attacks_by_bki_sq33.shift_right();
constexpr BitBoard attacks_by_bki_sq24 = attacks_by_bki_sq34.shift_right();
constexpr BitBoard attacks_by_bki_sq25 = attacks_by_bki_sq35.shift_right();
constexpr BitBoard attacks_by_bki_sq11 = attacks_by_bki_sq21.shift_right();
constexpr BitBoard attacks_by_bki_sq12 = attacks_by_bki_sq22.shift_right();
constexpr BitBoard attacks_by_bki_sq13 = attacks_by_bki_sq23.shift_right();
constexpr BitBoard attacks_by_bki_sq14 = attacks_by_bki_sq24.shift_right();
constexpr BitBoard attacks_by_bki_sq15 = attacks_by_bki_sq25.shift_right();
constexpr BitBoard attacks_by_bki_sq41 = attacks_by_bki_sq31.shift_left();
constexpr BitBoard attacks_by_bki_sq42 = attacks_by_bki_sq32.shift_left();
constexpr BitBoard attacks_by_bki_sq43 = attacks_by_bki_sq33.shift_left();
constexpr BitBoard attacks_by_bki_sq44 = attacks_by_bki_sq34.shift_left();
constexpr BitBoard attacks_by_bki_sq45 = attacks_by_bki_sq35.shift_left();
constexpr BitBoard attacks_by_bki_sq51 = attacks_by_bki_sq41.shift_left();
constexpr BitBoard attacks_by_bki_sq52 = attacks_by_bki_sq42.shift_left();
constexpr BitBoard attacks_by_bki_sq53 = attacks_by_bki_sq43.shift_left();
constexpr BitBoard attacks_by_bki_sq54 = attacks_by_bki_sq44.shift_left();
constexpr BitBoard attacks_by_bki_sq55 = attacks_by_bki_sq45.shift_left();

constexpr BitBoard attacks_by_wki_sq33
    = bb_23 | bb_24 | bb_32 | bb_34 | bb_43 | bb_44;
constexpr BitBoard attacks_by_wki_sq32 = attacks_by_wki_sq33.shift_above();
constexpr BitBoard attacks_by_wki_sq31 = attacks_by_wki_sq32.shift_above();
constexpr BitBoard attacks_by_wki_sq34 = attacks_by_wki_sq33.shift_below();
constexpr BitBoard attacks_by_wki_sq35 = attacks_by_wki_sq34.shift_below();
constexpr BitBoard attacks_by_wki_sq21 = attacks_by_wki_sq31.shift_right();
constexpr BitBoard attacks_by_wki_sq22 = attacks_by_wki_sq32.shift_right();
constexpr BitBoard attacks_by_wki_sq23 = attacks_by_wki_sq33.shift_right();
constexpr BitBoard attacks_by_wki_sq24 = attacks_by_wki_sq34.shift_right();
constexpr BitBoard attacks_by_wki_sq25 = attacks_by_wki_sq35.shift_right();
constexpr BitBoard attacks_by_wki_sq11 = attacks_by_wki_sq21.shift_right();
constexpr BitBoard attacks_by_wki_sq12 = attacks_by_wki_sq22.shift_right();
constexpr BitBoard attacks_by_wki_sq13 = attacks_by_wki_sq23.shift_right();
constexpr BitBoard attacks_by_wki_sq14 = attacks_by_wki_sq24.shift_right();
constexpr BitBoard attacks_by_wki_sq15 = attacks_by_wki_sq25.shift_right();
constexpr BitBoard attacks_by_wki_sq41 = attacks_by_wki_sq31.shift_left();
constexpr BitBoard attacks_by_wki_sq42 = attacks_by_wki_sq32.shift_left();
constexpr BitBoard attacks_by_wki_sq43 = attacks_by_wki_sq33.shift_left();
constexpr BitBoard attacks_by_wki_sq44 = attacks_by_wki_sq34.shift_left();
constexpr BitBoard attacks_by_wki_sq45 = attacks_by_wki_sq35.shift_left();
constexpr BitBoard attacks_by_wki_sq51 = attacks_by_wki_sq41.shift_left();
constexpr BitBoard attacks_by_wki_sq52 = attacks_by_wki_sq42.shift_left();
constexpr BitBoard attacks_by_wki_sq53 = attacks_by_wki_sq43.shift_left();
constexpr BitBoard attacks_by_wki_sq54 = attacks_by_wki_sq44.shift_left();
constexpr BitBoard attacks_by_wki_sq55 = attacks_by_wki_sq45.shift_left();

} // namespace internal

constexpr BitBoard attacks_by_fu[num_squares][num_colors] = {
    {bb_11.shift_above(), bb_11.shift_below()},
    {bb_12.shift_above(), bb_12.shift_below()},
    {bb_13.shift_above(), bb_13.shift_below()},
    {bb_14.shift_above(), bb_14.shift_below()},
    {bb_15.shift_above(), bb_15.shift_below()},
    {bb_21.shift_above(), bb_21.shift_below()},
    {bb_22.shift_above(), bb_22.shift_below()},
    {bb_23.shift_above(), bb_23.shift_below()},
    {bb_24.shift_above(), bb_24.shift_below()},
    {bb_25.shift_above(), bb_25.shift_below()},
    {bb_31.shift_above(), bb_31.shift_below()},
    {bb_32.shift_above(), bb_32.shift_below()},
    {bb_33.shift_above(), bb_33.shift_below()},
    {bb_34.shift_above(), bb_34.shift_below()},
    {bb_35.shift_above(), bb_35.shift_below()},
    {bb_41.shift_above(), bb_41.shift_below()},
    {bb_42.shift_above(), bb_42.shift_below()},
    {bb_43.shift_above(), bb_43.shift_below()},
    {bb_44.shift_above(), bb_44.shift_below()},
    {bb_45.shift_above(), bb_45.shift_below()},
    {bb_51.shift_above(), bb_51.shift_below()},
    {bb_52.shift_above(), bb_52.shift_below()},
    {bb_53.shift_above(), bb_53.shift_below()},
    {bb_54.shift_above(), bb_54.shift_below()},
    {bb_55.shift_above(), bb_55.shift_below()},
};
constexpr BitBoard attacks_by_gi[num_squares][num_colors] = {
    {internal::attacks_by_bgi_sq11, internal::attacks_by_wgi_sq11},
    {internal::attacks_by_bgi_sq12, internal::attacks_by_wgi_sq12},
    {internal::attacks_by_bgi_sq13, internal::attacks_by_wgi_sq13},
    {internal::attacks_by_bgi_sq14, internal::attacks_by_wgi_sq14},
    {internal::attacks_by_bgi_sq15, internal::attacks_by_wgi_sq15},
    {internal::attacks_by_bgi_sq21, internal::attacks_by_wgi_sq21},
    {internal::attacks_by_bgi_sq22, internal::attacks_by_wgi_sq22},
    {internal::attacks_by_bgi_sq23, internal::attacks_by_wgi_sq23},
    {internal::attacks_by_bgi_sq24, internal::attacks_by_wgi_sq24},
    {internal::attacks_by_bgi_sq25, internal::attacks_by_wgi_sq25},
    {internal::attacks_by_bgi_sq31, internal::attacks_by_wgi_sq31},
    {internal::attacks_by_bgi_sq32, internal::attacks_by_wgi_sq32},
    {internal::attacks_by_bgi_sq33, internal::attacks_by_wgi_sq33},
    {internal::attacks_by_bgi_sq34, internal::attacks_by_wgi_sq34},
    {internal::attacks_by_bgi_sq35, internal::attacks_by_wgi_sq35},
    {internal::attacks_by_bgi_sq41, internal::attacks_by_wgi_sq41},
    {internal::attacks_by_bgi_sq42, internal::attacks_by_wgi_sq42},
    {internal::attacks_by_bgi_sq43, internal::attacks_by_wgi_sq43},
    {internal::attacks_by_bgi_sq44, internal::attacks_by_wgi_sq44},
    {internal::attacks_by_bgi_sq45, internal::attacks_by_wgi_sq45},
    {internal::attacks_by_bgi_sq51, internal::attacks_by_wgi_sq51},
    {internal::attacks_by_bgi_sq52, internal::attacks_by_wgi_sq52},
    {internal::attacks_by_bgi_sq53, internal::attacks_by_wgi_sq53},
    {internal::attacks_by_bgi_sq54, internal::attacks_by_wgi_sq54},
    {internal::attacks_by_bgi_sq55, internal::attacks_by_wgi_sq55},
};
constexpr BitBoard attacks_by_ki[num_squares][num_colors] = {
    {internal::attacks_by_bki_sq11, internal::attacks_by_wki_sq11},
    {internal::attacks_by_bki_sq12, internal::attacks_by_wki_sq12},
    {internal::attacks_by_bki_sq13, internal::attacks_by_wki_sq13},
    {internal::attacks_by_bki_sq14, internal::attacks_by_wki_sq14},
    {internal::attacks_by_bki_sq15, internal::attacks_by_wki_sq15},
    {internal::attacks_by_bki_sq21, internal::attacks_by_wki_sq21},
    {internal::attacks_by_bki_sq22, internal::attacks_by_wki_sq22},
    {internal::attacks_by_bki_sq23, internal::attacks_by_wki_sq23},
    {internal::attacks_by_bki_sq24, internal::attacks_by_wki_sq24},
    {internal::attacks_by_bki_sq25, internal::attacks_by_wki_sq25},
    {internal::attacks_by_bki_sq31, internal::attacks_by_wki_sq31},
    {internal::attacks_by_bki_sq32, internal::attacks_by_wki_sq32},
    {internal::attacks_by_bki_sq33, internal::attacks_by_wki_sq33},
    {internal::attacks_by_bki_sq34, internal::attacks_by_wki_sq34},
    {internal::attacks_by_bki_sq35, internal::attacks_by_wki_sq35},
    {internal::attacks_by_bki_sq41, internal::attacks_by_wki_sq41},
    {internal::attacks_by_bki_sq42, internal::attacks_by_wki_sq42},
    {internal::attacks_by_bki_sq43, internal::attacks_by_wki_sq43},
    {internal::attacks_by_bki_sq44, internal::attacks_by_wki_sq44},
    {internal::attacks_by_bki_sq45, internal::attacks_by_wki_sq45},
    {internal::attacks_by_bki_sq51, internal::attacks_by_wki_sq51},
    {internal::attacks_by_bki_sq52, internal::attacks_by_wki_sq52},
    {internal::attacks_by_bki_sq53, internal::attacks_by_wki_sq53},
    {internal::attacks_by_bki_sq54, internal::attacks_by_wki_sq54},
    {internal::attacks_by_bki_sq55, internal::attacks_by_wki_sq55},
};
constexpr BitBoard attacks_by_ou[num_squares] = {
    bb_11.expand() & (~bb_11), bb_12.expand() & (~bb_12),
    bb_13.expand() & (~bb_13), bb_14.expand() & (~bb_14),
    bb_15.expand() & (~bb_15), bb_21.expand() & (~bb_21),
    bb_22.expand() & (~bb_22), bb_23.expand() & (~bb_23),
    bb_24.expand() & (~bb_24), bb_25.expand() & (~bb_25),
    bb_31.expand() & (~bb_31), bb_32.expand() & (~bb_32),
    bb_33.expand() & (~bb_33), bb_34.expand() & (~bb_34),
    bb_35.expand() & (~bb_35), bb_41.expand() & (~bb_41),
    bb_42.expand() & (~bb_42), bb_43.expand() & (~bb_43),
    bb_44.expand() & (~bb_44), bb_45.expand() & (~bb_45),
    bb_51.expand() & (~bb_51), bb_52.expand() & (~bb_52),
    bb_53.expand() & (~bb_53), bb_54.expand() & (~bb_54),
    bb_55.expand() & (~bb_55),
};

inline BitBoard get_attacks_by(
    const BoardPieceTypeEnum piece,
    const SquareEnum location,
    const BitBoard occupied = BitBoard(0))
{
    const auto piece_type = to_piece_type(piece);
    const auto color = get_color(piece);
    switch (piece_type) {
    case FU:
        return attacks_by_fu[location][color];
    case GI:
        return attacks_by_gi[location][color];
    case KA:
        return BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case HI:
        return BitBoard::ranging_attacks_to_adjacent(location, occupied);
    case KI:
    case TO:
    case NG:
        return attacks_by_ki[location][color];
    case UM:
        return attacks_by_ou[location]
               | BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case RY:
        return attacks_by_ou[location]
               | BitBoard::ranging_attacks_to_adjacent(location, occupied);
    case OU:
        return attacks_by_ou[location];
    default:
        break;
    }
    return BitBoard(0);
}

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_BITBOARD_HPP
