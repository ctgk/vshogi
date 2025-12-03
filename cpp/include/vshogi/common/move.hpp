#ifndef VSHOGI_MOVE_HPP
#define VSHOGI_MOVE_HPP

#include <cctype> // std::toupper

#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/piece_traits.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi
{

/**
 * @brief 16-bit representation of a move.
 * @details
 *       fedcba98 76543210
 *       ________ _*******       Destination square
 *       ________ *_______       Promotion flag (2 possibilities)
 *       _******* ________       Source square or piece
 * (MSB) xxxxxxxx xxxxxxxx (LSB)
 */
using move_t = std::uint16_t;

template <class P>
class MoveTraits
{
    using C = Configuration<P>;
    using PT = PieceTraits<P>;
    using ST = SquareTraits<P>;
    using Square = typename C::Square;
    using PieceType = typename C::PieceType;

    static constexpr uint source_shift = 8u;
    static constexpr uint promotion_shift = 7u;
    static constexpr std::uint16_t dst_mask = 0x007fu;
    static constexpr std::uint16_t prm_mask = 0x0080u;
    static constexpr std::uint16_t src_mask = 0x7f00u;
    static constexpr std::uint16_t full_mask = 0x7fffu;

public:
    MoveTraits() = delete;
    static move_t
    make_move(const Square src, const Square dst, const bool promote = false);
    static move_t make_move(const PieceType src, const Square dst);
    static move_t make_move(const char sfen[5]);

    static Square get_dst(const move_t& a);
    static Square get_src_sq(const move_t& a);
    static PieceType get_src_pt(const move_t& a);
    static bool get_promote(const move_t& a);
    static bool is_drop(const move_t& a);

    static move_t rotate(const move_t& a);
    static move_t hflip(const move_t& a);

    static uint num_policy_per_square();
    static uint to_policy_index(const move_t& a, const ColorEnum& by_side);

private:
    static move_t
    make_move(const uint src, const Square dst, const bool promote);
    static Square to_square(const char sfen[2]);
    static uint to_policy_index_src(const move_t& a, const ColorEnum& by_side);
};

template <class P>
move_t
MoveTraits<P>::make_move(const uint src, const Square dst, const bool promote)
{
    return static_cast<move_t>(
        (src << source_shift) | static_cast<uint>(promote << promotion_shift)
        | dst);
}

template <class P>
typename Configuration<P>::Square MoveTraits<P>::to_square(const char sfen[2])
{
    return ST::to_square(
        static_cast<typename C::File>(sfen[0] - '1'),
        static_cast<typename C::Rank>(sfen[1] - 'a'));
}

template <class P>
move_t
MoveTraits<P>::make_move(const Square src, const Square dst, const bool promote)
{
    return make_move(static_cast<uint>(src), dst, promote);
}

template <class P>
move_t MoveTraits<P>::make_move(const PieceType src, const Square dst)
{
    return make_move(static_cast<uint>(src) + C::num_squares, dst, false);
}

template <class P>
move_t MoveTraits<P>::make_move(const char sfen[5])
{
    return make_move(
        (sfen[1] == '*') ? PT::to_piece_type(sfen[0]) + C::num_squares
                         : to_square(sfen),
        to_square(sfen + 2),
        sfen[4] == '+');
}

template <class P>
typename Configuration<P>::Square MoveTraits<P>::get_dst(const move_t& a)
{
    return static_cast<Square>(a & dst_mask);
}

template <class P>
typename Configuration<P>::Square MoveTraits<P>::get_src_sq(const move_t& a)
{
    return static_cast<Square>(a >> source_shift);
}

template <class P>
typename Configuration<P>::PieceType MoveTraits<P>::get_src_pt(const move_t& a)
{
    return static_cast<PieceType>(get_src_sq(a) - C::num_squares);
}

template <class P>
bool MoveTraits<P>::get_promote(const move_t& a)
{
    return static_cast<bool>(a & prm_mask);
}

template <class P>
bool MoveTraits<P>::is_drop(const move_t& a)
{
    return get_src_sq(a) >= C::num_squares;
}

template <class P>
move_t MoveTraits<P>::rotate(const move_t& a)
{
    const auto dst_rotated = ST::rotate(get_dst(a));
    if (is_drop(a))
        return make_move(get_src_pt(a), dst_rotated);
    const auto src_rotated = ST::rotate(get_src_sq(a));
    return make_move(src_rotated, dst_rotated, get_promote(a));
}

template <class P>
move_t MoveTraits<P>::hflip(const move_t& a)
{
    const auto dst_hflipped = ST::hflip(get_dst(a));
    if (is_drop(a))
        return make_move(get_src_pt(a), dst_hflipped);
    const auto src_hflipped = ST::hflip(get_src_sq(a));
    return make_move(src_hflipped, dst_hflipped, get_promote(a));
}

template <class P>
uint MoveTraits<P>::num_policy_per_square()
{
    return 2 * C::num_dir_dl + C::num_stand_piece_types;
}

template <class P>
uint MoveTraits<P>::to_policy_index(const move_t& a, const ColorEnum& by_side)
{
    auto dst = get_dst(a);
    if (by_side == WHITE)
        dst = ST::rotate(dst);
    return dst * num_policy_per_square() + to_policy_index_src(a, by_side);
}

template <class P>
uint MoveTraits<P>::to_policy_index_src(
    const move_t& a, const ColorEnum& by_side)
{
    if (is_drop(a))
        return C::num_dir_dl * 2u + get_src_pt(a);
    const uint promo_offset = get_promote(a) ? C::num_dir_dl : 0u;
    auto dir = ST::direction(get_dst(a), get_src_sq(a));
    if (by_side == WHITE)
        dir = vshogi::rotate(dir);
    return dir + promo_offset;
}

} // namespace vshogi

#endif // VSHOGI_MOVE_HPP
