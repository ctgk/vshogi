#include "vshogi/animal_shogi/move.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_animal_shogi/test_animal_shogi.hpp"

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(Move){};

TEST(Move, source)
{
    CHECK_EQUAL(GI, Move(SQ_A1, GI).source_piece());
    CHECK_EQUAL(LI, Move(SQ_A1, LI).source_piece());
    CHECK_EQUAL(GI, Move(SQ_A1, GI).source_piece());
}

TEST(Move, to_dlshogi_policy_index)
{
    CHECK_EQUAL(60, Move(SQ_C2, SQ_B3).to_dlshogi_policy_index());
    CHECK_EQUAL(
        65, Move(SQ_C2, GI).to_dlshogi_policy_index()); // 5(C2) * 11 + 10(GI)
}

} // namespace test_vshogi::test_animal_shogi
