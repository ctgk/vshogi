#ifndef VSHOGI_JUDKINS_SHOGI_BITBOARD_HPP
#define VSHOGI_JUDKINS_SHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/judkins_shogi/piece.hpp"
#include "vshogi/judkins_shogi/squares.hpp"

namespace vshogi::judkins_shogi
{

class BitBoard
{
private:
    /**
     * @brief 64-bit binary representing ON and OFF of squares from 9A to 9H.
     * @details
     *           +--------------------------------------------- SQ_1F
     *           |                                +------------ SQ_6B
     *           |                                |+----------- SQ_1A
     *           |                                ||   +------- SQ_5A
     *           |                                ||   |+------ SQ_6A
     *           |                                ||   ||
     *           v                                vv   vv
     * (MSB) xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx (LSB)
     */
    std::uint64_t m_value;

public:
    constexpr BitBoard() : m_value(0UL)
    {
    }
    constexpr BitBoard(const std::uint64_t v) : m_value(v & 0x0fffffffff)
    {
    }
    constexpr static BitBoard from_square(const Squares::SquareEnum sq)
    {
        return BitBoard(1) << static_cast<unsigned int>(sq);
    }
    static BitBoard get_attacks_by(
        const Pieces::BoardPieceTypeEnum piece,
        const Squares::SquareEnum location);
    static BitBoard get_attacks_by(
        const Pieces::BoardPieceTypeEnum piece,
        const Squares::SquareEnum location,
        const BitBoard occupied);
    static BitBoard get_promotion_zone(const ColorEnum& c);

    /**
     * @brief Get pointer to array of attacking squares by non ranging piece.
     * @note Note that the array's length is 8.
     *
     * @param piece
     * @param location
     * @return const Squares::SquareEnum*
     */
    static const Squares::SquareEnum* get_attacks_by_non_ranging(
        const Pieces::BoardPieceTypeEnum& piece,
        const Squares::SquareEnum& location);

    /**
     * @brief Get pointer to array of squares along the given direction
     * from the given location.
     *
     * @param direction
     * @param location
     * @return const Squares::SquareEnum*
     */
    static const Squares::SquareEnum* get_squares_along(
        const DirectionEnum& direction, const Squares::SquareEnum& location);

