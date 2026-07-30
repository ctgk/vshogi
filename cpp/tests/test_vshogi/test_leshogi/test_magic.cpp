#include <iomanip>
#include <iostream>
#include <random>

#include "vshogi/common/magic.hpp"
#include "vshogi/variants/leshogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_leshogi
{

using namespace vshogi::leshogi;
using BT = vshogi::BitboardTraits<vshogi::leshogi::Parameters>;

TEST_GROUP (test_leshogi_magic) {
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
    vshogi::leshogi::bitboard_t get_premask(
        const vshogi::leshogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::leshogi;
        vshogi::leshogi::bitboard_t out{};
        for (auto&& dir : directions) {
            for (auto s = SquareTraits::shift(sq, dir);;) {
                const auto next = SquareTraits::shift(s, dir);
                if (next == SQ_NA)
                    break;
                out |= BitboardTraits::from_square(s);
                s = next;
            }
        }
        return out;
    }
    template <uint Shift>
    std::uint32_t find_magic_number(
        const vshogi::leshogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::leshogi;
        constexpr uint max_num_unique_occupancies = (1u << Shift);
        const bitboard_t premask = get_premask(sq, directions);
        const uint num_relevant_squares = vshogi::hamming_weight(premask);

        uint relevant_square_locations[Config::num_squares] = {};
        bitboard_t occupancies[max_num_unique_occupancies] = {};
        bitboard_t attacks[max_num_unique_occupancies] = {};

        for (uint ii = 0, jj = 0; ii < Config::num_squares; ++ii) {
            if (BT::is_one(premask, static_cast<SquareEnum>(ii)))
                relevant_square_locations[jj++] = ii;
        }
        for (uint ii = (1u << num_relevant_squares); ii--;) {
            occupancies[ii] = Magic::get_occupancy(
                ii, num_relevant_squares, relevant_square_locations);
            attacks[ii] = static_cast<bitboard_t>(0);
            for (auto&& dir : directions) {
                attacks[ii] |= Magic::compute_ray_to(sq, dir, occupancies[ii]);
            }
        }

        for (uint kk = 10000; kk--;) {
            bitboard_t used_attacks[max_num_unique_occupancies] = {};
            const std::uint32_t magic = sparse_random();
            bool found_magic = true;
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const auto index = Magic::to_magic_table_index<Shift>(
                    occupancies[ii], magic);
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
        const uint array[vshogi::leshogi::Config::num_squares],
        const bool hex = false)
    {
        using namespace vshogi::leshogi;
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

TEST(test_leshogi_magic, get_south_attack)
{
    {
        const auto actual
            = Magic::get_south_attack(SQ_1B, BT::from_square(SQ_1D));
        CHECK_EQUAL(0b01100u, actual);
    }
    {
        const auto actual
            = Magic::get_south_attack(SQ_1E, BT::from_square(SQ_1D));
        CHECK_EQUAL(0b100000u, actual);
    }
}

TEST(test_leshogi_magic, get_north_attack)
{
    {
        const auto actual
            = Magic::get_north_attack(SQ_2C, BT::from_rank(RANK2));
        CHECK_EQUAL(0b000000010000000u, actual);
    }
}

TEST(test_leshogi_magic, get_adjacent_attack)
{
    {
        const auto actual = Magic::get_adjacent_attack(SQ_3C);
        CHECK_EQUAL(0b000100111011000100000100u, actual);
    }
    {
        const auto actual
            = Magic::get_adjacent_attack(SQ_1A, BT::from_square(SQ_3A));
        CHECK_EQUAL(0b000000000001000001111110u, actual);
    }
}

TEST(test_leshogi_magic, get_diagonal_attack)
{
    {
        const auto actual = Magic::get_diagonal_attack(SQ_3C);
        CHECK_EQUAL(0b001010000000001010010001u, actual);
    }
    {
        const auto actual = Magic::get_diagonal_attack(
            SQ_2C, BT::from_square(SQ_3B) | BT::from_square(SQ_3D));
        CHECK_EQUAL(0b000000001010000000001010u, actual);
    }
}

// TEST(test_leshogi_magic, generate_magic_numbers)
// {
//     using namespace vshogi::leshogi;
//     std::uint32_t magics[Config::num_squares];

//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_lance>(
//                 sq,
//                 {vshogi::DIR_N,});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "leshogi::Magic::magic_number_north[leshogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";

//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_lance>(
//                 sq,
//                 {vshogi::DIR_S,});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "leshogi::Magic::magic_number_south[leshogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";

//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_adjacent>(
//                 sq,
//                 {vshogi::DIR_N, vshogi::DIR_W, vshogi::DIR_E, vshogi::DIR_S});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\ninline const std::uint32_t\n";
//     std::cout << "leshogi::Magic::magic_number_adjacent[leshogi::Config::"
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
//     std::cout << "leshogi::Magic::magic_number_diagonal[leshogi::Config::"
//                  "num_squares]={\n";
//     print_array(magics, true);
//     std::cout << "};";
// }

// TEST(test_leshogi_magic, generate_premasks)
// {
//     using namespace vshogi::leshogi;
//     uint premask_array[Config::num_squares] = {};

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         for (auto&& dir : {
//                  vshogi::DIR_N,
//              }) {
//             for (SquareEnum s = SquareTraits::shift(sq, dir);;) {
//                 const auto next = SquareTraits::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BT::from_square(s);
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "leshogi::Magic::premask_north[leshogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         for (auto&& dir : {
//                  vshogi::DIR_S,
//              }) {
//             for (SquareEnum s = SquareTraits::shift(sq, dir);;) {
//                 const auto next = SquareTraits::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BT::from_square(s);
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "leshogi::Magic::premask_south[leshogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         for (auto&& dir :
//              {vshogi::DIR_N, vshogi::DIR_W, vshogi::DIR_E, vshogi::DIR_S}) {
//             for (SquareEnum s = SquareTraits::shift(sq, dir);;) {
//                 const auto next = SquareTraits::shift(s, dir);
//                 if (next == SQ_NA)
//                     break;
//                 premask_array[sq] |= BT::from_square(s);
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "leshogi::Magic::premask_adjacent[leshogi::Config::num_"
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
//                 premask_array[sq] |= BT::from_square(s);
//                 s = next;
//             }
//         }
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "leshogi::Magic::premask_diagonal[leshogi::Config::num_"
//                  "squares]={\n";
//     print_array(premask_array, true);
//     std::cout << "};";
// }

} // namespace test_vshogi::test_leshogi
