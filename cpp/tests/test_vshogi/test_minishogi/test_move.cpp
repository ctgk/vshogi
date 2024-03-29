#include "vshogi/minishogi/move.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_minishogi/test_minishogi.hpp"

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(move){};

TEST(move, destination)
{
    CHECK_EQUAL(SQ_1A, Move(SQ_1A, SQ_1B, true).destination());
    CHECK_EQUAL(SQ_3E, Move(SQ_3E, GI).destination());
}

TEST(move, source)
{
    CHECK_EQUAL(SQ_1B, Move(SQ_1A, SQ_1B, true).source<SquareEnum>());
    CHECK_EQUAL(GI, Move(SQ_3E, GI).source<PieceTypeEnum>());
}

TEST(move, promote)
{
    CHECK_TRUE(Move(SQ_1A, SQ_1B, true).promote());
    CHECK_FALSE(Move(SQ_3E, GI).promote());
}

TEST(move, is_drop)
{
    CHECK_FALSE(Move(SQ_1A, SQ_1B, true).is_drop());
    CHECK_TRUE(Move(SQ_3E, GI).is_drop());
}

TEST(move, hash)
{
    CHECK_TRUE(
        Move(SQ_1A, SQ_1B, true) == Move(Move(SQ_1A, SQ_1B, true).hash()));
    CHECK_TRUE(Move(SQ_3E, GI) == Move(Move(SQ_3E, GI).hash()));
}

TEST(move, rotate)
{
    CHECK_TRUE(Move(SQ_5E, SQ_5D, true) == Move(SQ_1A, SQ_1B, true).rotate());
    CHECK_TRUE(Move(SQ_3A, GI) == Move(SQ_3E, GI).rotate());
}

TEST(move, to_dlshogi_policy_index)
{
    CHECK_EQUAL(
        12 * (8 * 2 + 5) + 7, Move(SQ_3C, SQ_1E).to_dlshogi_policy_index());
    CHECK_EQUAL(
        16 * (8 * 2 + 5) + 2 + 8,
        Move(SQ_4D, SQ_1A, true).to_dlshogi_policy_index());
    CHECK_EQUAL(
        6 * (8 * 2 + 5) + 8 * 2 + 1, Move(SQ_4B, GI).to_dlshogi_policy_index());
}

} // namespace test_vshogi::test_minishogi
