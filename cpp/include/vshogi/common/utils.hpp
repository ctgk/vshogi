#ifndef VSHOGI_COMMON_UTILS_HPP
#define VSHOGI_COMMON_UTILS_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace vshogi
{

using uint = unsigned int;

/**
 * @brief Iterator for Enum.
 *
 * @tparam ContiguousEnum Type of Enum.
 * @tparam End Non-inclusive end value of the iterator.
 */
template <class ContiguousEnum, uint End>
class EnumIterator
{
private:
    uint m_curr;

public:
    EnumIterator() : m_curr(0u)
    {
    }
    EnumIterator& operator++()
    {
        ++m_curr;
        return *this;
    }
    ContiguousEnum operator*() const
    {
        return static_cast<ContiguousEnum>(m_curr);
    }
    EnumIterator begin()
    {
        return *this;
    }
    EnumIterator end()
    {
        static const auto end_iter = EnumIterator(End);
        return end_iter;
    }
    bool operator!=(const EnumIterator& other)
    {
        return m_curr != other.m_curr;
    }

private:
    EnumIterator(const uint v) : m_curr(v)
    {
    }
};

inline void softmax(std::vector<float>& logits)
{
    if (logits.empty())
        return;
    const float maximum_value
        = *std::max_element(logits.cbegin(), logits.cend());
    float sum = 0.f;
    for (auto&& e : logits) {
        e -= maximum_value;
        e = std::exp(e);
        sum += e;
    }
    for (auto&& e : logits) {
        e /= sum;
    }
}

constexpr uint ntz(const std::uint64_t x)
{
    // https://stackoverflow.com/questions/45221914/how-do-you-efficiently-count-the-trailing-zero-bits-in-a-number

    // We return the number of trailing zeros in
    // the binary representation of x.
    //
    // We have that 0 <= x < 2^64.
    //
    // We begin by applying a function sensitive only
    // to the least significant bit (lsb) of x:
    //
    //   x -> x^(x-1)  e.g. 0b11001000 -> 0b00001111
    //
    // Observe that x^(x-1) == 2^(ntz(x)+1) - 1.

    const std::uint64_t y = x ^ (x - 1);

    // Next, we multiply by 0x03f79d71b4cb0a89,
    // and then roll off the first 58 bits.

    constexpr std::uint64_t debruijn = 0x03f79d71b4cb0a89;

    const std::uint8_t z = static_cast<std::uint8_t>((debruijn * y) >> 58);

    // What? Don't look at me like that.
    //
    // With 58 bits rolled off, only 6 bits remain,
    // so we must have one of 0, 1, 2, ..., 63.
    //
    // It turns out this number was judiciously
    // chosen to make it so each of the possible
    // values for y were mapped into distinct slots.
    //
    // So we just use a look-up table of all 64
    // possible answers, which have been precomputed in
    // advance by the the sort of people who write
    // chess engines in their spare time:

    constexpr uint lookup[]
        = {0,  47, 1,  56, 48, 27, 2,  60, 57, 49, 41, 37, 28, 16, 3,  61,
           54, 58, 35, 52, 50, 42, 21, 44, 38, 32, 29, 23, 17, 11, 4,  62,
           46, 55, 26, 59, 40, 36, 15, 53, 34, 51, 20, 43, 31, 22, 10, 45,
           25, 39, 14, 33, 19, 30, 9,  24, 13, 18, 8,  12, 7,  6,  5,  63};

    return lookup[z];
}

#ifdef __SIZEOF_INT128__
using uint128 = __uint128_t;
#else
class UInt128
{
private:
    std::uint64_t m_value[2];

    constexpr UInt128(const std::uint64_t& higher, const std::uint64_t& lower)
        : m_value{higher, lower}
    {
    }

public:
    constexpr UInt128() : m_value{}
    {
    }
    constexpr UInt128(const std::uint64_t& a) : m_value{0UL, a}
    {
    }

    constexpr operator bool() const
    {
        return static_cast<bool>(m_value[1]) || static_cast<bool>(m_value[0]);
    }
    constexpr operator std::uint64_t() const
    {
        return m_value[0];
    }

    constexpr UInt128 operator~() const
    {
        return UInt128(~m_value[1], ~m_value[0]);
    }

    constexpr UInt128 operator|(const UInt128& other) const
    {
        return UInt128(
            m_value[1] | other.m_value[1], m_value[0] | other.m_value[0]);
    }

    constexpr UInt128 operator&(const UInt128& other) const
    {
        return UInt128(
            m_value[1] & other.m_value[1], m_value[0] & other.m_value[0]);
    }

    constexpr UInt128 operator^(const UInt128& other) const
    {
        return UInt128(
            m_value[1] ^ other.m_value[1], m_value[0] ^ other.m_value[0]);
    }

    constexpr UInt128 operator<<(const uint shift_width) const
    {
        UInt128(lshift_higher(shift_width), lshift_lower(shift_width));
    }

    constexpr UInt128 operator>>(const uint shift_width) const
    {
        UInt128(rshift_higher(shift_width), rshift_lower(shift_width));
    }

    UInt128& operator|=(const UInt128& other)
    {
        m_value[0] |= other.m_value[0];
        m_value[1] |= other.m_value[1];
        return *this;
    }
    UInt128& operator&=(const UInt128& other)
    {
        m_value[0] &= other.m_value[0];
        m_value[1] &= other.m_value[1];
        return *this;
    }
    UInt128& operator^=(const UInt128& other)
    {
        m_value[0] ^= other.m_value[0];
        m_value[1] ^= other.m_value[1];
        return *this;
    }

private:
    constexpr std::uint64_t lshift_lower(const uint shift_width) const
    {
        return (shift_width == 0)   ? m_value[0]
               : (shift_width > 63) ? 0
                                    : (m_value[0] << shift_width);
    }
    constexpr std::uint64_t lshift_carry(const uint shift_width) const
    {
        return static_cast<std::uint64_t>(
            (shift_width == 0)    ? 0
            : (shift_width < 64)  ? (m_value[0] >> (64 - shift_width))
            : (shift_width < 128) ? (m_value[0] << (shift_width - 64))
                                  : 0);
    }
    constexpr std::uint64_t lshift_higher(const uint shift_width) const
    {
        const auto c = lshift_carry(shift_width);
        const auto v = (shift_width < 64) ? m_value[1] << shift_width : 0;
        return v | c;
    }
    constexpr std::uint64_t rshift_lower(const uint shift_width) const
    {
        const auto v = (shift_width == 0)   ? m_value[0]
                       : (shift_width > 63) ? 0
                                            : (m_value[0] >> shift_width);
        const auto c = rshift_carry(shift_width);
        return v | c;
    }
    constexpr std::uint64_t rshift_carry(const uint shift_width) const
    {
        const auto v = static_cast<std::uint64_t>(m_value[1]);
        return (shift_width == 0)     ? 0
               : (shift_width <= 64)  ? (v << (64 - shift_width))
               : (shift_width <= 128) ? (v >> (shift_width - 64))
                                      : 0;
    }
    constexpr std::uint64_t rshift_higher(const uint shift_width) const
    {
        return (shift_width <= 64) ? ((m_value[1]) >> shift_width) : 0;
    }
};
using uint128 = UInt128;
#endif

template <class UInt>
inline uint hamming_weight(UInt x);

template <>
inline uint hamming_weight(std::uint32_t x)
{
    // https://en.wikipedia.org/wiki/Hamming_weight
    constexpr std::uint32_t m1 = 0x55555555;
    constexpr std::uint32_t m2 = 0x33333333;
    constexpr std::uint32_t m4 = 0x0f0f0f0f;
    x -= (x >> 1U) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    x += x >> 8;
    x += x >> 16;
    return x & 0x7f;
}

template <>
inline uint hamming_weight(std::uint16_t x)
{
    return hamming_weight(static_cast<std::uint32_t>(x));
}

template <>
inline uint hamming_weight(std::uint64_t x)
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

template <>
inline uint hamming_weight(uint128 x)
{
    return hamming_weight(static_cast<std::uint64_t>(x))
           + hamming_weight(static_cast<std::uint64_t>(x >> 64));
}

} // namespace vshogi

#endif // VSHOGI_COMMON_UTILS_HPP
