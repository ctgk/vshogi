#ifndef VSHOGI_SQUARES_HPP
#define VSHOGI_SQUARES_HPP

#include <algorithm>
#include <cstddef>

#include "vshogi/common/color.hpp"
#include "vshogi/common/direction.hpp"
#include "vshogi/common/pieces.hpp"

namespace vshogi
{

template <class Config>
struct Squares
{
    Squares() = delete;

private:
    using PieceType = typename Config::PieceType;
    using ColoredPiece = typename Config::ColoredPiece;
    using Square = typename Config::Square;
    using File = typename Config::File;
    using Rank = typename Config::Rank;
    using PHelper = Pieces<Config>;

    static constexpr uint num_files = Config::num_files;
    static constexpr uint num_ranks = Config::num_ranks;
    static constexpr uint num_squares = Config::num_squares;
    static constexpr uint num_piece_types = Config::num_piece_types;
    static constexpr uint num_dir = Config::num_dir;
    static constexpr uint num_dir_dl = Config::num_dir_dl;
    inline static Square shift_table[num_squares][num_dir];
    inline static DirectionEnum direction_src_dst_table[num_squares]
                                                       [num_squares];
    inline static Square
        ranging_squares_to[num_squares][num_dir]
                          [(num_files > num_ranks) ? num_files : num_ranks];
    inline static Square non_ranging_attacks_array[2 * num_piece_types + 1]
                                                  [num_squares][9U];

    static constexpr File file_right_most()
    {
        return static_cast<File>(num_files - 1);
    }
    static constexpr File file_left_most()
    {
        return static_cast<File>(0);
    }

public:
    static constexpr Rank RANK1 = static_cast<Rank>(0); // NOLINT
    static constexpr Square SQ_NA = static_cast<Square>(num_squares); // NOLINT

    inline static Square file_to_square_array[num_files][num_ranks];

    static constexpr File to_file(const Square& sq)
    {
        return static_cast<File>(sq % num_files);
    }
    static constexpr Rank to_rank(const Square& sq)
    {
        return static_cast<Rank>(sq / num_files);
    }
    static constexpr Square to_square(const File& f, const Rank& r)
    {
        return static_cast<Square>(r * num_files + f);
    }
    static Square to_square(const char usi[2])
    {
        return to_square(
            hflip(static_cast<File>(usi[0] - '1')),
            static_cast<Rank>(usi[1] - 'a'));
    }
    static void to_usi(char usi[2], const Square& sq)
    {
        usi[0] = static_cast<char>(
            static_cast<int>(num_files - 1 - to_file(sq)) + '1');
        usi[1] = static_cast<char>(static_cast<int>(to_rank(sq)) + 'a');
    }
    static Square hflip(const Square& sq)
    {
        return to_square(hflip(to_file(sq)), to_rank(sq));
    }
    static File hflip(const File& f)
    {
        return static_cast<File>(
            static_cast<int>(num_files) - 1 - static_cast<int>(f));
    }

    static void init_tables()
    {
        for (auto sq : EnumIterator<Square, num_squares>()) {
            file_to_square_array[to_file(sq)][to_rank(sq)] = sq;
        }
        init_shift_table();

        // `shift_table` must be initialized, when calling the following.
        init_ranging_squares_table();
        init_non_ranging_attacks_array();
        init_direction_src_dst_table();
    }

    static bool in_promotion_zone(const Rank& r, const ColorEnum& c)
    {
        return (c == BLACK) ? (r < Config::num_promotion_ranks)
                            : (r > num_ranks - 1 - Config::num_promotion_ranks);
    }
    static bool in_promotion_zone(const Square& sq, const ColorEnum& c)
    {
        return in_promotion_zone(to_rank(sq), c);
    }

