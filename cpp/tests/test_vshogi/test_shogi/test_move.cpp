#include "vshogi/shogi/move.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_shogi/test_shogi.hpp"

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(move){};

TEST(move, usi)
{
    CHECK_TRUE(Move(SQ_1B, SQ_1A, true) == Move("1a1b+"));
    {
        char actual[6] = {'\0'};
        Move(SQ_3G, FU).to_usi(actual);
        STRCMP_EQUAL("P*3g", actual);
    }
    {
        char actual[6] = {'\0'};
        Move(SQ_1B, SQ_1A).to_usi(actual);
        STRCMP_EQUAL("1a1b", actual);
    }
    {
        char actual[6] = {'\0'};
        Move(SQ_1B, SQ_1A, true).to_usi(actual);
        STRCMP_EQUAL("1a1b+", actual);
    }
}

TEST(move, destination)
{
    CHECK_EQUAL(SQ_1A, Move(SQ_1A, SQ_1B, true).destination());
    CHECK_EQUAL(SQ_3E, Move(SQ_3E, KY).destination());
}

TEST(move, source)
{
    CHECK_EQUAL(SQ_1B, Move(SQ_1A, SQ_1B, false).source_square());
    CHECK_EQUAL(KE, Move(SQ_3I, KE).source_piece());
}

TEST(move, promote)
{
    CHECK_TRUE(Move(SQ_1A, SQ_1B, true).promote());
    CHECK_FALSE(Move(SQ_3G, FU).promote());
}

TEST(move, is_drop)
{
    CHECK_FALSE(Move(SQ_1A, SQ_1B, true).is_drop());
    CHECK_TRUE(Move(SQ_3G, FU).is_drop());
}

TEST(move, hash)
{
    CHECK_TRUE(
        Move(SQ_1A, SQ_1B, true) == Move(Move(SQ_1A, SQ_1B, true).hash()));
    CHECK_TRUE(Move(SQ_3E, GI) == Move(Move(SQ_3E, GI).hash()));
}

TEST(move, rotate)
{
    CHECK_TRUE(Move(SQ_9I, SQ_9H, true) == Move(SQ_1A, SQ_1B, true).rotate());
    CHECK_TRUE(Move(SQ_7G, HI) == Move(SQ_3C, HI).rotate());
}

TEST(move, to_dlshogi_policy_index)
{
    CHECK_EQUAL(
        24 * (10 * 2 + 7) + 6, Move(SQ_3C, SQ_3D).to_dlshogi_policy_index());
    CHECK_EQUAL(
        10 * (10 * 2 + 7) + 10,
        Move(SQ_8B, SQ_9A, true).to_dlshogi_policy_index());
    CHECK_EQUAL(
        80 * (10 * 2 + 7) + 20 + 2, Move(SQ_1I, KE).to_dlshogi_policy_index());
}

} // namespace test_vshogi::test_shogi
