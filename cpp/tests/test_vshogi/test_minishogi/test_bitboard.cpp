#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP (test_minishogi_bitboard) {
};

TEST(test_minishogi_bitboard, xor_operator)
{
    {
        const auto actual = (bb_1a | bb_2a) ^ (bb_2a | bb_3a);
        const auto expect = (bb_1a | bb_3a);
        CHECK_TRUE(expect == actual);
    }
    {
        auto actual = (bb_1a | bb_2a);
        actual ^= (bb_2a | bb_3a);
        const auto expect = (bb_1a | bb_3a);
        CHECK_TRUE(expect == actual);
    }
}

TEST(test_minishogi_bitboard, shift)
{
    CHECK_EQUAL(
        bb_na.value(),
        BitBoard::from_square(SQ_1A).shift(vshogi::DIR_NW).value());
    for (auto dir : Config::direction_iterator()) {
        for (auto sq : Config::square_iterator()) {
            CHECK_TRUE(
                BitBoard::from_square(Squares::shift(sq, dir))
                == BitBoard::from_square(sq).shift(dir));
        }
    }
}

TEST(test_minishogi_bitboard, is_one)
{
    CHECK_FALSE(bb_1a.is_one(SQ_NA));
    CHECK_TRUE(bb_1a.is_one(SQ_1A));
}

TEST(test_minishogi_bitboard, from_file)
{
    CHECK_EQUAL(
        bb_na.value(), BitBoard::from_file(Squares::to_file(SQ_NA)).value());
}

TEST(test_minishogi_bitboard, from_rank)
{
    CHECK_EQUAL(bb_ranka.value(), BitBoard::from_rank(Config::RANK_A).value());
    CHECK_EQUAL(bb_rankb.value(), BitBoard::from_rank(Config::RANK_B).value());
    CHECK_EQUAL(bb_rankd.value(), BitBoard::from_rank(Config::RANK_Y).value());
    CHECK_EQUAL(bb_ranke.value(), BitBoard::from_rank(Config::RANK_Z).value());
}

TEST(test_minishogi_bitboard, get_neighbor_2nd)
{
    {
        const auto actual = BitBoard::get_neighbor_2nd(SQ_NA, B_FU);
        CHECK_EQUAL(0, actual.value());
    }
    {
        const auto actual = BitBoard::get_neighbor_2nd(SQ_1A, B_KI);
        CHECK_EQUAL(9, actual.hamming_weight());
        CHECK_EQUAL(
            (bb_1a | bb_1b | bb_1c | bb_2a | bb_2b | bb_2c | bb_3a | bb_3b
             | bb_3c)
                .value(),
            actual.value());
    }
    {
        const auto actual = BitBoard::get_neighbor_2nd(SQ_3C, W_OU);
        CHECK_EQUAL((~BitBoard()).value(), actual.value());
    }
}

TEST(test_minishogi_bitboard, get_ray_to)
{
    {
        const auto actual = BitBoard::get_ray_to(SQ_NA, vshogi::DIR_NA);
        CHECK_EQUAL(0, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_ray_to(SQ_1B, vshogi::DIR_NA);
        CHECK_EQUAL(0, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_ray_to(SQ_NA, vshogi::DIR_N);
        CHECK_EQUAL(0, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_ray_to(SQ_1B, vshogi::DIR_N);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1A));
    }
}

TEST(test_minishogi_bitboard, fu)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_FU, SQ_3A);
        CHECK_EQUAL(0, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by(B_FU, SQ_4B);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_FU, SQ_4B);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4C));
    }
}

TEST(test_minishogi_bitboard, gi)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_GI, SQ_2E);
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1D));
        CHECK_TRUE(actual.is_one(SQ_2D));
        CHECK_TRUE(actual.is_one(SQ_3D));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_GI, SQ_2E);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1D));
        CHECK_TRUE(actual.is_one(SQ_3D));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_GI, SQ_3A);
        CHECK_FALSE(actual.is_one(SQ_2D));
    }
}

TEST(test_minishogi_bitboard, ki)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_KI, SQ_5A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
        CHECK_TRUE(actual.is_one(SQ_5B));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_KI, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(test_minishogi_bitboard, to)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_TO, SQ_5A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
        CHECK_TRUE(actual.is_one(SQ_5B));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_TO, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(test_minishogi_bitboard, ng)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_NG, SQ_5A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
        CHECK_TRUE(actual.is_one(SQ_5B));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_NG, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(test_minishogi_bitboard, ka)
{
    CHECK_EQUAL(8, BitBoard::get_attacks_by(B_KA, SQ_3C).hamming_weight());
    CHECK_TRUE(
        (bb_2b | bb_1c | bb_4b | bb_5c)
        == BitBoard::get_attacks_by(W_KA, SQ_3A));
    CHECK_EQUAL(
        7, BitBoard::get_attacks_by(B_KA, SQ_3C, bb_2b).hamming_weight());
    CHECK_EQUAL(
        7,
        BitBoard::get_attacks_by(B_KA, SQ_3C, bb_2b | bb_5e).hamming_weight());
    CHECK_EQUAL(
        6,
        BitBoard::get_attacks_by(B_KA, SQ_3C, bb_2b | bb_4d).hamming_weight());
}

TEST(test_minishogi_bitboard, um)
{
    CHECK_EQUAL(12, BitBoard::get_attacks_by(B_UM, SQ_3C).hamming_weight());
    CHECK_EQUAL(7, BitBoard::get_attacks_by(W_UM, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        11, BitBoard::get_attacks_by(B_UM, SQ_3C, bb_2b).hamming_weight());
    CHECK_EQUAL(
        11,
        BitBoard::get_attacks_by(B_UM, SQ_3C, bb_2b | bb_5e).hamming_weight());
    CHECK_EQUAL(
        10,
        BitBoard::get_attacks_by(B_UM, SQ_3C, bb_2b | bb_4d).hamming_weight());
}

TEST(test_minishogi_bitboard, hi)
{
    CHECK_EQUAL(8, BitBoard::get_attacks_by(B_HI, SQ_3C).hamming_weight());
    CHECK_EQUAL(8, BitBoard::get_attacks_by(W_HI, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        6, BitBoard::get_attacks_by(B_HI, SQ_2B, bb_3b).hamming_weight());
}

TEST(test_minishogi_bitboard, ry)
{
    CHECK_EQUAL(12, BitBoard::get_attacks_by(B_RY, SQ_3C).hamming_weight());
    CHECK_EQUAL(10, BitBoard::get_attacks_by(W_RY, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        10, BitBoard::get_attacks_by(B_RY, SQ_2B, bb_3b).hamming_weight());
}

TEST(test_minishogi_bitboard, ou)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_OU, SQ_2B);
        CHECK_EQUAL(8, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_OU, SQ_5E);
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4D));
        CHECK_TRUE(actual.is_one(SQ_4E));
        CHECK_TRUE(actual.is_one(SQ_5D));
    }
}

TEST(test_minishogi_bitboard, get_promotion_zone)
{
    {
        const auto actual = BitBoard::get_promotion_zone(vshogi::BLACK);
        CHECK_TRUE(bb_ranka == actual);
    }
    {
        const auto actual = BitBoard::get_promotion_zone(vshogi::WHITE);
        CHECK_TRUE(bb_ranke == actual);
    }
}

} // namespace test_vshogi::test_minishogi
