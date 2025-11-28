#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi;
using namespace vshogi::shogi;
using BT = vshogi::BitboardTraits<Parameters>;
using ST = vshogi::SquareTraits<Parameters>;

TEST_GROUP (test_shogi_bitboard_traits) {
};

TEST(test_shogi_bitboard_traits, from_square)
{
    CHECK_TRUE(1u == BT::from_square(SQ_1A));
    CHECK_TRUE(2u == BT::from_square(SQ_1B));
    CHECK_TRUE(0u == BT::from_square(SQ_NA));
}

TEST(test_shogi_bitboard_traits, from_rank)
{
    CHECK_EQUAL(9u, hamming_weight(BT::from_rank(RANK1)));
    CHECK_TRUE(BT::is_one(BT::from_rank(RANK1), SQ_9A));
    CHECK_FALSE(BT::is_one(BT::from_rank(RANK1), SQ_9B));
}

TEST(test_shogi_bitboard_traits, shift)
{
    CHECK_TRUE(
        BT::from_square(SQ_8A) == BT::shift(BT::from_square(SQ_9A), DIR_E));
    CHECK_TRUE(
        BT::from_square(SQ_8C) == BT::shift(BT::from_square(SQ_9A), DIR_SSE));
    for (auto dir : Config::direction_iterator()) {
        for (auto sq : Config::square_iterator()) {
            CHECK_TRUE(
                BT::from_square(ST::shift(sq, dir))
                == BT::shift(BT::from_square(sq), dir));
        }
    }
}

TEST(test_shogi_bitboard_traits, get_ray_to)
{
    CHECK_TRUE(0u == BT::get_ray_to(SQ_NA, DIR_NA));
    CHECK_TRUE(0u == BT::get_ray_to(SQ_1B, DIR_NA));
    CHECK_TRUE(0u == BT::get_ray_to(SQ_NA, DIR_N));
    CHECK_TRUE(0b111111110u == BT::get_ray_to(SQ_1A, DIR_S));
    CHECK_TRUE(BT::from_square(SQ_2D) == BT::get_ray_to(SQ_1B, DIR_SSW));
}

TEST(test_shogi_bitboard_traits, get_attack_by)
{
    CHECK_TRUE(BT::from_square(SQ_2F) == BT::get_attack_by(B_FU, SQ_2G));
    CHECK_TRUE(BT::from_square(SQ_2H) == BT::get_attack_by(W_FU, SQ_2G));
    CHECK_TRUE(0b011111111u == BT::get_attack_by(B_KY, SQ_1I));
    CHECK_TRUE(0b111111100u == BT::get_attack_by(W_KY, SQ_1B));
}

TEST(test_shogi_bitboard_traits, get_promotion_zone)
{
    CHECK_TRUE(27u == hamming_weight(BT::promotion_zone(BLACK)));
    CHECK_TRUE(BT::is_one(BT::promotion_zone(BLACK), SQ_3C));
    CHECK_FALSE(BT::is_one(BT::promotion_zone(BLACK), SQ_3D));
    CHECK_FALSE(BT::is_one(BT::promotion_zone(BLACK), SQ_3I));
    CHECK_TRUE(BT::is_one(BT::promotion_zone(WHITE), SQ_3I));
}

} // namespace test_vshogi::test_shogi
