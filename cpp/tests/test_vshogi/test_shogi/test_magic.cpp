#include <iomanip>
#include <iostream>
#include <random>

#include "vshogi/common/magic.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;
using BT = vshogi::BitboardTraits<Parameters>;

TEST_GROUP (test_shogi_magic) {
    std::uint32_t random_uint32()
    {
        const std::uint32_t u1 = static_cast<std::uint32_t>(random()) & 0xffffu;
        const std::uint32_t u2 = static_cast<std::uint32_t>(random()) & 0xffffu;
        return u1 | (u2 << 16);
    }
    std::uint32_t sparse_random()
    {
        return random_uint32() & random_uint32();
    }
    vshogi::shogi::bitboard_t get_premask(
        const vshogi::shogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::shogi;
        bitboard_t out{};
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
        const vshogi::shogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::shogi;
        const auto premask = get_premask(sq, directions);
        const uint num_relevant_squares = vshogi::hamming_weight(premask);
        constexpr uint max_unique_occupancies = (1u << Shift);
        if (num_relevant_squares == 0u)
            return 0xffffffff;

        uint relevant_square_locations[Config::num_squares] = {};
        bitboard_t occupancies[max_unique_occupancies] = {};
        bitboard_t attacks[max_unique_occupancies] = {};

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

        for (uint kk = 1000000; kk--;) {
            bitboard_t used_attacks[max_unique_occupancies] = {};
            const std::uint32_t magic = sparse_random();
            bool found_magic = true;
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const auto index = Magic::to_magic_table_index<Shift>(
                    occupancies[ii], magic);
                if (!used_attacks[index].any()) {
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
    template <class Int>
    void print_array(
        const Int array[vshogi::shogi::Config::num_squares],
        const bool hex = false)
    {
        using namespace vshogi::shogi;
        for (auto sq : Config::square_iterator()) {
            if (hex)
                std::cout << "0x" << std::hex << std::setfill('0')
                          << std::setw(sizeof(Int) * 2) << array[sq]
                          << std::dec;
            else
                std::cout << array[sq];

            if (SquareTraits::to_rank(sq) == RANK9)
                std::cout << ",\n";
            else
                std::cout << ", ";
        }
    }
    void print_array(
        const vshogi::uint128 array[vshogi::shogi::Config::num_squares])
    {
        using namespace vshogi::shogi;
        for (auto sq : Config::square_iterator()) {
            std::cout << "(static_cast<uint128>(0x" << std::hex
                      << std::setfill('0') << std::setw(5)
                      << static_cast<std::uint64_t>(array[sq] >> 64)
                      << ") << 64) | static_cast<uint128>(0x"
                      << std::setfill('0')
                      << std::setw(sizeof(std::uint64_t) * 2)
                      << static_cast<std::uint64_t>(array[sq]) << ")"
                      << std::dec;

            if (SquareTraits::to_rank(sq) == RANK9)
                std::cout << ",\n";
            else
                std::cout << ", ";
        }
    }
};

TEST(test_shogi_magic, get_south_attack)
{
    {
        const auto actual
            = Magic::get_south_attack(SQ_1E, BT::from_square(SQ_1H));
        CHECK_TRUE(0b011100000u == actual);
    }
}

TEST(test_shogi_magic, get_north_attack)
{
    {
        const auto actual
            = Magic::get_north_attack(SQ_2C, BT::from_rank(RANK2));
        CHECK_TRUE(0b000000010000000000u == actual);
    }
}

TEST(test_shogi_magic, get_adjacent_attack)
{
    {
        const auto actual = Magic::get_adjacent_attack(SQ_5E);
        CHECK_TRUE(
            ((BT::from_file(FILE5) | BT::from_rank(RANK5))
             ^ BT::from_square(SQ_5E))
            == actual);
    }
}

TEST(test_shogi_magic, get_diagonal_attack)
{
    {
        const auto actual = Magic::get_diagonal_attack(SQ_1A);
        CHECK_EQUAL(8u, vshogi::hamming_weight(actual));
    }
    {
        const auto actual
            = Magic::get_diagonal_attack(SQ_1A, BT::from_square(SQ_2B));
        CHECK_EQUAL(1u, vshogi::hamming_weight(actual));
    }
}

// TEST(test_shogi_magic, generate_magic_numbers)
// {
//     using namespace vshogi::shogi;
//     std::uint32_t magics[Config::num_squares] = {};

//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_lance>(
//                 sq,
//                 {
//                     vshogi::DIR_N,
//                 });
//         if (magic == 0u) {
//             std::cout << "\ntemplate <>\n";
//             std::cout << "inline const std::uint32_t "
//                          "shogi::Magic::magic_number_north["
//                          "shogi::Config::num_squares]={\n// clang-format off\n";
//             print_array(magics, true);
//             std::cout << "// clang-format on\n};";
//         }
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "shogi::Magic::magic_number_north["
//                  "shogi::Config::num_squares]={\n// clang-format off\n";
//     print_array(magics, true);
//     std::cout << "// clang-format on\n};";

//     std::fill_n(magics, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_lance>(
//                 sq,
//                 {
//                     vshogi::DIR_S,
//                 });
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "shogi::Magic::magic_number_south["
//                  "shogi::Config::num_squares]={\n// clang-format off\n";
//     print_array(magics, true);
//     std::cout << "// clang-format on\n};";

//     std::fill_n(magics, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_adjacent>(
//                 sq,
//                 {vshogi::DIR_N, vshogi::DIR_W, vshogi::DIR_E, vshogi::DIR_S});
//         if (magic == 0u) {
//             std::cout << "\ntemplate <>\n";
//             std::cout << "inline const std::uint32_t "
//                          "shogi::Magic::magic_number_adjacent["
//                          "shogi::Config::num_squares]={\n// clang-format off\n";
//             print_array(magics, true);
//             std::cout << "// clang-format on\n};";
//         }
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "shogi::Magic::magic_number_adjacent["
//                  "shogi::Config::num_squares]={\n// clang-format off\n";
//     print_array(magics, true);
//     std::cout << "// clang-format on\n};";

//     std::fill_n(magics, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         const std::uint32_t magic
//             = find_magic_number<Magic::log2_table_size_diagonal>(
//                 sq,
//                 {vshogi::DIR_NW,
//                  vshogi::DIR_NE,
//                  vshogi::DIR_SW,
//                  vshogi::DIR_SE});
//         if (magic == 0u) {
//             std::cout << "\ntemplate <>\n";
//             std::cout << "inline const std::uint32_t "
//                         "shogi::Magic::magic_number_diagonal["
//                         "shogi::Config::num_squares]={\n// clang-format off\n";
//             print_array(magics, true);
//             std::cout << "// clang-format on\n};";
//         }
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "shogi::Magic::magic_number_diagonal["
//                  "shogi::Config::num_squares]={\n// clang-format off\n";
//     print_array(magics, true);
//     std::cout << "// clang-format on\n};";
// }

// TEST(test_shogi_magic, generate_premasks)
// {
//     using namespace vshogi::shogi;
//     vshogi::uint128 premask_array[Config::num_squares] = {};
//     for (auto sq : Config::square_iterator()) {
//         for (auto&& dir : {vshogi::DIR_N,}) {
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
//     std::cout << "inline const uint128 shogi::Magic::premask_north["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : Config::square_iterator()) {
//         for (auto&& dir : {vshogi::DIR_S,}) {
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
//     std::cout << "inline const uint128 shogi::Magic::premask_south["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";

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
//     std::cout << "inline const uint128 "
//                  "shogi::Magic::premask_adjacent["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";

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
//     std::cout << "inline const uint128 "
//                  "shogi::Magic::premask_diagonal["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";
// }

} // namespace test_vshogi::test_shogi
