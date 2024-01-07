#include "vshogi/animal_shogi/bitboard.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi;
using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_bitboard){};

TEST(animal_shogi_bitboard, shift)
{
    {
        CHECK_TRUE(file_b_mask == file_a_mask.shift<DIR_E>());
        CHECK_TRUE(file_c_mask == file_b_mask.shift<DIR_E>());
        CHECK_TRUE(BitBoard(0) == file_c_mask.shift<DIR_E>());
    }
    {
        CHECK_TRUE(BitBoard(0) == file_a_mask.shift<DIR_W>());
        CHECK_TRUE(file_a_mask == file_b_mask.shift<DIR_W>());
        CHECK_TRUE(file_b_mask == file_c_mask.shift<DIR_W>());
    }
}

TEST(animal_shogi_bitboard, chick_attacks)
{
    CHECK_TRUE(bb_a1 == chick_attacks[SQ_A2][BLACK]);
    CHECK_TRUE(bb_a3 == chick_attacks[SQ_A2][WHITE]);
}

TEST(animal_shogi_bitboard, elephant_attacks)
{
    CHECK_TRUE(bb_b2 == elephant_attacks[SQ_A1]);
    CHECK_TRUE((bb_b2 | bb_b4) == elephant_attacks[SQ_C3]);
    CHECK_TRUE((bb_a3 | bb_c3) == elephant_attacks[SQ_B4]);
}

TEST(animal_shogi_bitboard, giraffe_attacks)
{
    CHECK_TRUE((bb_c3 | bb_b4) == giraffe_attacks[SQ_C4]);
}

TEST(animal_shogi_bitboard, lion_attacks)
{
    CHECK_TRUE((bb_a3 | bb_b3 | bb_b4) == lion_attacks[SQ_A4]);
}

TEST(animal_shogi_bitboard, hen_attacks)
{
    CHECK_TRUE((bb_b1 | bb_c2) == hen_attacks[SQ_C1][BLACK]);
    CHECK_TRUE((bb_b4 | bb_c3) == hen_attacks[SQ_C4][WHITE]);
}

} // namespace test_vshogi::test_animal_shogi
