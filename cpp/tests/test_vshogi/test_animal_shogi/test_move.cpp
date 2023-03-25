#include "vshogi/animal_shogi/move.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(Move){};

TEST(Move, source)
{
    CHECK_EQUAL(MS_GI, Move(SQ_A1, GI).source());
    CHECK_EQUAL(MS_A1, Move(SQ_A1, LI).source());
}

TEST(Move, policy_index)
{
    const auto m = Move::_from_policy_index(4);
    CHECK_EQUAL(SQ_A1, m.destination());
    CHECK_EQUAL(MS_B1, m.source());
    CHECK_EQUAL(4, m._to_policy_index());

    CHECK_EQUAL(60, Move(SQ_C2, SQ_B3)._to_policy_index());
    CHECK_EQUAL(65, Move(SQ_C2, GI)._to_policy_index()); // 5(C2) * 11 + 10(GI)
}

} // namespace test_vshogi::test_animal_shogi