    static Square shift(const Square& sq, const DirectionEnum& d)
    {
        return (d == DIR_NA || sq == SQ_NA) ? SQ_NA : shift_table[sq][d];
    }
    static DirectionEnum get_direction(const Square& dst, const Square& src)
    {
        if ((dst == SQ_NA) || (src == SQ_NA))
            return DIR_NA;
        return direction_src_dst_table[src][dst];
    }
    constexpr static int direction_to_delta(const DirectionEnum& d)
    {
        constexpr int nf = static_cast<int>(num_files);
        constexpr int table[]
            = {-nf - 1,
               -nf,
               1 - nf,
               -1,
               1,
               nf - 1,
               nf,
               nf + 1,
               2 * nf - 1,
               2 * nf + 1,
               -2 * nf - 1,
               1 - 2 * nf};
        return table[d];
    }
    static const Square*
    get_squares_along(const DirectionEnum& direction, const Square& location)
    {
        if ((direction == DIR_NA) || (location == SQ_NA))
            return nullptr;
        return ranging_squares_to[location][direction];
    }
    static const Square*
    get_non_ranging_attacks_by(const ColoredPiece& p, const Square& location)
    {
        if (PHelper::is_ranging_piece(p))
            return nullptr;
        return non_ranging_attacks_array[p][location];
    }

private:
    static void init_shift_table()
    {
        constexpr Rank r1 = static_cast<Rank>(0);
        constexpr Rank r2 = static_cast<Rank>(1);
        constexpr Rank rm = static_cast<Rank>(num_ranks - 2);
        constexpr Rank rn = static_cast<Rank>(num_ranks - 1);
        for (auto sq : EnumIterator<Square, num_squares>()) {
            const auto r = to_rank(sq);
            const auto f = to_file(sq);
            for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
                if (((r == r1) && has_dir_n(dir))
                    || ((r == r2) && (dir == DIR_NNW || dir == DIR_NNE))
                    || ((r == rn) && has_dir_s(dir))
                    || ((r == rm) && (dir == DIR_SSW || dir == DIR_SSE))
                    || ((f == file_right_most()) && has_dir_e(dir))
                    || ((f == file_left_most()) && has_dir_w(dir)))
                    shift_table[sq][dir] = SQ_NA;
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
        std::fill_n(&ranging_squares_to[0][0][0], size, SQ_NA);

        for (auto src : EnumIterator<Square, num_squares>()) {
            for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
                auto dst = src;
                int index = 0;
                while (true) {
                    dst = Squares::shift(dst, dir);
                    if (dst == SQ_NA)
                        break;
                    ranging_squares_to[src][dir][index++] = dst;
                    if (is_knight_direction(dir))
                        break;
                }
            }
        }
    }
    static void init_non_ranging_attacks_array()
    {
        std::fill_n(
            &non_ranging_attacks_array[0][0][0],
            sizeof(non_ranging_attacks_array)
                / sizeof(non_ranging_attacks_array[0][0][0]),
            SQ_NA);

        for (uint ii = 0; ii < 2 * num_piece_types; ++ii) {
            const auto pt = static_cast<PieceType>(ii % num_piece_types);
            if (PHelper::is_ranging_piece(pt))
                continue;
            const ColorEnum c = (ii < num_piece_types) ? BLACK : WHITE;
            const auto p = PHelper::to_board_piece(c, pt);
            const auto dir_ptr_begin = PHelper::get_attack_directions(p);
            for (auto sq : EnumIterator<Square, num_squares>()) {
                int index = 0;
                for (auto dir_ptr = dir_ptr_begin; *dir_ptr != DIR_NA;) {
                    const auto dst = shift(sq, *dir_ptr++);
                    if (dst == SQ_NA)
                        continue;
                    non_ranging_attacks_array[ii][sq][index++] = dst;
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
        for (auto src : EnumIterator<Square, num_squares>()) {
            for (auto dir : EnumIterator<DirectionEnum, num_dir>()) {
                for (auto dst = shift(src, dir); dst != SQ_NA;
                     dst = shift(dst, dir))
                    direction_src_dst_table[src][dst] = dir;
            }
        }
    }
};

} // namespace vshogi

#endif // VSHOGI_SQUARES_HPP
