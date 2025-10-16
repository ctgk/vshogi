#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP (test_minishogi_move) {
};

TEST(test_minishogi_move, sfen)
{
    CHECK_TRUE(Move(SQ_1A, SQ_1B, true) == Move("1a1b+"));
    {
        char actual[6] = {'\0'};
        Move(FU, SQ_3C).to_sfen(actual);
        STRCMP_EQUAL("P*3c", actual);
    }
    {
        char actual[6] = {'\0'};
        Move(SQ_1A, SQ_1B).to_sfen(actual);
        STRCMP_EQUAL("1a1b", actual);
    }
    {
        char actual[6] = {'\0'};
        Move(SQ_1A, SQ_1B, true).to_sfen(actual);
        STRCMP_EQUAL("1a1b+", actual);
    }
}

TEST(test_minishogi_move, destination)
{
    CHECK_EQUAL(SQ_1A, Move(SQ_1B, SQ_1A, true).destination());
    CHECK_EQUAL(SQ_3E, Move(GI, SQ_3E).destination());
}

TEST(test_minishogi_move, source)
{
    CHECK_EQUAL(SQ_1B, Move(SQ_1B, SQ_1A, true).source_square());
    CHECK_EQUAL(GI, Move(GI, SQ_3E).source_piece());
}

TEST(test_minishogi_move, promote)
{
    CHECK_TRUE(Move(SQ_1B, SQ_1A, true).promote());
    CHECK_FALSE(Move(GI, SQ_3E).promote());
}

TEST(test_minishogi_move, is_drop)
{
    CHECK_FALSE(Move(SQ_1B, SQ_1A, true).is_drop());
    CHECK_TRUE(Move(GI, SQ_3E).is_drop());
}

TEST(test_minishogi_move, hash)
{
    CHECK_TRUE(
        Move(SQ_1B, SQ_1A, true) == Move(Move(SQ_1B, SQ_1A, true).hash()));
    CHECK_TRUE(Move(GI, SQ_3E) == Move(Move(GI, SQ_3E).hash()));
}

TEST(test_minishogi_move, rotate)
{
    CHECK_TRUE(Move(SQ_5D, SQ_5E, true) == Move(SQ_1B, SQ_1A, true).rotate());
    CHECK_TRUE(Move(GI, SQ_3A) == Move(GI, SQ_3E).rotate());
}

TEST(test_minishogi_move, hflip)
{
    CHECK_TRUE(Move(SQ_5B, SQ_5A, true) == Move(SQ_1B, SQ_1A, true).hflip());
    CHECK_TRUE(Move(GI, SQ_3E) == Move(GI, SQ_3E).hflip());
}

TEST(test_minishogi_move, to_dlshogi_policy_index)
{
    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (8 * 2 + 5) + 7,
        Move(SQ_1E, SQ_3C).to_dlshogi_policy_index());
    CHECK_EQUAL(
        static_cast<int>(SQ_4D) * (8 * 2 + 5) + 2 + 8,
        Move(SQ_1A, SQ_4D, true).to_dlshogi_policy_index());
    CHECK_EQUAL(
        static_cast<int>(SQ_4B) * (8 * 2 + 5) + 8 * 2 + static_cast<int>(GI),
        Move(GI, SQ_4B).to_dlshogi_policy_index());
}

} // namespace test_vshogi::test_minishogi
