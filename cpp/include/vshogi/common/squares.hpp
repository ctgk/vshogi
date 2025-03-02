#ifndef VSHOGI_SQUARES_HPP
#define VSHOGI_SQUARES_HPP

#include <algorithm>
#include <cstddef>

#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/pieces.hpp"

namespace vshogi
{

template <class Parameters>
struct Squares
{
    Squares() = delete;

private:
    using C = Configuration<Parameters>;
    using PieceType = typename C::PieceType;
    using ColoredPiece = typename C::ColoredPiece;
    using Square = typename C::Square;
    using File = typename C::File;
    using Rank = typename C::Rank;

    inline static Square shift_table[C::num_squares][C::num_dir];
    inline static DirectionEnum direction_src_dst_table[C::num_squares]
                                                       [C::num_squares];
    inline static Square ranging_squares_to[C::num_squares][C::num_dir]
                                           [C::board_length];

public:
    static constexpr File to_file(const Square& sq)
    {
        return static_cast<File>(sq / C::num_ranks);
    }
    static constexpr Rank to_rank(const Square& sq)
    {
        return static_cast<Rank>(sq % C::num_ranks);
    }
    static constexpr Square to_square(const File& f, const Rank& r)
    {
        return static_cast<Square>(f * C::num_ranks + r);
    }
    static Square to_square(const char sfen[2])
    {
        return to_square(
            static_cast<File>(sfen[0] - '1'), static_cast<Rank>(sfen[1] - 'a'));
    }
    static void to_sfen(char sfen[2], const Square& sq)
    {
        sfen[0] = static_cast<char>(static_cast<int>(to_file(sq)) + '1');
        sfen[1] = static_cast<char>(static_cast<int>(to_rank(sq)) + 'a');
    }
    static Square hflip(const Square& sq)
    {
        return to_square(hflip(to_file(sq)), to_rank(sq));
    }
    static File hflip(const File& f)
    {
        return static_cast<File>(
            static_cast<int>(C::num_files) - 1 - static_cast<int>(f));
    }

    static void init_tables()
    {
        init_shift_table();

        // `shift_table` must be initialized, when calling the following.
        init_ranging_squares_table();
        init_direction_src_dst_table();
    }

    static bool in_promotion_zone(const Rank& r, const ColorEnum& c)
    {
        return (c == BLACK) ? (r < C::num_promotion_ranks)
                            : (r > C::num_ranks - 1 - C::num_promotion_ranks);
    }
    static bool in_promotion_zone(const Square& sq, const ColorEnum& c)
    {
        return in_promotion_zone(to_rank(sq), c);
    }

    static Square shift(const Square& sq, const DirectionEnum& d)
    {
        return (d == DIR_NA || sq == C::SQ_NA) ? C::SQ_NA : shift_table[sq][d];
    }
    static DirectionEnum get_direction(const Square& dst, const Square& src)
    {
        if ((dst == C::SQ_NA) || (src == C::SQ_NA))
            return DIR_NA;
        return direction_src_dst_table[src][dst];
    }
    constexpr static int direction_to_delta(const DirectionEnum& d)
    {
        constexpr int r = static_cast<int>(C::num_ranks);
        constexpr int table[] = {
            // clang-format off
            -1+r, -1, -1-r,
               r,       -r,
            +1+r, +1, +1-r,
            +2+r,     +2-r,
            -2+r,     -2-r,
            // clang-format on
        };
        return table[d];
    }
    static const Square*
    get_squares_along(const DirectionEnum& direction, const Square& location)
    {
        if ((direction == DIR_NA) || (location == C::SQ_NA))
            return nullptr;
        return ranging_squares_to[location][direction];
    }

private:
    static void init_shift_table()
    {
        constexpr File f1 = static_cast<File>(0);
        constexpr File fn = static_cast<File>(C::num_ranks - 1);
        constexpr Rank r1 = static_cast<Rank>(0);
        constexpr Rank r2 = static_cast<Rank>(1);
        constexpr Rank rm = static_cast<Rank>(C::num_ranks - 2);
        constexpr Rank rn = static_cast<Rank>(C::num_ranks - 1);
        for (auto sq : EnumIterator<Square, C::num_squares>()) {
            const auto r = to_rank(sq);
            const auto f = to_file(sq);
            for (auto dir : EnumIterator<DirectionEnum, C::num_dir>()) {
                if (((r == r1) && has_dir_n(dir))
                    || ((r == r2) && (dir == DIR_NNW || dir == DIR_NNE))
                    || ((r == rn) && has_dir_s(dir))
                    || ((r == rm) && (dir == DIR_SSW || dir == DIR_SSE))
                    || ((f == f1) && has_dir_e(dir))
                    || ((f == fn) && has_dir_w(dir)))
                    shift_table[sq][dir] = C::SQ_NA;
                else
                    shift_table[sq][dir] = static_cast<Square>(
                        static_cast<int>(sq) + direction_to_delta(dir));
            }
        }
    }
    static void init_ranging_squares_table()
    {
        constexpr int size
            = sizeof(ranging_squares_to) / sizeof(ranging_squares_to[0][0][0]);
        std::fill_n(&ranging_squares_to[0][0][0], size, C::SQ_NA);

        for (auto src : EnumIterator<Square, C::num_squares>()) {
            for (auto dir : EnumIterator<DirectionEnum, C::num_dir>()) {
                auto dst = src;
                int index = 0;
                while (true) {
                    dst = Squares::shift(dst, dir);
                    if (dst == C::SQ_NA)
                        break;
                    ranging_squares_to[src][dir][index++] = dst;
                    if (is_knight_direction(dir))
                        break;
                }
            }
        }
    }
    static void init_direction_src_dst_table()
    {
        std::fill_n(
            &direction_src_dst_table[0][0],
            sizeof(direction_src_dst_table)
                / sizeof(direction_src_dst_table[0][0]),
            DIR_NA);
        for (auto src : EnumIterator<Square, C::num_squares>()) {
            for (auto dir : EnumIterator<DirectionEnum, C::num_dir>()) {
                for (auto dst = shift(src, dir); dst != C::SQ_NA;
                     dst = shift(dst, dir))
                    direction_src_dst_table[src][dst] = dir;
            }
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_SQUARES_HPP
