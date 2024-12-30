#include <iomanip>
#include <iostream>
#include <random>

#include "vshogi/common/magic.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

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
    vshogi::shogi::BitBoard get_premask(
        const vshogi::shogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::shogi;
        BitBoard out{};
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
        const vshogi::shogi::SquareEnum& sq,
        const std::vector<vshogi::DirectionEnum>& directions)
    {
        using namespace vshogi::shogi;
        const auto premask = get_premask(sq, directions);
        const uint num_relevant_squares = premask.hamming_weight();
        constexpr uint max_unique_occupancies
            = (1u << Config::log2_magic_table_size);
        if (num_relevant_squares == 0u)
            return 0xffffffff;

        uint relevant_square_locations[Config::num_squares] = {};
        BitBoard occupancies[max_unique_occupancies] = {};
        BitBoard attacks[max_unique_occupancies] = {};

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

        for (uint kk = 10000000; kk--;) {
            BitBoard used_attacks[max_unique_occupancies] = {};
            const std::uint32_t magic = sparse_random();
            bool found_magic = true;
            for (uint ii = (1u << num_relevant_squares); ii--;) {
                const auto index = Magic::to_magic_table_index(
                    occupancies[ii].value(), magic);
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
        for (auto sq :
             vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
            if (hex)
                std::cout << "0x" << std::hex << std::setfill('0')
                          << std::setw(sizeof(Int) * 2) << array[sq]
                          << std::dec;
            else
                std::cout << array[sq];

            if (Squares::to_rank(sq) == RANK9)
                std::cout << ",\n";
            else
                std::cout << ", ";
        }
    }
    void print_array(
        const vshogi::uint128 array[vshogi::shogi::Config::num_squares])
    {
        using namespace vshogi::shogi;
        for (auto sq :
             vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
            std::cout << "(static_cast<uint128>(0x" << std::hex
                      << std::setfill('0') << std::setw(5)
                      << static_cast<std::uint64_t>(array[sq] >> 64)
                      << ") << 64) | static_cast<uint128>(0x"
                      << std::setfill('0')
                      << std::setw(sizeof(std::uint64_t) * 2)
                      << static_cast<std::uint64_t>(array[sq]) << ")"
                      << std::dec;

            if (Squares::to_rank(sq) == RANK9)
                std::cout << ",\n";
            else
                std::cout << ", ";
        }
    }
};

TEST(test_shogi_magic, get_south_attack)
{
    using namespace vshogi::shogi;
    {
        const auto sq = SQ_9A;
        const auto occ = BitBoard::from_square(SQ_9F);
        const auto actual = Magic::get_south_attack(sq, occ);
        CHECK_TRUE(
            (bb_9b | bb_9c | bb_9d | bb_9e | bb_9f).value() == actual.value());
    }
}

TEST(test_shogi_magic, get_adjacent_attack)
{
    using namespace vshogi::shogi;
    {
        const auto actual = Magic::get_adjacent_attack(SQ_5E);
        CHECK_TRUE((bb_file5 | bb_ranke).clear(SQ_5E) == actual);
    }
    {
        const auto sq = SQ_5A;
        const auto occ = BitBoard();
        const auto actual = Magic::get_adjacent_attack(sq, occ);
        CHECK_TRUE(
            BitBoard::get_attacks_by(B_HI, sq, occ).value() == actual.value());
    }
    {
        const auto sq = SQ_5A;
        const auto occ = BitBoard::from_square(SQ_5B);
        const auto actual = Magic::get_adjacent_attack(sq, occ);
        CHECK_TRUE(
            BitBoard::get_attacks_by(B_HI, sq, occ).value() == actual.value());
    }
    {
        const auto sq = SQ_3C;
        const auto occ = bb_1a;
        const auto actual = Magic::get_adjacent_attack(sq, occ);
        CHECK_TRUE(
            BitBoard::get_attacks_by(B_HI, sq, occ).value() == actual.value());
    }
}

TEST(test_shogi_magic, get_diagonal_attack)
{
    using namespace vshogi::shogi;
    {
        const auto actual = Magic::get_diagonal_attack(SQ_1A);
        CHECK_TRUE(
            (bb_2b | bb_3c | bb_4d | bb_5e | bb_6f | bb_7g | bb_8h | bb_9i)
            == actual);
    }
    {
        const auto sq = SQ_5A;
        const auto occ = BitBoard();
        const auto actual = Magic::get_diagonal_attack(sq, occ);
        CHECK_TRUE(
            BitBoard::get_attacks_by(B_KA, sq, occ).value() == actual.value());
    }
    {
        const auto sq = SQ_5A;
        const auto occ = bb_4b;
        const auto actual = Magic::get_diagonal_attack(sq, occ);
        CHECK_TRUE(
            BitBoard::get_attacks_by(B_KA, sq, occ).value() == actual.value());
    }
    {
        const auto sq = SQ_3C;
        const auto occ = bb_2b | bb_2d;
        const auto actual = Magic::get_diagonal_attack(sq, occ);
        CHECK_TRUE(
            BitBoard::get_attacks_by(B_KA, sq, occ).value() == actual.value());
    }
}

// TEST(test_shogi_magic, generate_magic_numbers)
// {
//     using namespace vshogi::shogi;
//     std::uint32_t magics[Config::num_squares] = {};

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic
//             = find_magic_number(sq, {vshogi::DIR_N,});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "shogi::Magic::magic_number_north["
//                  "shogi::Config::num_squares]={\n// clang-format off\n";
//     print_array(magics, true);
//     std::cout << "// clang-format on\n};";

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic
//             = find_magic_number(sq, {vshogi::DIR_S,});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "shogi::Magic::magic_number_south["
//                  "shogi::Config::num_squares]={\n// clang-format off\n";
//     print_array(magics, true);
//     std::cout << "// clang-format on\n};";

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic
//             = find_magic_number(sq, {vshogi::DIR_N, vshogi::DIR_S});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "shogi::Magic::magic_number_vertical["
//                  "shogi::Config::num_squares]={\n// clang-format off\n";
//     print_array(magics, true);
//     std::cout << "// clang-format on\n};";

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic
//             = find_magic_number(sq, {vshogi::DIR_W, vshogi::DIR_E});
//         CHECK_FALSE(magic == 0u);
//         magics[sq] = magic;
//     }
//     std::cout << "\ntemplate <>\n";
//     std::cout << "inline const std::uint32_t "
//                  "shogi::Magic::magic_number_horizontal["
//                  "shogi::Config::num_squares]={\n// clang-format off\n";
//     print_array(magics, true);
//     std::cout << "// clang-format on\n};";

//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         const std::uint32_t magic = find_magic_number(
//             sq,
//             {vshogi::DIR_NW, vshogi::DIR_NE, vshogi::DIR_SW, vshogi::DIR_SE});
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
//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         for (auto&& dir : {vshogi::DIR_N,}) {
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
//     std::cout << "inline const shogi::BitBoard shogi::Magic::premask_north["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         for (auto&& dir : {vshogi::DIR_S,}) {
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
//     std::cout << "inline const shogi::BitBoard shogi::Magic::premask_south["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";

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
//     std::cout << "inline const shogi::BitBoard shogi::Magic::premask_vertical["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";

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
//     std::cout << "inline const shogi::BitBoard "
//                  "shogi::Magic::premask_horizontal["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";

//     std::fill_n(premask_array, Config::num_squares, 0u);
//     for (auto sq : vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
//         for (auto&& dir :
//              {vshogi::DIR_NW, vshogi::DIR_NE, vshogi::DIR_SW, vshogi::DIR_SE}) {
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
//     std::cout << "inline const shogi::BitBoard "
//                  "shogi::Magic::premask_diagonal["
//                  "shogi::Config::num_squares]={\n"
//                  "// clang-format off\n";
//     print_array(premask_array);
//     std::cout << "// clang-format on\n" << "};";
// }

} // namespace test_vshogi::test_shogi
