#ifndef VSHOGI_SQUARES_HPP
#define VSHOGI_SQUARES_HPP

#include <algorithm>
#include <cstddef>

#include "vshogi/common/color.hpp"
#include "vshogi/common/config.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/piece_traits.hpp"

namespace vshogi
{

template <class Parameters>
struct Squares
{
    Squares() = delete;

private:
    using C = Configuration<Parameters>;
    using PieceType = typename C::PieceType;
    using Square = typename C::Square;
    using File = typename C::File;
    using Rank = typename C::Rank;

    inline static Square shift_table[C::num_squares][C::num_dir];
    inline static DirectionEnum direction_src_dst_table[C::num_squares]
                                                       [C::num_squares];
    inline static Square sliding_squares_to[C::num_squares][C::num_dir]
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
    static std::string to_jpn(const Square& sq)
    {
        const auto f = to_file(sq);
        const auto r = to_rank(sq);
        static const std::string arabic[] = {
            u8"\uFF11",
            u8"\uFF12",
            u8"\uFF13",
            u8"\uFF14",
            u8"\uFF15",
            u8"\uFF16",
            u8"\uFF17",
            u8"\uFF18",
            u8"\uFF19",
        };
        static const std::string kanji[] = {
            u8"\u4E00",
            u8"\u4E8C",
            u8"\u4E09",
            u8"\u56DB",
            u8"\u4E94",
            u8"\u516D",
            u8"\u4E03",
            u8"\u516B",
            u8"\u4E5D",
        };
        return arabic[f] + kanji[r];
    }
    static std::string to_eng(const Square& sq)
    {
        const auto f = to_file(sq);
        const auto r = to_rank(sq);
        return std::string(1, '1' + f) + std::string(1, '1' + r);
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
    static Square rotate(const Square& sq)
    {
        return static_cast<Square>(C::num_squares - 1u - sq);
    }

    static void init_tables()
    {
        init_shift_table();

        // `shift_table` must be initialized, when calling the following.
        init_sliding_squares_table();
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

    /**
     * @brief Estimate direction for source to destination.
     *
     * @param src Source square
     * @param dst Destination square
     * @return DirectionEnum
     */
    static DirectionEnum direction(const Square& src, const Square& dst)
    {
        if ((dst == C::SQ_NA) || (src == C::SQ_NA))
            return DIR_NA;
        return direction_src_dst_table[src][dst];
    }
    static uint chebyshev_distance(const Square& a, const Square& b)
    {
        const int af = static_cast<int>(to_file(a));
        const int bf = static_cast<int>(to_file(b));
        const int ar = static_cast<int>(to_rank(a));
        const int br = static_cast<int>(to_rank(b));
        return static_cast<uint>(
            std::max(std::abs(af - bf), std::abs(ar - br)));
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

    /**
     * @brief Get array of squares from the given source to the given direction.
     *
     * @param src exclusive source square.
     * @param direction direction of the ray.
     * @return const Square*
     */
    static const Square*
    ray_from(const Square& src, const DirectionEnum& direction)
    {
        if ((direction == DIR_NA) || (src == C::SQ_NA))
            return nullptr;
        return sliding_squares_to[src][direction];
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
        for (auto sq : C::square_iterator()) {
            const auto r = to_rank(sq);
            const auto f = to_file(sq);
            for (auto dir : C::direction_iterator()) {
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
    static void init_sliding_squares_table()
    {
        constexpr int size
            = sizeof(sliding_squares_to) / sizeof(sliding_squares_to[0][0][0]);
        std::fill_n(&sliding_squares_to[0][0][0], size, C::SQ_NA);

        for (auto src : C::square_iterator()) {
            for (auto dir : C::direction_iterator()) {
                auto dst = src;
                int index = 0;
                while (true) {
                    dst = Squares::shift(dst, dir);
                    if (dst == C::SQ_NA)
                        break;
                    sliding_squares_to[src][dir][index++] = dst;
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
        for (auto src : C::square_iterator()) {
            for (auto dir : C::direction_iterator()) {
                for (auto dst = shift(src, dir); dst != C::SQ_NA;
                     dst = shift(dst, dir))
                    direction_src_dst_table[src][dst] = dir;
            }
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_SQUARES_HPP
