#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi;
using namespace vshogi::minishogi;
using MT = vshogi::MoveTraits<Parameters>;

TEST_GROUP (test_minishogi_move) {
};

TEST(test_minishogi_move, get_dst)
{
    CHECK_EQUAL(SQ_1A, MT::get_dst(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(SQ_3E, MT::get_dst(MT::make_move(GI, SQ_3E)));
}

TEST(test_minishogi_move, get_src)
{
    CHECK_EQUAL(SQ_1B, MT::get_src_sq(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(GI, MT::get_src_pt(MT::make_move(GI, SQ_3E)));
}

TEST(test_minishogi_move, get_promote)
{
    CHECK_TRUE(MT::get_promote(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_FALSE(MT::get_promote(MT::make_move(GI, SQ_3E)));
}

TEST(test_minishogi_move, is_drop)
{
    CHECK_FALSE(MT::is_drop(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_TRUE(MT::is_drop(MT::make_move(GI, SQ_3E)));
}

TEST(test_minishogi_move, rotate)
{
    CHECK_EQUAL(
        MT::make_move(SQ_5D, SQ_5E, true),
        MT::rotate(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(MT::make_move(GI, SQ_3A), MT::rotate(MT::make_move(GI, SQ_3E)));
}

TEST(test_minishogi_move, hflip)
{
    CHECK_EQUAL(
        MT::make_move(SQ_5B, SQ_5A, true),
        MT::hflip(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(MT::make_move(GI, SQ_3E), MT::hflip(MT::make_move(GI, SQ_3E)));
}

TEST(test_minishogi_move, to_policy_index)
{
    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (8 * 2 + 5) + 7,
        MT::to_policy_index(MT::make_move(SQ_1E, SQ_3C), BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_4D) * (8 * 2 + 5) + 2 + 8,
        MT::to_policy_index(MT::make_move(SQ_1A, SQ_4D, true), BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_4B) * (8 * 2 + 5) + 8 * 2 + static_cast<int>(GI),
        MT::to_policy_index(MT::make_move(GI, SQ_4B), BLACK));

    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (8 * 2 + 5) + 7,
        MT::to_policy_index(MT::make_move(SQ_5A, SQ_3C), WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_4D) * (8 * 2 + 5) + 2 + 8,
        MT::to_policy_index(MT::make_move(SQ_5E, SQ_2B, true), WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_4B) * (8 * 2 + 5) + 8 * 2 + static_cast<int>(GI),
        MT::to_policy_index(MT::make_move(GI, SQ_2D), WHITE));
}

} // namespace test_vshogi::test_minishogi
