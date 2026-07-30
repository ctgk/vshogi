#include "vshogi/common/bitboard_traits.hpp"
#include "vshogi/variants/leshogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_leshogi
{

using namespace vshogi::leshogi;
using BT = vshogi::BitboardTraits<Parameters>;

TEST_GROUP (test_leshogi_bitboard_traits) {
};

TEST(test_leshogi_bitboard_traits, test_from_square)
{
    CHECK_EQUAL(0b000000000000000000000001u, BT::from_square(SQ_1A));
    CHECK_EQUAL(0b000000000000000000000010u, BT::from_square(SQ_1B));
    CHECK_EQUAL(0b000000000000000000000100u, BT::from_square(SQ_1C));
    CHECK_EQUAL(0b000000000000000001000000u, BT::from_square(SQ_2A));
    CHECK_EQUAL(0b100000000000000000000000u, BT::from_square(SQ_4F));
    CHECK_EQUAL(0b000000000000000000000000u, BT::from_square(SQ_NA));
}

TEST(test_leshogi_bitboard_traits, test_from_rank)
{
    CHECK_EQUAL(0b000001000001000001000001u, BT::from_rank(RANK1));
    CHECK_EQUAL(0b000010000010000010000010u, BT::from_rank(RANK2));
    CHECK_EQUAL(0b000100000100000100000100u, BT::from_rank(RANK3));
    CHECK_EQUAL(0b001000001000001000001000u, BT::from_rank(RANK4));
    CHECK_EQUAL(0b010000010000010000010000u, BT::from_rank(RANK5));
    CHECK_EQUAL(0b100000100000100000100000u, BT::from_rank(RANK6));
}

TEST(test_leshogi_bitboard_traits, test_from_file)
{
    CHECK_EQUAL(0b000000000000000000111111u, BT::from_file(FILE1));
    CHECK_EQUAL(0b000000000000111111000000u, BT::from_file(FILE2));
    CHECK_EQUAL(0b000000111111000000000000u, BT::from_file(FILE3));
    CHECK_EQUAL(0b111111000000000000000000u, BT::from_file(FILE4));
}

TEST(test_leshogi_bitboard_traits, test_promotion_zone)
{
    using namespace vshogi;
    CHECK_EQUAL(0b000011000011000011000011u, BT::promotion_zone(BLACK));
    CHECK_EQUAL(0b110000110000110000110000u, BT::promotion_zone(WHITE));
}

TEST(test_leshogi_bitboard_traits, test_shift)
{
    using namespace vshogi;
    // clang-format off
    CHECK_EQUAL(  0b000000000000011111000000u,
        BT::shift(0b000000000000000000111111u, DIR_NW));
    CHECK_EQUAL(  0b000000000000000000000000u,
        BT::shift(0b000001000001000001000001u, DIR_N));
    CHECK_EQUAL(  0b000000000000000001000000u,
        BT::shift(0b000000000000000000000001u, DIR_W));
    CHECK_EQUAL(  0b000001000001000001000000u,
        BT::shift(0b000001000001000001000001u, DIR_W));
    CHECK_EQUAL(  0b000000000001000001000001u,
        BT::shift(0b000001000001000001000001u, DIR_E));
    CHECK_EQUAL(  0b000000111110000000000000u,
        BT::shift(0b000000000000111111000000u, DIR_SW));
    CHECK_EQUAL(  0b000000000000111110000000u,
        BT::shift(0b000000000000111111000000u, DIR_S));
    CHECK_EQUAL(  0b000000000000000000111110u,
        BT::shift(0b000000000000111111000000u, DIR_SE));
    // clang-format on
}

TEST(test_leshogi_bitboard_traits, test_is_one)
{
    CHECK_FALSE(BT::is_one(0b0000000000000001111100000u, SQ_1A));
    CHECK_TRUE(BT::is_one(0b0000000000000001111100000u, SQ_2A));
    CHECK_FALSE(BT::is_one(0b0000000000000001111100000u, SQ_NA));
}

TEST(test_leshogi_bitboard_traits, test_get_ray_to)
{
    using namespace vshogi;
    CHECK_EQUAL(0b000001000001000000000000u, BT::get_ray_to(SQ_2A, DIR_W));
    CHECK_EQUAL(0b000000000000000000000001u, BT::get_ray_to(SQ_2A, DIR_E));
    CHECK_EQUAL(0b000000000000111110000000u, BT::get_ray_to(SQ_2A, DIR_S));
    CHECK_EQUAL(0b000000000000000000000010u, BT::get_ray_to(SQ_2A, DIR_SE));
    CHECK_EQUAL(0b000000000000000000000000u, BT::get_ray_to(SQ_2A, DIR_NA));
    CHECK_EQUAL(0b000000000000000000000000u, BT::get_ray_to(SQ_NA, DIR_SW));
}

TEST(test_leshogi_bitboard_traits, test_get_mask_between)
{
    CHECK_EQUAL(
        0b000000000000000000000000u, BT::get_mask_between(SQ_1A, SQ_1B));
    CHECK_EQUAL(
        0b000000000000000000001110u, BT::get_mask_between(SQ_1A, SQ_1E));
}

TEST(test_leshogi_bitboard_traits, test_get_placeable)
{
    CHECK_EQUAL(0b111110111110111110111110u, BT::get_placeable(B_FU));
    CHECK_EQUAL(0b111100111100111100111100u, BT::get_placeable(B_KE));
    CHECK_EQUAL(0b001111001111001111001111u, BT::get_placeable(W_KE));
    CHECK_EQUAL(0b111111111111111111111111u, BT::get_placeable(B_KI));
}

TEST(test_leshogi_bitboard_traits, test_get_attack_by)
{
    CHECK_EQUAL(0b000000000000000000000001u, BT::get_attack_by(B_FU, SQ_1B));
    CHECK_EQUAL(0b000010000010000010111101u, BT::get_attack_by(W_HI, SQ_1B));
}

TEST(test_leshogi_bitboard_traits, test_get_pre_reverse_attack)
{
    CHECK_EQUAL(
        0b000000000000000110000100u, BT::get_pre_reverse_attack(SQ_1A, B_FU));
    CHECK_EQUAL(
        0b101011010111001010010101u, BT::get_pre_reverse_attack(SQ_1E, B_KA));
    CHECK_EQUAL(
        0b000000111000111000111000u, BT::get_pre_reverse_attack(SQ_1F, W_OU));
}

TEST(test_leshogi_bitboard_traits, test_iterator)
{
    auto it = BT::iterator(0b000000000000000000011010u);
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

} // namespace test_vshogi::test_leshogi
