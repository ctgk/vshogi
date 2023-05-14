#ifndef VSHOGI_MINISHOGI_BITBOARD_HPP
#define VSHOGI_MINISHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/color.hpp"
#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"

namespace vshogi::minishogi
{

/**
 * @brief N-bit binary board representing ON and OFF of each square.
 * @details
 *              +--------------------------------- SQ_1E
 *              |                     +----------- SQ_5B
 *              |                     |+---------- SQ_1A
 *              |                     ||  +------- SQ_4A
 *              |                     ||  |+------ SQ_5A
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
    constexpr std::uint32_t get_value() const
    {
        return m_value;
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
    constexpr BitBoard operator<<(const int shift_width) const
    {
        return (shift_width > 0) ? BitBoard(
                   static_cast<std::uint32_t>(m_value << shift_width))
                                 : BitBoard(static_cast<std::uint32_t>(
                                     m_value >> -shift_width));
    }
    bool operator==(const BitBoard& other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const BitBoard other) const
    {
        return m_value != other.m_value;
    }
    template <DirectionEnum D>
    constexpr BitBoard shift() const
    {
        constexpr auto bb_f1234
            = ~(BitBoard::from_square(SQ_5A) | BitBoard::from_square(SQ_5B)
                | BitBoard::from_square(SQ_5C) | BitBoard::from_square(SQ_5D)
                | BitBoard::from_square(SQ_5E));
        constexpr auto bb_f2345
            = ~(BitBoard::from_square(SQ_1A) | BitBoard::from_square(SQ_1B)
                | BitBoard::from_square(SQ_1C) | BitBoard::from_square(SQ_1D)
                | BitBoard::from_square(SQ_1E));
        constexpr BitBoard mask[] = {
            // clang-format off
            bb_f1234, ~BitBoard(0), bb_f2345,
            bb_f1234,               bb_f2345,
            bb_f1234, ~BitBoard(0), bb_f2345,
            // clang-format on
        };
        return (*this & mask[D]) << direction_to_delta(D);
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
        return *this | shift<DIR_N>() | shift<DIR_E>() | shift<DIR_S>()
               | shift<DIR_W>();
    }
    constexpr BitBoard expand_diagonally() const
    {
        return *this | shift<DIR_NW>() | shift<DIR_NE>() | shift<DIR_SW>()
               | shift<DIR_SE>();
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
        return ranging_attacks_to<DIR_NW>(sq, occupied)
               | ranging_attacks_to<DIR_NE>(sq, occupied)
               | ranging_attacks_to<DIR_SW>(sq, occupied)
               | ranging_attacks_to<DIR_SE>(sq, occupied);
    }
    static BitBoard
    ranging_attacks_to_adjacent(const SquareEnum sq, const BitBoard occupied)
    {
        return ranging_attacks_to<DIR_N>(sq, occupied)
               | ranging_attacks_to<DIR_E>(sq, occupied)
               | ranging_attacks_to<DIR_W>(sq, occupied)
               | ranging_attacks_to<DIR_S>(sq, occupied);
    }
};

constexpr BitBoard bb_1a = BitBoard::from_square(SQ_1A);
constexpr BitBoard bb_1b = BitBoard::from_square(SQ_1B);
constexpr BitBoard bb_1c = BitBoard::from_square(SQ_1C);
constexpr BitBoard bb_1d = BitBoard::from_square(SQ_1D);
constexpr BitBoard bb_1e = BitBoard::from_square(SQ_1E);
constexpr BitBoard bb_2a = BitBoard::from_square(SQ_2A);
constexpr BitBoard bb_2b = BitBoard::from_square(SQ_2B);
constexpr BitBoard bb_2c = BitBoard::from_square(SQ_2C);
constexpr BitBoard bb_2d = BitBoard::from_square(SQ_2D);
constexpr BitBoard bb_2e = BitBoard::from_square(SQ_2E);
constexpr BitBoard bb_3a = BitBoard::from_square(SQ_3A);
constexpr BitBoard bb_3b = BitBoard::from_square(SQ_3B);
constexpr BitBoard bb_3c = BitBoard::from_square(SQ_3C);
constexpr BitBoard bb_3d = BitBoard::from_square(SQ_3D);
constexpr BitBoard bb_3e = BitBoard::from_square(SQ_3E);
constexpr BitBoard bb_4a = BitBoard::from_square(SQ_4A);
constexpr BitBoard bb_4b = BitBoard::from_square(SQ_4B);
constexpr BitBoard bb_4c = BitBoard::from_square(SQ_4C);
constexpr BitBoard bb_4d = BitBoard::from_square(SQ_4D);
constexpr BitBoard bb_4e = BitBoard::from_square(SQ_4E);
constexpr BitBoard bb_5a = BitBoard::from_square(SQ_5A);
constexpr BitBoard bb_5b = BitBoard::from_square(SQ_5B);
constexpr BitBoard bb_5c = BitBoard::from_square(SQ_5C);
constexpr BitBoard bb_5d = BitBoard::from_square(SQ_5D);
constexpr BitBoard bb_5e = BitBoard::from_square(SQ_5E);

constexpr BitBoard bb_ra = bb_1a | bb_2a | bb_3a | bb_4a | bb_5a;
constexpr BitBoard bb_rb = bb_1b | bb_2b | bb_3b | bb_4b | bb_5b;
constexpr BitBoard bb_rc = bb_1c | bb_2c | bb_3c | bb_4c | bb_5c;
constexpr BitBoard bb_rd = bb_1d | bb_2d | bb_3d | bb_4d | bb_5d;
constexpr BitBoard bb_re = bb_1e | bb_2e | bb_3e | bb_4e | bb_5e;
constexpr BitBoard rank_bbs[] = {bb_ra, bb_rb, bb_rc, bb_rd, bb_re};

constexpr BitBoard bb_f1 = bb_1a | bb_1b | bb_1c | bb_1d | bb_1e;
constexpr BitBoard bb_f2 = bb_2a | bb_2b | bb_2c | bb_2d | bb_2e;
constexpr BitBoard bb_f3 = bb_3a | bb_3b | bb_3c | bb_3d | bb_3e;
constexpr BitBoard bb_f4 = bb_4a | bb_4b | bb_4c | bb_4d | bb_4e;
constexpr BitBoard bb_f5 = bb_5a | bb_5b | bb_5c | bb_5d | bb_5e;
constexpr BitBoard file_bbs[] = {bb_f1, bb_f2, bb_f3, bb_f4, bb_f5};

namespace internal
{

constexpr BitBoard attacks_by_bgi_sq3c = bb_2b | bb_2d | bb_3b | bb_4b | bb_4d;
constexpr BitBoard attacks_by_bgi_sq3d = attacks_by_bgi_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq3e = attacks_by_bgi_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq3b = attacks_by_bgi_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq3a = attacks_by_bgi_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq2a = attacks_by_bgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2b = attacks_by_bgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2c = attacks_by_bgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2d = attacks_by_bgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2e = attacks_by_bgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1a = attacks_by_bgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1b = attacks_by_bgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1c = attacks_by_bgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1d = attacks_by_bgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1e = attacks_by_bgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4a = attacks_by_bgi_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4b = attacks_by_bgi_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4c = attacks_by_bgi_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4d = attacks_by_bgi_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4e = attacks_by_bgi_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5a = attacks_by_bgi_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5b = attacks_by_bgi_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5c = attacks_by_bgi_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5d = attacks_by_bgi_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5e = attacks_by_bgi_sq4e.shift<DIR_W>();

constexpr BitBoard attacks_by_wgi_sq3c = bb_2b | bb_2d | bb_3d | bb_4b | bb_4d;
constexpr BitBoard attacks_by_wgi_sq3d = attacks_by_wgi_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq3e = attacks_by_wgi_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq3b = attacks_by_wgi_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq3a = attacks_by_wgi_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq2a = attacks_by_wgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2b = attacks_by_wgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2c = attacks_by_wgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2d = attacks_by_wgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2e = attacks_by_wgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1a = attacks_by_wgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1b = attacks_by_wgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1c = attacks_by_wgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1d = attacks_by_wgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1e = attacks_by_wgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4a = attacks_by_wgi_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4b = attacks_by_wgi_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4c = attacks_by_wgi_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4d = attacks_by_wgi_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4e = attacks_by_wgi_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5a = attacks_by_wgi_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5b = attacks_by_wgi_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5c = attacks_by_wgi_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5d = attacks_by_wgi_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5e = attacks_by_wgi_sq4e.shift<DIR_W>();

constexpr BitBoard attacks_by_bki_sq3c
    = bb_2b | bb_2c | bb_3b | bb_3d | bb_4b | bb_4c;
constexpr BitBoard attacks_by_bki_sq3b = attacks_by_bki_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq3a = attacks_by_bki_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq3d = attacks_by_bki_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq3e = attacks_by_bki_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq2a = attacks_by_bki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2b = attacks_by_bki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2c = attacks_by_bki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2d = attacks_by_bki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2e = attacks_by_bki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1a = attacks_by_bki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1b = attacks_by_bki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1c = attacks_by_bki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1d = attacks_by_bki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1e = attacks_by_bki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4a = attacks_by_bki_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4b = attacks_by_bki_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4c = attacks_by_bki_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4d = attacks_by_bki_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4e = attacks_by_bki_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5a = attacks_by_bki_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5b = attacks_by_bki_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5c = attacks_by_bki_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5d = attacks_by_bki_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5e = attacks_by_bki_sq4e.shift<DIR_W>();

constexpr BitBoard attacks_by_wki_sq3c
    = bb_2c | bb_2d | bb_3b | bb_3d | bb_4c | bb_4d;
constexpr BitBoard attacks_by_wki_sq3b = attacks_by_wki_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq3a = attacks_by_wki_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq3d = attacks_by_wki_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq3e = attacks_by_wki_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq2a = attacks_by_wki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2b = attacks_by_wki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2c = attacks_by_wki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2d = attacks_by_wki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2e = attacks_by_wki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1a = attacks_by_wki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1b = attacks_by_wki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1c = attacks_by_wki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1d = attacks_by_wki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1e = attacks_by_wki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4a = attacks_by_wki_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4b = attacks_by_wki_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4c = attacks_by_wki_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4d = attacks_by_wki_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4e = attacks_by_wki_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5a = attacks_by_wki_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5b = attacks_by_wki_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5c = attacks_by_wki_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5d = attacks_by_wki_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5e = attacks_by_wki_sq4e.shift<DIR_W>();

} // namespace internal

constexpr BitBoard attacks_by_fu[num_squares][num_colors] = {
    {bb_5a.shift<DIR_N>(), bb_5a.shift<DIR_S>()},
    {bb_4a.shift<DIR_N>(), bb_4a.shift<DIR_S>()},
    {bb_3a.shift<DIR_N>(), bb_3a.shift<DIR_S>()},
    {bb_2a.shift<DIR_N>(), bb_2a.shift<DIR_S>()},
    {bb_1a.shift<DIR_N>(), bb_1a.shift<DIR_S>()},
    {bb_5b.shift<DIR_N>(), bb_5b.shift<DIR_S>()},
    {bb_4b.shift<DIR_N>(), bb_4b.shift<DIR_S>()},
    {bb_3b.shift<DIR_N>(), bb_3b.shift<DIR_S>()},
    {bb_2b.shift<DIR_N>(), bb_2b.shift<DIR_S>()},
    {bb_1b.shift<DIR_N>(), bb_1b.shift<DIR_S>()},
    {bb_5c.shift<DIR_N>(), bb_5c.shift<DIR_S>()},
    {bb_4c.shift<DIR_N>(), bb_4c.shift<DIR_S>()},
    {bb_3c.shift<DIR_N>(), bb_3c.shift<DIR_S>()},
    {bb_2c.shift<DIR_N>(), bb_2c.shift<DIR_S>()},
    {bb_1c.shift<DIR_N>(), bb_1c.shift<DIR_S>()},
    {bb_5d.shift<DIR_N>(), bb_5d.shift<DIR_S>()},
    {bb_4d.shift<DIR_N>(), bb_4d.shift<DIR_S>()},
    {bb_3d.shift<DIR_N>(), bb_3d.shift<DIR_S>()},
    {bb_2d.shift<DIR_N>(), bb_2d.shift<DIR_S>()},
    {bb_1d.shift<DIR_N>(), bb_1d.shift<DIR_S>()},
    {bb_5e.shift<DIR_N>(), bb_5e.shift<DIR_S>()},
    {bb_4e.shift<DIR_N>(), bb_4e.shift<DIR_S>()},
    {bb_3e.shift<DIR_N>(), bb_3e.shift<DIR_S>()},
    {bb_2e.shift<DIR_N>(), bb_2e.shift<DIR_S>()},
    {bb_1e.shift<DIR_N>(), bb_1e.shift<DIR_S>()},
};
constexpr BitBoard attacks_by_gi[num_squares][num_colors] = {
    {internal::attacks_by_bgi_sq5a, internal::attacks_by_wgi_sq5a},
    {internal::attacks_by_bgi_sq4a, internal::attacks_by_wgi_sq4a},
    {internal::attacks_by_bgi_sq3a, internal::attacks_by_wgi_sq3a},
    {internal::attacks_by_bgi_sq2a, internal::attacks_by_wgi_sq2a},
    {internal::attacks_by_bgi_sq1a, internal::attacks_by_wgi_sq1a},
    {internal::attacks_by_bgi_sq5b, internal::attacks_by_wgi_sq5b},
    {internal::attacks_by_bgi_sq4b, internal::attacks_by_wgi_sq4b},
    {internal::attacks_by_bgi_sq3b, internal::attacks_by_wgi_sq3b},
    {internal::attacks_by_bgi_sq2b, internal::attacks_by_wgi_sq2b},
    {internal::attacks_by_bgi_sq1b, internal::attacks_by_wgi_sq1b},
    {internal::attacks_by_bgi_sq5c, internal::attacks_by_wgi_sq5c},
    {internal::attacks_by_bgi_sq4c, internal::attacks_by_wgi_sq4c},
    {internal::attacks_by_bgi_sq3c, internal::attacks_by_wgi_sq3c},
    {internal::attacks_by_bgi_sq2c, internal::attacks_by_wgi_sq2c},
    {internal::attacks_by_bgi_sq1c, internal::attacks_by_wgi_sq1c},
    {internal::attacks_by_bgi_sq5d, internal::attacks_by_wgi_sq5d},
    {internal::attacks_by_bgi_sq4d, internal::attacks_by_wgi_sq4d},
    {internal::attacks_by_bgi_sq3d, internal::attacks_by_wgi_sq3d},
    {internal::attacks_by_bgi_sq2d, internal::attacks_by_wgi_sq2d},
    {internal::attacks_by_bgi_sq1d, internal::attacks_by_wgi_sq1d},
    {internal::attacks_by_bgi_sq5e, internal::attacks_by_wgi_sq5e},
    {internal::attacks_by_bgi_sq4e, internal::attacks_by_wgi_sq4e},
    {internal::attacks_by_bgi_sq3e, internal::attacks_by_wgi_sq3e},
    {internal::attacks_by_bgi_sq2e, internal::attacks_by_wgi_sq2e},
    {internal::attacks_by_bgi_sq1e, internal::attacks_by_wgi_sq1e},
};
constexpr BitBoard attacks_by_ki[num_squares][num_colors] = {
    {internal::attacks_by_bki_sq5a, internal::attacks_by_wki_sq5a},
    {internal::attacks_by_bki_sq4a, internal::attacks_by_wki_sq4a},
    {internal::attacks_by_bki_sq3a, internal::attacks_by_wki_sq3a},
    {internal::attacks_by_bki_sq2a, internal::attacks_by_wki_sq2a},
    {internal::attacks_by_bki_sq1a, internal::attacks_by_wki_sq1a},
    {internal::attacks_by_bki_sq5b, internal::attacks_by_wki_sq5b},
    {internal::attacks_by_bki_sq4b, internal::attacks_by_wki_sq4b},
    {internal::attacks_by_bki_sq3b, internal::attacks_by_wki_sq3b},
    {internal::attacks_by_bki_sq2b, internal::attacks_by_wki_sq2b},
    {internal::attacks_by_bki_sq1b, internal::attacks_by_wki_sq1b},
    {internal::attacks_by_bki_sq5c, internal::attacks_by_wki_sq5c},
    {internal::attacks_by_bki_sq4c, internal::attacks_by_wki_sq4c},
    {internal::attacks_by_bki_sq3c, internal::attacks_by_wki_sq3c},
    {internal::attacks_by_bki_sq2c, internal::attacks_by_wki_sq2c},
    {internal::attacks_by_bki_sq1c, internal::attacks_by_wki_sq1c},
    {internal::attacks_by_bki_sq5d, internal::attacks_by_wki_sq5d},
    {internal::attacks_by_bki_sq4d, internal::attacks_by_wki_sq4d},
    {internal::attacks_by_bki_sq3d, internal::attacks_by_wki_sq3d},
    {internal::attacks_by_bki_sq2d, internal::attacks_by_wki_sq2d},
    {internal::attacks_by_bki_sq1d, internal::attacks_by_wki_sq1d},
    {internal::attacks_by_bki_sq5e, internal::attacks_by_wki_sq5e},
    {internal::attacks_by_bki_sq4e, internal::attacks_by_wki_sq4e},
    {internal::attacks_by_bki_sq3e, internal::attacks_by_wki_sq3e},
    {internal::attacks_by_bki_sq2e, internal::attacks_by_wki_sq2e},
    {internal::attacks_by_bki_sq1e, internal::attacks_by_wki_sq1e},
};
constexpr BitBoard attacks_by_ou[num_squares] = {
    bb_5a.expand() & (~bb_5a), bb_4a.expand() & (~bb_4a),
    bb_3a.expand() & (~bb_3a), bb_2a.expand() & (~bb_2a),
    bb_1a.expand() & (~bb_1a), bb_5b.expand() & (~bb_5b),
    bb_4b.expand() & (~bb_4b), bb_3b.expand() & (~bb_3b),
    bb_2b.expand() & (~bb_2b), bb_1b.expand() & (~bb_1b),
    bb_5c.expand() & (~bb_5c), bb_4c.expand() & (~bb_4c),
    bb_3c.expand() & (~bb_3c), bb_2c.expand() & (~bb_2c),
    bb_1c.expand() & (~bb_1c), bb_5d.expand() & (~bb_5d),
    bb_4d.expand() & (~bb_4d), bb_3d.expand() & (~bb_3d),
    bb_2d.expand() & (~bb_2d), bb_1d.expand() & (~bb_1d),
    bb_5e.expand() & (~bb_5e), bb_4e.expand() & (~bb_4e),
    bb_3e.expand() & (~bb_3e), bb_2e.expand() & (~bb_2e),
    bb_1e.expand() & (~bb_1e),
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
