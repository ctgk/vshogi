#include "vshogi/animal_shogi/bitboard.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi;
using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_bitboard){};

TEST(animal_shogi_bitboard, one_file_right)
{
    CHECK_TRUE(file_b_mask == file_a_mask.one_file_right());
    CHECK_TRUE(file_c_mask == file_b_mask.one_file_right());
    CHECK_TRUE(BitBoard(0) == file_c_mask.one_file_right());
}

TEST(animal_shogi_bitboard, one_file_left)
{
    CHECK_TRUE(BitBoard(0) == file_a_mask.one_file_left());
    CHECK_TRUE(file_a_mask == file_b_mask.one_file_left());
    CHECK_TRUE(file_b_mask == file_c_mask.one_file_left());
}

TEST(animal_shogi_bitboard, chick_attacks)
{
    CHECK_TRUE(square_a1_mask == chick_attacks[SQ_A2][BLACK]);
    CHECK_TRUE(square_a3_mask == chick_attacks[SQ_A2][WHITE]);
}

TEST(animal_shogi_bitboard, elephant_attacks)
{
    CHECK_TRUE(square_b2_mask == elephant_attacks[SQ_A1]);
    CHECK_TRUE((square_b2_mask | square_b4_mask) == elephant_attacks[SQ_C3]);
    CHECK_TRUE((square_a3_mask | square_c3_mask) == elephant_attacks[SQ_B4]);
}

TEST(animal_shogi_bitboard, giraffe_attacks)
{
    CHECK_TRUE((square_c3_mask | square_b4_mask) == giraffe_attacks[SQ_C4]);
}

TEST(animal_shogi_bitboard, lion_attacks)
{
    CHECK_TRUE(
        (square_a3_mask | square_b3_mask | square_b4_mask)
        == lion_attacks[SQ_A4]);
}

TEST(animal_shogi_bitboard, hen_attacks)
{
    CHECK_TRUE((square_b1_mask | square_c2_mask) == hen_attacks[SQ_C1][BLACK]);
    CHECK_TRUE((square_b4_mask | square_c3_mask) == hen_attacks[SQ_C4][WHITE]);
}

} // namespace test_vshogi::test_animal_shogi
