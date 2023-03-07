#include "vshogi/animal_shogi/bitboard.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_bitboard){};

TEST(animal_shogi_bitboard, chick_attacks)
{
    CHECK_TRUE(square_a1_mask == chick_attacks[SQ_A2][BLACK]);
    CHECK_TRUE(square_a3_mask == chick_attacks[SQ_A2][WHITE]);
}

} // namespace test_vshogi::test_animal_shogi
