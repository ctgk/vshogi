#include "vshogi/minishogi/move.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(move){};

TEST(move, destination)
{
    CHECK_EQUAL(SQ_1A, Move(SQ_1A, MS_1B, true).destination());
    CHECK_EQUAL(SQ_3E, Move(SQ_3E, MS_GI, false).destination());
}

TEST(move, source)
{
    CHECK_EQUAL(MS_1B, Move(SQ_1A, MS_1B, true).source());
    CHECK_EQUAL(MS_GI, Move(SQ_3E, MS_GI, false).source());
}

TEST(move, promote)
{
    CHECK_TRUE(Move(SQ_1A, MS_1B, true).promote());
    CHECK_FALSE(Move(SQ_3E, MS_GI, false).promote());
}

TEST(move, is_drop)
{
    CHECK_FALSE(Move(SQ_1A, MS_1B, true).is_drop());
    CHECK_TRUE(Move(SQ_3E, MS_GI, false).is_drop());
}

TEST(move, hash)
{
    CHECK_TRUE(
        Move(SQ_1A, MS_1B, true) == Move(Move(SQ_1A, MS_1B, true).hash()));
    CHECK_TRUE(
        Move(SQ_3E, MS_GI, false) == Move(Move(SQ_3E, MS_GI, false).hash()));
}

TEST(move, rotate)
{
    CHECK_TRUE(Move(SQ_5E, MS_5D, true) == Move(SQ_1A, MS_1B, true).rotate());
    CHECK_TRUE(Move(SQ_3A, MS_GI, false) == Move(SQ_3E, MS_GI, false).rotate());
}

} // namespace test_vshogi::test_minishogi
