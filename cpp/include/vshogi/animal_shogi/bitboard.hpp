#ifndef VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP
#define VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP

#include <cstdint>

namespace vshogi::animal_shogi
{

/**
 * @brief Binary board representing ON and OFF of each squares.
 * @details
 *            *------------------- C4
 *            |           *------- B1
 *            |           |*------ A1
 *            |           ||
 *            v           vv
 * (MSB) xxxx xxxx xxxx xxxx (LSB)
 *
 */
class BitBoard
{
private:
    std::uint16_t m_value;

public:
    constexpr BitBoard() : m_value(0U)
    {
    }
    constexpr BitBoard(const std::uint16_t v) : m_value(v)
    {
    }
    BitBoard operator~() const
    {
        return BitBoard(~m_value);
    }
    BitBoard& operator&=(const BitBoard other)
    {
        m_value &= other.m_value;
        return *this;
    }
    BitBoard operator&(const BitBoard other) const
    {
        return BitBoard(m_value & other.m_value);
    }
    BitBoard& operator|=(const BitBoard other)
    {
        m_value |= other.m_value;
        return *this;
    }
    BitBoard operator|(const BitBoard other) const
    {
        return BitBoard(m_value | other.m_value);
    }
    BitBoard& operator^=(const BitBoard other)
    {
        m_value ^= other.m_value;
        return *this;
    }
    BitBoard operator^(const BitBoard other) const
    {
        return BitBoard(m_value ^ other.m_value);
    }
    bool operator==(const BitBoard other) const
    {
        return m_value == other.m_value;
    }
    bool operator!=(const BitBoard other) const
    {
        return m_value != other.m_value;
    }
};

constexpr BitBoard rank_1 = BitBoard(0b000000000111);
constexpr BitBoard rank_2 = BitBoard(0b000000111000);
constexpr BitBoard rank_3 = BitBoard(0b000111000000);
constexpr BitBoard rank_4 = BitBoard(0b111000000000);
constexpr BitBoard file_a = BitBoard(0b001001001001);
constexpr BitBoard file_b = BitBoard(0b010010010010);
constexpr BitBoard file_c = BitBoard(0b100100100100);

} // namespace vshogi::animal_shogi

#endif // VSHOGI_ANIMAL_SHOGI_BITBOARD_HPP
