#ifndef VSHOGI_SHOGI_BITBOARD_HPP
#define VSHOGI_SHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/shogi/piece.hpp"
#include "vshogi/shogi/squares.hpp"

namespace vshogi::shogi
{

class BitBoard
{
private:
    /**
     * @brief 64-bit binary representing ON and OFF of squares from 9A to 9H.
     * @details
     *       +----------------------------------- SQ_9H
     *       |                  +---------------- SQ_9B
     *       |                  |+--------------- SQ_1A
     *       |                  ||       +------- SQ_8A
     *       |                  ||       |+------ SQ_9A
     *       |                  ||       ||
     *       v                  vv       vv
     * (MSB) xxxxxxxx ... xxxxxxxx xxxxxxxx (LSB)
     */
    std::uint64_t m_value_9a_to_9h;

    /**
     * @brief 32-bit binary representing ON and OFF of squares from 8H to 1I.
     * @details
     *                       +------------------------ 1I
     *                       | +---------------------- 2I
     *                       | |      +--------------- 9I
     *                       | |      | +------------- 1H
     *                       | |      | |     +------- 7H
     *                       | |      | |     |+------ 8H
     *                       | |      | |     ||
     *                       v v      v v     vv
     * (MSB) xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx (LSB)
     */
    std::uint32_t m_value_8h_to_1i;

