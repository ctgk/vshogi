#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP (test_judkins_shogi_generator) {
};

TEST(test_judkins_shogi_generator, soldier_move_generator)
{
    {
        const auto s = State("6/6/6/2N3/6/k4K b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_TRUE(Move(SQ_3B, SQ_4D, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5B, SQ_4D, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("5k/6/6/2N3/6/6 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_TRUE(Move(SQ_3B, SQ_4D, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5B, SQ_4D, true) == *iter);
        ++iter;
        CHECK_TRUE(iter.is_end());
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("5k/6/6/2N3/6/B4K b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
        CHECK_TRUE(Move(SQ_3B, SQ_4D, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5B, SQ_4D, true) == *iter);
        ++iter;
        CHECK_TRUE(iter.is_end());
        CHECK_FALSE(iter != iter.end());
    }
}

} // namespace test_vshogi::test_judkins_shogi
