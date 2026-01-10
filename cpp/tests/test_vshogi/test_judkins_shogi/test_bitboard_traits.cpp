#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;
using ST = vshogi::SquareTraits<Parameters>;
using BT = vshogi::BitboardTraits<Parameters>;

TEST_GROUP (test_judkins_shogi_bitboard_traits) {
};

TEST(test_judkins_shogi_bitboard_traits, shift)
{
    using namespace vshogi;
    CHECK_EQUAL(0u, BT::shift(1u, DIR_NW));
    CHECK_EQUAL(0u, BT::shift(0b100000u, DIR_SSW));
    CHECK_EQUAL(0b010001000000u, BT::shift(0b010001u, DIR_W));
    for (auto dir : Config::direction_iterator()) {
        for (auto sq : Config::square_iterator()) {
            const auto actual = BT::shift(BT::from_square(sq), dir);
            const auto expect = BT::from_square(ST::shift(sq, dir));
            CHECK_EQUAL(expect, actual);
        }
    }
}

TEST(test_judkins_shogi_bitboard_traits, get_ray_to)
{
    using namespace vshogi;
    CHECK_EQUAL(0u, BT::get_ray_to(SQ_NA, DIR_NA));
    CHECK_EQUAL(0u, BT::get_ray_to(SQ_1B, DIR_NA));
    CHECK_EQUAL(0u, BT::get_ray_to(SQ_NA, DIR_N));
    CHECK_EQUAL(1u, BT::get_ray_to(SQ_1B, DIR_N));
    CHECK_EQUAL(0b001000000000u, BT::get_ray_to(SQ_1B, DIR_SSW));
}

TEST(test_judkins_shogi_bitboard_traits, get_placeable)
{
    CHECK_EQUAL(
        0b111110111110111110111110111110111110u, BT::get_placeable(B_FU));
    CHECK_EQUAL(
        0b111100111100111100111100111100111100u, BT::get_placeable(B_KE));
    CHECK_EQUAL(
        0b111111111111111111111111111111111111u, BT::get_placeable(B_GI));
    CHECK_EQUAL(
        0b011111011111011111011111011111011111u, BT::get_placeable(W_FU));
    CHECK_EQUAL(
        0b001111001111001111001111001111001111u, BT::get_placeable(W_KE));
}

TEST(test_judkins_shogi_bitboard_traits, get_attack_by)
{
    CHECK_EQUAL(0u, BT::get_attack_by(B_FU, SQ_3A));
    CHECK_EQUAL(1u, BT::get_attack_by(B_FU, SQ_1B));
    CHECK_EQUAL(0b100u, BT::get_attack_by(W_FU, SQ_1B));
    CHECK_EQUAL(
        0b000100000000000100000000000000000000u,
        BT::get_attack_by(B_KE, SQ_5E));
    CHECK_EQUAL(
        0b000000000000000000000000000010000000u,
        BT::get_attack_by(B_KE, SQ_1D));
    CHECK_EQUAL(
        0b001000000000001000000000000000000000u,
        BT::get_attack_by(W_KE, SQ_5B));
    CHECK_EQUAL(0b101000001000101000u, BT::get_attack_by(B_GI, SQ_2E));
    CHECK_EQUAL(0b010000000000010000u, BT::get_attack_by(W_GI, SQ_2F));
    CHECK_EQUAL(0b000000000010000010u, BT::get_attack_by(W_GI, SQ_1A));
    CHECK_EQUAL(0b000000000001000010u, BT::get_attack_by(B_KI, SQ_1A));
    CHECK_EQUAL(0b000110000101000110u, BT::get_attack_by(W_KI, SQ_2B));
    CHECK_EQUAL(0b000000100000010000u, BT::get_attack_by(W_TO, SQ_1F));
    CHECK_EQUAL(0b000000011000101000u, BT::get_attack_by(B_TO, SQ_1E));
    CHECK_EQUAL(
        0b100000010001001010000000001010010001u,
        BT::get_attack_by(B_KA, SQ_3C));
    CHECK_EQUAL(
        0b100000010001001110001010001110010001u,
        BT::get_attack_by(W_UM, SQ_3C));
    CHECK_EQUAL(
        0b000010000010000010000010000010111101u,
        BT::get_attack_by(B_HI, SQ_1B));
    CHECK_EQUAL(
        0b000010000010000010000010000111111101u,
        BT::get_attack_by(W_RY, SQ_1B));
    CHECK_EQUAL(
        0b000000000000000000000000000111000101u,
        BT::get_attack_by(B_OU, SQ_1B));
}

TEST(test_judkins_shogi_bitboard_traits, get_promotion_zone)
{
    using namespace vshogi;
    CHECK_EQUAL(
        0b000011000011000011000011000011000011u, BT::promotion_zone(BLACK));
    CHECK_EQUAL(
        0b110000110000110000110000110000110000u, BT::promotion_zone(WHITE));
}

} // namespace test_vshogi::test_judkins_shogi