    constexpr std::uint64_t
    lshift_9a_to_9h(const unsigned int shift_width) const
    {
        return (shift_width == 0)   ? m_value_9a_to_9h
               : (shift_width > 63) ? 0
                                    : (m_value_9a_to_9h << shift_width);
    }
    constexpr std::uint32_t lshift_carry(const unsigned int shift_width) const
    {
        return static_cast<std::uint32_t>(
            (shift_width == 0)   ? 0
            : (shift_width < 64) ? (m_value_9a_to_9h >> (64 - shift_width))
            : (shift_width < 96) ? (m_value_9a_to_9h << (shift_width - 64))
                                 : 0);
    }
    constexpr std::uint32_t
    lshift_8h_to_1i(const unsigned int shift_width) const
    {
        const auto c = lshift_carry(shift_width);
        const auto v = (shift_width < 32) ? m_value_8h_to_1i << shift_width : 0;
        return v | c;
    }
    constexpr std::uint64_t
    rshift_9a_to_9h(const unsigned int shift_width) const
    {
        const auto v = (shift_width == 0)   ? m_value_9a_to_9h
                       : (shift_width > 63) ? 0
                                            : (m_value_9a_to_9h >> shift_width);
        const auto c = rshift_carry(shift_width);
        return v | c;
    }
    constexpr std::uint64_t rshift_carry(const unsigned int shift_width) const
    {
        const auto v = static_cast<std::uint64_t>(m_value_8h_to_1i);
        return (shift_width == 0)    ? 0
               : (shift_width <= 64) ? (v << (64 - shift_width))
               : (shift_width <= 96) ? (v >> (shift_width - 64))
                                     : 0;
    }
    constexpr std::uint32_t
    rshift_8h_to_1i(const unsigned int shift_width) const
    {
        return (shift_width <= 32) ? (m_value_8h_to_1i >> shift_width) : 0;
    }

public:
    constexpr BitBoard() : m_value_9a_to_9h(0UL), m_value_8h_to_1i(0U)
    {
    }
    constexpr BitBoard(
        const std::uint64_t v_9a_to_9h, const std::uint32_t v_8h_to_1i)
        : m_value_9a_to_9h(v_9a_to_9h),
          m_value_8h_to_1i(v_8h_to_1i & 0x0001ffff)
    {
    }
    constexpr static BitBoard from_square(const SquareEnum sq)
    {
        return BitBoard(1, 0) << static_cast<unsigned int>(sq);
    }
    std::uint64_t get_value_9a_to_9h() const
    {
        return m_value_9a_to_9h;
    }
    std::uint32_t get_value_8h_to_1i() const
    {
        return m_value_8h_to_1i;
    }
    constexpr BitBoard operator|(const BitBoard other) const
    {
        return BitBoard(
            m_value_9a_to_9h | other.m_value_9a_to_9h,
            m_value_8h_to_1i | other.m_value_8h_to_1i);
    }
    BitBoard& operator|=(const BitBoard other)
    {
        m_value_9a_to_9h |= other.m_value_9a_to_9h;
        m_value_8h_to_1i |= other.m_value_8h_to_1i;
        return *this;
    }
    constexpr BitBoard operator&(const BitBoard other) const
    {
        return BitBoard(
            m_value_9a_to_9h & other.m_value_9a_to_9h,
            m_value_8h_to_1i & other.m_value_8h_to_1i);
    }
    constexpr BitBoard operator~() const
    {
        return BitBoard(~m_value_9a_to_9h, ~m_value_8h_to_1i);
    }
    constexpr bool any() const
    {
        return (m_value_9a_to_9h > 0) || (m_value_8h_to_1i > 0);
    }
    constexpr BitBoard operator<<(const unsigned int shift_width) const
    {
        return BitBoard(
            lshift_9a_to_9h(shift_width), lshift_8h_to_1i(shift_width));
    }
    constexpr BitBoard operator>>(const unsigned int shift_width) const
    {
        return BitBoard(
            rshift_9a_to_9h(shift_width), rshift_8h_to_1i(shift_width));
    }
    template <DirectionEnum D>
    constexpr BitBoard shift() const
    {
        constexpr auto bb_f12345678
            = ~(BitBoard::from_square(SQ_9A) | BitBoard::from_square(SQ_9B)
                | BitBoard::from_square(SQ_9C) | BitBoard::from_square(SQ_9D)
                | BitBoard::from_square(SQ_9E) | BitBoard::from_square(SQ_9F)
                | BitBoard::from_square(SQ_9G) | BitBoard::from_square(SQ_9H)
                | BitBoard::from_square(SQ_9I));
        constexpr auto bb_f23456789
            = ~(BitBoard::from_square(SQ_1A) | BitBoard::from_square(SQ_1B)
                | BitBoard::from_square(SQ_1C) | BitBoard::from_square(SQ_1D)
                | BitBoard::from_square(SQ_1E) | BitBoard::from_square(SQ_1F)
                | BitBoard::from_square(SQ_1G) | BitBoard::from_square(SQ_1H)
                | BitBoard::from_square(SQ_1I));
        constexpr BitBoard mask[] = {
            // clang-format off
            bb_f12345678, ~BitBoard(0, 0), bb_f23456789,
            bb_f12345678,                  bb_f23456789,
            bb_f12345678, ~BitBoard(0, 0), bb_f23456789,
            bb_f12345678,                  bb_f23456789,
            bb_f12345678,                  bb_f23456789,
            // clang-format on
        };
        constexpr auto delta = direction_to_delta(D);
        if constexpr (delta > 0)
            return (*this & mask[D]) << static_cast<unsigned int>(delta);
        else
            return (*this & mask[D]) >> static_cast<unsigned int>(-delta);
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
    constexpr BitBoard expand_neighbor() const
    {
        return expand_adjacently() | expand_diagonally();
    }

    /**
     * @brief Get ranging attacks.
     *
     * @tparam D Direction of the ranging attack.
     * @param sq Source of the ranging attack.
     * @param occupied Bit-board representing occupation by other pieces.
     * @return BitBoard Ranging attacks.
     */
    template <DirectionEnum D>
    static BitBoard
    ranging_attacks_to(const SquareEnum sq, const BitBoard occupied)
    {
        const auto base = BitBoard::from_square(sq);
        auto attacks = base;
        for (int i = 8; i--;) {
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
    bool is_one(const SquareEnum sq) const
    {
        if (sq < SQ_8H) {
            return static_cast<bool>(
                (1UL << static_cast<std::uint64_t>(sq)) & m_value_9a_to_9h);
        } else {
            return static_cast<bool>(
                (1U << (static_cast<std::uint32_t>(sq) - 64))
                & m_value_8h_to_1i);
        }
    }
    int hamming_weight() const
    {
        return hamming_weight_9a_to_9h() + hamming_weight_8h_to_1i();
    }

private:
    int hamming_weight_9a_to_9h() const
    {
        constexpr std::uint64_t m1 = 0x5555555555555555; // 0101...
        constexpr std::uint64_t m2 = 0x3333333333333333; // 00110011..
        constexpr std::uint64_t m4 = 0x0f0f0f0f0f0f0f0f; // 0000111100001111..
        auto x = m_value_9a_to_9h;
        x -= (x >> 1) & m1; //put count of each 2 bits into those 2 bits
        x = (x & m2) + ((x >> 2) & m2); // each 4 bits into those 4 bits
        x = (x + (x >> 4)) & m4; //put count of each 8 bits into those 8 bits
        x += x >> 8; //put count of each 16 bits into their lowest 8 bits
        x += x >> 16; //put count of each 32 bits into their lowest 8 bits
        x += x >> 32; //put count of each 64 bits into their lowest 8 bits
        return x & 0x7f;
    }
    int hamming_weight_8h_to_1i() const
    {
        // https://en.wikipedia.org/wiki/Hamming_weight
        constexpr std::uint32_t m1 = 0x55555555;
        constexpr std::uint32_t m2 = 0x33333333;
        constexpr std::uint32_t m4 = 0x0f0f0f0f;
        constexpr std::uint32_t h01 = 0x01010101;
        auto x = m_value_8h_to_1i;
        x -= (x >> 1U) & m1;
        x = (x & m2) + ((x >> 2) & m2);
        x = (x + (x >> 4)) & m4;
        x += x >> 8;
        x += x >> 16;
        return x & 0x7f;
    }
};

constexpr BitBoard bb_1a = BitBoard::from_square(SQ_1A);
constexpr BitBoard bb_1b = BitBoard::from_square(SQ_1B);
constexpr BitBoard bb_1c = BitBoard::from_square(SQ_1C);
constexpr BitBoard bb_1d = BitBoard::from_square(SQ_1D);
constexpr BitBoard bb_1e = BitBoard::from_square(SQ_1E);
constexpr BitBoard bb_1f = BitBoard::from_square(SQ_1F);
constexpr BitBoard bb_1g = BitBoard::from_square(SQ_1G);
constexpr BitBoard bb_1h = BitBoard::from_square(SQ_1H);
constexpr BitBoard bb_1i = BitBoard::from_square(SQ_1I);
constexpr BitBoard bb_2a = BitBoard::from_square(SQ_2A);
constexpr BitBoard bb_2b = BitBoard::from_square(SQ_2B);
constexpr BitBoard bb_2c = BitBoard::from_square(SQ_2C);
constexpr BitBoard bb_2d = BitBoard::from_square(SQ_2D);
constexpr BitBoard bb_2e = BitBoard::from_square(SQ_2E);
constexpr BitBoard bb_2f = BitBoard::from_square(SQ_2F);
constexpr BitBoard bb_2g = BitBoard::from_square(SQ_2G);
constexpr BitBoard bb_2h = BitBoard::from_square(SQ_2H);
constexpr BitBoard bb_2i = BitBoard::from_square(SQ_2I);
constexpr BitBoard bb_3a = BitBoard::from_square(SQ_3A);
constexpr BitBoard bb_3b = BitBoard::from_square(SQ_3B);
constexpr BitBoard bb_3c = BitBoard::from_square(SQ_3C);
constexpr BitBoard bb_3d = BitBoard::from_square(SQ_3D);
constexpr BitBoard bb_3e = BitBoard::from_square(SQ_3E);
constexpr BitBoard bb_3f = BitBoard::from_square(SQ_3F);
constexpr BitBoard bb_3g = BitBoard::from_square(SQ_3G);
constexpr BitBoard bb_3h = BitBoard::from_square(SQ_3H);
constexpr BitBoard bb_3i = BitBoard::from_square(SQ_3I);
constexpr BitBoard bb_4a = BitBoard::from_square(SQ_4A);
constexpr BitBoard bb_4b = BitBoard::from_square(SQ_4B);
constexpr BitBoard bb_4c = BitBoard::from_square(SQ_4C);
constexpr BitBoard bb_4d = BitBoard::from_square(SQ_4D);
constexpr BitBoard bb_4e = BitBoard::from_square(SQ_4E);
constexpr BitBoard bb_4f = BitBoard::from_square(SQ_4F);
constexpr BitBoard bb_4g = BitBoard::from_square(SQ_4G);
constexpr BitBoard bb_4h = BitBoard::from_square(SQ_4H);
constexpr BitBoard bb_4i = BitBoard::from_square(SQ_4I);
constexpr BitBoard bb_5a = BitBoard::from_square(SQ_5A);
constexpr BitBoard bb_5b = BitBoard::from_square(SQ_5B);
constexpr BitBoard bb_5c = BitBoard::from_square(SQ_5C);
constexpr BitBoard bb_5d = BitBoard::from_square(SQ_5D);
constexpr BitBoard bb_5e = BitBoard::from_square(SQ_5E);
constexpr BitBoard bb_5f = BitBoard::from_square(SQ_5F);
constexpr BitBoard bb_5g = BitBoard::from_square(SQ_5G);
constexpr BitBoard bb_5h = BitBoard::from_square(SQ_5H);
constexpr BitBoard bb_5i = BitBoard::from_square(SQ_5I);
constexpr BitBoard bb_6a = BitBoard::from_square(SQ_6A);
constexpr BitBoard bb_6b = BitBoard::from_square(SQ_6B);
constexpr BitBoard bb_6c = BitBoard::from_square(SQ_6C);
constexpr BitBoard bb_6d = BitBoard::from_square(SQ_6D);
constexpr BitBoard bb_6e = BitBoard::from_square(SQ_6E);
constexpr BitBoard bb_6f = BitBoard::from_square(SQ_6F);
constexpr BitBoard bb_6g = BitBoard::from_square(SQ_6G);
constexpr BitBoard bb_6h = BitBoard::from_square(SQ_6H);
constexpr BitBoard bb_6i = BitBoard::from_square(SQ_6I);
constexpr BitBoard bb_7a = BitBoard::from_square(SQ_7A);
constexpr BitBoard bb_7b = BitBoard::from_square(SQ_7B);
constexpr BitBoard bb_7c = BitBoard::from_square(SQ_7C);
constexpr BitBoard bb_7d = BitBoard::from_square(SQ_7D);
constexpr BitBoard bb_7e = BitBoard::from_square(SQ_7E);
constexpr BitBoard bb_7f = BitBoard::from_square(SQ_7F);
constexpr BitBoard bb_7g = BitBoard::from_square(SQ_7G);
constexpr BitBoard bb_7h = BitBoard::from_square(SQ_7H);
constexpr BitBoard bb_7i = BitBoard::from_square(SQ_7I);
constexpr BitBoard bb_8a = BitBoard::from_square(SQ_8A);
constexpr BitBoard bb_8b = BitBoard::from_square(SQ_8B);
constexpr BitBoard bb_8c = BitBoard::from_square(SQ_8C);
constexpr BitBoard bb_8d = BitBoard::from_square(SQ_8D);
constexpr BitBoard bb_8e = BitBoard::from_square(SQ_8E);
constexpr BitBoard bb_8f = BitBoard::from_square(SQ_8F);
constexpr BitBoard bb_8g = BitBoard::from_square(SQ_8G);
constexpr BitBoard bb_8h = BitBoard::from_square(SQ_8H);
constexpr BitBoard bb_8i = BitBoard::from_square(SQ_8I);
constexpr BitBoard bb_9a = BitBoard::from_square(SQ_9A);
constexpr BitBoard bb_9b = BitBoard::from_square(SQ_9B);
constexpr BitBoard bb_9c = BitBoard::from_square(SQ_9C);
constexpr BitBoard bb_9d = BitBoard::from_square(SQ_9D);
constexpr BitBoard bb_9e = BitBoard::from_square(SQ_9E);
constexpr BitBoard bb_9f = BitBoard::from_square(SQ_9F);
constexpr BitBoard bb_9g = BitBoard::from_square(SQ_9G);
constexpr BitBoard bb_9h = BitBoard::from_square(SQ_9H);
constexpr BitBoard bb_9i = BitBoard::from_square(SQ_9I);

constexpr BitBoard attacks_by_fu[num_squares][num_colors] = {
    {bb_9a.shift<DIR_N>(), bb_9a.shift<DIR_S>()},
    {bb_8a.shift<DIR_N>(), bb_8a.shift<DIR_S>()},
    {bb_7a.shift<DIR_N>(), bb_7a.shift<DIR_S>()},
    {bb_6a.shift<DIR_N>(), bb_6a.shift<DIR_S>()},
    {bb_5a.shift<DIR_N>(), bb_5a.shift<DIR_S>()},
    {bb_4a.shift<DIR_N>(), bb_4a.shift<DIR_S>()},
    {bb_3a.shift<DIR_N>(), bb_3a.shift<DIR_S>()},
    {bb_2a.shift<DIR_N>(), bb_2a.shift<DIR_S>()},
    {bb_1a.shift<DIR_N>(), bb_1a.shift<DIR_S>()},
    {bb_9b.shift<DIR_N>(), bb_9b.shift<DIR_S>()},
    {bb_8b.shift<DIR_N>(), bb_8b.shift<DIR_S>()},
    {bb_7b.shift<DIR_N>(), bb_7b.shift<DIR_S>()},
    {bb_6b.shift<DIR_N>(), bb_6b.shift<DIR_S>()},
    {bb_5b.shift<DIR_N>(), bb_5b.shift<DIR_S>()},
    {bb_4b.shift<DIR_N>(), bb_4b.shift<DIR_S>()},
    {bb_3b.shift<DIR_N>(), bb_3b.shift<DIR_S>()},
    {bb_2b.shift<DIR_N>(), bb_2b.shift<DIR_S>()},
    {bb_1b.shift<DIR_N>(), bb_1b.shift<DIR_S>()},
    {bb_9c.shift<DIR_N>(), bb_9c.shift<DIR_S>()},
    {bb_8c.shift<DIR_N>(), bb_8c.shift<DIR_S>()},
    {bb_7c.shift<DIR_N>(), bb_7c.shift<DIR_S>()},
    {bb_6c.shift<DIR_N>(), bb_6c.shift<DIR_S>()},
    {bb_5c.shift<DIR_N>(), bb_5c.shift<DIR_S>()},
    {bb_4c.shift<DIR_N>(), bb_4c.shift<DIR_S>()},
    {bb_3c.shift<DIR_N>(), bb_3c.shift<DIR_S>()},
    {bb_2c.shift<DIR_N>(), bb_2c.shift<DIR_S>()},
    {bb_1c.shift<DIR_N>(), bb_1c.shift<DIR_S>()},
    {bb_9d.shift<DIR_N>(), bb_9d.shift<DIR_S>()},
    {bb_8d.shift<DIR_N>(), bb_8d.shift<DIR_S>()},
    {bb_7d.shift<DIR_N>(), bb_7d.shift<DIR_S>()},
    {bb_6d.shift<DIR_N>(), bb_6d.shift<DIR_S>()},
    {bb_5d.shift<DIR_N>(), bb_5d.shift<DIR_S>()},
    {bb_4d.shift<DIR_N>(), bb_4d.shift<DIR_S>()},
    {bb_3d.shift<DIR_N>(), bb_3d.shift<DIR_S>()},
    {bb_2d.shift<DIR_N>(), bb_2d.shift<DIR_S>()},
    {bb_1d.shift<DIR_N>(), bb_1d.shift<DIR_S>()},
    {bb_9e.shift<DIR_N>(), bb_9e.shift<DIR_S>()},
    {bb_8e.shift<DIR_N>(), bb_8e.shift<DIR_S>()},
    {bb_7e.shift<DIR_N>(), bb_7e.shift<DIR_S>()},
    {bb_6e.shift<DIR_N>(), bb_6e.shift<DIR_S>()},
    {bb_5e.shift<DIR_N>(), bb_5e.shift<DIR_S>()},
    {bb_4e.shift<DIR_N>(), bb_4e.shift<DIR_S>()},
    {bb_3e.shift<DIR_N>(), bb_3e.shift<DIR_S>()},
    {bb_2e.shift<DIR_N>(), bb_2e.shift<DIR_S>()},
    {bb_1e.shift<DIR_N>(), bb_1e.shift<DIR_S>()},
    {bb_9f.shift<DIR_N>(), bb_9f.shift<DIR_S>()},
    {bb_8f.shift<DIR_N>(), bb_8f.shift<DIR_S>()},
    {bb_7f.shift<DIR_N>(), bb_7f.shift<DIR_S>()},
    {bb_6f.shift<DIR_N>(), bb_6f.shift<DIR_S>()},
    {bb_5f.shift<DIR_N>(), bb_5f.shift<DIR_S>()},
    {bb_4f.shift<DIR_N>(), bb_4f.shift<DIR_S>()},
    {bb_3f.shift<DIR_N>(), bb_3f.shift<DIR_S>()},
    {bb_2f.shift<DIR_N>(), bb_2f.shift<DIR_S>()},
    {bb_1f.shift<DIR_N>(), bb_1f.shift<DIR_S>()},
    {bb_9g.shift<DIR_N>(), bb_9g.shift<DIR_S>()},
    {bb_8g.shift<DIR_N>(), bb_8g.shift<DIR_S>()},
    {bb_7g.shift<DIR_N>(), bb_7g.shift<DIR_S>()},
    {bb_6g.shift<DIR_N>(), bb_6g.shift<DIR_S>()},
    {bb_5g.shift<DIR_N>(), bb_5g.shift<DIR_S>()},
    {bb_4g.shift<DIR_N>(), bb_4g.shift<DIR_S>()},
    {bb_3g.shift<DIR_N>(), bb_3g.shift<DIR_S>()},
    {bb_2g.shift<DIR_N>(), bb_2g.shift<DIR_S>()},
    {bb_1g.shift<DIR_N>(), bb_1g.shift<DIR_S>()},
    {bb_9h.shift<DIR_N>(), bb_9h.shift<DIR_S>()},
    {bb_8h.shift<DIR_N>(), bb_8h.shift<DIR_S>()},
    {bb_7h.shift<DIR_N>(), bb_7h.shift<DIR_S>()},
    {bb_6h.shift<DIR_N>(), bb_6h.shift<DIR_S>()},
    {bb_5h.shift<DIR_N>(), bb_5h.shift<DIR_S>()},
    {bb_4h.shift<DIR_N>(), bb_4h.shift<DIR_S>()},
    {bb_3h.shift<DIR_N>(), bb_3h.shift<DIR_S>()},
    {bb_2h.shift<DIR_N>(), bb_2h.shift<DIR_S>()},
    {bb_1h.shift<DIR_N>(), bb_1h.shift<DIR_S>()},
    {bb_9i.shift<DIR_N>(), bb_9i.shift<DIR_S>()},
    {bb_8i.shift<DIR_N>(), bb_8i.shift<DIR_S>()},
    {bb_7i.shift<DIR_N>(), bb_7i.shift<DIR_S>()},
    {bb_6i.shift<DIR_N>(), bb_6i.shift<DIR_S>()},
    {bb_5i.shift<DIR_N>(), bb_5i.shift<DIR_S>()},
    {bb_4i.shift<DIR_N>(), bb_4i.shift<DIR_S>()},
    {bb_3i.shift<DIR_N>(), bb_3i.shift<DIR_S>()},
    {bb_2i.shift<DIR_N>(), bb_2i.shift<DIR_S>()},
    {bb_1i.shift<DIR_N>(), bb_1i.shift<DIR_S>()},
};

constexpr BitBoard attacks_by_ke[num_squares][num_colors] = {
    {bb_9a.shift<DIR_NNW>() | bb_9a.shift<DIR_NNE>(),
     bb_9a.shift<DIR_SSW>() | bb_9a.shift<DIR_SSE>()},
    {bb_8a.shift<DIR_NNW>() | bb_8a.shift<DIR_NNE>(),
     bb_8a.shift<DIR_SSW>() | bb_8a.shift<DIR_SSE>()},
    {bb_7a.shift<DIR_NNW>() | bb_7a.shift<DIR_NNE>(),
     bb_7a.shift<DIR_SSW>() | bb_7a.shift<DIR_SSE>()},
    {bb_6a.shift<DIR_NNW>() | bb_6a.shift<DIR_NNE>(),
     bb_6a.shift<DIR_SSW>() | bb_6a.shift<DIR_SSE>()},
    {bb_5a.shift<DIR_NNW>() | bb_5a.shift<DIR_NNE>(),
     bb_5a.shift<DIR_SSW>() | bb_5a.shift<DIR_SSE>()},
    {bb_4a.shift<DIR_NNW>() | bb_4a.shift<DIR_NNE>(),
     bb_4a.shift<DIR_SSW>() | bb_4a.shift<DIR_SSE>()},
    {bb_3a.shift<DIR_NNW>() | bb_3a.shift<DIR_NNE>(),
     bb_3a.shift<DIR_SSW>() | bb_3a.shift<DIR_SSE>()},
    {bb_2a.shift<DIR_NNW>() | bb_2a.shift<DIR_NNE>(),
     bb_2a.shift<DIR_SSW>() | bb_2a.shift<DIR_SSE>()},
    {bb_1a.shift<DIR_NNW>() | bb_1a.shift<DIR_NNE>(),
     bb_1a.shift<DIR_SSW>() | bb_1a.shift<DIR_SSE>()},
    {bb_9b.shift<DIR_NNW>() | bb_9b.shift<DIR_NNE>(),
     bb_9b.shift<DIR_SSW>() | bb_9b.shift<DIR_SSE>()},
    {bb_8b.shift<DIR_NNW>() | bb_8b.shift<DIR_NNE>(),
     bb_8b.shift<DIR_SSW>() | bb_8b.shift<DIR_SSE>()},
    {bb_7b.shift<DIR_NNW>() | bb_7b.shift<DIR_NNE>(),
     bb_7b.shift<DIR_SSW>() | bb_7b.shift<DIR_SSE>()},
    {bb_6b.shift<DIR_NNW>() | bb_6b.shift<DIR_NNE>(),
     bb_6b.shift<DIR_SSW>() | bb_6b.shift<DIR_SSE>()},
    {bb_5b.shift<DIR_NNW>() | bb_5b.shift<DIR_NNE>(),
     bb_5b.shift<DIR_SSW>() | bb_5b.shift<DIR_SSE>()},
    {bb_4b.shift<DIR_NNW>() | bb_4b.shift<DIR_NNE>(),
     bb_4b.shift<DIR_SSW>() | bb_4b.shift<DIR_SSE>()},
    {bb_3b.shift<DIR_NNW>() | bb_3b.shift<DIR_NNE>(),
     bb_3b.shift<DIR_SSW>() | bb_3b.shift<DIR_SSE>()},
    {bb_2b.shift<DIR_NNW>() | bb_2b.shift<DIR_NNE>(),
     bb_2b.shift<DIR_SSW>() | bb_2b.shift<DIR_SSE>()},
    {bb_1b.shift<DIR_NNW>() | bb_1b.shift<DIR_NNE>(),
     bb_1b.shift<DIR_SSW>() | bb_1b.shift<DIR_SSE>()},
    {bb_9c.shift<DIR_NNW>() | bb_9c.shift<DIR_NNE>(),
     bb_9c.shift<DIR_SSW>() | bb_9c.shift<DIR_SSE>()},
    {bb_8c.shift<DIR_NNW>() | bb_8c.shift<DIR_NNE>(),
     bb_8c.shift<DIR_SSW>() | bb_8c.shift<DIR_SSE>()},
    {bb_7c.shift<DIR_NNW>() | bb_7c.shift<DIR_NNE>(),
     bb_7c.shift<DIR_SSW>() | bb_7c.shift<DIR_SSE>()},
    {bb_6c.shift<DIR_NNW>() | bb_6c.shift<DIR_NNE>(),
     bb_6c.shift<DIR_SSW>() | bb_6c.shift<DIR_SSE>()},
    {bb_5c.shift<DIR_NNW>() | bb_5c.shift<DIR_NNE>(),
     bb_5c.shift<DIR_SSW>() | bb_5c.shift<DIR_SSE>()},
    {bb_4c.shift<DIR_NNW>() | bb_4c.shift<DIR_NNE>(),
     bb_4c.shift<DIR_SSW>() | bb_4c.shift<DIR_SSE>()},
    {bb_3c.shift<DIR_NNW>() | bb_3c.shift<DIR_NNE>(),
     bb_3c.shift<DIR_SSW>() | bb_3c.shift<DIR_SSE>()},
    {bb_2c.shift<DIR_NNW>() | bb_2c.shift<DIR_NNE>(),
     bb_2c.shift<DIR_SSW>() | bb_2c.shift<DIR_SSE>()},
    {bb_1c.shift<DIR_NNW>() | bb_1c.shift<DIR_NNE>(),
     bb_1c.shift<DIR_SSW>() | bb_1c.shift<DIR_SSE>()},
    {bb_9d.shift<DIR_NNW>() | bb_9d.shift<DIR_NNE>(),
     bb_9d.shift<DIR_SSW>() | bb_9d.shift<DIR_SSE>()},
    {bb_8d.shift<DIR_NNW>() | bb_8d.shift<DIR_NNE>(),
     bb_8d.shift<DIR_SSW>() | bb_8d.shift<DIR_SSE>()},
    {bb_7d.shift<DIR_NNW>() | bb_7d.shift<DIR_NNE>(),
     bb_7d.shift<DIR_SSW>() | bb_7d.shift<DIR_SSE>()},
    {bb_6d.shift<DIR_NNW>() | bb_6d.shift<DIR_NNE>(),
     bb_6d.shift<DIR_SSW>() | bb_6d.shift<DIR_SSE>()},
    {bb_5d.shift<DIR_NNW>() | bb_5d.shift<DIR_NNE>(),
     bb_5d.shift<DIR_SSW>() | bb_5d.shift<DIR_SSE>()},
    {bb_4d.shift<DIR_NNW>() | bb_4d.shift<DIR_NNE>(),
     bb_4d.shift<DIR_SSW>() | bb_4d.shift<DIR_SSE>()},
    {bb_3d.shift<DIR_NNW>() | bb_3d.shift<DIR_NNE>(),
     bb_3d.shift<DIR_SSW>() | bb_3d.shift<DIR_SSE>()},
    {bb_2d.shift<DIR_NNW>() | bb_2d.shift<DIR_NNE>(),
     bb_2d.shift<DIR_SSW>() | bb_2d.shift<DIR_SSE>()},
    {bb_1d.shift<DIR_NNW>() | bb_1d.shift<DIR_NNE>(),
     bb_1d.shift<DIR_SSW>() | bb_1d.shift<DIR_SSE>()},
    {bb_9e.shift<DIR_NNW>() | bb_9e.shift<DIR_NNE>(),
     bb_9e.shift<DIR_SSW>() | bb_9e.shift<DIR_SSE>()},
    {bb_8e.shift<DIR_NNW>() | bb_8e.shift<DIR_NNE>(),
     bb_8e.shift<DIR_SSW>() | bb_8e.shift<DIR_SSE>()},
    {bb_7e.shift<DIR_NNW>() | bb_7e.shift<DIR_NNE>(),
     bb_7e.shift<DIR_SSW>() | bb_7e.shift<DIR_SSE>()},
    {bb_6e.shift<DIR_NNW>() | bb_6e.shift<DIR_NNE>(),
     bb_6e.shift<DIR_SSW>() | bb_6e.shift<DIR_SSE>()},
    {bb_5e.shift<DIR_NNW>() | bb_5e.shift<DIR_NNE>(),
     bb_5e.shift<DIR_SSW>() | bb_5e.shift<DIR_SSE>()},
    {bb_4e.shift<DIR_NNW>() | bb_4e.shift<DIR_NNE>(),
     bb_4e.shift<DIR_SSW>() | bb_4e.shift<DIR_SSE>()},
    {bb_3e.shift<DIR_NNW>() | bb_3e.shift<DIR_NNE>(),
     bb_3e.shift<DIR_SSW>() | bb_3e.shift<DIR_SSE>()},
    {bb_2e.shift<DIR_NNW>() | bb_2e.shift<DIR_NNE>(),
     bb_2e.shift<DIR_SSW>() | bb_2e.shift<DIR_SSE>()},
    {bb_1e.shift<DIR_NNW>() | bb_1e.shift<DIR_NNE>(),
     bb_1e.shift<DIR_SSW>() | bb_1e.shift<DIR_SSE>()},
    {bb_9f.shift<DIR_NNW>() | bb_9f.shift<DIR_NNE>(),
     bb_9f.shift<DIR_SSW>() | bb_9f.shift<DIR_SSE>()},
    {bb_8f.shift<DIR_NNW>() | bb_8f.shift<DIR_NNE>(),
     bb_8f.shift<DIR_SSW>() | bb_8f.shift<DIR_SSE>()},
    {bb_7f.shift<DIR_NNW>() | bb_7f.shift<DIR_NNE>(),
     bb_7f.shift<DIR_SSW>() | bb_7f.shift<DIR_SSE>()},
    {bb_6f.shift<DIR_NNW>() | bb_6f.shift<DIR_NNE>(),
     bb_6f.shift<DIR_SSW>() | bb_6f.shift<DIR_SSE>()},
    {bb_5f.shift<DIR_NNW>() | bb_5f.shift<DIR_NNE>(),
     bb_5f.shift<DIR_SSW>() | bb_5f.shift<DIR_SSE>()},
    {bb_4f.shift<DIR_NNW>() | bb_4f.shift<DIR_NNE>(),
     bb_4f.shift<DIR_SSW>() | bb_4f.shift<DIR_SSE>()},
    {bb_3f.shift<DIR_NNW>() | bb_3f.shift<DIR_NNE>(),
     bb_3f.shift<DIR_SSW>() | bb_3f.shift<DIR_SSE>()},
    {bb_2f.shift<DIR_NNW>() | bb_2f.shift<DIR_NNE>(),
     bb_2f.shift<DIR_SSW>() | bb_2f.shift<DIR_SSE>()},
    {bb_1f.shift<DIR_NNW>() | bb_1f.shift<DIR_NNE>(),
     bb_1f.shift<DIR_SSW>() | bb_1f.shift<DIR_SSE>()},
    {bb_9g.shift<DIR_NNW>() | bb_9g.shift<DIR_NNE>(),
     bb_9g.shift<DIR_SSW>() | bb_9g.shift<DIR_SSE>()},
    {bb_8g.shift<DIR_NNW>() | bb_8g.shift<DIR_NNE>(),
     bb_8g.shift<DIR_SSW>() | bb_8g.shift<DIR_SSE>()},
    {bb_7g.shift<DIR_NNW>() | bb_7g.shift<DIR_NNE>(),
     bb_7g.shift<DIR_SSW>() | bb_7g.shift<DIR_SSE>()},
    {bb_6g.shift<DIR_NNW>() | bb_6g.shift<DIR_NNE>(),
     bb_6g.shift<DIR_SSW>() | bb_6g.shift<DIR_SSE>()},
    {bb_5g.shift<DIR_NNW>() | bb_5g.shift<DIR_NNE>(),
     bb_5g.shift<DIR_SSW>() | bb_5g.shift<DIR_SSE>()},
    {bb_4g.shift<DIR_NNW>() | bb_4g.shift<DIR_NNE>(),
     bb_4g.shift<DIR_SSW>() | bb_4g.shift<DIR_SSE>()},
    {bb_3g.shift<DIR_NNW>() | bb_3g.shift<DIR_NNE>(),
     bb_3g.shift<DIR_SSW>() | bb_3g.shift<DIR_SSE>()},
    {bb_2g.shift<DIR_NNW>() | bb_2g.shift<DIR_NNE>(),
     bb_2g.shift<DIR_SSW>() | bb_2g.shift<DIR_SSE>()},
    {bb_1g.shift<DIR_NNW>() | bb_1g.shift<DIR_NNE>(),
     bb_1g.shift<DIR_SSW>() | bb_1g.shift<DIR_SSE>()},
    {bb_9h.shift<DIR_NNW>() | bb_9h.shift<DIR_NNE>(),
     bb_9h.shift<DIR_SSW>() | bb_9h.shift<DIR_SSE>()},
    {bb_8h.shift<DIR_NNW>() | bb_8h.shift<DIR_NNE>(),
     bb_8h.shift<DIR_SSW>() | bb_8h.shift<DIR_SSE>()},
    {bb_7h.shift<DIR_NNW>() | bb_7h.shift<DIR_NNE>(),
     bb_7h.shift<DIR_SSW>() | bb_7h.shift<DIR_SSE>()},
    {bb_6h.shift<DIR_NNW>() | bb_6h.shift<DIR_NNE>(),
     bb_6h.shift<DIR_SSW>() | bb_6h.shift<DIR_SSE>()},
    {bb_5h.shift<DIR_NNW>() | bb_5h.shift<DIR_NNE>(),
     bb_5h.shift<DIR_SSW>() | bb_5h.shift<DIR_SSE>()},
    {bb_4h.shift<DIR_NNW>() | bb_4h.shift<DIR_NNE>(),
     bb_4h.shift<DIR_SSW>() | bb_4h.shift<DIR_SSE>()},
    {bb_3h.shift<DIR_NNW>() | bb_3h.shift<DIR_NNE>(),
     bb_3h.shift<DIR_SSW>() | bb_3h.shift<DIR_SSE>()},
    {bb_2h.shift<DIR_NNW>() | bb_2h.shift<DIR_NNE>(),
     bb_2h.shift<DIR_SSW>() | bb_2h.shift<DIR_SSE>()},
    {bb_1h.shift<DIR_NNW>() | bb_1h.shift<DIR_NNE>(),
     bb_1h.shift<DIR_SSW>() | bb_1h.shift<DIR_SSE>()},
    {bb_9i.shift<DIR_NNW>() | bb_9i.shift<DIR_NNE>(),
     bb_9i.shift<DIR_SSW>() | bb_9i.shift<DIR_SSE>()},
    {bb_8i.shift<DIR_NNW>() | bb_8i.shift<DIR_NNE>(),
     bb_8i.shift<DIR_SSW>() | bb_8i.shift<DIR_SSE>()},
    {bb_7i.shift<DIR_NNW>() | bb_7i.shift<DIR_NNE>(),
     bb_7i.shift<DIR_SSW>() | bb_7i.shift<DIR_SSE>()},
    {bb_6i.shift<DIR_NNW>() | bb_6i.shift<DIR_NNE>(),
     bb_6i.shift<DIR_SSW>() | bb_6i.shift<DIR_SSE>()},
    {bb_5i.shift<DIR_NNW>() | bb_5i.shift<DIR_NNE>(),
     bb_5i.shift<DIR_SSW>() | bb_5i.shift<DIR_SSE>()},
    {bb_4i.shift<DIR_NNW>() | bb_4i.shift<DIR_NNE>(),
     bb_4i.shift<DIR_SSW>() | bb_4i.shift<DIR_SSE>()},
    {bb_3i.shift<DIR_NNW>() | bb_3i.shift<DIR_NNE>(),
     bb_3i.shift<DIR_SSW>() | bb_3i.shift<DIR_SSE>()},
    {bb_2i.shift<DIR_NNW>() | bb_2i.shift<DIR_NNE>(),
     bb_2i.shift<DIR_SSW>() | bb_2i.shift<DIR_SSE>()},
    {bb_1i.shift<DIR_NNW>() | bb_1i.shift<DIR_NNE>(),
     bb_1i.shift<DIR_SSW>() | bb_1i.shift<DIR_SSE>()},
};

namespace internal
{

constexpr BitBoard attacks_by_bgi_sq5e = bb_4d | bb_5d | bb_6d | bb_4f | bb_6f;
constexpr BitBoard attacks_by_bgi_sq5d = attacks_by_bgi_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq5c = attacks_by_bgi_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq5b = attacks_by_bgi_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq5a = attacks_by_bgi_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq5f = attacks_by_bgi_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq5g = attacks_by_bgi_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq5h = attacks_by_bgi_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq5i = attacks_by_bgi_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq4a = attacks_by_bgi_sq5a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4b = attacks_by_bgi_sq5b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4c = attacks_by_bgi_sq5c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4d = attacks_by_bgi_sq5d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4e = attacks_by_bgi_sq5e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4f = attacks_by_bgi_sq5f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4g = attacks_by_bgi_sq5g.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4h = attacks_by_bgi_sq5h.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq4i = attacks_by_bgi_sq5i.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3a = attacks_by_bgi_sq4a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3b = attacks_by_bgi_sq4b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3c = attacks_by_bgi_sq4c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3d = attacks_by_bgi_sq4d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3e = attacks_by_bgi_sq4e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3f = attacks_by_bgi_sq4f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3g = attacks_by_bgi_sq4g.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3h = attacks_by_bgi_sq4h.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq3i = attacks_by_bgi_sq4i.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2a = attacks_by_bgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2b = attacks_by_bgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2c = attacks_by_bgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2d = attacks_by_bgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2e = attacks_by_bgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2f = attacks_by_bgi_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2g = attacks_by_bgi_sq3g.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2h = attacks_by_bgi_sq3h.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2i = attacks_by_bgi_sq3i.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1a = attacks_by_bgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1b = attacks_by_bgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1c = attacks_by_bgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1d = attacks_by_bgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1e = attacks_by_bgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1f = attacks_by_bgi_sq2f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1g = attacks_by_bgi_sq2g.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1h = attacks_by_bgi_sq2h.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1i = attacks_by_bgi_sq2i.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq6a = attacks_by_bgi_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6b = attacks_by_bgi_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6c = attacks_by_bgi_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6d = attacks_by_bgi_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6e = attacks_by_bgi_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6f = attacks_by_bgi_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6g = attacks_by_bgi_sq5g.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6h = attacks_by_bgi_sq5h.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6i = attacks_by_bgi_sq5i.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7a = attacks_by_bgi_sq6a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7b = attacks_by_bgi_sq6b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7c = attacks_by_bgi_sq6c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7d = attacks_by_bgi_sq6d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7e = attacks_by_bgi_sq6e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7f = attacks_by_bgi_sq6f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7g = attacks_by_bgi_sq6g.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7h = attacks_by_bgi_sq6h.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq7i = attacks_by_bgi_sq6i.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8a = attacks_by_bgi_sq7a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8b = attacks_by_bgi_sq7b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8c = attacks_by_bgi_sq7c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8d = attacks_by_bgi_sq7d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8e = attacks_by_bgi_sq7e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8f = attacks_by_bgi_sq7f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8g = attacks_by_bgi_sq7g.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8h = attacks_by_bgi_sq7h.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq8i = attacks_by_bgi_sq7i.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9a = attacks_by_bgi_sq8a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9b = attacks_by_bgi_sq8b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9c = attacks_by_bgi_sq8c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9d = attacks_by_bgi_sq8d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9e = attacks_by_bgi_sq8e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9f = attacks_by_bgi_sq8f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9g = attacks_by_bgi_sq8g.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9h = attacks_by_bgi_sq8h.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq9i = attacks_by_bgi_sq8i.shift<DIR_W>();

constexpr BitBoard attacks_by_wgi_sq5e = bb_4d | bb_6d | bb_4f | bb_5f | bb_6f;
constexpr BitBoard attacks_by_wgi_sq5d = attacks_by_wgi_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq5c = attacks_by_wgi_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq5b = attacks_by_wgi_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq5a = attacks_by_wgi_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq5f = attacks_by_wgi_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq5g = attacks_by_wgi_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq5h = attacks_by_wgi_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq5i = attacks_by_wgi_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq4a = attacks_by_wgi_sq5a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4b = attacks_by_wgi_sq5b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4c = attacks_by_wgi_sq5c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4d = attacks_by_wgi_sq5d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4e = attacks_by_wgi_sq5e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4f = attacks_by_wgi_sq5f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4g = attacks_by_wgi_sq5g.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4h = attacks_by_wgi_sq5h.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq4i = attacks_by_wgi_sq5i.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3a = attacks_by_wgi_sq4a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3b = attacks_by_wgi_sq4b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3c = attacks_by_wgi_sq4c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3d = attacks_by_wgi_sq4d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3e = attacks_by_wgi_sq4e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3f = attacks_by_wgi_sq4f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3g = attacks_by_wgi_sq4g.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3h = attacks_by_wgi_sq4h.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq3i = attacks_by_wgi_sq4i.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2a = attacks_by_wgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2b = attacks_by_wgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2c = attacks_by_wgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2d = attacks_by_wgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2e = attacks_by_wgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2f = attacks_by_wgi_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2g = attacks_by_wgi_sq3g.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2h = attacks_by_wgi_sq3h.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2i = attacks_by_wgi_sq3i.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1a = attacks_by_wgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1b = attacks_by_wgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1c = attacks_by_wgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1d = attacks_by_wgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1e = attacks_by_wgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1f = attacks_by_wgi_sq2f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1g = attacks_by_wgi_sq2g.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1h = attacks_by_wgi_sq2h.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1i = attacks_by_wgi_sq2i.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq6a = attacks_by_wgi_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6b = attacks_by_wgi_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6c = attacks_by_wgi_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6d = attacks_by_wgi_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6e = attacks_by_wgi_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6f = attacks_by_wgi_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6g = attacks_by_wgi_sq5g.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6h = attacks_by_wgi_sq5h.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6i = attacks_by_wgi_sq5i.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7a = attacks_by_wgi_sq6a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7b = attacks_by_wgi_sq6b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7c = attacks_by_wgi_sq6c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7d = attacks_by_wgi_sq6d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7e = attacks_by_wgi_sq6e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7f = attacks_by_wgi_sq6f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7g = attacks_by_wgi_sq6g.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7h = attacks_by_wgi_sq6h.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq7i = attacks_by_wgi_sq6i.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8a = attacks_by_wgi_sq7a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8b = attacks_by_wgi_sq7b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8c = attacks_by_wgi_sq7c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8d = attacks_by_wgi_sq7d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8e = attacks_by_wgi_sq7e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8f = attacks_by_wgi_sq7f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8g = attacks_by_wgi_sq7g.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8h = attacks_by_wgi_sq7h.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq8i = attacks_by_wgi_sq7i.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9a = attacks_by_wgi_sq8a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9b = attacks_by_wgi_sq8b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9c = attacks_by_wgi_sq8c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9d = attacks_by_wgi_sq8d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9e = attacks_by_wgi_sq8e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9f = attacks_by_wgi_sq8f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9g = attacks_by_wgi_sq8g.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9h = attacks_by_wgi_sq8h.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq9i = attacks_by_wgi_sq8i.shift<DIR_W>();

constexpr BitBoard attacks_by_bki_sq5e
    = bb_4d | bb_5d | bb_6d | bb_4e | bb_6e | bb_5f;
constexpr BitBoard attacks_by_bki_sq5d = attacks_by_bki_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq5c = attacks_by_bki_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq5b = attacks_by_bki_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq5a = attacks_by_bki_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq5f = attacks_by_bki_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq5g = attacks_by_bki_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq5h = attacks_by_bki_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq5i = attacks_by_bki_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq4a = attacks_by_bki_sq5a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4b = attacks_by_bki_sq5b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4c = attacks_by_bki_sq5c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4d = attacks_by_bki_sq5d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4e = attacks_by_bki_sq5e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4f = attacks_by_bki_sq5f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4g = attacks_by_bki_sq5g.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4h = attacks_by_bki_sq5h.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq4i = attacks_by_bki_sq5i.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3a = attacks_by_bki_sq4a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3b = attacks_by_bki_sq4b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3c = attacks_by_bki_sq4c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3d = attacks_by_bki_sq4d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3e = attacks_by_bki_sq4e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3f = attacks_by_bki_sq4f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3g = attacks_by_bki_sq4g.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3h = attacks_by_bki_sq4h.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq3i = attacks_by_bki_sq4i.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2a = attacks_by_bki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2b = attacks_by_bki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2c = attacks_by_bki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2d = attacks_by_bki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2e = attacks_by_bki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2f = attacks_by_bki_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2g = attacks_by_bki_sq3g.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2h = attacks_by_bki_sq3h.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2i = attacks_by_bki_sq3i.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1a = attacks_by_bki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1b = attacks_by_bki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1c = attacks_by_bki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1d = attacks_by_bki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1e = attacks_by_bki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1f = attacks_by_bki_sq2f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1g = attacks_by_bki_sq2g.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1h = attacks_by_bki_sq2h.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1i = attacks_by_bki_sq2i.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq6a = attacks_by_bki_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6b = attacks_by_bki_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6c = attacks_by_bki_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6d = attacks_by_bki_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6e = attacks_by_bki_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6f = attacks_by_bki_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6g = attacks_by_bki_sq5g.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6h = attacks_by_bki_sq5h.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6i = attacks_by_bki_sq5i.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7a = attacks_by_bki_sq6a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7b = attacks_by_bki_sq6b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7c = attacks_by_bki_sq6c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7d = attacks_by_bki_sq6d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7e = attacks_by_bki_sq6e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7f = attacks_by_bki_sq6f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7g = attacks_by_bki_sq6g.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7h = attacks_by_bki_sq6h.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq7i = attacks_by_bki_sq6i.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8a = attacks_by_bki_sq7a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8b = attacks_by_bki_sq7b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8c = attacks_by_bki_sq7c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8d = attacks_by_bki_sq7d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8e = attacks_by_bki_sq7e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8f = attacks_by_bki_sq7f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8g = attacks_by_bki_sq7g.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8h = attacks_by_bki_sq7h.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq8i = attacks_by_bki_sq7i.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9a = attacks_by_bki_sq8a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9b = attacks_by_bki_sq8b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9c = attacks_by_bki_sq8c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9d = attacks_by_bki_sq8d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9e = attacks_by_bki_sq8e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9f = attacks_by_bki_sq8f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9g = attacks_by_bki_sq8g.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9h = attacks_by_bki_sq8h.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq9i = attacks_by_bki_sq8i.shift<DIR_W>();

constexpr BitBoard attacks_by_wki_sq5e
    = bb_5d | bb_4e | bb_6e | bb_4f | bb_5f | bb_6f;
constexpr BitBoard attacks_by_wki_sq5d = attacks_by_wki_sq5e.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq5c = attacks_by_wki_sq5d.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq5b = attacks_by_wki_sq5c.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq5a = attacks_by_wki_sq5b.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq5f = attacks_by_wki_sq5e.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq5g = attacks_by_wki_sq5f.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq5h = attacks_by_wki_sq5g.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq5i = attacks_by_wki_sq5h.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq4a = attacks_by_wki_sq5a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4b = attacks_by_wki_sq5b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4c = attacks_by_wki_sq5c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4d = attacks_by_wki_sq5d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4e = attacks_by_wki_sq5e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4f = attacks_by_wki_sq5f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4g = attacks_by_wki_sq5g.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4h = attacks_by_wki_sq5h.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq4i = attacks_by_wki_sq5i.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3a = attacks_by_wki_sq4a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3b = attacks_by_wki_sq4b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3c = attacks_by_wki_sq4c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3d = attacks_by_wki_sq4d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3e = attacks_by_wki_sq4e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3f = attacks_by_wki_sq4f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3g = attacks_by_wki_sq4g.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3h = attacks_by_wki_sq4h.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq3i = attacks_by_wki_sq4i.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2a = attacks_by_wki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2b = attacks_by_wki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2c = attacks_by_wki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2d = attacks_by_wki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2e = attacks_by_wki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2f = attacks_by_wki_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2g = attacks_by_wki_sq3g.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2h = attacks_by_wki_sq3h.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2i = attacks_by_wki_sq3i.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1a = attacks_by_wki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1b = attacks_by_wki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1c = attacks_by_wki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1d = attacks_by_wki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1e = attacks_by_wki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1f = attacks_by_wki_sq2f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1g = attacks_by_wki_sq2g.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1h = attacks_by_wki_sq2h.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1i = attacks_by_wki_sq2i.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq6a = attacks_by_wki_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6b = attacks_by_wki_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6c = attacks_by_wki_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6d = attacks_by_wki_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6e = attacks_by_wki_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6f = attacks_by_wki_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6g = attacks_by_wki_sq5g.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6h = attacks_by_wki_sq5h.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6i = attacks_by_wki_sq5i.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7a = attacks_by_wki_sq6a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7b = attacks_by_wki_sq6b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7c = attacks_by_wki_sq6c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7d = attacks_by_wki_sq6d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7e = attacks_by_wki_sq6e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7f = attacks_by_wki_sq6f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7g = attacks_by_wki_sq6g.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7h = attacks_by_wki_sq6h.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq7i = attacks_by_wki_sq6i.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8a = attacks_by_wki_sq7a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8b = attacks_by_wki_sq7b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8c = attacks_by_wki_sq7c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8d = attacks_by_wki_sq7d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8e = attacks_by_wki_sq7e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8f = attacks_by_wki_sq7f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8g = attacks_by_wki_sq7g.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8h = attacks_by_wki_sq7h.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq8i = attacks_by_wki_sq7i.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9a = attacks_by_wki_sq8a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9b = attacks_by_wki_sq8b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9c = attacks_by_wki_sq8c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9d = attacks_by_wki_sq8d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9e = attacks_by_wki_sq8e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9f = attacks_by_wki_sq8f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9g = attacks_by_wki_sq8g.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9h = attacks_by_wki_sq8h.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq9i = attacks_by_wki_sq8i.shift<DIR_W>();

} // namespace internal

constexpr BitBoard attacks_by_gi[num_squares][num_colors] = {
    {internal::attacks_by_bgi_sq9a, internal::attacks_by_wgi_sq9a},
    {internal::attacks_by_bgi_sq8a, internal::attacks_by_wgi_sq8a},
    {internal::attacks_by_bgi_sq7a, internal::attacks_by_wgi_sq7a},
    {internal::attacks_by_bgi_sq6a, internal::attacks_by_wgi_sq6a},
    {internal::attacks_by_bgi_sq5a, internal::attacks_by_wgi_sq5a},
    {internal::attacks_by_bgi_sq4a, internal::attacks_by_wgi_sq4a},
    {internal::attacks_by_bgi_sq3a, internal::attacks_by_wgi_sq3a},
    {internal::attacks_by_bgi_sq2a, internal::attacks_by_wgi_sq2a},
    {internal::attacks_by_bgi_sq1a, internal::attacks_by_wgi_sq1a},
    {internal::attacks_by_bgi_sq9b, internal::attacks_by_wgi_sq9b},
    {internal::attacks_by_bgi_sq8b, internal::attacks_by_wgi_sq8b},
    {internal::attacks_by_bgi_sq7b, internal::attacks_by_wgi_sq7b},
    {internal::attacks_by_bgi_sq6b, internal::attacks_by_wgi_sq6b},
    {internal::attacks_by_bgi_sq5b, internal::attacks_by_wgi_sq5b},
    {internal::attacks_by_bgi_sq4b, internal::attacks_by_wgi_sq4b},
    {internal::attacks_by_bgi_sq3b, internal::attacks_by_wgi_sq3b},
    {internal::attacks_by_bgi_sq2b, internal::attacks_by_wgi_sq2b},
    {internal::attacks_by_bgi_sq1b, internal::attacks_by_wgi_sq1b},
    {internal::attacks_by_bgi_sq9c, internal::attacks_by_wgi_sq9c},
    {internal::attacks_by_bgi_sq8c, internal::attacks_by_wgi_sq8c},
    {internal::attacks_by_bgi_sq7c, internal::attacks_by_wgi_sq7c},
    {internal::attacks_by_bgi_sq6c, internal::attacks_by_wgi_sq6c},
    {internal::attacks_by_bgi_sq5c, internal::attacks_by_wgi_sq5c},
    {internal::attacks_by_bgi_sq4c, internal::attacks_by_wgi_sq4c},
    {internal::attacks_by_bgi_sq3c, internal::attacks_by_wgi_sq3c},
    {internal::attacks_by_bgi_sq2c, internal::attacks_by_wgi_sq2c},
    {internal::attacks_by_bgi_sq1c, internal::attacks_by_wgi_sq1c},
    {internal::attacks_by_bgi_sq9d, internal::attacks_by_wgi_sq9d},
    {internal::attacks_by_bgi_sq8d, internal::attacks_by_wgi_sq8d},
    {internal::attacks_by_bgi_sq7d, internal::attacks_by_wgi_sq7d},
    {internal::attacks_by_bgi_sq6d, internal::attacks_by_wgi_sq6d},
    {internal::attacks_by_bgi_sq5d, internal::attacks_by_wgi_sq5d},
    {internal::attacks_by_bgi_sq4d, internal::attacks_by_wgi_sq4d},
    {internal::attacks_by_bgi_sq3d, internal::attacks_by_wgi_sq3d},
    {internal::attacks_by_bgi_sq2d, internal::attacks_by_wgi_sq2d},
    {internal::attacks_by_bgi_sq1d, internal::attacks_by_wgi_sq1d},
    {internal::attacks_by_bgi_sq9e, internal::attacks_by_wgi_sq9e},
    {internal::attacks_by_bgi_sq8e, internal::attacks_by_wgi_sq8e},
    {internal::attacks_by_bgi_sq7e, internal::attacks_by_wgi_sq7e},
    {internal::attacks_by_bgi_sq6e, internal::attacks_by_wgi_sq6e},
    {internal::attacks_by_bgi_sq5e, internal::attacks_by_wgi_sq5e},
    {internal::attacks_by_bgi_sq4e, internal::attacks_by_wgi_sq4e},
    {internal::attacks_by_bgi_sq3e, internal::attacks_by_wgi_sq3e},
    {internal::attacks_by_bgi_sq2e, internal::attacks_by_wgi_sq2e},
    {internal::attacks_by_bgi_sq1e, internal::attacks_by_wgi_sq1e},
    {internal::attacks_by_bgi_sq9f, internal::attacks_by_wgi_sq9f},
    {internal::attacks_by_bgi_sq8f, internal::attacks_by_wgi_sq8f},
    {internal::attacks_by_bgi_sq7f, internal::attacks_by_wgi_sq7f},
    {internal::attacks_by_bgi_sq6f, internal::attacks_by_wgi_sq6f},
    {internal::attacks_by_bgi_sq5f, internal::attacks_by_wgi_sq5f},
    {internal::attacks_by_bgi_sq4f, internal::attacks_by_wgi_sq4f},
    {internal::attacks_by_bgi_sq3f, internal::attacks_by_wgi_sq3f},
    {internal::attacks_by_bgi_sq2f, internal::attacks_by_wgi_sq2f},
    {internal::attacks_by_bgi_sq1f, internal::attacks_by_wgi_sq1f},
    {internal::attacks_by_bgi_sq9g, internal::attacks_by_wgi_sq9g},
    {internal::attacks_by_bgi_sq8g, internal::attacks_by_wgi_sq8g},
    {internal::attacks_by_bgi_sq7g, internal::attacks_by_wgi_sq7g},
    {internal::attacks_by_bgi_sq6g, internal::attacks_by_wgi_sq6g},
    {internal::attacks_by_bgi_sq5g, internal::attacks_by_wgi_sq5g},
    {internal::attacks_by_bgi_sq4g, internal::attacks_by_wgi_sq4g},
    {internal::attacks_by_bgi_sq3g, internal::attacks_by_wgi_sq3g},
    {internal::attacks_by_bgi_sq2g, internal::attacks_by_wgi_sq2g},
    {internal::attacks_by_bgi_sq1g, internal::attacks_by_wgi_sq1g},
    {internal::attacks_by_bgi_sq9h, internal::attacks_by_wgi_sq9h},
    {internal::attacks_by_bgi_sq8h, internal::attacks_by_wgi_sq8h},
    {internal::attacks_by_bgi_sq7h, internal::attacks_by_wgi_sq7h},
    {internal::attacks_by_bgi_sq6h, internal::attacks_by_wgi_sq6h},
    {internal::attacks_by_bgi_sq5h, internal::attacks_by_wgi_sq5h},
    {internal::attacks_by_bgi_sq4h, internal::attacks_by_wgi_sq4h},
    {internal::attacks_by_bgi_sq3h, internal::attacks_by_wgi_sq3h},
    {internal::attacks_by_bgi_sq2h, internal::attacks_by_wgi_sq2h},
    {internal::attacks_by_bgi_sq1h, internal::attacks_by_wgi_sq1h},
    {internal::attacks_by_bgi_sq9i, internal::attacks_by_wgi_sq9i},
    {internal::attacks_by_bgi_sq8i, internal::attacks_by_wgi_sq8i},
    {internal::attacks_by_bgi_sq7i, internal::attacks_by_wgi_sq7i},
    {internal::attacks_by_bgi_sq6i, internal::attacks_by_wgi_sq6i},
    {internal::attacks_by_bgi_sq5i, internal::attacks_by_wgi_sq5i},
    {internal::attacks_by_bgi_sq4i, internal::attacks_by_wgi_sq4i},
    {internal::attacks_by_bgi_sq3i, internal::attacks_by_wgi_sq3i},
    {internal::attacks_by_bgi_sq2i, internal::attacks_by_wgi_sq2i},
    {internal::attacks_by_bgi_sq1i, internal::attacks_by_wgi_sq1i},
};

constexpr BitBoard attacks_by_ki[num_squares][num_colors] = {
    {internal::attacks_by_bki_sq9a, internal::attacks_by_wki_sq9a},
    {internal::attacks_by_bki_sq8a, internal::attacks_by_wki_sq8a},
    {internal::attacks_by_bki_sq7a, internal::attacks_by_wki_sq7a},
    {internal::attacks_by_bki_sq6a, internal::attacks_by_wki_sq6a},
    {internal::attacks_by_bki_sq5a, internal::attacks_by_wki_sq5a},
    {internal::attacks_by_bki_sq4a, internal::attacks_by_wki_sq4a},
    {internal::attacks_by_bki_sq3a, internal::attacks_by_wki_sq3a},
    {internal::attacks_by_bki_sq2a, internal::attacks_by_wki_sq2a},
    {internal::attacks_by_bki_sq1a, internal::attacks_by_wki_sq1a},
    {internal::attacks_by_bki_sq9b, internal::attacks_by_wki_sq9b},
    {internal::attacks_by_bki_sq8b, internal::attacks_by_wki_sq8b},
    {internal::attacks_by_bki_sq7b, internal::attacks_by_wki_sq7b},
    {internal::attacks_by_bki_sq6b, internal::attacks_by_wki_sq6b},
    {internal::attacks_by_bki_sq5b, internal::attacks_by_wki_sq5b},
    {internal::attacks_by_bki_sq4b, internal::attacks_by_wki_sq4b},
    {internal::attacks_by_bki_sq3b, internal::attacks_by_wki_sq3b},
    {internal::attacks_by_bki_sq2b, internal::attacks_by_wki_sq2b},
    {internal::attacks_by_bki_sq1b, internal::attacks_by_wki_sq1b},
    {internal::attacks_by_bki_sq9c, internal::attacks_by_wki_sq9c},
    {internal::attacks_by_bki_sq8c, internal::attacks_by_wki_sq8c},
    {internal::attacks_by_bki_sq7c, internal::attacks_by_wki_sq7c},
    {internal::attacks_by_bki_sq6c, internal::attacks_by_wki_sq6c},
    {internal::attacks_by_bki_sq5c, internal::attacks_by_wki_sq5c},
    {internal::attacks_by_bki_sq4c, internal::attacks_by_wki_sq4c},
    {internal::attacks_by_bki_sq3c, internal::attacks_by_wki_sq3c},
    {internal::attacks_by_bki_sq2c, internal::attacks_by_wki_sq2c},
    {internal::attacks_by_bki_sq1c, internal::attacks_by_wki_sq1c},
    {internal::attacks_by_bki_sq9d, internal::attacks_by_wki_sq9d},
    {internal::attacks_by_bki_sq8d, internal::attacks_by_wki_sq8d},
    {internal::attacks_by_bki_sq7d, internal::attacks_by_wki_sq7d},
    {internal::attacks_by_bki_sq6d, internal::attacks_by_wki_sq6d},
    {internal::attacks_by_bki_sq5d, internal::attacks_by_wki_sq5d},
    {internal::attacks_by_bki_sq4d, internal::attacks_by_wki_sq4d},
    {internal::attacks_by_bki_sq3d, internal::attacks_by_wki_sq3d},
    {internal::attacks_by_bki_sq2d, internal::attacks_by_wki_sq2d},
    {internal::attacks_by_bki_sq1d, internal::attacks_by_wki_sq1d},
    {internal::attacks_by_bki_sq9e, internal::attacks_by_wki_sq9e},
    {internal::attacks_by_bki_sq8e, internal::attacks_by_wki_sq8e},
    {internal::attacks_by_bki_sq7e, internal::attacks_by_wki_sq7e},
    {internal::attacks_by_bki_sq6e, internal::attacks_by_wki_sq6e},
    {internal::attacks_by_bki_sq5e, internal::attacks_by_wki_sq5e},
    {internal::attacks_by_bki_sq4e, internal::attacks_by_wki_sq4e},
    {internal::attacks_by_bki_sq3e, internal::attacks_by_wki_sq3e},
    {internal::attacks_by_bki_sq2e, internal::attacks_by_wki_sq2e},
    {internal::attacks_by_bki_sq1e, internal::attacks_by_wki_sq1e},
    {internal::attacks_by_bki_sq9f, internal::attacks_by_wki_sq9f},
    {internal::attacks_by_bki_sq8f, internal::attacks_by_wki_sq8f},
    {internal::attacks_by_bki_sq7f, internal::attacks_by_wki_sq7f},
    {internal::attacks_by_bki_sq6f, internal::attacks_by_wki_sq6f},
    {internal::attacks_by_bki_sq5f, internal::attacks_by_wki_sq5f},
    {internal::attacks_by_bki_sq4f, internal::attacks_by_wki_sq4f},
    {internal::attacks_by_bki_sq3f, internal::attacks_by_wki_sq3f},
    {internal::attacks_by_bki_sq2f, internal::attacks_by_wki_sq2f},
    {internal::attacks_by_bki_sq1f, internal::attacks_by_wki_sq1f},
    {internal::attacks_by_bki_sq9g, internal::attacks_by_wki_sq9g},
    {internal::attacks_by_bki_sq8g, internal::attacks_by_wki_sq8g},
    {internal::attacks_by_bki_sq7g, internal::attacks_by_wki_sq7g},
    {internal::attacks_by_bki_sq6g, internal::attacks_by_wki_sq6g},
    {internal::attacks_by_bki_sq5g, internal::attacks_by_wki_sq5g},
    {internal::attacks_by_bki_sq4g, internal::attacks_by_wki_sq4g},
    {internal::attacks_by_bki_sq3g, internal::attacks_by_wki_sq3g},
    {internal::attacks_by_bki_sq2g, internal::attacks_by_wki_sq2g},
    {internal::attacks_by_bki_sq1g, internal::attacks_by_wki_sq1g},
    {internal::attacks_by_bki_sq9h, internal::attacks_by_wki_sq9h},
    {internal::attacks_by_bki_sq8h, internal::attacks_by_wki_sq8h},
    {internal::attacks_by_bki_sq7h, internal::attacks_by_wki_sq7h},
    {internal::attacks_by_bki_sq6h, internal::attacks_by_wki_sq6h},
    {internal::attacks_by_bki_sq5h, internal::attacks_by_wki_sq5h},
    {internal::attacks_by_bki_sq4h, internal::attacks_by_wki_sq4h},
    {internal::attacks_by_bki_sq3h, internal::attacks_by_wki_sq3h},
    {internal::attacks_by_bki_sq2h, internal::attacks_by_wki_sq2h},
    {internal::attacks_by_bki_sq1h, internal::attacks_by_wki_sq1h},
    {internal::attacks_by_bki_sq9i, internal::attacks_by_wki_sq9i},
    {internal::attacks_by_bki_sq8i, internal::attacks_by_wki_sq8i},
    {internal::attacks_by_bki_sq7i, internal::attacks_by_wki_sq7i},
    {internal::attacks_by_bki_sq6i, internal::attacks_by_wki_sq6i},
    {internal::attacks_by_bki_sq5i, internal::attacks_by_wki_sq5i},
    {internal::attacks_by_bki_sq4i, internal::attacks_by_wki_sq4i},
    {internal::attacks_by_bki_sq3i, internal::attacks_by_wki_sq3i},
    {internal::attacks_by_bki_sq2i, internal::attacks_by_wki_sq2i},
    {internal::attacks_by_bki_sq1i, internal::attacks_by_wki_sq1i},
};

constexpr BitBoard attacks_by_ou[num_squares] = {
    bb_9a.expand_neighbor() & ~bb_9a, bb_8a.expand_neighbor() & ~bb_8a,
    bb_7a.expand_neighbor() & ~bb_7a, bb_6a.expand_neighbor() & ~bb_6a,
    bb_5a.expand_neighbor() & ~bb_5a, bb_4a.expand_neighbor() & ~bb_4a,
    bb_3a.expand_neighbor() & ~bb_3a, bb_2a.expand_neighbor() & ~bb_2a,
    bb_1a.expand_neighbor() & ~bb_1a, bb_9b.expand_neighbor() & ~bb_9b,
    bb_8b.expand_neighbor() & ~bb_8b, bb_7b.expand_neighbor() & ~bb_7b,
    bb_6b.expand_neighbor() & ~bb_6b, bb_5b.expand_neighbor() & ~bb_5b,
    bb_4b.expand_neighbor() & ~bb_4b, bb_3b.expand_neighbor() & ~bb_3b,
    bb_2b.expand_neighbor() & ~bb_2b, bb_1b.expand_neighbor() & ~bb_1b,
    bb_9c.expand_neighbor() & ~bb_9c, bb_8c.expand_neighbor() & ~bb_8c,
    bb_7c.expand_neighbor() & ~bb_7c, bb_6c.expand_neighbor() & ~bb_6c,
    bb_5c.expand_neighbor() & ~bb_5c, bb_4c.expand_neighbor() & ~bb_4c,
    bb_3c.expand_neighbor() & ~bb_3c, bb_2c.expand_neighbor() & ~bb_2c,
    bb_1c.expand_neighbor() & ~bb_1c, bb_9d.expand_neighbor() & ~bb_9d,
    bb_8d.expand_neighbor() & ~bb_8d, bb_7d.expand_neighbor() & ~bb_7d,
    bb_6d.expand_neighbor() & ~bb_6d, bb_5d.expand_neighbor() & ~bb_5d,
    bb_4d.expand_neighbor() & ~bb_4d, bb_3d.expand_neighbor() & ~bb_3d,
    bb_2d.expand_neighbor() & ~bb_2d, bb_1d.expand_neighbor() & ~bb_1d,
    bb_9e.expand_neighbor() & ~bb_9e, bb_8e.expand_neighbor() & ~bb_8e,
    bb_7e.expand_neighbor() & ~bb_7e, bb_6e.expand_neighbor() & ~bb_6e,
    bb_5e.expand_neighbor() & ~bb_5e, bb_4e.expand_neighbor() & ~bb_4e,
    bb_3e.expand_neighbor() & ~bb_3e, bb_2e.expand_neighbor() & ~bb_2e,
    bb_1e.expand_neighbor() & ~bb_1e, bb_9f.expand_neighbor() & ~bb_9f,
    bb_8f.expand_neighbor() & ~bb_8f, bb_7f.expand_neighbor() & ~bb_7f,
    bb_6f.expand_neighbor() & ~bb_6f, bb_5f.expand_neighbor() & ~bb_5f,
    bb_4f.expand_neighbor() & ~bb_4f, bb_3f.expand_neighbor() & ~bb_3f,
    bb_2f.expand_neighbor() & ~bb_2f, bb_1f.expand_neighbor() & ~bb_1f,
    bb_9g.expand_neighbor() & ~bb_9g, bb_8g.expand_neighbor() & ~bb_8g,
    bb_7g.expand_neighbor() & ~bb_7g, bb_6g.expand_neighbor() & ~bb_6g,
    bb_5g.expand_neighbor() & ~bb_5g, bb_4g.expand_neighbor() & ~bb_4g,
    bb_3g.expand_neighbor() & ~bb_3g, bb_2g.expand_neighbor() & ~bb_2g,
    bb_1g.expand_neighbor() & ~bb_1g, bb_9h.expand_neighbor() & ~bb_9h,
    bb_8h.expand_neighbor() & ~bb_8h, bb_7h.expand_neighbor() & ~bb_7h,
    bb_6h.expand_neighbor() & ~bb_6h, bb_5h.expand_neighbor() & ~bb_5h,
    bb_4h.expand_neighbor() & ~bb_4h, bb_3h.expand_neighbor() & ~bb_3h,
    bb_2h.expand_neighbor() & ~bb_2h, bb_1h.expand_neighbor() & ~bb_1h,
    bb_9i.expand_neighbor() & ~bb_9i, bb_8i.expand_neighbor() & ~bb_8i,
    bb_7i.expand_neighbor() & ~bb_7i, bb_6i.expand_neighbor() & ~bb_6i,
    bb_5i.expand_neighbor() & ~bb_5i, bb_4i.expand_neighbor() & ~bb_4i,
    bb_3i.expand_neighbor() & ~bb_3i, bb_2i.expand_neighbor() & ~bb_2i,
    bb_1i.expand_neighbor() & ~bb_1i,
};

inline BitBoard get_attacks_by(
    const BoardPieceTypeEnum piece,
    const SquareEnum location,
    const BitBoard occupied = BitBoard(0, 0))
{
    const auto piece_type = to_piece_type(piece);
    const auto color = get_color(piece);
    switch (piece_type) {
    case FU:
        return attacks_by_fu[location][color];
    case KY:
        return (color == BLACK)
                   ? BitBoard::ranging_attacks_to<DIR_N>(location, occupied)
                   : BitBoard::ranging_attacks_to<DIR_S>(location, occupied);
    case KE:
        return attacks_by_ke[location][color];
    case GI:
        return attacks_by_gi[location][color];
    case KA:
        return BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case KI:
    case TO:
    case NY:
    case NK:
    case NG:
        return attacks_by_ki[location][color];
    case HI:
        return BitBoard::ranging_attacks_to_adjacent(location, occupied);
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
    return BitBoard(0, 0);
}

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_BITBOARD_HPP
