#ifndef VSHOGI_SHOGI_BITBOARD_HPP
#define VSHOGI_SHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/shogi/piece.hpp"
#include "vshogi/shogi/squares.hpp"

namespace vshogi::shogi
{

template <int Mask64bitHigher>
class UInt128
{
private:
#ifdef __SIZEOF_INT128__
    __uint128_t m_value;
    static constexpr __uint128_t mask128
        = (static_cast<__uint128_t>(Mask64bitHigher) << 64)
          | 0xffffffffffffffff;
#elif
    std::uint64_t m_value[2];
#endif

#ifdef __SIZEOF_INT128__
    constexpr UInt128(const __uint128_t& a) : m_value{a}
    {
    }
#elif
    constexpr UInt128(const std::uint64_t& higher, const std::uint64_t& lower)
        : m_value{higher, lower}
    {
    }
#endif

public:
    constexpr UInt128() : m_value{}
    {
    }
#ifdef __SIZEOF_INT128__
    constexpr UInt128(const std::uint64_t& a)
        : m_value{static_cast<__uint128_t>(a)}
    {
    }
#elif
    constexpr UInt128(const std::uint64_t& a) : m_value{0UL, a}
    {
    }
#endif

    constexpr operator bool() const
    {
#ifdef __SIZEOF_INT128__
        return static_cast<bool>(m_value & mask128);
#elif
        return static_cast<bool>(m_value[1] & Mask64bitHigher)
               || static_cast<bool>(m_value[0]);
#endif
    }
    constexpr operator std::uint64_t() const
    {
#ifdef __SIZEOF_INT128__
        return static_cast<std::uint64_t>(m_value);
#elif
        return m_value[0];
#endif
    }

    constexpr UInt128 operator~() const
    {
#ifdef __SIZEOF_INT128__
        return UInt128(~m_value);
#elif
        return UInt128(~m_value[1], ~m_value[0]);
#endif
    }

    constexpr UInt128 operator|(const UInt128& other) const
    {
#ifdef __SIZEOF_INT128__
        return UInt128(m_value | other.m_value);
#elif
        return UInt128(
            m_value[1] | other.m_value[1], m_value[0] | other.m_value[0]);
#endif
    }

    constexpr UInt128 operator&(const UInt128& other) const
    {
#ifdef __SIZEOF_INT128__
        return UInt128(m_value & other.m_value);
#elif
        return UInt128(
            m_value[1] & other.m_value[1], m_value[0] & other.m_value[0]);
#endif
    }

    constexpr UInt128 operator^(const UInt128& other) const
    {
#ifdef __SIZEOF_INT128__
        return UInt128(m_value ^ other.m_value);
#elif
        return UInt128(
            m_value[1] ^ other.m_value[1], m_value[0] ^ other.m_value[0]);
#endif
    }

    constexpr UInt128 operator<<(const unsigned int shift_width) const
    {
#ifdef __SIZEOF_INT128__
        return UInt128(m_value << shift_width);
#elif
        UInt128(lshift_higher(shift_width), lshift_lower(shift_width));
#endif
    }

    constexpr UInt128 operator>>(const unsigned int shift_width) const
    {
#ifdef __SIZEOF_INT128__
        return UInt128((m_value & mask128) >> shift_width);
#elif
        UInt128(
            rshift_higher(shift_width) & Mask64bitHigher,
            rshift_lower(shift_width));
#endif
    }

