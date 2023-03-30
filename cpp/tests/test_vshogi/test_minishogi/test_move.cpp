#include "vshogi/minishogi/move.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(move){};

TEST(move, destination)
{
    CHECK_EQUAL(SQ_11, Move(SQ_11, MS_12, true).destination());
    CHECK_EQUAL(SQ_35, Move(SQ_35, MS_GI, false).destination());
}

TEST(move, source)
{
    CHECK_EQUAL(MS_12, Move(SQ_11, MS_12, true).source());
    CHECK_EQUAL(MS_GI, Move(SQ_35, MS_GI, false).source());
}

TEST(move, promote)
{
    CHECK_TRUE(Move(SQ_11, MS_12, true).promote());
    CHECK_FALSE(Move(SQ_35, MS_GI, false).promote());
}

TEST(move, is_drop)
{
    CHECK_FALSE(Move(SQ_11, MS_12, true).is_drop());
    CHECK_TRUE(Move(SQ_35, MS_GI, false).is_drop());
}

TEST(move, hash)
{
    CHECK_TRUE(
        Move(SQ_11, MS_12, true) == Move(Move(SQ_11, MS_12, true).hash()));
    CHECK_TRUE(
        Move(SQ_35, MS_GI, false) == Move(Move(SQ_35, MS_GI, false).hash()));
}

TEST(move, rotate)
{
    CHECK_TRUE(Move(SQ_55, MS_54, true) == Move(SQ_11, MS_12, true).rotate());
    CHECK_TRUE(Move(SQ_31, MS_GI, false) == Move(SQ_35, MS_GI, false).rotate());
}

} // namespace test_vshogi::test_minishogi
