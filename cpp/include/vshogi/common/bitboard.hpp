#ifndef VSHOGI_BITBOARD_HPP
#define VSHOGI_BITBOARD_HPP

#include <cstdint>

#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

template <class UInt, class Squares, class BoardPiece, uint NumAttackTypes>
class BitBoard
{
public:
    using SquareEnum = typename Squares::SquareEnum;

private:
    UInt m_value;

    static constexpr UInt mask
        = (static_cast<UInt>(1) << Squares::num_squares) - static_cast<UInt>(1);
    static const BitBoard square_to_bitboard_array[Squares::num_squares + 1U];
    static BitBoard attacks_table[NumAttackTypes][Squares::num_squares];

public:
    constexpr BitBoard() : m_value()
    {
    }
    template <class Int>
    constexpr BitBoard(const Int& v) : m_value(UInt(v) & mask)
    {
    }
    constexpr BitBoard operator~() const
    {
        return BitBoard(~m_value);
    }
    constexpr BitBoard operator|(const BitBoard& other) const
    {
        return BitBoard(m_value | other.m_value);
    }
    BitBoard& operator|=(const BitBoard& other)
    {
        m_value |= other.m_value;
        return *this;
    }
    constexpr BitBoard operator&(const BitBoard& other) const
    {
        return BitBoard(m_value & other.m_value);
    }
    BitBoard& operator&=(const BitBoard& other)
    {
        m_value &= other.m_value;
        return *this;
    }
    constexpr BitBoard operator<<(const uint& shift_width) const
    {
        return BitBoard(m_value << shift_width);
    }
    constexpr BitBoard operator>>(const uint& shift_width) const
    {
        return BitBoard(m_value >> shift_width);
    }
    constexpr bool operator==(const BitBoard& other) const
    {
        return m_value == other.m_value;
    }
    constexpr bool any() const
    {
        return static_cast<bool>(m_value);
    }
    bool is_one(const SquareEnum& sq) const
    {
        return static_cast<bool>(from_square(sq).m_value & m_value);
    }
    uint hamming_weight() const
    {
        return hamming_weight_64bit(static_cast<std::uint64_t>(m_value));
    }

    static BitBoard from_square(const SquareEnum& sq)
    {
        return square_to_bitboard_array[sq];
    }
    static constexpr BitBoard get_promotion_zone(const ColorEnum& c);

    template <DirectionEnum Dir>
    constexpr BitBoard shift() const;
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

    template <DirectionEnum Dir>
    static BitBoard
    ranging_attacks_to(SquareEnum sq, const BitBoard& occupied = BitBoard())
    {
        BitBoard out{};
        while (true) {
            sq = Squares::shift(sq, Dir);
            if (sq == Squares::SQ_NA)
                break; // reached the end of the board
            else if (occupied.is_one(sq)) {
                out |= BitBoard::from_square(sq);
                break; // reached a piece
            } else {
                out |= BitBoard::from_square(sq); // continue
            }
        }
        return out;
    }
    static BitBoard ranging_attacks_to_adjacent(
        const SquareEnum& sq, const BitBoard& occupied = BitBoard())
    {
        return ranging_attacks_to<DIR_N>(sq, occupied)
               | ranging_attacks_to<DIR_E>(sq, occupied)
               | ranging_attacks_to<DIR_W>(sq, occupied)
               | ranging_attacks_to<DIR_S>(sq, occupied);
    }
    static BitBoard ranging_attacks_to_diagonal(
        const SquareEnum& sq, const BitBoard& occupied = BitBoard())
    {
        return ranging_attacks_to<DIR_NW>(sq, occupied)
               | ranging_attacks_to<DIR_NE>(sq, occupied)
               | ranging_attacks_to<DIR_SW>(sq, occupied)
               | ranging_attacks_to<DIR_SE>(sq, occupied);
    }

    static BitBoard get_attacks_by(const BoardPiece& p, const SquareEnum& sq);
    static BitBoard get_attacks_by(
        const BoardPiece& p, const SquareEnum& sq, const BitBoard& occupied);
    static void init_tables();

private:
    uint hamming_weight_64bit(std::uint64_t x) const
    {
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

} // namespace vshogi

#endif // VSHOGI_BITBOARD_HPP
