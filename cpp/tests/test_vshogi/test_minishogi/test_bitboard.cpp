#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(minishogi_bitboard){};

TEST(minishogi_bitboard, xor_operator)
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

TEST(minishogi_bitboard, shift)
{
    for (auto dir :
         vshogi::EnumIterator<vshogi::DirectionEnum, Config::num_dir>()) {
        for (auto sq :
             vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
            CHECK_TRUE(
                BitBoard::from_square(Squares::shift(sq, dir))
                == BitBoard::from_square(sq).shift(dir));
        }
    }
}

TEST(minishogi_bitboard, is_one)
{
    CHECK_FALSE(bb_1a.is_one(SQ_NA));
    CHECK_TRUE(bb_1a.is_one(SQ_1A));
}

TEST(minishogi_bitboard, fu)
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

TEST(minishogi_bitboard, gi)
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

TEST(minishogi_bitboard, ki)
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

TEST(minishogi_bitboard, to)
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

TEST(minishogi_bitboard, ng)
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

TEST(minishogi_bitboard, ka)
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

TEST(minishogi_bitboard, um)
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

TEST(minishogi_bitboard, hi)
{
    CHECK_EQUAL(8, BitBoard::get_attacks_by(B_HI, SQ_3C).hamming_weight());
    CHECK_EQUAL(8, BitBoard::get_attacks_by(W_HI, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        6, BitBoard::get_attacks_by(B_HI, SQ_2B, bb_3b).hamming_weight());
}

TEST(minishogi_bitboard, ry)
{
    CHECK_EQUAL(12, BitBoard::get_attacks_by(B_RY, SQ_3C).hamming_weight());
    CHECK_EQUAL(10, BitBoard::get_attacks_by(W_RY, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        10, BitBoard::get_attacks_by(B_RY, SQ_2B, bb_3b).hamming_weight());
}

TEST(minishogi_bitboard, ou)
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

TEST(minishogi_bitboard, get_promotion_zone)
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
