#include "vshogi/minishogi/bitboard.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(bitboard){};

TEST(bitboard, fu)
{
    {
        const auto actual = get_attacks_by(B_FU, SQ_3A);
        CHECK_EQUAL(0, actual.hamming_weight());
    }
    {
        const auto actual = get_attacks_by(B_FU, SQ_4B);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
    }
    {
        const auto actual = get_attacks_by(W_FU, SQ_4B);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4C));
    }
}

TEST(bitboard, gi)
{
    {
        const auto actual = get_attacks_by(B_GI, SQ_2E);
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1D));
        CHECK_TRUE(actual.is_one(SQ_2D));
        CHECK_TRUE(actual.is_one(SQ_3D));
    }
    {
        const auto actual = get_attacks_by(W_GI, SQ_2E);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1D));
        CHECK_TRUE(actual.is_one(SQ_3D));
    }
    {
        const auto actual = get_attacks_by(W_GI, SQ_3A);
        CHECK_FALSE(actual.is_one(SQ_2D));
    }
}

TEST(bitboard, ki)
{
    {
        const auto actual = get_attacks_by(B_KI, SQ_5A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
        CHECK_TRUE(actual.is_one(SQ_5B));
    }
    {
        const auto actual = get_attacks_by(W_KI, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(bitboard, to)
{
    {
        const auto actual = get_attacks_by(B_TO, SQ_5A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
        CHECK_TRUE(actual.is_one(SQ_5B));
    }
    {
        const auto actual = get_attacks_by(W_TO, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(bitboard, ng)
{
    {
        const auto actual = get_attacks_by(B_NG, SQ_5A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
        CHECK_TRUE(actual.is_one(SQ_5B));
    }
    {
        const auto actual = get_attacks_by(W_NG, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(bitboard, ka)
{
    CHECK_EQUAL(8, get_attacks_by(B_KA, SQ_3C).hamming_weight());
    CHECK_TRUE((bb_2b | bb_1c | bb_4b | bb_5c) == get_attacks_by(W_KA, SQ_3A));
    CHECK_EQUAL(7, get_attacks_by(B_KA, SQ_3C, bb_2b).hamming_weight());
    CHECK_EQUAL(7, get_attacks_by(B_KA, SQ_3C, bb_2b | bb_5e).hamming_weight());
    CHECK_EQUAL(6, get_attacks_by(B_KA, SQ_3C, bb_2b | bb_4d).hamming_weight());
}

TEST(bitboard, um)
{
    CHECK_EQUAL(12, get_attacks_by(B_UM, SQ_3C).hamming_weight());
    CHECK_EQUAL(7, get_attacks_by(W_UM, SQ_3A).hamming_weight());
    CHECK_EQUAL(11, get_attacks_by(B_UM, SQ_3C, bb_2b).hamming_weight());
    CHECK_EQUAL(
        11, get_attacks_by(B_UM, SQ_3C, bb_2b | bb_5e).hamming_weight());
    CHECK_EQUAL(
        10, get_attacks_by(B_UM, SQ_3C, bb_2b | bb_4d).hamming_weight());
}

TEST(bitboard, hi)
{
    CHECK_EQUAL(8, get_attacks_by(B_HI, SQ_3C).hamming_weight());
    CHECK_EQUAL(8, get_attacks_by(W_HI, SQ_3A).hamming_weight());
    CHECK_EQUAL(6, get_attacks_by(B_HI, SQ_2B, bb_3b).hamming_weight());
}

TEST(bitboard, ry)
{
    CHECK_EQUAL(12, get_attacks_by(B_RY, SQ_3C).hamming_weight());
    CHECK_EQUAL(10, get_attacks_by(W_RY, SQ_3A).hamming_weight());
    CHECK_EQUAL(10, get_attacks_by(B_RY, SQ_2B, bb_3b).hamming_weight());
}

TEST(bitboard, ou)
{
    {
        const auto actual = get_attacks_by(B_OU, SQ_2B);
        CHECK_EQUAL(8, actual.hamming_weight());
    }
    {
        const auto actual = get_attacks_by(W_OU, SQ_5E);
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4D));
        CHECK_TRUE(actual.is_one(SQ_4E));
        CHECK_TRUE(actual.is_one(SQ_5D));
    }
}

} // namespace test_vshogi::test_minishogi
