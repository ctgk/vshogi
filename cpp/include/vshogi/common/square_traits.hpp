#ifndef VSHOGI_SQUARE_TRAITS_HPP
#define VSHOGI_SQUARE_TRAITS_HPP

#include <algorithm>
#include <cstddef>

#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/piece_traits.hpp"

namespace vshogi
{

template <class P>
struct SquareTraits
{
private:
    using C = Configuration<P>;
    using Square = typename C::Square;
    using File = typename C::File;
    using Rank = typename C::Rank;

    inline static Square table_shift[C::num_squares + 1u][DIR_NA + 1u];
    inline static DirectionEnum table_direction[C::num_squares + 1u]
                                               [C::num_squares + 1u];
    inline static Square table_ray_from[C::num_squares + 1u][DIR_NA + 1u]
                                       [C::board_length];
    SquareTraits() = delete;

public:
    static constexpr File to_file(const Square& sq);
    static constexpr Rank to_rank(const Square& sq);
    static constexpr Square to_square(const File& f, const Rank& r);
    static Square hflip(const Square& sq);
    static Square rotate(const Square& sq);
    static bool in_promotion_zone(const Square& sq, const ColorEnum& by_side);
    static uint chebyshev_distance(const Square& a, const Square& b);

    static void init_tables();
    static Square shift(const Square& sq, const DirectionEnum& dir);
    static DirectionEnum direction(const Square& src, const Square& dst);
    static const Square* ray_from(const Square& src, const DirectionEnum& dir);

private:
    static File hflip(const File& f);
    static bool in_promotion_zone(const Rank& r, const ColorEnum& by_side);
    static void init_table_shift();
    static void init_table_direction();
    static void init_table_ray_from();
};

template <class P>
constexpr typename SquareTraits<P>::File
SquareTraits<P>::to_file(const Square& sq)
{
    assert((0 <= sq) and (sq < C::num_squares));
    return static_cast<File>(sq / C::num_ranks);
}

template <class P>
constexpr typename SquareTraits<P>::Rank
SquareTraits<P>::to_rank(const Square& sq)
{
    assert((0 <= sq) and (sq < C::num_squares));
    return static_cast<Rank>(sq % C::num_ranks);
}

template <class P>
constexpr typename SquareTraits<P>::Square
SquareTraits<P>::to_square(const File& f, const Rank& r)
{
    return static_cast<Square>(f * C::num_ranks + r);
}

template <class P>
typename SquareTraits<P>::File SquareTraits<P>::hflip(const File& f)
{
    return static_cast<File>(C::num_files - 1u - f);
}

template <class P>
typename SquareTraits<P>::Square SquareTraits<P>::hflip(const Square& sq)
{
    return to_square(hflip(to_file(sq)), to_rank(sq));
}

template <class P>
typename SquareTraits<P>::Square SquareTraits<P>::rotate(const Square& sq)
{
    assert((0 <= sq) and (sq < C::num_squares));
    return static_cast<Square>(C::num_squares - 1u - sq);
}

template <class P>
bool SquareTraits<P>::in_promotion_zone(const Rank& r, const ColorEnum& by_side)
{
    constexpr auto r_th_w = C::num_ranks - 1 - C::num_promotion_ranks;
    return (by_side == BLACK) ? (r < C::num_promotion_ranks) : (r > r_th_w);
}

template <class P>
bool SquareTraits<P>::in_promotion_zone(
    const Square& sq, const ColorEnum& by_side)
{
    assert((0 <= sq) and (sq < C::num_squares));
    return in_promotion_zone(to_rank(sq), by_side);
}

template <class P>
typename SquareTraits<P>::Square
SquareTraits<P>::shift(const Square& sq, const DirectionEnum& dir)
{
    return table_shift[sq][dir];
}

template <class P>
DirectionEnum SquareTraits<P>::direction(const Square& src, const Square& dst)
{
    return table_direction[src][dst];
}

template <class P>
const typename SquareTraits<P>::Square*
SquareTraits<P>::ray_from(const Square& src, const DirectionEnum& dir)
{
    return table_ray_from[src][dir];
}

template <class P>
uint SquareTraits<P>::chebyshev_distance(const Square& a, const Square& b)
{
    const int af = static_cast<int>(to_file(a));
    const int ar = static_cast<int>(to_rank(a));
    const int bf = static_cast<int>(to_file(b));
    const int br = static_cast<int>(to_rank(b));
    return static_cast<uint>(std::max(std::abs(af - bf), std::abs(ar - br)));
}

template <class P>
void SquareTraits<P>::init_tables()
{
    init_table_shift();
    init_table_direction();
    init_table_ray_from();
}

template <class P>
void SquareTraits<P>::init_table_shift()
{
    constexpr int df[] = {1, 0, -1, 1, -1, 1, 0, -1, 1, -1, 1, -1};
    constexpr int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1, 2, 2, -2, -2};

    std::fill_n(
        &table_shift[0][0],
        sizeof(table_shift) / sizeof(table_shift[0][0]),
        C::SQ_NA);
    for (auto src : C::square_iterator()) {
        for (auto dir : C::direction_iterator()) {
            const auto f
                = static_cast<File>(static_cast<int>(to_file(src)) + df[dir]);
            const auto r
                = static_cast<Rank>(static_cast<int>(to_rank(src)) + dr[dir]);
            if ((f < C::num_files) && (r < C::num_ranks))
                table_shift[src][dir] = to_square(f, r);
        }
    }
}

template <class P>
void SquareTraits<P>::init_table_direction()
{
    std::fill_n(
        &table_direction[0][0],
        sizeof(table_direction) / sizeof(table_direction[0][0]),
        DIR_NA);
    for (auto src : C::square_iterator()) {
        for (auto dir : C::direction_iterator()) {
            for (auto dst = shift(src, dir); dst != C::SQ_NA;
                 dst = shift(dst, dir)) {
                table_direction[src][dst] = dir;
                if (is_knight_direction(dir))
                    break;
            }
        }
    }
}

template <class P>
void SquareTraits<P>::init_table_ray_from()
{
    std::fill_n(
        &table_ray_from[0][0][0],
        sizeof(table_ray_from) / sizeof(table_ray_from[0][0][0]),
        C::SQ_NA);
    for (auto src : C::square_iterator()) {
        for (auto dir : C::direction_iterator()) {
            uint index = 0u;
            for (auto dst = shift(src, dir); dst != C::SQ_NA;
                 dst = shift(dst, dir)) {
                table_ray_from[src][dir][index++] = dst;
                if (is_knight_direction(dir))
                    break;
            }
        }
    }
}

} // namespace vshogi

#endif // VSHOGI_SQUARE_TRAITS_HPP