    UInt128& operator|=(const UInt128& other)
    {
#ifdef __SIZEOF_INT128__
        m_value |= other.m_value;
#elif
        m_value[0] |= other.m_value[0];
        m_value[1] |= other.m_value[1];
#endif
        return *this;
    }
    UInt128& operator&=(const UInt128& other)
    {
#ifdef __SIZEOF_INT128__
        m_value &= other.m_value;
#elif
        m_value[0] &= other.m_value[0];
        m_value[1] &= other.m_value[1];
#endif
        return *this;
    }
    UInt128& operator^=(const UInt128& other)
    {
#ifdef __SIZEOF_INT128__
        m_value ^= other.m_value;
#elif
        m_value[0] ^= other.m_value[0];
        m_value[1] ^= other.m_value[1];
#endif
        return *this;
    }

#ifndef __SIZEOF_INT128__
private:
    constexpr std::uint64_t lshift_lower(const unsigned int shift_width) const
    {
        return (shift_width == 0)   ? m_value[0]
               : (shift_width > 63) ? 0
                                    : (m_value[0] << shift_width);
    }
    constexpr std::uint64_t lshift_carry(const unsigned int shift_width) const
    {
        return static_cast<std::uint64_t>(
            (shift_width == 0)    ? 0
            : (shift_width < 64)  ? (m_value[0] >> (64 - shift_width))
            : (shift_width < 128) ? (m_value[0] << (shift_width - 64))
                                  : 0);
    }
    constexpr std::uint64_t lshift_higher(const unsigned int shift_width) const
    {
        const auto c = lshift_carry(shift_width);
        const auto v = (shift_width < 64) ? m_value[1] << shift_width : 0;
        return v | c;
    }
    constexpr std::uint64_t rshift_lower(const unsigned int shift_width) const
    {
        const auto v = (shift_width == 0)   ? m_value[0]
                       : (shift_width > 63) ? 0
                                            : (m_value[0] >> shift_width);
        const auto c = rshift_carry(shift_width);
        return v | c;
    }
    constexpr std::uint64_t rshift_carry(const unsigned int shift_width) const
    {
        const auto v = static_cast<std::uint64_t>(m_value[1]);
        return (shift_width == 0)     ? 0
               : (shift_width <= 64)  ? (v << (64 - shift_width))
               : (shift_width <= 128) ? (v >> (shift_width - 64))
                                      : 0;
    }
    constexpr std::uint64_t rshift_higher(const unsigned int shift_width) const
    {
        return (shift_width <= 64) ? ((m_value[1]) >> shift_width) : 0;
    }
#endif
};

class BitBoard
{
private:
    /**
     * @brief 128-bit binary representing ON and OFF of squares from 9A to 1I.
     * @details
     *                  +------------------------------------------------------- SQ_1I (80)
     *                  |                 +------------------------------------- SQ_8H (64)
     *                  |                 | +----------------------------------- SQ_9H (63)
     *                  |                 | |                  +---------------- SQ_9B
     *                  |                 | |                  |+--------------- SQ_1A
     *                  |                 | |                  ||       +------- SQ_8A
     *                  |                 | |                  ||       |+------ SQ_9A (0)
     *                  |                 | |                  ||       ||
     *                  v                 v v                  vv       vv
     * (MSB) ... xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx ... xxxxxxxx xxxxxxxx (LSB)
     */
    UInt128<0x01ffff> m_value;

