#include "vshogi/variants/leshogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_leshogi
{

using namespace vshogi;
using namespace vshogi::leshogi;
using MT = vshogi::MoveTraits<Parameters>;

TEST_GROUP (test_leshogi_move) {
};

TEST(test_leshogi_move, test_get_dst)
{
    CHECK_EQUAL(SQ_1A, MT::get_dst(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(SQ_3E, MT::get_dst(MT::make_move(KY, SQ_3E)));
}

TEST(test_leshogi_move, test_get_src)
{
    CHECK_EQUAL(SQ_1B, MT::get_src_sq(MT::make_move(SQ_1B, SQ_1A, false)));
    CHECK_EQUAL(KE, MT::get_src_pt(MT::make_move(KE, SQ_3E)));
}

TEST(test_leshogi_move, test_get_promote)
{
    CHECK_TRUE(MT::get_promote(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_FALSE(MT::get_promote(MT::make_move(FU, SQ_1A)));
}

TEST(test_leshogi_move, test_is_drop)
{
    CHECK_FALSE(MT::is_drop(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_TRUE(MT::is_drop(MT::make_move(FU, SQ_1A)));
}

TEST(test_leshogi_move, test_rotate)
{
    CHECK_EQUAL(
        MT::make_move(SQ_4E, SQ_4F, true),
        MT::rotate(MT::make_move(SQ_1B, SQ_1A, true)));
    CHECK_EQUAL(MT::make_move(HI, SQ_2D), MT::rotate(MT::make_move(HI, SQ_3C)));
}

TEST(test_leshogi_move, test_to_policy_index)
{
    constexpr uint num_srcs
        = Config::num_dir_dl * 2 + Config::num_stand_piece_types;
    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * num_srcs + 6,
        MT::to_policy_index(MT::make_move(SQ_3D, SQ_3C), BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_3B) * num_srcs + Config::num_dir_dl + 0,
        MT::to_policy_index(MT::make_move(SQ_4A, SQ_3B, true), BLACK));
    CHECK_EQUAL(
        static_cast<int>(SQ_1F) * num_srcs + 2 * Config::num_dir_dl + 2,
        MT::to_policy_index(MT::make_move(KE, SQ_1F), BLACK));

    CHECK_EQUAL(
        static_cast<int>(SQ_3C) * num_srcs + 6,
        MT::to_policy_index(MT::make_move(SQ_2C, SQ_2D), WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_3B) * num_srcs + Config::num_dir_dl + 0,
        MT::to_policy_index(MT::make_move(SQ_1F, SQ_2E, true), WHITE));
    CHECK_EQUAL(
        static_cast<int>(SQ_1F) * num_srcs + 2 * Config::num_dir_dl + 2,
        MT::to_policy_index(MT::make_move(KE, SQ_4A), WHITE));
}

} // namespace test_vshogi::test_leshogi
