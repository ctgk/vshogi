#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP (test_judkins_shogi_move) {
};

TEST(test_judkins_shogi_move, destination)
{
    CHECK_EQUAL(SQ_1A, Move(SQ_1B, SQ_1A, true).destination());
    CHECK_EQUAL(SQ_3E, Move(GI, SQ_3E).destination());
}

TEST(test_judkins_shogi_move, source)
{
    CHECK_EQUAL(SQ_1B, Move(SQ_1B, SQ_1A, true).source_square());
    CHECK_EQUAL(GI, Move(GI, SQ_3E).source_piece());
}

TEST(test_judkins_shogi_move, promote)
{
    CHECK_TRUE(Move(SQ_1B, SQ_1A, true).promote());
    CHECK_FALSE(Move(GI, SQ_3E).promote());
}

TEST(test_judkins_shogi_move, is_drop)
{
    CHECK_FALSE(Move(SQ_1B, SQ_1A, true).is_drop());
    CHECK_TRUE(Move(GI, SQ_3E).is_drop());
}

TEST(test_judkins_shogi_move, hash)
{
    CHECK_TRUE(
        Move(SQ_1B, SQ_1A, true) == Move(Move(SQ_1B, SQ_1A, true).hash()));
    CHECK_TRUE(Move(GI, SQ_3E) == Move(Move(GI, SQ_3E).hash()));
}

TEST(test_judkins_shogi_move, rotate)
{
    CHECK_TRUE(Move(SQ_5D, SQ_5E, true) == Move(SQ_2C, SQ_2B, true).rotate());
    CHECK_TRUE(Move(GI, SQ_3A) == Move(GI, SQ_4F).rotate());
}

TEST(test_judkins_shogi_move, to_dlshogi_policy_index)
{
    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (10 * 2 + 6) + 7,
        Move(SQ_1E, SQ_3C).to_dlshogi_policy_index(vshogi::BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_4D) * (10 * 2 + 6) + 2 + 10,
        Move(SQ_1A, SQ_4D, true).to_dlshogi_policy_index(vshogi::BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_4B) * (10 * 2 + 6) + 10 * 2 + 2,
        Move(GI, SQ_4B).to_dlshogi_policy_index(vshogi::BLACK));

    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (10 * 2 + 6) + 7,
        Move(SQ_6B, SQ_4D).to_dlshogi_policy_index(vshogi::WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_4D) * (10 * 2 + 6) + 2 + 10,
        Move(SQ_6F, SQ_3C, true).to_dlshogi_policy_index(vshogi::WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_4B) * (10 * 2 + 6) + 10 * 2 + 2,
        Move(GI, SQ_3E).to_dlshogi_policy_index(vshogi::WHITE));
}

} // namespace test_vshogi::test_judkins_shogi
