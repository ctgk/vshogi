#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(Move){};

TEST(Move, usi)
{
    CHECK_TRUE(Move(SQ_B1, SQ_A1, true) == Move("a1b1+"));
    {
        char actual[6] = {'\0'};
        Move(SQ_C3, CH).to_usi(actual);
        STRCMP_EQUAL("C*c3", actual);
    }
    {
        char actual[6] = {'\0'};
        Move(SQ_B1, SQ_A1).to_usi(actual);
        STRCMP_EQUAL("a1b1", actual);
    }
}

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
