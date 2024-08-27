#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_generator){};

TEST(animal_shogi_generator, king_move_generator)
{
    {
        const auto s = State("1l1/3/1L1/3 b -");
        auto iter = KingMoveGenerator(s);
        CHECK_TRUE(Move(SQ_A2, SQ_B3) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_B2, SQ_B3) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_C2, SQ_B3) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_A3, SQ_B3) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_C3, SQ_B3) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_A4, SQ_B3) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_B4, SQ_B3) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_C4, SQ_B3) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
}

TEST(animal_shogi_generator, drop_move_generator)
{
    {
        const auto s = State("glg/3/1L1/3 b C");
        auto iter = DropMoveGenerator(s);
        CHECK_TRUE(Move(SQ_A2, CH) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_B2, CH) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_C2, CH) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_A3, CH) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_C3, CH) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_A4, CH) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_B4, CH) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_C4, CH) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
}

} // namespace test_vshogi::test_minishogi
