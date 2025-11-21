#include <iomanip>
#include <iostream>
#include <random>

#include "vshogi/common/magic.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

TEST_GROUP (test_minishogi_magic) {
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
            for (auto s = SquareTraits::shift(sq, dir);;) {
                const auto next = SquareTraits::shift(s, dir);
                if (next == SQ_NA)
                    break;
                out |= BitBoard::from_square(s);
                s = next;
            }
        }
        return out;
    }
    template <uint Shift>
    std::uint32_t find_magic_number(
        const vshogi::minishogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::minishogi;
        constexpr uint max_num_unique_occupancies = (1u << Shift);
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
            attacks[ii] = BitBoard();
            for (auto&& dir : directions) {
                attacks[ii]
                    |= BitBoard::compute_ray_to(sq, dir, occupancies[ii]);
            }
        }

        for (uint kk = 10000; kk--;) {
            BitBoard used_attacks[max_num_unique_occupancies] = {};
            const std::uint32_t magic = sparse_random();
            bool found_magic = true;
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const auto index = Magic::to_magic_table_index<Shift>(
                    occupancies[ii].value(), magic);
                if (used_attacks[index].value() == 0u) {
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
        for (auto sq : Config::square_iterator()) {
            if (hex)
                std::cout << "0x" << std::hex << std::setfill('0')
                          << std::setw(8) << array[sq] << std::dec;
            else
                std::cout << array[sq];

            if (SquareTraits::to_rank(sq) == RANK5)
                std::cout << ",\n";
            else
                std::cout << ", ";
        }
    }
};

TEST(test_minishogi_magic, get_adjacent_attack)
{
    using namespace vshogi::minishogi;
    {
        const auto actual = Magic::get_adjacent_attack(SQ_3C);
        CHECK_EQUAL((bb_file3 | bb_rankc).clear(SQ_3C).value(), actual.value());
    }
    {
        const SquareEnum sq_list[] = {SQ_5A, SQ_5A, SQ_3C};
        const BitBoard occ_list[] = {BitBoard(), bb_5b, bb_1a};
        for (auto ii = sizeof(sq_list) / sizeof(sq_list[0]); ii--;) {
            const auto sq = sq_list[ii];
            const auto occ = occ_list[ii];
            const auto expect
                = BitBoard::compute_ray_to(sq, vshogi::DIR_N, occ)
                  | BitBoard::compute_ray_to(sq, vshogi::DIR_W, occ)
                  | BitBoard::compute_ray_to(sq, vshogi::DIR_E, occ)
                  | BitBoard::compute_ray_to(sq, vshogi::DIR_S, occ);
            const auto actual = Magic::get_adjacent_attack(sq, occ);
            CHECK_EQUAL(expect.value(), actual.value());
        }
    }
}

TEST(test_minishogi_magic, get_diagonal_attack)
{
    using namespace vshogi::minishogi;
    {
        const auto actual = Magic::get_diagonal_attack(SQ_3C);
        CHECK_EQUAL(
            (bb_1a | bb_2b | bb_4d | bb_5e | bb_5a | bb_4b | bb_2d | bb_1e)
                .value(),
            actual.value());
    }
    {
        const SquareEnum sq_list[] = {SQ_5A, SQ_5A, SQ_3C, SQ_3C};
        const BitBoard occ_list[]
            = {BitBoard(), bb_4b, bb_2b | bb_2d, bb_4b | bb_2b | bb_2d};
        for (auto ii = sizeof(sq_list) / sizeof(sq_list[0]); ii--;) {
            const auto sq = sq_list[ii];
            const auto occ = occ_list[ii];
            const auto expect
                = BitBoard::compute_ray_to(sq, vshogi::DIR_NW, occ)
                  | BitBoard::compute_ray_to(sq, vshogi::DIR_NE, occ)
                  | BitBoard::compute_ray_to(sq, vshogi::DIR_SW, occ)
                  | BitBoard::compute_ray_to(sq, vshogi::DIR_SE, occ);
            const auto actual = Magic::get_diagonal_attack(sq, occ);
            CHECK_EQUAL(expect.value(), actual.value());
        }
    }
}

// TEST(test_minishogi_magic, generate_magic_numbers)
// {
//     using namespace vshogi::minishogi;
//     std::uint32_t magics[Config::num_squares];

//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_adjacent>(
//                 sq,
//                 {vshogi::DIR_N, vshogi::DIR_W, vshogi::DIR_E, vshogi::DIR_S});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "minishogi::Magic::magic_number_adjacent[minishogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";

//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_diagonal>(
//                 sq,
//                 {vshogi::DIR_NW,
//                  vshogi::DIR_NE,
//                  vshogi::DIR_SW,
//                  vshogi::DIR_SE});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "minishogi::Magic::magic_number_diagonal[minishogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";
// }

// TEST(test_minishogi_magic, generate_premasks)
// {
//     using namespace vshogi::minishogi;
//     uint premask_array[Config::num_squares] = {};

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         for (auto&& dir :
//              {vshogi::DIR_N, vshogi::DIR_W, vshogi::DIR_E, vshogi::DIR_S}) {
//             for (SquareEnum s = SquareTraits::shift(sq, dir);;) {
//                 const auto next = SquareTraits::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BitBoard::from_square(s).value();
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "minishogi::Magic::premask_adjacent[minishogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         for (auto&& dir :
//              {vshogi::DIR_NW, vshogi::DIR_NE, vshogi::DIR_SW, vshogi::DIR_SE}) {
//             for (SquareEnum s = SquareTraits::shift(sq, dir);;) {
//                 const auto next = SquareTraits::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BitBoard::from_square(s).value();
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "minishogi::Magic::premask_diagonal[minishogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";
// }

} // namespace test_vshogi::test_minishogi
