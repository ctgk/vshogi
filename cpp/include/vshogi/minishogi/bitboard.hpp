#ifndef VSHOGI_MINISHOGI_BITBOARD_HPP
#define VSHOGI_MINISHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/color.hpp"
#include "vshogi/minishogi/piece.hpp"
#include "vshogi/minishogi/squares.hpp"

namespace vshogi::minishogi
{

/**
 * @brief 32-bit binary board representing ON and OFF of each square.
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
    using SquareEnum = Squares::SquareEnum;
    static constexpr auto direction_to_delta = Squares::direction_to_delta;
    static constexpr auto SQ_5A = Squares::SQ_5A; // NOLINT
    static constexpr auto SQ_4A = Squares::SQ_4A; // NOLINT
    static constexpr auto SQ_3A = Squares::SQ_3A; // NOLINT
    static constexpr auto SQ_2A = Squares::SQ_2A; // NOLINT
    static constexpr auto SQ_1A = Squares::SQ_1A; // NOLINT
    static constexpr auto SQ_5B = Squares::SQ_5B; // NOLINT
    static constexpr auto SQ_4B = Squares::SQ_4B; // NOLINT
    static constexpr auto SQ_3B = Squares::SQ_3B; // NOLINT
    static constexpr auto SQ_2B = Squares::SQ_2B; // NOLINT
    static constexpr auto SQ_1B = Squares::SQ_1B; // NOLINT
    static constexpr auto SQ_5C = Squares::SQ_5C; // NOLINT
    static constexpr auto SQ_4C = Squares::SQ_4C; // NOLINT
    static constexpr auto SQ_3C = Squares::SQ_3C; // NOLINT
    static constexpr auto SQ_2C = Squares::SQ_2C; // NOLINT
    static constexpr auto SQ_1C = Squares::SQ_1C; // NOLINT
    static constexpr auto SQ_5D = Squares::SQ_5D; // NOLINT
    static constexpr auto SQ_4D = Squares::SQ_4D; // NOLINT
    static constexpr auto SQ_3D = Squares::SQ_3D; // NOLINT
    static constexpr auto SQ_2D = Squares::SQ_2D; // NOLINT
    static constexpr auto SQ_1D = Squares::SQ_1D; // NOLINT
    static constexpr auto SQ_5E = Squares::SQ_5E; // NOLINT
    static constexpr auto SQ_4E = Squares::SQ_4E; // NOLINT
    static constexpr auto SQ_3E = Squares::SQ_3E; // NOLINT
    static constexpr auto SQ_2E = Squares::SQ_2E; // NOLINT
    static constexpr auto SQ_1E = Squares::SQ_1E; // NOLINT

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
    static BitBoard get_attacks_by(
        const Pieces::BoardPieceTypeEnum& piece,
        const Squares::SquareEnum& location);
    static BitBoard get_attacks_by(
        const Pieces::BoardPieceTypeEnum& piece,
        const Squares::SquareEnum& location,
        const BitBoard& occupied);

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

    constexpr std::uint32_t get_value() const
    {
        return m_value;
    }
    constexpr BitBoard operator|(const BitBoard other) const
    {
        return BitBoard(m_value | other.m_value);
    }
    BitBoard& operator|=(const BitBoard& other)
    {
        m_value = static_cast<std::uint32_t>(m_value | other.m_value);
        return *this;
    }
    constexpr BitBoard operator&(const BitBoard other) const
    {
        return BitBoard(m_value & other.m_value);
    }
    BitBoard& operator&=(const BitBoard& other)
    {
        m_value = static_cast<std::uint32_t>(m_value & other.m_value);
        return *this;
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
        BitBoard r = BitBoard::from_square(sq).shift<D>();
        if ((r & occupied).any())
            return r;
        r |= r.shift<D>();
        if ((r & occupied).any())
            return r;
        r |= r.shift<D>();
        if ((r & occupied).any())
            return r;
        r |= r.shift<D>();
        return r;
    }
};

void init_non_ranging_attacks_table();

constexpr BitBoard bb_1a = BitBoard::from_square(Squares::SQ_1A);
constexpr BitBoard bb_1b = BitBoard::from_square(Squares::SQ_1B);
constexpr BitBoard bb_1c = BitBoard::from_square(Squares::SQ_1C);
constexpr BitBoard bb_1d = BitBoard::from_square(Squares::SQ_1D);
constexpr BitBoard bb_1e = BitBoard::from_square(Squares::SQ_1E);
constexpr BitBoard bb_2a = BitBoard::from_square(Squares::SQ_2A);
constexpr BitBoard bb_2b = BitBoard::from_square(Squares::SQ_2B);
constexpr BitBoard bb_2c = BitBoard::from_square(Squares::SQ_2C);
constexpr BitBoard bb_2d = BitBoard::from_square(Squares::SQ_2D);
constexpr BitBoard bb_2e = BitBoard::from_square(Squares::SQ_2E);
constexpr BitBoard bb_3a = BitBoard::from_square(Squares::SQ_3A);
constexpr BitBoard bb_3b = BitBoard::from_square(Squares::SQ_3B);
constexpr BitBoard bb_3c = BitBoard::from_square(Squares::SQ_3C);
constexpr BitBoard bb_3d = BitBoard::from_square(Squares::SQ_3D);
constexpr BitBoard bb_3e = BitBoard::from_square(Squares::SQ_3E);
constexpr BitBoard bb_4a = BitBoard::from_square(Squares::SQ_4A);
constexpr BitBoard bb_4b = BitBoard::from_square(Squares::SQ_4B);
constexpr BitBoard bb_4c = BitBoard::from_square(Squares::SQ_4C);
constexpr BitBoard bb_4d = BitBoard::from_square(Squares::SQ_4D);
constexpr BitBoard bb_4e = BitBoard::from_square(Squares::SQ_4E);
constexpr BitBoard bb_5a = BitBoard::from_square(Squares::SQ_5A);
constexpr BitBoard bb_5b = BitBoard::from_square(Squares::SQ_5B);
constexpr BitBoard bb_5c = BitBoard::from_square(Squares::SQ_5C);
constexpr BitBoard bb_5d = BitBoard::from_square(Squares::SQ_5D);
constexpr BitBoard bb_5e = BitBoard::from_square(Squares::SQ_5E);

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

} // namespace vshogi::minishogi

#endif // VSHOGI_MINISHOGI_BITBOARD_HPP