    using SquareEnum = Squares::SquareEnum;
    static constexpr auto SQ_9A = Squares::SQ_9A; // NOLINT
    static constexpr auto SQ_8A = Squares::SQ_8A; // NOLINT
    static constexpr auto SQ_7A = Squares::SQ_7A; // NOLINT
    static constexpr auto SQ_6A = Squares::SQ_6A; // NOLINT
    static constexpr auto SQ_5A = Squares::SQ_5A; // NOLINT
    static constexpr auto SQ_4A = Squares::SQ_4A; // NOLINT
    static constexpr auto SQ_3A = Squares::SQ_3A; // NOLINT
    static constexpr auto SQ_2A = Squares::SQ_2A; // NOLINT
    static constexpr auto SQ_1A = Squares::SQ_1A; // NOLINT
    static constexpr auto SQ_9B = Squares::SQ_9B; // NOLINT
    static constexpr auto SQ_8B = Squares::SQ_8B; // NOLINT
    static constexpr auto SQ_7B = Squares::SQ_7B; // NOLINT
    static constexpr auto SQ_6B = Squares::SQ_6B; // NOLINT
    static constexpr auto SQ_5B = Squares::SQ_5B; // NOLINT
    static constexpr auto SQ_4B = Squares::SQ_4B; // NOLINT
    static constexpr auto SQ_3B = Squares::SQ_3B; // NOLINT
    static constexpr auto SQ_2B = Squares::SQ_2B; // NOLINT
    static constexpr auto SQ_1B = Squares::SQ_1B; // NOLINT
    static constexpr auto SQ_9C = Squares::SQ_9C; // NOLINT
    static constexpr auto SQ_8C = Squares::SQ_8C; // NOLINT
    static constexpr auto SQ_7C = Squares::SQ_7C; // NOLINT
    static constexpr auto SQ_6C = Squares::SQ_6C; // NOLINT
    static constexpr auto SQ_5C = Squares::SQ_5C; // NOLINT
    static constexpr auto SQ_4C = Squares::SQ_4C; // NOLINT
    static constexpr auto SQ_3C = Squares::SQ_3C; // NOLINT
    static constexpr auto SQ_2C = Squares::SQ_2C; // NOLINT
    static constexpr auto SQ_1C = Squares::SQ_1C; // NOLINT
    static constexpr auto SQ_9D = Squares::SQ_9D; // NOLINT
    static constexpr auto SQ_8D = Squares::SQ_8D; // NOLINT
    static constexpr auto SQ_7D = Squares::SQ_7D; // NOLINT
    static constexpr auto SQ_6D = Squares::SQ_6D; // NOLINT
    static constexpr auto SQ_5D = Squares::SQ_5D; // NOLINT
    static constexpr auto SQ_4D = Squares::SQ_4D; // NOLINT
    static constexpr auto SQ_3D = Squares::SQ_3D; // NOLINT
    static constexpr auto SQ_2D = Squares::SQ_2D; // NOLINT
    static constexpr auto SQ_1D = Squares::SQ_1D; // NOLINT
    static constexpr auto SQ_9E = Squares::SQ_9E; // NOLINT
    static constexpr auto SQ_8E = Squares::SQ_8E; // NOLINT
    static constexpr auto SQ_7E = Squares::SQ_7E; // NOLINT
    static constexpr auto SQ_6E = Squares::SQ_6E; // NOLINT
    static constexpr auto SQ_5E = Squares::SQ_5E; // NOLINT
    static constexpr auto SQ_4E = Squares::SQ_4E; // NOLINT
    static constexpr auto SQ_3E = Squares::SQ_3E; // NOLINT
    static constexpr auto SQ_2E = Squares::SQ_2E; // NOLINT
    static constexpr auto SQ_1E = Squares::SQ_1E; // NOLINT
    static constexpr auto SQ_9F = Squares::SQ_9F; // NOLINT
    static constexpr auto SQ_8F = Squares::SQ_8F; // NOLINT
    static constexpr auto SQ_7F = Squares::SQ_7F; // NOLINT
    static constexpr auto SQ_6F = Squares::SQ_6F; // NOLINT
    static constexpr auto SQ_5F = Squares::SQ_5F; // NOLINT
    static constexpr auto SQ_4F = Squares::SQ_4F; // NOLINT
    static constexpr auto SQ_3F = Squares::SQ_3F; // NOLINT
    static constexpr auto SQ_2F = Squares::SQ_2F; // NOLINT
    static constexpr auto SQ_1F = Squares::SQ_1F; // NOLINT
    static constexpr auto SQ_9G = Squares::SQ_9G; // NOLINT
    static constexpr auto SQ_8G = Squares::SQ_8G; // NOLINT
    static constexpr auto SQ_7G = Squares::SQ_7G; // NOLINT
    static constexpr auto SQ_6G = Squares::SQ_6G; // NOLINT
    static constexpr auto SQ_5G = Squares::SQ_5G; // NOLINT
    static constexpr auto SQ_4G = Squares::SQ_4G; // NOLINT
    static constexpr auto SQ_3G = Squares::SQ_3G; // NOLINT
    static constexpr auto SQ_2G = Squares::SQ_2G; // NOLINT
    static constexpr auto SQ_1G = Squares::SQ_1G; // NOLINT
    static constexpr auto SQ_9H = Squares::SQ_9H; // NOLINT
    static constexpr auto SQ_8H = Squares::SQ_8H; // NOLINT
    static constexpr auto SQ_7H = Squares::SQ_7H; // NOLINT
    static constexpr auto SQ_6H = Squares::SQ_6H; // NOLINT
    static constexpr auto SQ_5H = Squares::SQ_5H; // NOLINT
    static constexpr auto SQ_4H = Squares::SQ_4H; // NOLINT
    static constexpr auto SQ_3H = Squares::SQ_3H; // NOLINT
    static constexpr auto SQ_2H = Squares::SQ_2H; // NOLINT
    static constexpr auto SQ_1H = Squares::SQ_1H; // NOLINT
    static constexpr auto SQ_9I = Squares::SQ_9I; // NOLINT
    static constexpr auto SQ_8I = Squares::SQ_8I; // NOLINT
    static constexpr auto SQ_7I = Squares::SQ_7I; // NOLINT
    static constexpr auto SQ_6I = Squares::SQ_6I; // NOLINT
    static constexpr auto SQ_5I = Squares::SQ_5I; // NOLINT
    static constexpr auto SQ_4I = Squares::SQ_4I; // NOLINT
    static constexpr auto SQ_3I = Squares::SQ_3I; // NOLINT
    static constexpr auto SQ_2I = Squares::SQ_2I; // NOLINT
    static constexpr auto SQ_1I = Squares::SQ_1I; // NOLINT

public:
    constexpr BitBoard() : m_value{}
    {
    }
    constexpr BitBoard(const std::uint64_t v) : m_value{v}
    {
    }
    constexpr BitBoard(const UInt128<0x01ffff>& v) : m_value{v}
    {
    }
    static void init_tables();
    static BitBoard from_square(const SquareEnum sq);
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
        return static_cast<bool>(m_value);
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
        constexpr auto bb_f12345678
            = ~((BitBoard(1) << static_cast<unsigned int>(SQ_9A))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_9B))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_9C))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_9D))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_9E))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_9F))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_9G))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_9H))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_9I)));
        constexpr auto bb_f23456789
            = ~((BitBoard(1) << static_cast<unsigned int>(SQ_1A))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_1B))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_1C))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_1D))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_1E))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_1F))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_1G))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_1H))
                | (BitBoard(1) << static_cast<unsigned int>(SQ_1I)));
        constexpr BitBoard mask[] = {
            // clang-format off
            bb_f12345678, ~BitBoard(0), bb_f23456789,
            bb_f12345678,               bb_f23456789,
            bb_f12345678, ~BitBoard(0), bb_f23456789,
            bb_f12345678,               bb_f23456789,
            bb_f12345678,               bb_f23456789,
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
        return static_cast<bool>(
            (UInt128<0x01ffff>(1UL) << static_cast<unsigned int>(sq))
            & m_value);
    }
    int hamming_weight() const
    {
        return hamming_weight_64bit(static_cast<std::uint64_t>(m_value))
               + hamming_weight_64bit(
                   static_cast<std::uint64_t>(m_value >> 64U));
    }

