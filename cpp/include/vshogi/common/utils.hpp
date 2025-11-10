#ifndef VSHOGI_COMMON_UTILS_HPP
#define VSHOGI_COMMON_UTILS_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <type_traits>
#include <vector>

namespace vshogi
{

using uint = unsigned int;
using ZobristHashType = std::uint64_t;
static std::random_device seed_gen;
static std::default_random_engine random_engine(seed_gen());
static std::uniform_real_distribution<float>
    dist01(0.f, 0.9999f); // for numerical stability

/**
 * @brief
 * https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
 *
 * @tparam T
 * @param val
 * @return int
 */
template <typename T>
int sign(T val)
{
    return (T(0) < val) - (val < T(0));
}

enum FullPieceTypes : uint
{
    PT_FU, //!< Fu (Pawn)
    PT_KY, //!< Kyo (Lance)
    PT_KE, //!< Kei (Knight)
    PT_GI, //!< Gin (Silver)
    PT_KA, //!< Kaku (Bishop)
    PT_HI, //!< Hisha (Rook)
    PT_KI, //!< Kin (Gold)
    PT_OU, //!< Ou, Gyoku (King)
    PT_TO, //!< Tokin (Promoted Pawn)
    PT_NY, //!< Nari-Kyo (Promoted Lance)
    PT_NK, //!< Nari-Kei (Promoted Knight)
    PT_NG, //!< Nari-Gin (Promoted Silver)
    PT_UM, //!< Uma (Promoted Bishop)
    PT_RY, //!< Ryu (Promoted Rook)
    PT_NA, //!< NA
};

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
    EnumIterator(const uint v) : m_curr(v)
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

template <class T>
inline uint ntz(const T x);

template <>
inline uint ntz(const std::uint32_t x)
{
    return static_cast<uint>(__builtin_ctz(x));
}

template <>
inline uint ntz(const std::uint64_t x)
{
    return static_cast<uint>(__builtin_ctzll(x));
}

template <>
inline uint ntz(const uint128 x)
{
    std::uint64_t x64 = static_cast<std::uint64_t>(x);
    if (static_cast<bool>(x64))
        return ntz(x64);
    return ntz(static_cast<std::uint32_t>(x >> 64)) + 64u;
}

template <class UInt>
inline uint hamming_weight(UInt x);

template <>
inline uint hamming_weight(std::uint32_t x)
{
    return static_cast<uint>(__builtin_popcount(x));
}

template <>
inline uint hamming_weight(std::uint16_t x)
{
    return hamming_weight(static_cast<std::uint32_t>(x));
}

template <>
inline uint hamming_weight(std::uint64_t x)
{
    return static_cast<uint>(__builtin_popcountll(x));
}

template <>
inline uint hamming_weight(uint128 x)
{
    return hamming_weight(static_cast<std::uint64_t>(x))
           + hamming_weight(static_cast<std::uint32_t>(x >> 64));
}

} // namespace vshogi

#endif // VSHOGI_COMMON_UTILS_HPP
