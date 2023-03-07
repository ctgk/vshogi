#include "vshogi/animal_shogi/bitboard.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_bitboard){};

TEST(animal_shogi_bitboard, one_file_right)
{
    CHECK_TRUE(file_b_mask == file_a_mask.one_file_right());
    CHECK_TRUE(file_c_mask == file_b_mask.one_file_right());
    CHECK_TRUE(BitBoard(0) == file_c_mask.one_file_right());
}

TEST(animal_shogi_bitboard, chick_attacks)
{
    CHECK_TRUE(square_a1_mask == chick_attacks[SQ_A2][BLACK]);
    CHECK_TRUE(square_a3_mask == chick_attacks[SQ_A2][WHITE]);
}

} // namespace test_vshogi::test_animal_shogi
