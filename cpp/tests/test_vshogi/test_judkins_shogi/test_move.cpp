#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;
using namespace vshogi;
using MT = vshogi::MoveTraits<Parameters>;

TEST_GROUP (test_judkins_shogi_move) {
};

TEST(test_judkins_shogi_move, get_dst)
{
    CHECK_EQUAL(SQ_1A, MT::get_dst(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(SQ_3E, MT::get_dst(MT::make_move(GI, SQ_3E)));
}

TEST(test_judkins_shogi_move, get_src)
{
    CHECK_EQUAL(SQ_1B, MT::get_src_sq(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(GI, MT::get_src_pt(MT::make_move(GI, SQ_3E)));
}

TEST(test_judkins_shogi_move, promote)
{
    CHECK_TRUE(MT::get_promote(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_FALSE(MT::get_promote(MT::make_move(GI, SQ_3E)));
}

TEST(test_judkins_shogi_move, is_drop)
{
    CHECK_FALSE(MT::is_drop(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_TRUE(MT::is_drop(MT::make_move(GI, SQ_3E)));
}

TEST(test_judkins_shogi_move, rotate)
{
    CHECK_EQUAL(
        MT::make_move(SQ_5D, SQ_5E, true),
        MT::rotate(MT::make_move(SQ_2C, SQ_2B, true)));
    CHECK_EQUAL(MT::make_move(GI, SQ_3A), MT::rotate(MT::make_move(GI, SQ_4F)));
}

TEST(test_judkins_shogi_move, to_policy_index)
{
    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (10 * 2 + 6) + 7,
        MT::to_policy_index(MT::make_move(SQ_1E, SQ_3C), BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_4D) * (10 * 2 + 6) + 2 + 10,
        MT::to_policy_index(MT::make_move(SQ_1A, SQ_4D, true), BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_4B) * (10 * 2 + 6) + 10 * 2 + 2,
        MT::to_policy_index(MT::make_move(GI, SQ_4B), BLACK));

    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (10 * 2 + 6) + 7,
        MT::to_policy_index(MT::make_move(SQ_6B, SQ_4D), WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_4D) * (10 * 2 + 6) + 2 + 10,
        MT::to_policy_index(MT::make_move(SQ_6F, SQ_3C, true), WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_4B) * (10 * 2 + 6) + 10 * 2 + 2,
        MT::to_policy_index(MT::make_move(GI, SQ_3E), WHITE));
}

} // namespace test_vshogi::test_judkins_shogi