    std::uint64_t get_value() const
    {
        return m_value;
    }
    constexpr bool operator==(const BitBoard other) const
    {
        return m_value == other.m_value;
    }
    constexpr BitBoard operator|(const BitBoard other) const
    {
        return BitBoard(m_value | other.m_value);
    }
    BitBoard& operator|=(const BitBoard other)
    {
        m_value |= other.m_value;
        return *this;
    }
    constexpr BitBoard operator&(const BitBoard other) const
    {
        return BitBoard(m_value & other.m_value);
    }
    BitBoard& operator&=(const BitBoard other)
    {
        m_value &= other.m_value;
        return *this;
    }
    constexpr BitBoard operator~() const
    {
        return BitBoard(~m_value);
    }
    constexpr bool any() const
    {
        return (m_value > 0);
    }
    constexpr BitBoard operator<<(const unsigned int shift_width) const
    {
        return BitBoard(m_value << shift_width);
    }
    constexpr BitBoard operator>>(const unsigned int shift_width) const
    {
        return BitBoard(m_value >> shift_width);
    }
    template <DirectionEnum D>
    constexpr BitBoard shift() const
    {
        constexpr auto bb_f12345
            = ~(BitBoard::from_square(SQ_6A) | BitBoard::from_square(SQ_6B)
                | BitBoard::from_square(SQ_6C) | BitBoard::from_square(SQ_6D)
                | BitBoard::from_square(SQ_6E) | BitBoard::from_square(SQ_6F));
        constexpr auto bb_f23456
            = ~(BitBoard::from_square(SQ_1A) | BitBoard::from_square(SQ_1B)
                | BitBoard::from_square(SQ_1C) | BitBoard::from_square(SQ_1D)
                | BitBoard::from_square(SQ_1E) | BitBoard::from_square(SQ_1F));
        constexpr BitBoard mask[] = {
            // clang-format off
            bb_f12345, ~BitBoard(0), bb_f23456,
            bb_f12345,               bb_f23456,
            bb_f12345, ~BitBoard(0), bb_f23456,
            bb_f12345,               bb_f23456,
            bb_f12345,               bb_f23456,
            // clang-format on
        };
        constexpr auto delta = Squares::direction_to_delta(D);
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
    ranging_attacks_to(const Squares::SquareEnum sq, const BitBoard occupied)
    {
        const auto base = BitBoard::from_square(sq);
        auto attacks = base;
        constexpr int max_length = 5;
        for (int i = 5; i--;) {
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
    static BitBoard ranging_attacks_to_diagonal(
        const Squares::SquareEnum sq, const BitBoard occupied)
    {
        return ranging_attacks_to<DIR_NW>(sq, occupied)
               | ranging_attacks_to<DIR_NE>(sq, occupied)
               | ranging_attacks_to<DIR_SW>(sq, occupied)
               | ranging_attacks_to<DIR_SE>(sq, occupied);
    }
    static BitBoard ranging_attacks_to_adjacent(
        const Squares::SquareEnum sq, const BitBoard occupied)
    {
        return ranging_attacks_to<DIR_N>(sq, occupied)
               | ranging_attacks_to<DIR_E>(sq, occupied)
               | ranging_attacks_to<DIR_W>(sq, occupied)
               | ranging_attacks_to<DIR_S>(sq, occupied);
    }
    bool is_one(const Squares::SquareEnum sq) const
    {
        return static_cast<bool>(
            (1UL << static_cast<std::uint64_t>(sq)) & m_value);
    }
    int hamming_weight() const
    {
        // https://en.wikipedia.org/wiki/Hamming_weight
        constexpr std::uint64_t m1 = 0x5555555555555555; // 0101...
        constexpr std::uint64_t m2 = 0x3333333333333333; // 00110011..
        constexpr std::uint64_t m4 = 0x0f0f0f0f0f0f0f0f; // 0000111100001111..
        auto x = m_value;
        x -= (x >> 1) & m1; //put count of each 2 bits into those 2 bits
        x = (x & m2) + ((x >> 2) & m2); // each 4 bits into those 4 bits
        x = (x + (x >> 4)) & m4; //put count of each 8 bits into those 8 bits
        x += x >> 8; //put count of each 16 bits into their lowest 8 bits
        x += x >> 16; //put count of each 32 bits into their lowest 8 bits
        x += x >> 32; //put count of each 64 bits into their lowest 8 bits
        return x & 0x7f;
    }
};

void init_non_ranging_attacks_table();
void init_ranging_squares_table();

constexpr BitBoard bb_1a = BitBoard::from_square(SQ_1A);
constexpr BitBoard bb_1b = BitBoard::from_square(SQ_1B);
constexpr BitBoard bb_1c = BitBoard::from_square(SQ_1C);
constexpr BitBoard bb_1d = BitBoard::from_square(SQ_1D);
constexpr BitBoard bb_1e = BitBoard::from_square(SQ_1E);
constexpr BitBoard bb_1f = BitBoard::from_square(SQ_1F);
constexpr BitBoard bb_2a = BitBoard::from_square(SQ_2A);
constexpr BitBoard bb_2b = BitBoard::from_square(SQ_2B);
constexpr BitBoard bb_2c = BitBoard::from_square(SQ_2C);
constexpr BitBoard bb_2d = BitBoard::from_square(SQ_2D);
constexpr BitBoard bb_2e = BitBoard::from_square(SQ_2E);
constexpr BitBoard bb_2f = BitBoard::from_square(SQ_2F);
constexpr BitBoard bb_3a = BitBoard::from_square(SQ_3A);
constexpr BitBoard bb_3b = BitBoard::from_square(SQ_3B);
constexpr BitBoard bb_3c = BitBoard::from_square(SQ_3C);
constexpr BitBoard bb_3d = BitBoard::from_square(SQ_3D);
constexpr BitBoard bb_3e = BitBoard::from_square(SQ_3E);
constexpr BitBoard bb_3f = BitBoard::from_square(SQ_3F);
constexpr BitBoard bb_4a = BitBoard::from_square(SQ_4A);
constexpr BitBoard bb_4b = BitBoard::from_square(SQ_4B);
constexpr BitBoard bb_4c = BitBoard::from_square(SQ_4C);
constexpr BitBoard bb_4d = BitBoard::from_square(SQ_4D);
constexpr BitBoard bb_4e = BitBoard::from_square(SQ_4E);
constexpr BitBoard bb_4f = BitBoard::from_square(SQ_4F);
constexpr BitBoard bb_5a = BitBoard::from_square(SQ_5A);
constexpr BitBoard bb_5b = BitBoard::from_square(SQ_5B);
constexpr BitBoard bb_5c = BitBoard::from_square(SQ_5C);
constexpr BitBoard bb_5d = BitBoard::from_square(SQ_5D);
constexpr BitBoard bb_5e = BitBoard::from_square(SQ_5E);
constexpr BitBoard bb_5f = BitBoard::from_square(SQ_5F);
constexpr BitBoard bb_6a = BitBoard::from_square(SQ_6A);
constexpr BitBoard bb_6b = BitBoard::from_square(SQ_6B);
constexpr BitBoard bb_6c = BitBoard::from_square(SQ_6C);
constexpr BitBoard bb_6d = BitBoard::from_square(SQ_6D);
constexpr BitBoard bb_6e = BitBoard::from_square(SQ_6E);
constexpr BitBoard bb_6f = BitBoard::from_square(SQ_6F);

constexpr BitBoard bb_file1 = bb_1a | bb_1b | bb_1c | bb_1d | bb_1e | bb_1f;
constexpr BitBoard bb_file2 = bb_2a | bb_2b | bb_2c | bb_2d | bb_2e | bb_2f;
constexpr BitBoard bb_file3 = bb_3a | bb_3b | bb_3c | bb_3d | bb_3e | bb_3f;
constexpr BitBoard bb_file4 = bb_4a | bb_4b | bb_4c | bb_4d | bb_4e | bb_4f;
constexpr BitBoard bb_file5 = bb_5a | bb_5b | bb_5c | bb_5d | bb_5e | bb_5f;
constexpr BitBoard bb_file6 = bb_6a | bb_6b | bb_6c | bb_6d | bb_6e | bb_6f;
constexpr BitBoard bb_ranka = bb_1a | bb_2a | bb_3a | bb_4a | bb_5a | bb_6a;
constexpr BitBoard bb_rankb = bb_1b | bb_2b | bb_3b | bb_4b | bb_5b | bb_6b;
constexpr BitBoard bb_rankc = bb_1c | bb_2c | bb_3c | bb_4c | bb_5c | bb_6c;
constexpr BitBoard bb_rankd = bb_1d | bb_2d | bb_3d | bb_4d | bb_5d | bb_6d;
constexpr BitBoard bb_ranke = bb_1e | bb_2e | bb_3e | bb_4e | bb_5e | bb_6e;
constexpr BitBoard bb_rankf = bb_1f | bb_2f | bb_3f | bb_4f | bb_5f | bb_6f;

constexpr BitBoard attacks_by_fu[Squares::num_squares][num_colors] = {
    {bb_6a.shift<DIR_N>(), bb_6a.shift<DIR_S>()},
    {bb_5a.shift<DIR_N>(), bb_5a.shift<DIR_S>()},
    {bb_4a.shift<DIR_N>(), bb_4a.shift<DIR_S>()},
    {bb_3a.shift<DIR_N>(), bb_3a.shift<DIR_S>()},
    {bb_2a.shift<DIR_N>(), bb_2a.shift<DIR_S>()},
    {bb_1a.shift<DIR_N>(), bb_1a.shift<DIR_S>()},
    {bb_6b.shift<DIR_N>(), bb_6b.shift<DIR_S>()},
    {bb_5b.shift<DIR_N>(), bb_5b.shift<DIR_S>()},
    {bb_4b.shift<DIR_N>(), bb_4b.shift<DIR_S>()},
    {bb_3b.shift<DIR_N>(), bb_3b.shift<DIR_S>()},
    {bb_2b.shift<DIR_N>(), bb_2b.shift<DIR_S>()},
    {bb_1b.shift<DIR_N>(), bb_1b.shift<DIR_S>()},
    {bb_6c.shift<DIR_N>(), bb_6c.shift<DIR_S>()},
    {bb_5c.shift<DIR_N>(), bb_5c.shift<DIR_S>()},
    {bb_4c.shift<DIR_N>(), bb_4c.shift<DIR_S>()},
    {bb_3c.shift<DIR_N>(), bb_3c.shift<DIR_S>()},
    {bb_2c.shift<DIR_N>(), bb_2c.shift<DIR_S>()},
    {bb_1c.shift<DIR_N>(), bb_1c.shift<DIR_S>()},
    {bb_6d.shift<DIR_N>(), bb_6d.shift<DIR_S>()},
    {bb_5d.shift<DIR_N>(), bb_5d.shift<DIR_S>()},
    {bb_4d.shift<DIR_N>(), bb_4d.shift<DIR_S>()},
    {bb_3d.shift<DIR_N>(), bb_3d.shift<DIR_S>()},
    {bb_2d.shift<DIR_N>(), bb_2d.shift<DIR_S>()},
    {bb_1d.shift<DIR_N>(), bb_1d.shift<DIR_S>()},
    {bb_6e.shift<DIR_N>(), bb_6e.shift<DIR_S>()},
    {bb_5e.shift<DIR_N>(), bb_5e.shift<DIR_S>()},
    {bb_4e.shift<DIR_N>(), bb_4e.shift<DIR_S>()},
    {bb_3e.shift<DIR_N>(), bb_3e.shift<DIR_S>()},
    {bb_2e.shift<DIR_N>(), bb_2e.shift<DIR_S>()},
    {bb_1e.shift<DIR_N>(), bb_1e.shift<DIR_S>()},
    {bb_6f.shift<DIR_N>(), bb_6f.shift<DIR_S>()},
    {bb_5f.shift<DIR_N>(), bb_5f.shift<DIR_S>()},
    {bb_4f.shift<DIR_N>(), bb_4f.shift<DIR_S>()},
    {bb_3f.shift<DIR_N>(), bb_3f.shift<DIR_S>()},
    {bb_2f.shift<DIR_N>(), bb_2f.shift<DIR_S>()},
    {bb_1f.shift<DIR_N>(), bb_1f.shift<DIR_S>()},
};

constexpr BitBoard attacks_by_ke[Squares::num_squares][num_colors] = {
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
};

namespace internal
{

constexpr BitBoard attacks_by_bgi_sq3c = bb_2b | bb_3b | bb_4b | bb_2d | bb_4d;
constexpr BitBoard attacks_by_bgi_sq3b = attacks_by_bgi_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq3a = attacks_by_bgi_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_bgi_sq3d = attacks_by_bgi_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq3e = attacks_by_bgi_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq3f = attacks_by_bgi_sq3e.shift<DIR_S>();
constexpr BitBoard attacks_by_bgi_sq4a = attacks_by_bgi_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4b = attacks_by_bgi_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4c = attacks_by_bgi_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4d = attacks_by_bgi_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4e = attacks_by_bgi_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq4f = attacks_by_bgi_sq3f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5a = attacks_by_bgi_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5b = attacks_by_bgi_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5c = attacks_by_bgi_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5d = attacks_by_bgi_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5e = attacks_by_bgi_sq4e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq5f = attacks_by_bgi_sq4f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6a = attacks_by_bgi_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6b = attacks_by_bgi_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6c = attacks_by_bgi_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6d = attacks_by_bgi_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6e = attacks_by_bgi_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq6f = attacks_by_bgi_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_bgi_sq2a = attacks_by_bgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2b = attacks_by_bgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2c = attacks_by_bgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2d = attacks_by_bgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2e = attacks_by_bgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq2f = attacks_by_bgi_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1a = attacks_by_bgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1b = attacks_by_bgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1c = attacks_by_bgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1d = attacks_by_bgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1e = attacks_by_bgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bgi_sq1f = attacks_by_bgi_sq2f.shift<DIR_E>();

constexpr BitBoard attacks_by_wgi_sq3c = bb_4d | bb_3d | bb_2d | bb_4b | bb_2b;
constexpr BitBoard attacks_by_wgi_sq3b = attacks_by_wgi_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq3a = attacks_by_wgi_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_wgi_sq3d = attacks_by_wgi_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq3e = attacks_by_wgi_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq3f = attacks_by_wgi_sq3e.shift<DIR_S>();
constexpr BitBoard attacks_by_wgi_sq4a = attacks_by_wgi_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4b = attacks_by_wgi_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4c = attacks_by_wgi_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4d = attacks_by_wgi_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4e = attacks_by_wgi_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq4f = attacks_by_wgi_sq3f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5a = attacks_by_wgi_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5b = attacks_by_wgi_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5c = attacks_by_wgi_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5d = attacks_by_wgi_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5e = attacks_by_wgi_sq4e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq5f = attacks_by_wgi_sq4f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6a = attacks_by_wgi_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6b = attacks_by_wgi_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6c = attacks_by_wgi_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6d = attacks_by_wgi_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6e = attacks_by_wgi_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq6f = attacks_by_wgi_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_wgi_sq2a = attacks_by_wgi_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2b = attacks_by_wgi_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2c = attacks_by_wgi_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2d = attacks_by_wgi_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2e = attacks_by_wgi_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq2f = attacks_by_wgi_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1a = attacks_by_wgi_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1b = attacks_by_wgi_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1c = attacks_by_wgi_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1d = attacks_by_wgi_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1e = attacks_by_wgi_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wgi_sq1f = attacks_by_wgi_sq2f.shift<DIR_E>();

constexpr BitBoard attacks_by_bki_sq3c
    = bb_2b | bb_3b | bb_4b | bb_2c | bb_4c | bb_3d;
constexpr BitBoard attacks_by_bki_sq3b = attacks_by_bki_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq3a = attacks_by_bki_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_bki_sq3d = attacks_by_bki_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq3e = attacks_by_bki_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq3f = attacks_by_bki_sq3e.shift<DIR_S>();
constexpr BitBoard attacks_by_bki_sq4a = attacks_by_bki_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4b = attacks_by_bki_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4c = attacks_by_bki_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4d = attacks_by_bki_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4e = attacks_by_bki_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq4f = attacks_by_bki_sq3f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5a = attacks_by_bki_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5b = attacks_by_bki_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5c = attacks_by_bki_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5d = attacks_by_bki_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5e = attacks_by_bki_sq4e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq5f = attacks_by_bki_sq4f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6a = attacks_by_bki_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6b = attacks_by_bki_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6c = attacks_by_bki_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6d = attacks_by_bki_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6e = attacks_by_bki_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq6f = attacks_by_bki_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_bki_sq2a = attacks_by_bki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2b = attacks_by_bki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2c = attacks_by_bki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2d = attacks_by_bki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2e = attacks_by_bki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq2f = attacks_by_bki_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1a = attacks_by_bki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1b = attacks_by_bki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1c = attacks_by_bki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1d = attacks_by_bki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1e = attacks_by_bki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_bki_sq1f = attacks_by_bki_sq2f.shift<DIR_E>();

constexpr BitBoard attacks_by_wki_sq3c
    = bb_4d | bb_3d | bb_2d | bb_4c | bb_2c | bb_3b;
constexpr BitBoard attacks_by_wki_sq3b = attacks_by_wki_sq3c.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq3a = attacks_by_wki_sq3b.shift<DIR_N>();
constexpr BitBoard attacks_by_wki_sq3d = attacks_by_wki_sq3c.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq3e = attacks_by_wki_sq3d.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq3f = attacks_by_wki_sq3e.shift<DIR_S>();
constexpr BitBoard attacks_by_wki_sq4a = attacks_by_wki_sq3a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4b = attacks_by_wki_sq3b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4c = attacks_by_wki_sq3c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4d = attacks_by_wki_sq3d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4e = attacks_by_wki_sq3e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq4f = attacks_by_wki_sq3f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5a = attacks_by_wki_sq4a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5b = attacks_by_wki_sq4b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5c = attacks_by_wki_sq4c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5d = attacks_by_wki_sq4d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5e = attacks_by_wki_sq4e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq5f = attacks_by_wki_sq4f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6a = attacks_by_wki_sq5a.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6b = attacks_by_wki_sq5b.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6c = attacks_by_wki_sq5c.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6d = attacks_by_wki_sq5d.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6e = attacks_by_wki_sq5e.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq6f = attacks_by_wki_sq5f.shift<DIR_W>();
constexpr BitBoard attacks_by_wki_sq2a = attacks_by_wki_sq3a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2b = attacks_by_wki_sq3b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2c = attacks_by_wki_sq3c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2d = attacks_by_wki_sq3d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2e = attacks_by_wki_sq3e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq2f = attacks_by_wki_sq3f.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1a = attacks_by_wki_sq2a.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1b = attacks_by_wki_sq2b.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1c = attacks_by_wki_sq2c.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1d = attacks_by_wki_sq2d.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1e = attacks_by_wki_sq2e.shift<DIR_E>();
constexpr BitBoard attacks_by_wki_sq1f = attacks_by_wki_sq2f.shift<DIR_E>();

constexpr BitBoard diagonal_6a_to_se
    = bb_6a | bb_5b | bb_4c | bb_3d | bb_2e | bb_1f;
constexpr BitBoard diagonal_5a_to_se = diagonal_6a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_4a_to_se = diagonal_5a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_3a_to_se = diagonal_4a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_2a_to_se = diagonal_3a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_1a_to_se = diagonal_2a_to_se.shift<DIR_N>();
constexpr BitBoard diagonal_6b_to_se = diagonal_6a_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_6c_to_se = diagonal_6b_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_6d_to_se = diagonal_6c_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_6e_to_se = diagonal_6d_to_se.shift<DIR_S>();
constexpr BitBoard diagonal_6f_to_se = diagonal_6e_to_se.shift<DIR_S>();

constexpr BitBoard diagonal_1a_to_sw
    = bb_1a | bb_2b | bb_3c | bb_4d | bb_5e | bb_6f;
constexpr BitBoard diagonal_2a_to_sw = diagonal_1a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_3a_to_sw = diagonal_2a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_4a_to_sw = diagonal_3a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_5a_to_sw = diagonal_4a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_6a_to_sw = diagonal_5a_to_sw.shift<DIR_N>();
constexpr BitBoard diagonal_1b_to_sw = diagonal_1a_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1c_to_sw = diagonal_1b_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1d_to_sw = diagonal_1c_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1e_to_sw = diagonal_1d_to_sw.shift<DIR_S>();
constexpr BitBoard diagonal_1f_to_sw = diagonal_1e_to_sw.shift<DIR_S>();

} // namespace internal

constexpr BitBoard attacks_by_gi[Squares::num_squares][num_colors] = {
    {internal::attacks_by_bgi_sq6a, internal::attacks_by_wgi_sq6a},
    {internal::attacks_by_bgi_sq5a, internal::attacks_by_wgi_sq5a},
    {internal::attacks_by_bgi_sq4a, internal::attacks_by_wgi_sq4a},
    {internal::attacks_by_bgi_sq3a, internal::attacks_by_wgi_sq3a},
    {internal::attacks_by_bgi_sq2a, internal::attacks_by_wgi_sq2a},
    {internal::attacks_by_bgi_sq1a, internal::attacks_by_wgi_sq1a},
    {internal::attacks_by_bgi_sq6b, internal::attacks_by_wgi_sq6b},
    {internal::attacks_by_bgi_sq5b, internal::attacks_by_wgi_sq5b},
    {internal::attacks_by_bgi_sq4b, internal::attacks_by_wgi_sq4b},
    {internal::attacks_by_bgi_sq3b, internal::attacks_by_wgi_sq3b},
    {internal::attacks_by_bgi_sq2b, internal::attacks_by_wgi_sq2b},
    {internal::attacks_by_bgi_sq1b, internal::attacks_by_wgi_sq1b},
    {internal::attacks_by_bgi_sq6c, internal::attacks_by_wgi_sq6c},
    {internal::attacks_by_bgi_sq5c, internal::attacks_by_wgi_sq5c},
    {internal::attacks_by_bgi_sq4c, internal::attacks_by_wgi_sq4c},
    {internal::attacks_by_bgi_sq3c, internal::attacks_by_wgi_sq3c},
    {internal::attacks_by_bgi_sq2c, internal::attacks_by_wgi_sq2c},
    {internal::attacks_by_bgi_sq1c, internal::attacks_by_wgi_sq1c},
    {internal::attacks_by_bgi_sq6d, internal::attacks_by_wgi_sq6d},
    {internal::attacks_by_bgi_sq5d, internal::attacks_by_wgi_sq5d},
    {internal::attacks_by_bgi_sq4d, internal::attacks_by_wgi_sq4d},
    {internal::attacks_by_bgi_sq3d, internal::attacks_by_wgi_sq3d},
    {internal::attacks_by_bgi_sq2d, internal::attacks_by_wgi_sq2d},
    {internal::attacks_by_bgi_sq1d, internal::attacks_by_wgi_sq1d},
    {internal::attacks_by_bgi_sq6e, internal::attacks_by_wgi_sq6e},
    {internal::attacks_by_bgi_sq5e, internal::attacks_by_wgi_sq5e},
    {internal::attacks_by_bgi_sq4e, internal::attacks_by_wgi_sq4e},
    {internal::attacks_by_bgi_sq3e, internal::attacks_by_wgi_sq3e},
    {internal::attacks_by_bgi_sq2e, internal::attacks_by_wgi_sq2e},
    {internal::attacks_by_bgi_sq1e, internal::attacks_by_wgi_sq1e},
    {internal::attacks_by_bgi_sq6f, internal::attacks_by_wgi_sq6f},
    {internal::attacks_by_bgi_sq5f, internal::attacks_by_wgi_sq5f},
    {internal::attacks_by_bgi_sq4f, internal::attacks_by_wgi_sq4f},
    {internal::attacks_by_bgi_sq3f, internal::attacks_by_wgi_sq3f},
    {internal::attacks_by_bgi_sq2f, internal::attacks_by_wgi_sq2f},
    {internal::attacks_by_bgi_sq1f, internal::attacks_by_wgi_sq1f},
};

constexpr BitBoard attacks_by_ki[Squares::num_squares][num_colors] = {
    {internal::attacks_by_bki_sq6a, internal::attacks_by_wki_sq6a},
    {internal::attacks_by_bki_sq5a, internal::attacks_by_wki_sq5a},
    {internal::attacks_by_bki_sq4a, internal::attacks_by_wki_sq4a},
    {internal::attacks_by_bki_sq3a, internal::attacks_by_wki_sq3a},
    {internal::attacks_by_bki_sq2a, internal::attacks_by_wki_sq2a},
    {internal::attacks_by_bki_sq1a, internal::attacks_by_wki_sq1a},
    {internal::attacks_by_bki_sq6b, internal::attacks_by_wki_sq6b},
    {internal::attacks_by_bki_sq5b, internal::attacks_by_wki_sq5b},
    {internal::attacks_by_bki_sq4b, internal::attacks_by_wki_sq4b},
    {internal::attacks_by_bki_sq3b, internal::attacks_by_wki_sq3b},
    {internal::attacks_by_bki_sq2b, internal::attacks_by_wki_sq2b},
    {internal::attacks_by_bki_sq1b, internal::attacks_by_wki_sq1b},
    {internal::attacks_by_bki_sq6c, internal::attacks_by_wki_sq6c},
    {internal::attacks_by_bki_sq5c, internal::attacks_by_wki_sq5c},
    {internal::attacks_by_bki_sq4c, internal::attacks_by_wki_sq4c},
    {internal::attacks_by_bki_sq3c, internal::attacks_by_wki_sq3c},
    {internal::attacks_by_bki_sq2c, internal::attacks_by_wki_sq2c},
    {internal::attacks_by_bki_sq1c, internal::attacks_by_wki_sq1c},
    {internal::attacks_by_bki_sq6d, internal::attacks_by_wki_sq6d},
    {internal::attacks_by_bki_sq5d, internal::attacks_by_wki_sq5d},
    {internal::attacks_by_bki_sq4d, internal::attacks_by_wki_sq4d},
    {internal::attacks_by_bki_sq3d, internal::attacks_by_wki_sq3d},
    {internal::attacks_by_bki_sq2d, internal::attacks_by_wki_sq2d},
    {internal::attacks_by_bki_sq1d, internal::attacks_by_wki_sq1d},
    {internal::attacks_by_bki_sq6e, internal::attacks_by_wki_sq6e},
    {internal::attacks_by_bki_sq5e, internal::attacks_by_wki_sq5e},
    {internal::attacks_by_bki_sq4e, internal::attacks_by_wki_sq4e},
    {internal::attacks_by_bki_sq3e, internal::attacks_by_wki_sq3e},
    {internal::attacks_by_bki_sq2e, internal::attacks_by_wki_sq2e},
    {internal::attacks_by_bki_sq1e, internal::attacks_by_wki_sq1e},
    {internal::attacks_by_bki_sq6f, internal::attacks_by_wki_sq6f},
    {internal::attacks_by_bki_sq5f, internal::attacks_by_wki_sq5f},
    {internal::attacks_by_bki_sq4f, internal::attacks_by_wki_sq4f},
    {internal::attacks_by_bki_sq3f, internal::attacks_by_wki_sq3f},
    {internal::attacks_by_bki_sq2f, internal::attacks_by_wki_sq2f},
    {internal::attacks_by_bki_sq1f, internal::attacks_by_wki_sq1f},
};

constexpr BitBoard attacks_by_ou[Squares::num_squares] = {
    bb_6a.expand_neighbor() & ~bb_6a, bb_5a.expand_neighbor() & ~bb_5a,
    bb_4a.expand_neighbor() & ~bb_4a, bb_3a.expand_neighbor() & ~bb_3a,
    bb_2a.expand_neighbor() & ~bb_2a, bb_1a.expand_neighbor() & ~bb_1a,
    bb_6b.expand_neighbor() & ~bb_6b, bb_5b.expand_neighbor() & ~bb_5b,
    bb_4b.expand_neighbor() & ~bb_4b, bb_3b.expand_neighbor() & ~bb_3b,
    bb_2b.expand_neighbor() & ~bb_2b, bb_1b.expand_neighbor() & ~bb_1b,
    bb_6c.expand_neighbor() & ~bb_6c, bb_5c.expand_neighbor() & ~bb_5c,
    bb_4c.expand_neighbor() & ~bb_4c, bb_3c.expand_neighbor() & ~bb_3c,
    bb_2c.expand_neighbor() & ~bb_2c, bb_1c.expand_neighbor() & ~bb_1c,
    bb_6d.expand_neighbor() & ~bb_6d, bb_5d.expand_neighbor() & ~bb_5d,
    bb_4d.expand_neighbor() & ~bb_4d, bb_3d.expand_neighbor() & ~bb_3d,
    bb_2d.expand_neighbor() & ~bb_2d, bb_1d.expand_neighbor() & ~bb_1d,
    bb_6e.expand_neighbor() & ~bb_6e, bb_5e.expand_neighbor() & ~bb_5e,
    bb_4e.expand_neighbor() & ~bb_4e, bb_3e.expand_neighbor() & ~bb_3e,
    bb_2e.expand_neighbor() & ~bb_2e, bb_1e.expand_neighbor() & ~bb_1e,
    bb_6f.expand_neighbor() & ~bb_6f, bb_5f.expand_neighbor() & ~bb_5f,
    bb_4f.expand_neighbor() & ~bb_4f, bb_3f.expand_neighbor() & ~bb_3f,
    bb_2f.expand_neighbor() & ~bb_2f, bb_1f.expand_neighbor() & ~bb_1f,
};

constexpr BitBoard attacks_by_hi[Squares::num_squares] = {
    (bb_file6 | bb_ranka) & (~bb_6a), (bb_file5 | bb_ranka) & (~bb_5a),
    (bb_file4 | bb_ranka) & (~bb_4a), (bb_file3 | bb_ranka) & (~bb_3a),
    (bb_file2 | bb_ranka) & (~bb_2a), (bb_file1 | bb_ranka) & (~bb_1a),
    (bb_file6 | bb_rankb) & (~bb_6b), (bb_file5 | bb_rankb) & (~bb_5b),
    (bb_file4 | bb_rankb) & (~bb_4b), (bb_file3 | bb_rankb) & (~bb_3b),
    (bb_file2 | bb_rankb) & (~bb_2b), (bb_file1 | bb_rankb) & (~bb_1b),
    (bb_file6 | bb_rankc) & (~bb_6c), (bb_file5 | bb_rankc) & (~bb_5c),
    (bb_file4 | bb_rankc) & (~bb_4c), (bb_file3 | bb_rankc) & (~bb_3c),
    (bb_file2 | bb_rankc) & (~bb_2c), (bb_file1 | bb_rankc) & (~bb_1c),
    (bb_file6 | bb_rankd) & (~bb_6d), (bb_file5 | bb_rankd) & (~bb_5d),
    (bb_file4 | bb_rankd) & (~bb_4d), (bb_file3 | bb_rankd) & (~bb_3d),
    (bb_file2 | bb_rankd) & (~bb_2d), (bb_file1 | bb_rankd) & (~bb_1d),
    (bb_file6 | bb_ranke) & (~bb_6e), (bb_file5 | bb_ranke) & (~bb_5e),
    (bb_file4 | bb_ranke) & (~bb_4e), (bb_file3 | bb_ranke) & (~bb_3e),
    (bb_file2 | bb_ranke) & (~bb_2e), (bb_file1 | bb_ranke) & (~bb_1e),
    (bb_file6 | bb_rankf) & (~bb_6f), (bb_file5 | bb_rankf) & (~bb_5f),
    (bb_file4 | bb_rankf) & (~bb_4f), (bb_file3 | bb_rankf) & (~bb_3f),
    (bb_file2 | bb_rankf) & (~bb_2f), (bb_file1 | bb_rankf) & (~bb_1f),
};

constexpr BitBoard attacks_by_ka[Squares::num_squares] = {
    (internal::diagonal_6a_to_se | internal::diagonal_6a_to_sw) & (~bb_6a),
    (internal::diagonal_5a_to_se | internal::diagonal_5a_to_sw) & (~bb_5a),
    (internal::diagonal_4a_to_se | internal::diagonal_4a_to_sw) & (~bb_4a),
    (internal::diagonal_3a_to_se | internal::diagonal_3a_to_sw) & (~bb_3a),
    (internal::diagonal_2a_to_se | internal::diagonal_2a_to_sw) & (~bb_2a),
    (internal::diagonal_1a_to_se | internal::diagonal_1a_to_sw) & (~bb_1a),
    (internal::diagonal_6b_to_se | internal::diagonal_5a_to_sw) & (~bb_6b),
    (internal::diagonal_6a_to_se | internal::diagonal_4a_to_sw) & (~bb_5b),
    (internal::diagonal_5a_to_se | internal::diagonal_3a_to_sw) & (~bb_4b),
    (internal::diagonal_4a_to_se | internal::diagonal_2a_to_sw) & (~bb_3b),
    (internal::diagonal_3a_to_se | internal::diagonal_1a_to_sw) & (~bb_2b),
    (internal::diagonal_2a_to_se | internal::diagonal_1b_to_sw) & (~bb_1b),
    (internal::diagonal_6c_to_se | internal::diagonal_4a_to_sw) & (~bb_6c),
    (internal::diagonal_6b_to_se | internal::diagonal_3a_to_sw) & (~bb_5c),
    (internal::diagonal_6a_to_se | internal::diagonal_2a_to_sw) & (~bb_4c),
    (internal::diagonal_5a_to_se | internal::diagonal_1a_to_sw) & (~bb_3c),
    (internal::diagonal_4a_to_se | internal::diagonal_1b_to_sw) & (~bb_2c),
    (internal::diagonal_3a_to_se | internal::diagonal_1c_to_sw) & (~bb_1c),
    (internal::diagonal_6d_to_se | internal::diagonal_3a_to_sw) & (~bb_6d),
    (internal::diagonal_6c_to_se | internal::diagonal_2a_to_sw) & (~bb_5d),
    (internal::diagonal_6b_to_se | internal::diagonal_1a_to_sw) & (~bb_4d),
    (internal::diagonal_6a_to_se | internal::diagonal_1b_to_sw) & (~bb_3d),
    (internal::diagonal_5a_to_se | internal::diagonal_1c_to_sw) & (~bb_2d),
    (internal::diagonal_4a_to_se | internal::diagonal_1d_to_sw) & (~bb_1d),
    (internal::diagonal_6e_to_se | internal::diagonal_2a_to_sw) & (~bb_6e),
    (internal::diagonal_6d_to_se | internal::diagonal_1a_to_sw) & (~bb_5e),
    (internal::diagonal_6c_to_se | internal::diagonal_1b_to_sw) & (~bb_4e),
    (internal::diagonal_6b_to_se | internal::diagonal_1c_to_sw) & (~bb_3e),
    (internal::diagonal_6a_to_se | internal::diagonal_1d_to_sw) & (~bb_2e),
    (internal::diagonal_5a_to_se | internal::diagonal_1e_to_sw) & (~bb_1e),
    (internal::diagonal_6f_to_se | internal::diagonal_1a_to_sw) & (~bb_6f),
    (internal::diagonal_6e_to_se | internal::diagonal_1b_to_sw) & (~bb_5f),
    (internal::diagonal_6d_to_se | internal::diagonal_1c_to_sw) & (~bb_4f),
    (internal::diagonal_6c_to_se | internal::diagonal_1d_to_sw) & (~bb_3f),
    (internal::diagonal_6b_to_se | internal::diagonal_1e_to_sw) & (~bb_2f),
    (internal::diagonal_6a_to_se | internal::diagonal_1f_to_sw) & (~bb_1f),
};

inline BitBoard BitBoard::get_attacks_by(
    const Pieces::BoardPieceTypeEnum piece, const Squares::SquareEnum location)
{
    const auto piece_type = Pieces::to_piece_type(piece);
    const auto color = Pieces::get_color(piece);
    switch (piece_type) {
    case Pieces::FU:
        return attacks_by_fu[location][color];
    case Pieces::KE:
        return attacks_by_ke[location][color];
    case Pieces::GI:
        return attacks_by_gi[location][color];
    case Pieces::KA:
        return attacks_by_ka[location];
    case Pieces::KI:
    case Pieces::TO:
    case Pieces::NK:
    case Pieces::NG:
        return attacks_by_ki[location][color];
    case Pieces::HI:
        return attacks_by_hi[location];
    case Pieces::UM:
        return attacks_by_ou[location] | attacks_by_ka[location];
    case Pieces::RY:
        return attacks_by_ou[location] | attacks_by_hi[location];
    case Pieces::OU:
        return attacks_by_ou[location];
    default:
        break;
    }
    return BitBoard(0);
}

inline BitBoard BitBoard::get_attacks_by(
    const Pieces::BoardPieceTypeEnum piece,
    const Squares::SquareEnum location,
    const BitBoard occupied)
{
    const auto piece_type = Pieces::to_piece_type(piece);
    const auto color = Pieces::get_color(piece);
    switch (piece_type) {
    case Pieces::FU:
        return attacks_by_fu[location][color];
    case Pieces::KE:
        return attacks_by_ke[location][color];
    case Pieces::GI:
        return attacks_by_gi[location][color];
    case Pieces::KA:
        return BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case Pieces::KI:
    case Pieces::TO:
    case Pieces::NK:
    case Pieces::NG:
        return attacks_by_ki[location][color];
    case Pieces::HI:
        return BitBoard::ranging_attacks_to_adjacent(location, occupied);
    case Pieces::UM:
        return attacks_by_ou[location]
               | BitBoard::ranging_attacks_to_diagonal(location, occupied);
    case Pieces::RY:
        return attacks_by_ou[location]
               | BitBoard::ranging_attacks_to_adjacent(location, occupied);
    case Pieces::OU:
        return attacks_by_ou[location];
    default:
        break;
    }
    return BitBoard(0);
}

inline BitBoard BitBoard::get_promotion_zone(const ColorEnum& c)
{
    return (c == BLACK) ? (bb_ranka | bb_rankb) : (bb_ranke | bb_rankf);
}

} // namespace vshogi::judkins_shogi

#endif // VSHOGI_JUDKINS_SHOGI_BITBOARD_HPP
