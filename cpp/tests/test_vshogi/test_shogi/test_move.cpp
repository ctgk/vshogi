#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (test_shogi_move) {
};

TEST(test_shogi_move, sfen)
{
    CHECK_TRUE(Move(SQ_1A, SQ_1B, true) == Move("1a1b+"));
    {
        char actual[6] = {'\0'};
        Move(FU, SQ_3G).to_sfen(actual);
        STRCMP_EQUAL("P*3g", actual);
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

TEST(test_shogi_move, destination)
{
    CHECK_EQUAL(SQ_1A, Move(SQ_1B, SQ_1A, true).destination());
    CHECK_EQUAL(SQ_3E, Move(KY, SQ_3E).destination());
}

TEST(test_shogi_move, source)
{
    CHECK_EQUAL(SQ_1B, Move(SQ_1B, SQ_1A, false).source_square());
    CHECK_EQUAL(KE, Move(KE, SQ_3I).source_piece());
}

TEST(test_shogi_move, promote)
{
    CHECK_TRUE(Move(SQ_1B, SQ_1A, true).promote());
    CHECK_FALSE(Move(FU, SQ_3G).promote());
}

TEST(test_shogi_move, is_drop)
{
    CHECK_FALSE(Move(SQ_1B, SQ_1A, true).is_drop());
    CHECK_TRUE(Move(FU, SQ_3G).is_drop());
}

TEST(test_shogi_move, hash)
{
    CHECK_TRUE(
        Move(SQ_1B, SQ_1A, true) == Move(Move(SQ_1B, SQ_1A, true).hash()));
    CHECK_TRUE(Move(GI, SQ_3E) == Move(Move(GI, SQ_3E).hash()));
}

TEST(test_shogi_move, rotate)
{
    CHECK_TRUE(Move(SQ_9H, SQ_9I, true) == Move(SQ_1B, SQ_1A, true).rotate());
    CHECK_TRUE(Move(HI, SQ_7G) == Move(HI, SQ_3C).rotate());
}

TEST(test_shogi_move, to_dlshogi_policy_index)
{
    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (10 * 2 + 7) + 6,
        Move(SQ_3D, SQ_3C).to_dlshogi_policy_index(vshogi::BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_8B) * (10 * 2 + 7) + 10,
        Move(SQ_9A, SQ_8B, true).to_dlshogi_policy_index(vshogi::BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_1I) * (10 * 2 + 7) + 20 + 2,
        Move(KE, SQ_1I).to_dlshogi_policy_index(vshogi::BLACK));

    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (10 * 2 + 7) + 6,
        Move(SQ_7F, SQ_7G).to_dlshogi_policy_index(vshogi::WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_8B) * (10 * 2 + 7) + 10,
        Move(SQ_1I, SQ_2H, true).to_dlshogi_policy_index(vshogi::WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_1I) * (10 * 2 + 7) + 20 + 2,
        Move(KE, SQ_9A).to_dlshogi_policy_index(vshogi::WHITE));
}

} // namespace test_vshogi::test_shogi
