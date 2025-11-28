#ifndef VSHOGI_COMMON_BITBOARD_TRAITS_HPP
#define VSHOGI_COMMON_BITBOARD_TRAITS_HPP

#include <cassert>

#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"

namespace vshogi
{

template <class P>
struct BitboardTraits
{
private:
    using C = Configuration<P>;
    using PT = PieceTraits<P>;
    using ST = SquareTraits<P>;
    using Piece = typename C::Piece;
    using Square = typename C::Square;
    using Rank = typename C::Rank;
    using File = typename C::File;
    BitboardTraits() = delete;

public:
    using bitboard_t = typename C::bitboard_t;
    static constexpr bitboard_t full();
    static constexpr bitboard_t from_square(const Square& sq);
    static constexpr bitboard_t from_rank(const Rank& r);
    static constexpr bitboard_t from_file(const File& f);
    static bitboard_t promotion_zone(const ColorEnum& by_side);

    static constexpr bitboard_t invert(const bitboard_t& a);
    static constexpr bitboard_t
    shift(const bitboard_t& m, const DirectionEnum& dir);
    static bool is_one(const bitboard_t& m, const Square& sq);

    static bitboard_t get_ray_to(const Square& sq, const DirectionEnum& dir);
    static bitboard_t get_mask_between(const Square& a, const Square& b);
    static bitboard_t get_placeable(const Piece& p);
    static bitboard_t get_attack_by(const Piece& p, const Square& sq);
    static bitboard_t
    get_attack_by(const Piece& p, const Square& sq, const bitboard_t& occupied);
    static bitboard_t
    get_pre_reverse_attack(const Square& target, const Piece& attacker);
    static void init_tables();

    class Iterator
    {
    private:
        bitboard_t m_mask;
        uint m_curr;

    public:
        Iterator();
        Iterator(const bitboard_t& m);
        Square operator*() const;
        Iterator& operator++();
        Iterator begin() const;
        Iterator end() const;
        bool operator!=(const Iterator& other) const;
        operator bool() const;
    };
    static Iterator iterator(const bitboard_t& m);

private:
    static bitboard_t table_attacks[C::VOID + 1u][C::num_squares + 1u];

    /**
     * @brief Table of pre-reverse-attack maps
     * @note This table is in piece-major order, because the table is typically
     * referred by passing a fixed king square with various pieces.
     * `table_pre_reverse_attack[king_sq][{var}]`
     */
    static bitboard_t table_pre_reverse_attack[C::SQ_NA + 1u][C::VOID + 1u];
    static bitboard_t table_ray[C::num_squares + 1u][DIR_NA + 1u];
    static bitboard_t table_mask_between[C::num_squares + 1u]
                                        [C::num_squares + 1u];

    static constexpr bitboard_t ones(const uint num_ones);
    static constexpr bitboard_t mask = ones(C::num_squares);
    template <uint NumSqsFromSQ1A = C::num_files>
    static constexpr bitboard_t rank1();
    template <uint N>
    static constexpr bitboard_t top_n_rank();

