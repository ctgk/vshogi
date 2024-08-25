#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(minishogi_generator){};

TEST(minishogi_generator, king_move_generator)
{
    {
        const auto s = State("5/2k2/5/2K2/5 b -");
        auto iter = KingMoveGenerator(&s);
        CHECK_TRUE(Move(SQ_4D, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2D, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_4E, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3E, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2E, SQ_3D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
}

} // namespace test_vshogi::test_minishogi
