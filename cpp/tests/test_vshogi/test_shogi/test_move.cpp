#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi;
using namespace vshogi::shogi;
using MT = vshogi::MoveTraits<Parameters>;

TEST_GROUP (test_shogi_move) {
};

TEST(test_shogi_move, get_dst)
{
    CHECK_EQUAL(SQ_1A, MT::get_dst(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(SQ_3E, MT::get_dst(MT::make_move(KY, SQ_3E)));
}

TEST(test_shogi_move, get_src)
{
    CHECK_EQUAL(SQ_1B, MT::get_src_sq(MT::make_move(SQ_1B, SQ_1A, false)));
    CHECK_EQUAL(KE, MT::get_src_pt(MT::make_move(KE, SQ_3I)));
}

TEST(test_shogi_move, get_promote)
{
    CHECK_TRUE(MT::get_promote(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_FALSE(MT::get_promote(MT::make_move(FU, SQ_3G)));
}

TEST(test_shogi_move, is_drop)
{
    CHECK_FALSE(MT::is_drop(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_TRUE(MT::is_drop(MT::make_move(FU, SQ_3G)));
}

TEST(test_shogi_move, rotate)
{
    CHECK_EQUAL(
        MT::make_move(SQ_9H, SQ_9I, true),
        MT::rotate(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(MT::make_move(HI, SQ_7G), MT::rotate(MT::make_move(HI, SQ_3C)));
}

TEST(test_shogi_move, to_policy_index)
{
    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (10 * 2 + 7) + 6,
        MT::to_policy_index(MT::make_move(SQ_3D, SQ_3C), BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_8B) * (10 * 2 + 7) + 10,
        MT::to_policy_index(MT::make_move(SQ_9A, SQ_8B, true), BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_1I) * (10 * 2 + 7) + 20 + 2,
        MT::to_policy_index(MT::make_move(KE, SQ_1I), BLACK));

    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * (10 * 2 + 7) + 6,
        MT::to_policy_index(MT::make_move(SQ_7F, SQ_7G), WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_8B) * (10 * 2 + 7) + 10,
        MT::to_policy_index(MT::make_move(SQ_1I, SQ_2H, true), WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_1I) * (10 * 2 + 7) + 20 + 2,
        MT::to_policy_index(MT::make_move(KE, SQ_9A), WHITE));
}

} // namespace test_vshogi::test_shogi
