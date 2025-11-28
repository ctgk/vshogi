#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;
using BT = vshogi::BitboardTraits<Parameters>;

TEST_GROUP (test_minishogi_bitboard_traits) {
};

TEST(test_minishogi_bitboard_traits, from_square)
{
    CHECK_EQUAL(0b0000000000000000000000001u, BT::from_square(SQ_1A));
    CHECK_EQUAL(0b0000000000000000000000010u, BT::from_square(SQ_1B));
    CHECK_EQUAL(0b0000000000000000000000100u, BT::from_square(SQ_1C));
    CHECK_EQUAL(0b0000000000000000000100000u, BT::from_square(SQ_2A));
    CHECK_EQUAL(0b1000000000000000000000000u, BT::from_square(SQ_5E));
    CHECK_EQUAL(0b0000000000000000000000000u, BT::from_square(SQ_NA));
}
TEST(test_minishogi_bitboard_traits, from_rank)
{
    CHECK_EQUAL(0b0000100001000010000100001u, BT::from_rank(RANK1));
    CHECK_EQUAL(0b0001000010000100001000010u, BT::from_rank(RANK2));
    CHECK_EQUAL(0b0010000100001000010000100u, BT::from_rank(RANK3));
    CHECK_EQUAL(0b0100001000010000100001000u, BT::from_rank(RANK4));
    CHECK_EQUAL(0b1000010000100001000010000u, BT::from_rank(RANK5));
}
TEST(test_minishogi_bitboard_traits, from_file)
{
    CHECK_EQUAL(0b0000000000000000000011111u, BT::from_file(FILE1));
    CHECK_EQUAL(0b0000000000000001111100000u, BT::from_file(FILE2));
    CHECK_EQUAL(0b0000000000111110000000000u, BT::from_file(FILE3));
    CHECK_EQUAL(0b0000011111000000000000000u, BT::from_file(FILE4));
    CHECK_EQUAL(0b1111100000000000000000000u, BT::from_file(FILE5));
}
TEST(test_minishogi_bitboard_traits, promotion_zone)
{
    using namespace vshogi;
    CHECK_EQUAL(0b0000100001000010000100001u, BT::promotion_zone(BLACK));
    CHECK_EQUAL(0b1000010000100001000010000u, BT::promotion_zone(WHITE));
}
TEST(test_minishogi_bitboard_traits, shift)
{
    using namespace vshogi;
    // clang-format off
    CHECK_EQUAL(  0b0000000000000000111100000u,
        BT::shift(0b0000000000000000000011111u, DIR_NW));
    CHECK_EQUAL(  0b0000000000000000000000000u,
        BT::shift(0b0000100001000010000100001u, DIR_N));
    CHECK_EQUAL(  0b0000000000000000000100000u,
        BT::shift(0b0000000000000000000000001u, DIR_W));
    CHECK_EQUAL(  0b0000100001000010000100000u,
        BT::shift(0b0000100001000010000100001u, DIR_W));
    CHECK_EQUAL(  0b0000000001000010000100001u,
        BT::shift(0b0000100001000010000100001u, DIR_E));
    CHECK_EQUAL(  0b0000000000111100000000000u,
        BT::shift(0b0000000000000001111100000u, DIR_SW));
    CHECK_EQUAL(  0b0000000000000001111000000u,
        BT::shift(0b0000000000000001111100000u, DIR_S));
    CHECK_EQUAL(  0b0000000000000000000011110u,
        BT::shift(0b0000000000000001111100000u, DIR_SE));
    // clang-format on
}
TEST(test_minishogi_bitboard_traits, is_one)
{
    CHECK_FALSE(BT::is_one(0b0000000000000001111100000u, SQ_1A));
    CHECK_TRUE(BT::is_one(0b0000000000000001111100000u, SQ_2A));
    CHECK_FALSE(BT::is_one(0b0000000000000001111100000u, SQ_NA));
}
TEST(test_minishogi_bitboard_traits, get_ray_to)
{
    using namespace vshogi;
    CHECK_EQUAL(0b0000100001000010000000000u, BT::get_ray_to(SQ_2A, DIR_W));
    CHECK_EQUAL(0b0000000000000000000000001u, BT::get_ray_to(SQ_2A, DIR_E));
    CHECK_EQUAL(0b0000000000000001111000000u, BT::get_ray_to(SQ_2A, DIR_S));
    CHECK_EQUAL(0b0000000000000000000000010u, BT::get_ray_to(SQ_2A, DIR_SE));
    CHECK_EQUAL(0b0000000000000000000000000u, BT::get_ray_to(SQ_2A, DIR_NA));
    CHECK_EQUAL(0b0000000000000000000000000u, BT::get_ray_to(SQ_NA, DIR_SW));
}
TEST(test_minishogi_bitboard_traits, get_mask_between)
{
    CHECK_EQUAL(
        0b0000000000000000000000000u, BT::get_mask_between(SQ_1A, SQ_1B));
    CHECK_EQUAL(
        0b0000000000000000000001110u, BT::get_mask_between(SQ_1A, SQ_1E));
}
TEST(test_minishogi_bitboard_traits, get_placeable)
{
    CHECK_EQUAL(0b1111011110111101111011110u, BT::get_placeable(B_FU));
    CHECK_EQUAL(0b0111101111011110111101111u, BT::get_placeable(W_FU));
    CHECK_EQUAL(0b1111111111111111111111111u, BT::get_placeable(B_KI));
}
TEST(test_minishogi_bitboard_traits, get_attack_by)
{
    CHECK_EQUAL(0b0000000000000000000000001u, BT::get_attack_by(B_FU, SQ_1B));
    CHECK_EQUAL(0b0001000010000100001011101u, BT::get_attack_by(W_HI, SQ_1B));
}

TEST(test_minishogi_bitboard_traits, get_pre_reverse_attack)
{
    CHECK_EQUAL(
        0b0000000000000000001000100u, BT::get_pre_reverse_attack(SQ_1A, B_FU));
    CHECK_EQUAL(
        0b1010101011101010101010101u, BT::get_pre_reverse_attack(SQ_1E, B_KA));
    CHECK_EQUAL(
        0b0000000000111001110011100u, BT::get_pre_reverse_attack(SQ_1E, W_OU));
}

TEST(test_minishogi_bitboard_traits, iterator)
{
    auto it = BT::iterator(0b0000000000000000000011010u);
    CHECK_TRUE(it);
    CHECK_EQUAL(SQ_1B, *it);
    ++it;
    CHECK_TRUE(it);
    CHECK_EQUAL(SQ_1D, *it);
    ++it;
    CHECK_TRUE(it);
    CHECK_EQUAL(SQ_1E, *it);
    ++it;
    CHECK_FALSE(it);
    CHECK_FALSE(it != it.end());
}

} // namespace test_vshogi::test_minishogi
