#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (test_shogi_generator) {
};

TEST(test_shogi_generator, soldier_move_generator)
{
    const auto s = State("b8/8K/9/9/9/9/9/7l1/8K w -");
    auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
    CHECK_TRUE(Move(SQ_2H, SQ_2I, true) == *iter);
    ++iter;
    CHECK_TRUE(iter.is_end());
    CHECK_FALSE(iter != iter.end());
}

} // namespace test_vshogi::test_shogi
