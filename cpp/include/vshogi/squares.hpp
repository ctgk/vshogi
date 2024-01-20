#ifndef VSHOGI_SQUARES_HPP
#define VSHOGI_SQUARES_HPP

#include <algorithm>
#include <cstddef>

#include "vshogi/color.hpp"
#include "vshogi/direction.hpp"

namespace vshogi
{

template <
    int NumFiles,
    int NumRanks,
    class Square,
    class File,
    class Rank,
    int NumDir,
    int NumDirDL,
    int NumNonRangingBoardPiece>
struct Squares
{
    Squares() = delete;
    using SquareEnum = Square;
    using FileEnum = File;
    using RankEnum = Rank;

    static constexpr int num_squares = NumFiles * NumRanks;
    static constexpr int num_files = NumFiles;
    static constexpr int num_ranks = NumRanks;
    static constexpr int num_directions = NumDir;
    static constexpr int num_directions_dlshogi = NumDirDL;
    static constexpr Rank RANK1 = static_cast<Rank>(0); // NOLINT
    static constexpr Square SQ_NA = static_cast<Square>(num_squares); // NOLINT

    inline static Square square_array[static_cast<std::size_t>(num_squares)];
    inline static File file_array[static_cast<std::size_t>(num_files)];
    inline static Rank rank_array[static_cast<std::size_t>(num_ranks)];
    inline static DirectionEnum
        direction_array[static_cast<std::size_t>(num_directions)];
    inline static DirectionEnum
        direction_dlshogi_array[static_cast<std::size_t>(
            num_directions_dlshogi)];
    inline static Square file_to_square_array[static_cast<std::size_t>(
        num_files)][static_cast<std::size_t>(num_ranks)];

private:
    inline static Square shift_table[static_cast<std::size_t>(num_squares)]
                                    [static_cast<std::size_t>(num_directions)];
    inline static Square
        ranging_squares_to[static_cast<std::size_t>(num_squares)]
                          [static_cast<std::size_t>(num_directions)]
                          [static_cast<std::size_t>(
                              (NumFiles > NumRanks) ? NumFiles : NumRanks)];
    inline static Square non_ranging_attacks_array[static_cast<std::size_t>(
        NumNonRangingBoardPiece)][static_cast<std::size_t>(num_squares)][9UL];

    static constexpr File fe()
    {
        return static_cast<File>(0);
    }
    static constexpr File fw()
    {
        return static_cast<File>(NumFiles - 1);
    }

public:
    static constexpr File to_file(const Square& sq)
    {
        return static_cast<File>(num_files - 1 - sq % num_files);
    }
    static constexpr Rank to_rank(const Square& sq)
    {
        return static_cast<Rank>(sq / num_files);
    }
    static constexpr Square to_square(const File& f, const Rank& r)
    {
        return static_cast<Square>(r * num_files + num_files - 1 - f);
    }
    static Square to_square(const char usi[2])
    {
        return to_square(
            static_cast<File>(usi[0] - '1'), static_cast<Rank>(usi[1] - 'a'));
    }
    static void to_usi(char usi[2], const Square& sq)
    {
        usi[0] = static_cast<char>(static_cast<int>(to_file(sq)) + '1');
        usi[1] = static_cast<char>(static_cast<int>(to_rank(sq)) + 'a');
    }

    static void init_tables()
    {
        constexpr Rank r1 = static_cast<Rank>(0);
        constexpr Rank r2 = static_cast<Rank>(1);
        constexpr Rank rm = static_cast<Rank>(num_ranks - 2);
        constexpr Rank rn = static_cast<Rank>(num_ranks - 1);

        for (int ii = num_squares; ii--;) {
            const auto sq = static_cast<Square>(ii);
            square_array[ii] = sq;
            file_to_square_array[to_file(sq)][to_rank(sq)] = sq;
        }
        init_file_array();
        init_rank_array();
        init_direction_array();
        init_direction_dlshogi_array();

        for (auto&& sq : square_array) {
            const auto r = to_rank(sq);
            const auto f = to_file(sq);
            for (auto&& dir : direction_array) {
                if (((r == r1) && has_dir_n(dir))
                    || ((r == r2) && (dir == DIR_NNW || dir == DIR_NNE))
                    || ((r == rn) && has_dir_s(dir))
                    || ((r == rm) && (dir == DIR_SSW || dir == DIR_SSE))
                    || ((f == fe()) && has_dir_e(dir))
                    || ((f == fw()) && has_dir_w(dir)))
                    shift_table[sq][dir] = SQ_NA;
                else
                    shift_table[sq][dir]
                        = static_cast<Square>(sq + direction_to_delta(dir));
            }
        }

        init_ranging_squares_table();
        init_non_ranging_attacks_array();
    }

    static bool in_promotion_zone(const Rank& r, const ColorEnum& c);
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
        if (to_file(dst) == to_file(src))
            return (src < dst) ? DIR_S : DIR_N;
        if (to_rank(dst) == to_rank(src))
            return (src < dst) ? DIR_E : DIR_W;
        return get_direction_for_diagonal_or_knight(dst, src);
    }
    constexpr static int direction_to_delta(const DirectionEnum& d)
    {
        constexpr int table[12]
            = {-num_files - 1,
               -num_files,
               1 - num_files,
               -1,
               1,
               num_files - 1,
               num_files,
               num_files + 1,
               2 * num_files - 1,
               2 * num_files + 1,
               -2 * num_files - 1,
               1 - 2 * num_files};
        return table[d];
    }
    static const Square*
    get_squares_along(const DirectionEnum& direction, const Square& location)
    {
        if ((direction == DIR_NA) || (location == SQ_NA))
            return nullptr;
        return ranging_squares_to[location][direction];
    }
    template <class BoardPiece>
    static const Square*
    get_non_ranging_attacks_by(const BoardPiece& p, const Square& location);

private:
    static DirectionEnum
    get_direction_for_diagonal_or_knight(const Square& dst, const Square& src);

    static void init_file_array()
    {
        for (int ii = num_files; ii--;)
            file_array[ii] = static_cast<File>(ii);
    }
    static void init_rank_array()
    {
        for (int ii = num_ranks; ii--;)
            rank_array[ii] = static_cast<Rank>(ii);
    }
    static void init_direction_array()
    {
        for (int ii = num_directions; ii--;)
            direction_array[ii] = static_cast<DirectionEnum>(ii);
    }
    static void init_direction_dlshogi_array()
    {
        for (int ii = num_directions_dlshogi; ii--;)
            direction_dlshogi_array[ii] = static_cast<DirectionEnum>(ii);
    }
    static void init_ranging_squares_table()
    {
        constexpr int size
            = sizeof(ranging_squares_to) / sizeof(ranging_squares_to[0][0][0]);
        std::fill_n(&ranging_squares_to[0][0][0], size, SQ_NA);

        for (auto& src : Squares::square_array) {
            for (auto& dir : Squares::direction_array) {
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
    static void init_non_ranging_attacks_array();
};

} // namespace vshogi

#endif // VSHOGI_SQUARES_HPP
