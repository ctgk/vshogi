#include <iomanip>
#include <iostream>
#include <random>

#include "vshogi/common/magic.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

TEST_GROUP (minishogi_magic) {
    std::uint32_t random_uint32()
    {
        const std::uint32_t u1 = static_cast<std::uint32_t>(random()) & 0xffffu;
        const std::uint32_t u2 = static_cast<std::uint32_t>(random()) & 0xffffu;
        return u1 | (u2 << 16);
    }
    std::uint32_t sparse_random()
    {
        return random_uint32() & random_uint32() & random_uint32();
    }
    vshogi::minishogi::BitBoard get_premask(
        const vshogi::minishogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::minishogi;
        vshogi::minishogi::BitBoard out{};
        for (auto&& dir : directions) {
            for (auto s = Squares::shift(sq, dir);;) {
                const auto next = Squares::shift(s, dir);
                if (next == SQ_NA)
                    break;
                out |= BitBoard::from_square(s);
                s = next;
            }
        }
        return out;
    }
    std::uint32_t find_magic_number(
        const vshogi::minishogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::minishogi;
        constexpr uint max_num_unique_occupancies = 1u << 6;
        const BitBoard premask = get_premask(sq, directions);
        const uint num_relevant_squares = premask.hamming_weight();

        uint relevant_square_locations[Config::num_squares] = {};
        BitBoard occupancies[max_num_unique_occupancies] = {};
        BitBoard attacks[max_num_unique_occupancies] = {};

        for (uint ii = 0, jj = 0; ii < Config::num_squares; ++ii) {
            if (premask.is_one(static_cast<SquareEnum>(ii)))
                relevant_square_locations[jj++] = ii;
        }
        for (uint ii = (1u << num_relevant_squares); ii--;) {
            occupancies[ii] = Magic::get_occupancy(
                ii, num_relevant_squares, relevant_square_locations);
            attacks[ii]
                = BitBoard::compute_ray_to(sq, directions[0], occupancies[ii])
                  | BitBoard::compute_ray_to(
                      sq, directions[1], occupancies[ii]);
        }

        for (uint kk = 10000; kk--;) {
            BitBoard used_attacks[max_num_unique_occupancies] = {};
            const std::uint32_t magic = sparse_random();
            bool found_magic = true;
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const auto index
                    = Magic::to_magic_table_index(occupancies[ii], magic);
                if (used_attacks[index] == 0u) {
                    used_attacks[index] = attacks[ii];
                } else if (used_attacks[index] != attacks[ii]) {
                    found_magic = false;
                    break;
                }
            }
            if (found_magic)
                return magic;
        }
        return 0u;
    }
    void print_array(
        const uint array[vshogi::minishogi::Config::num_squares],
        const bool hex = false)
    {
        using namespace vshogi::minishogi;
        for (auto sq :
             vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
            if (hex)
                std::cout << "0x" << std::hex << std::setfill('0')
                          << std::setw(8) << array[sq] << std::dec;
            else
                std::cout << array[sq];

            if (Squares::to_file(sq) == FILE1)
                std::cout << ",\n";
            else
                std::cout << ", ";
        }
    }
};

TEST(minishogi_magic, get_adjacent_attack)
{
    using namespace vshogi::minishogi;
    const SquareEnum sq_list[] = {SQ_5A, SQ_5A, SQ_3C};
    const BitBoard occ_list[] = {BitBoard(), bb_5b, bb_1a};
    for (auto ii = sizeof(sq_list) / sizeof(sq_list[0]); ii--;) {
        const auto sq = sq_list[ii];
        const auto occ = occ_list[ii];
        const auto expect = BitBoard::compute_ray_to(sq, vshogi::DIR_N, occ)
                            | BitBoard::compute_ray_to(sq, vshogi::DIR_W, occ)
                            | BitBoard::compute_ray_to(sq, vshogi::DIR_E, occ)
                            | BitBoard::compute_ray_to(sq, vshogi::DIR_S, occ);
        const auto actual = Magic::get_adjacent_attack(sq, occ);
        CHECK_EQUAL(expect.value(), actual.value());
    }
}

TEST(minishogi_magic, get_diagonal_attack)
{
    using namespace vshogi::minishogi;
    const SquareEnum sq_list[] = {SQ_5A, SQ_5A, SQ_3C, SQ_3C};
    const BitBoard occ_list[]
        = {BitBoard(), bb_4b, bb_2b | bb_2d, bb_4b | bb_2b | bb_2d};
    for (auto ii = sizeof(sq_list) / sizeof(sq_list[0]); ii--;) {
        const auto sq = sq_list[ii];
        const auto occ = occ_list[ii];
        const auto expect = BitBoard::compute_ray_to(sq, vshogi::DIR_NW, occ)
                            | BitBoard::compute_ray_to(sq, vshogi::DIR_NE, occ)
                            | BitBoard::compute_ray_to(sq, vshogi::DIR_SW, occ)
                            | BitBoard::compute_ray_to(sq, vshogi::DIR_SE, occ);
        const auto actual = Magic::get_diagonal_attack(sq, occ);
        CHECK_EQUAL(expect.value(), actual.value());
    }
}

// TEST(minishogi_magic, generate_magic_numbers)
// {
//     using namespace vshogi::minishogi;
//     std::uint32_t magics[Config::num_squares];

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic
//             = find_magic_number(sq, {vshogi::DIR_N, vshogi::DIR_S});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "minishogi::Magic::magic_number_vertical[minishogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic
//             = find_magic_number(sq, {vshogi::DIR_W, vshogi::DIR_E});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "minishogi::Magic::magic_number_horizontal[minishogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic
//             = find_magic_number(sq, {vshogi::DIR_NW, vshogi::DIR_SE});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "minishogi::Magic::magic_number_nw_se[minishogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic
//             = find_magic_number(sq, {vshogi::DIR_SW, vshogi::DIR_NE});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "minishogi::Magic::magic_number_sw_ne[minishogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";
// }

// TEST(minishogi_magic, generate_premasks)
// {
//     using namespace vshogi::minishogi;
//     uint premask_array[Config::num_squares] = {};

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         for (auto&& dir : {vshogi::DIR_N, vshogi::DIR_S}) {
//             for (SquareEnum s = Squares::shift(sq, dir);;) {
//                 const auto next = Squares::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BitBoard::from_square(s).value();
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const minishogi::BitBoard "
//                  "minishogi::Magic::premask_vertical[minishogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         for (auto&& dir : {vshogi::DIR_W, vshogi::DIR_E}) {
//             for (SquareEnum s = Squares::shift(sq, dir);;) {
//                 const auto next = Squares::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BitBoard::from_square(s).value();
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const minishogi::BitBoard "
//                  "minishogi::Magic::premask_horizontal[minishogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         for (auto&& dir : {vshogi::DIR_NW, vshogi::DIR_SE}) {
//             for (SquareEnum s = Squares::shift(sq, dir);;) {
//                 const auto next = Squares::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BitBoard::from_square(s).value();
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const minishogi::BitBoard "
//                  "minishogi::Magic::premask_nw_se[minishogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         for (auto&& dir : {vshogi::DIR_SW, vshogi::DIR_NE}) {
//             for (SquareEnum s = Squares::shift(sq, dir);;) {
//                 const auto next = Squares::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BitBoard::from_square(s).value();
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const minishogi::BitBoard "
//                  "minishogi::Magic::premask_sw_ne[minishogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";
// }

} // namespace test_vshogi::test_minishogi