    static bitboard_t compute_ray_to(
        const Square& src,
        const DirectionEnum& dir,
        const Square& end = C::SQ_NA);
    static bitboard_t compute_mask_between(const Square& a, const Square& b);
    static bitboard_t compute_attack_by(const Piece& p, const Square& sq);
    static bitboard_t
    compute_pre_reverse_attack(const Piece& attacker, const Square& target);
};

template <class P>
constexpr typename BitboardTraits<P>::bitboard_t BitboardTraits<P>::full()
{
    return mask;
}

template <class P>
constexpr typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::from_square(const Square& sq)
{
    return (static_cast<bitboard_t>(1) << sq) & mask;
}

template <class P>
constexpr typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::from_rank(const Rank& r)
{
    return rank1<>() << r;
}

template <class P>
constexpr typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::from_file(const File& f)
{
    constexpr auto file1 = ones(C::num_ranks);
    return file1 << (f * C::num_ranks);
}

template <class P>
typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::promotion_zone(const ColorEnum& by_side)
{
    constexpr uint s = C::num_ranks - C::num_promotion_ranks;
    constexpr bitboard_t z_b = top_n_rank<C::num_promotion_ranks>();
    static const bitboard_t table[2] = {z_b, z_b << s};
    return table[by_side];
}

template <class P>
constexpr typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::invert(const bitboard_t& a)
{
    return mask & (~a);
}

template <class P>
constexpr typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::shift(const bitboard_t& m, const DirectionEnum& dir)
{
    constexpr int r = static_cast<int>(C::num_ranks);
    constexpr int table[] = {
        // clang-format off
        -1+r, -1, -1-r,
          +r,       -r,
        +1+r, +1, +1-r,
        +2+r,     +2-r,
        -2+r,     -2-r,
        // clang-format on
    };
    constexpr bitboard_t all = top_n_rank<C::num_ranks>();
    constexpr bitboard_t all_but_a = ~top_n_rank<1u>();
    constexpr bitboard_t all_but_z = top_n_rank<C::num_ranks - 1u>();
    constexpr bitboard_t all_but_ab = ~top_n_rank<2u>();
    constexpr bitboard_t all_but_yz = top_n_rank<C::num_ranks - 2u>();
    constexpr bitboard_t rank_mask[] = {
        // clang-format off
        all_but_a, all_but_a, all_but_a,
        all,                  all,
        all_but_z, all_but_z, all_but_z,
        all_but_yz,           all_but_yz,
        all_but_ab,           all_but_ab,
        // clang-format on
    };
    const auto delta = table[dir];
    if (delta > 0)
        return ((m & rank_mask[dir]) << static_cast<uint>(delta)) & mask;
    else
        return (m & rank_mask[dir]) >> static_cast<uint>(-delta);
}

template <class P>
bool BitboardTraits<P>::is_one(const bitboard_t& m, const Square& sq)
{
    assert((m >> C::SQ_NA) == 0u);
    return static_cast<bool>((m >> sq) & static_cast<bitboard_t>(1));
}

template <class P>
typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::get_ray_to(const Square& src, const DirectionEnum& dir)
{
    return table_ray[src][dir];
}

template <class P>
typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::get_mask_between(const Square& a, const Square& b)
{
    return table_mask_between[a][b];
}

template <class P>
typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::get_attack_by(const Piece& p, const Square& sq)
{
    return table_attacks[p][sq];
}

template <class P>
typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::get_pre_reverse_attack(
    const Square& target, const Piece& attacker)
{
    return table_pre_reverse_attack[target][attacker];
}

template <class P>
constexpr typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::ones(const uint num_ones)
{
    return ~(~bitboard_t() << num_ones);
}

template <class P>
void BitboardTraits<P>::init_tables()
{
    for (auto sq : C::square_iterator()) {
        for (auto dir : C::direction_iterator())
            table_ray[sq][dir] = compute_ray_to(sq, dir);
    }
    for (auto p : EnumIterator<Piece, C::num_colored_piece_types>()) {
        for (auto sq : C::square_iterator())
            table_attacks[p][sq] = compute_attack_by(p, sq);
    }
    for (auto s1 : C::square_iterator()) {
        for (auto s2 : C::square_iterator())
            table_mask_between[s1][s2] = compute_mask_between(s1, s2);
    }
    for (auto sq : C::square_iterator()) {
        for (auto p : EnumIterator<Piece, C::num_colored_piece_types>())
            table_pre_reverse_attack[sq][p] = compute_pre_reverse_attack(p, sq);
    }
}

template <class P>
BitboardTraits<P>::Iterator::Iterator() : m_mask{}, m_curr{C::num_squares}
{
}

template <class P>
BitboardTraits<P>::Iterator::Iterator(const bitboard_t& m) : m_mask{m}, m_curr{}
{
    if (!static_cast<bool>(m_mask)) {
        m_curr = C::num_squares;
        return;
    }
    const auto shift = std::min(ntz(m_mask), C::num_squares);
    m_curr = shift;
    m_mask >>= shift;
}

template <class P>
typename BitboardTraits<P>::Square
BitboardTraits<P>::Iterator::operator*() const
{
    return static_cast<Square>(m_curr);
}

template <class P>
typename BitboardTraits<P>::Iterator& BitboardTraits<P>::Iterator::operator++()
{
    ++m_curr;
    m_mask >>= 1u;
    if (!static_cast<bool>(m_mask)) {
        m_curr = C::num_squares;
        return *this;
    }
    const auto shift = ntz(m_mask);
    m_curr += shift;
    m_mask >>= shift;
    return *this;
}

template <class P>
typename BitboardTraits<P>::Iterator BitboardTraits<P>::Iterator::begin() const
{
    return *this;
}

template <class P>
typename BitboardTraits<P>::Iterator BitboardTraits<P>::Iterator::end() const
{
    static const auto end_iter = Iterator();
    return end_iter;
}

template <class P>
bool BitboardTraits<P>::Iterator::operator!=(const Iterator& other) const
{
    return m_curr != other.m_curr;
}

template <class P>
BitboardTraits<P>::Iterator::operator bool() const
{
    return m_curr < C::num_squares;
}

template <class P>
typename BitboardTraits<P>::Iterator
BitboardTraits<P>::iterator(const bitboard_t& m)
{
    return Iterator(m);
}

template <class P>
template <uint NumSqsFromSQ1A>
constexpr typename BitboardTraits<P>::bitboard_t BitboardTraits<P>::rank1()
{
    if constexpr (NumSqsFromSQ1A == 0u)
        return static_cast<bitboard_t>(0);
    else
        return (rank1<NumSqsFromSQ1A - 1u>() << C::num_ranks)
               | static_cast<bitboard_t>(1);
}

template <class P>
template <uint N>
constexpr typename BitboardTraits<P>::bitboard_t BitboardTraits<P>::top_n_rank()
{
    if constexpr (N == 0u)
        return static_cast<bitboard_t>(0);
    else
        return rank1<>() | (top_n_rank<N - 1u>() << 1u);
}

template <class P>
typename BitboardTraits<P>::bitboard_t BitboardTraits<P>::compute_ray_to(
    const Square& src, const DirectionEnum& dir, const Square& end)
{
    bitboard_t out{};
    if (dir == DIR_NA)
        return out;
    for (auto s = ST::ray_from(src, dir); *s != end;)
        out ^= from_square(*s++);
    return out;
}

template <class P>
typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::compute_mask_between(const Square& a, const Square& b)
{
    const auto dir_from_a_to_b = ST::direction(a, b);
    return compute_ray_to(a, dir_from_a_to_b, b);
}

template <class P>
typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::compute_attack_by(const Piece& p, const Square& sq)
{
    bitboard_t out{};
    if (PT::is_slider(p)) {
        for (auto d = PT::get_attack_directions(p); *d != DIR_NA; ++d) {
            if (PT::slidable_to(p, *d))
                out |= get_ray_to(sq, *d);
        }
    }
    for (auto d = PT::get_attack_directions(p); *d != DIR_NA;)
        out |= from_square(ST::shift(sq, *d++));
    return out;
}

template <class P>
typename BitboardTraits<P>::bitboard_t
BitboardTraits<P>::compute_pre_reverse_attack(
    const Piece& attacker, const Square& target)
{
    assert(get_attack_by(PT::make_piece(BLACK, C::FU), static_cast<Square>(1)));
    bitboard_t out{};
    const auto c = PT::get_color(attacker);
    for (Square src : C::square_iterator()) {
        bool src_is_in_pre_reverse_attack = false;
        for (Square dst : iterator(get_attack_by(attacker, src))) {
            if (is_one(get_attack_by(attacker, dst), target)) {
                src_is_in_pre_reverse_attack = true;
                break;
            }
            if (PT::is_promotable(attacker)
                && (ST::in_promotion_zone(src, c)
                    || ST::in_promotion_zone(dst, c))
                && is_one(
                    get_attack_by(PT::promote_nocheck(attacker), dst),
                    target)) {
                src_is_in_pre_reverse_attack = true;
                break;
            }
        }
        if (src_is_in_pre_reverse_attack)
            out ^= from_square(src);
    }
    return out;
}

} // namespace vshogi

#endif // VSHOGI_COMMON_BITBOARD_TRAITS_HPP