private:
    static int hamming_weight_64bit(std::uint64_t x)
    {
        // https://en.wikipedia.org/wiki/Hamming_weight
        constexpr std::uint64_t m1 = 0x5555555555555555; // 0101...
        constexpr std::uint64_t m2 = 0x3333333333333333; // 00110011..
        constexpr std::uint64_t m4 = 0x0f0f0f0f0f0f0f0f; // 0000111100001111..
        x -= (x >> 1) & m1; //put count of each 2 bits into those 2 bits
        x = (x & m2) + ((x >> 2) & m2); // each 4 bits into those 4 bits
        x = (x + (x >> 4)) & m4; //put count of each 8 bits into those 8 bits
        x += x >> 8; //put count of each 16 bits into their lowest 8 bits
        x += x >> 16; //put count of each 32 bits into their lowest 8 bits
        x += x >> 32; //put count of each 64 bits into their lowest 8 bits
        return x & 0x7f;
    }
};

// clang-format off
constexpr BitBoard bb_1a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1A));
constexpr BitBoard bb_1b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1B));
constexpr BitBoard bb_1c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1C));
constexpr BitBoard bb_1d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1D));
constexpr BitBoard bb_1e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1E));
constexpr BitBoard bb_1f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1F));
constexpr BitBoard bb_1g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1G));
constexpr BitBoard bb_1h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1H));
constexpr BitBoard bb_1i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_1I));
constexpr BitBoard bb_2a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2A));
constexpr BitBoard bb_2b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2B));
constexpr BitBoard bb_2c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2C));
constexpr BitBoard bb_2d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2D));
constexpr BitBoard bb_2e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2E));
constexpr BitBoard bb_2f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2F));
constexpr BitBoard bb_2g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2G));
constexpr BitBoard bb_2h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2H));
constexpr BitBoard bb_2i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_2I));
constexpr BitBoard bb_3a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3A));
constexpr BitBoard bb_3b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3B));
constexpr BitBoard bb_3c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3C));
constexpr BitBoard bb_3d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3D));
constexpr BitBoard bb_3e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3E));
constexpr BitBoard bb_3f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3F));
constexpr BitBoard bb_3g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3G));
constexpr BitBoard bb_3h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3H));
constexpr BitBoard bb_3i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_3I));
constexpr BitBoard bb_4a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4A));
constexpr BitBoard bb_4b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4B));
constexpr BitBoard bb_4c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4C));
constexpr BitBoard bb_4d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4D));
constexpr BitBoard bb_4e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4E));
constexpr BitBoard bb_4f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4F));
constexpr BitBoard bb_4g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4G));
constexpr BitBoard bb_4h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4H));
constexpr BitBoard bb_4i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_4I));
constexpr BitBoard bb_5a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5A));
constexpr BitBoard bb_5b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5B));
constexpr BitBoard bb_5c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5C));
constexpr BitBoard bb_5d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5D));
constexpr BitBoard bb_5e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5E));
constexpr BitBoard bb_5f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5F));
constexpr BitBoard bb_5g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5G));
constexpr BitBoard bb_5h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5H));
constexpr BitBoard bb_5i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_5I));
constexpr BitBoard bb_6a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6A));
constexpr BitBoard bb_6b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6B));
constexpr BitBoard bb_6c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6C));
constexpr BitBoard bb_6d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6D));
constexpr BitBoard bb_6e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6E));
constexpr BitBoard bb_6f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6F));
constexpr BitBoard bb_6g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6G));
constexpr BitBoard bb_6h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6H));
constexpr BitBoard bb_6i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_6I));
constexpr BitBoard bb_7a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7A));
constexpr BitBoard bb_7b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7B));
constexpr BitBoard bb_7c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7C));
constexpr BitBoard bb_7d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7D));
constexpr BitBoard bb_7e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7E));
constexpr BitBoard bb_7f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7F));
constexpr BitBoard bb_7g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7G));
constexpr BitBoard bb_7h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7H));
constexpr BitBoard bb_7i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_7I));
constexpr BitBoard bb_8a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8A));
constexpr BitBoard bb_8b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8B));
constexpr BitBoard bb_8c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8C));
constexpr BitBoard bb_8d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8D));
constexpr BitBoard bb_8e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8E));
constexpr BitBoard bb_8f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8F));
constexpr BitBoard bb_8g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8G));
constexpr BitBoard bb_8h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8H));
constexpr BitBoard bb_8i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_8I));
constexpr BitBoard bb_9a = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9A));
constexpr BitBoard bb_9b = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9B));
constexpr BitBoard bb_9c = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9C));
constexpr BitBoard bb_9d = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9D));
constexpr BitBoard bb_9e = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9E));
constexpr BitBoard bb_9f = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9F));
constexpr BitBoard bb_9g = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9G));
constexpr BitBoard bb_9h = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9H));
constexpr BitBoard bb_9i = (BitBoard(1) << static_cast<unsigned int>(Squares::SQ_9I));
constexpr BitBoard bb_file1 = bb_1a | bb_1b | bb_1c | bb_1d | bb_1e | bb_1f | bb_1g | bb_1h | bb_1i;
constexpr BitBoard bb_file2 = bb_2a | bb_2b | bb_2c | bb_2d | bb_2e | bb_2f | bb_2g | bb_2h | bb_2i;
constexpr BitBoard bb_file3 = bb_3a | bb_3b | bb_3c | bb_3d | bb_3e | bb_3f | bb_3g | bb_3h | bb_3i;
constexpr BitBoard bb_file4 = bb_4a | bb_4b | bb_4c | bb_4d | bb_4e | bb_4f | bb_4g | bb_4h | bb_4i;
constexpr BitBoard bb_file5 = bb_5a | bb_5b | bb_5c | bb_5d | bb_5e | bb_5f | bb_5g | bb_5h | bb_5i;
constexpr BitBoard bb_file6 = bb_6a | bb_6b | bb_6c | bb_6d | bb_6e | bb_6f | bb_6g | bb_6h | bb_6i;
constexpr BitBoard bb_file7 = bb_7a | bb_7b | bb_7c | bb_7d | bb_7e | bb_7f | bb_7g | bb_7h | bb_7i;
constexpr BitBoard bb_file8 = bb_8a | bb_8b | bb_8c | bb_8d | bb_8e | bb_8f | bb_8g | bb_8h | bb_8i;
constexpr BitBoard bb_file9 = bb_9a | bb_9b | bb_9c | bb_9d | bb_9e | bb_9f | bb_9g | bb_9h | bb_9i;
constexpr BitBoard bb_ranka = bb_1a | bb_2a | bb_3a | bb_4a | bb_5a | bb_6a | bb_7a | bb_8a | bb_9a;
constexpr BitBoard bb_rankb = bb_1b | bb_2b | bb_3b | bb_4b | bb_5b | bb_6b | bb_7b | bb_8b | bb_9b;
constexpr BitBoard bb_rankc = bb_1c | bb_2c | bb_3c | bb_4c | bb_5c | bb_6c | bb_7c | bb_8c | bb_9c;
constexpr BitBoard bb_rankd = bb_1d | bb_2d | bb_3d | bb_4d | bb_5d | bb_6d | bb_7d | bb_8d | bb_9d;
constexpr BitBoard bb_ranke = bb_1e | bb_2e | bb_3e | bb_4e | bb_5e | bb_6e | bb_7e | bb_8e | bb_9e;
constexpr BitBoard bb_rankf = bb_1f | bb_2f | bb_3f | bb_4f | bb_5f | bb_6f | bb_7f | bb_8f | bb_9f;
constexpr BitBoard bb_rankg = bb_1g | bb_2g | bb_3g | bb_4g | bb_5g | bb_6g | bb_7g | bb_8g | bb_9g;
constexpr BitBoard bb_rankh = bb_1h | bb_2h | bb_3h | bb_4h | bb_5h | bb_6h | bb_7h | bb_8h | bb_9h;
constexpr BitBoard bb_ranki = bb_1i | bb_2i | bb_3i | bb_4i | bb_5i | bb_6i | bb_7i | bb_8i | bb_9i;
// clang-format on

inline BitBoard BitBoard::get_promotion_zone(const ColorEnum& c)
{
    return (c == BLACK) ? (bb_ranka | bb_rankb | bb_rankc)
                        : (bb_rankg | bb_rankh | bb_ranki);
}

} // namespace vshogi::shogi

#endif // VSHOGI_SHOGI_BITBOARD_HPP
