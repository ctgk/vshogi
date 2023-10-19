#include "vshogi/judkins_shogi/bitboard.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_judkins_shogi/test_judkins_shogi.hpp"

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP(bitboard){};

TEST(bitboard, fu)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_FU, SQ_3A);
        CHECK_EQUAL(0, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_FU, SQ_3A);
        CHECK_EQUAL(SQ_NA, actual[0]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(B_FU, SQ_4B);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_FU, SQ_4B);
        CHECK_EQUAL(SQ_4A, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_FU, SQ_4B);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4C));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_FU, SQ_4B);
        CHECK_EQUAL(SQ_4C, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
}

TEST(bitboard, ke)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_KE, SQ_5E);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4C));
        CHECK_TRUE(actual.is_one(SQ_6C));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_KE, SQ_5E);
        CHECK_EQUAL(SQ_6C, actual[0]);
        CHECK_EQUAL(SQ_4C, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(B_KE, SQ_1D);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2B));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_KE, SQ_1D);
        CHECK_EQUAL(SQ_2B, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_KE, SQ_5B);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4D));
        CHECK_TRUE(actual.is_one(SQ_6D));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_KE, SQ_5B);
        CHECK_EQUAL(SQ_6D, actual[0]);
        CHECK_EQUAL(SQ_4D, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
}

TEST(bitboard, gi)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_GI, SQ_2E);
        CHECK_TRUE((bb_3d | bb_2d | bb_1d | bb_3f | bb_1f) == actual);
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_GI, SQ_2E);
        CHECK_EQUAL(SQ_3D, actual[0]);
        CHECK_EQUAL(SQ_2D, actual[1]);
        CHECK_EQUAL(SQ_1D, actual[2]);
        CHECK_EQUAL(SQ_3F, actual[3]);
        CHECK_EQUAL(SQ_1F, actual[4]);
        CHECK_EQUAL(SQ_NA, actual[5]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_GI, SQ_2F);
        CHECK_TRUE((bb_3e | bb_1e) == actual);
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_GI, SQ_2F);
        CHECK_EQUAL(SQ_3E, actual[0]);
        CHECK_EQUAL(SQ_1E, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_GI, SQ_3A);
        CHECK_FALSE(actual.is_one(SQ_2D));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_GI, SQ_3A);
        CHECK_EQUAL(SQ_4B, actual[0]);
        CHECK_EQUAL(SQ_3B, actual[1]);
        CHECK_EQUAL(SQ_2B, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
    }
}

TEST(bitboard, ki)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_KI, SQ_6A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_5A));
        CHECK_TRUE(actual.is_one(SQ_6B));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_KI, SQ_6A);
        CHECK_EQUAL(SQ_5A, actual[0]);
        CHECK_EQUAL(SQ_6B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_KI, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_KI, SQ_3C);
        CHECK_EQUAL(SQ_3B, actual[0]);
        CHECK_EQUAL(SQ_4C, actual[1]);
        CHECK_EQUAL(SQ_2C, actual[2]);
        CHECK_EQUAL(SQ_4D, actual[3]);
        CHECK_EQUAL(SQ_3D, actual[4]);
        CHECK_EQUAL(SQ_2D, actual[5]);
        CHECK_EQUAL(SQ_NA, actual[6]);
    }
}

TEST(bitboard, to)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_TO, SQ_6A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_5A));
        CHECK_TRUE(actual.is_one(SQ_6B));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_TO, SQ_6A);
        CHECK_EQUAL(SQ_5A, actual[0]);
        CHECK_EQUAL(SQ_6B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_TO, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_TO, SQ_3C);
        CHECK_EQUAL(SQ_3B, actual[0]);
        CHECK_EQUAL(SQ_4C, actual[1]);
        CHECK_EQUAL(SQ_2C, actual[2]);
        CHECK_EQUAL(SQ_4D, actual[3]);
        CHECK_EQUAL(SQ_3D, actual[4]);
        CHECK_EQUAL(SQ_2D, actual[5]);
        CHECK_EQUAL(SQ_NA, actual[6]);
    }
}

TEST(bitboard, nk)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_NK, SQ_6A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_5A));
        CHECK_TRUE(actual.is_one(SQ_6B));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_NK, SQ_6A);
        CHECK_EQUAL(SQ_5A, actual[0]);
        CHECK_EQUAL(SQ_6B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_NK, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_NK, SQ_3C);
        CHECK_EQUAL(SQ_3B, actual[0]);
        CHECK_EQUAL(SQ_4C, actual[1]);
        CHECK_EQUAL(SQ_2C, actual[2]);
        CHECK_EQUAL(SQ_4D, actual[3]);
        CHECK_EQUAL(SQ_3D, actual[4]);
        CHECK_EQUAL(SQ_2D, actual[5]);
        CHECK_EQUAL(SQ_NA, actual[6]);
    }
}

TEST(bitboard, ng)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_NG, SQ_6A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_5A));
        CHECK_TRUE(actual.is_one(SQ_6B));
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_NG, SQ_6A);
        CHECK_EQUAL(SQ_5A, actual[0]);
        CHECK_EQUAL(SQ_6B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_NG, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_NG, SQ_3C);
        CHECK_EQUAL(SQ_3B, actual[0]);
        CHECK_EQUAL(SQ_4C, actual[1]);
        CHECK_EQUAL(SQ_2C, actual[2]);
        CHECK_EQUAL(SQ_4D, actual[3]);
        CHECK_EQUAL(SQ_3D, actual[4]);
        CHECK_EQUAL(SQ_2D, actual[5]);
        CHECK_EQUAL(SQ_NA, actual[6]);
    }
}

TEST(bitboard, ka)
{
    CHECK_EQUAL(9, BitBoard::get_attacks_by(B_KA, SQ_3C).hamming_weight());
    CHECK_TRUE(
        (bb_2b | bb_1c | bb_4b | bb_5c | bb_6d)
        == BitBoard::get_attacks_by(W_KA, SQ_3A));
    CHECK_EQUAL(
        8, BitBoard::get_attacks_by(B_KA, SQ_3C, bb_2b).hamming_weight());
    CHECK_EQUAL(
        7,
        BitBoard::get_attacks_by(B_KA, SQ_3C, bb_2b | bb_5e).hamming_weight());
    CHECK_EQUAL(
        6,
        BitBoard::get_attacks_by(B_KA, SQ_3C, bb_2b | bb_4d).hamming_weight());
    CHECK_TRUE(nullptr == BitBoard::get_attacks_by_non_ranging(B_KA, SQ_3C));
}

TEST(bitboard, um)
{
    CHECK_EQUAL(13, BitBoard::get_attacks_by(B_UM, SQ_3C).hamming_weight());
    CHECK_EQUAL(8, BitBoard::get_attacks_by(W_UM, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        12, BitBoard::get_attacks_by(B_UM, SQ_3C, bb_2b).hamming_weight());
    CHECK_EQUAL(
        11,
        BitBoard::get_attacks_by(B_UM, SQ_3C, bb_2b | bb_5e).hamming_weight());
    CHECK_EQUAL(
        10,
        BitBoard::get_attacks_by(B_UM, SQ_3C, bb_2b | bb_4d).hamming_weight());
    CHECK_TRUE(nullptr == BitBoard::get_attacks_by_non_ranging(W_UM, SQ_3A));
}

TEST(bitboard, hi)
{
    CHECK_EQUAL(10, BitBoard::get_attacks_by(B_HI, SQ_3C).hamming_weight());
    CHECK_EQUAL(10, BitBoard::get_attacks_by(W_HI, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        7, BitBoard::get_attacks_by(B_HI, SQ_2B, bb_3b).hamming_weight());
    CHECK_TRUE(nullptr == BitBoard::get_attacks_by_non_ranging(W_HI, SQ_3A));
}

TEST(bitboard, ry)
{
    CHECK_EQUAL(14, BitBoard::get_attacks_by(B_RY, SQ_3C).hamming_weight());
    CHECK_EQUAL(12, BitBoard::get_attacks_by(W_RY, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        11, BitBoard::get_attacks_by(B_RY, SQ_2B, bb_3b).hamming_weight());
    CHECK_TRUE(nullptr == BitBoard::get_attacks_by_non_ranging(B_RY, SQ_3C));
}

TEST(bitboard, ou)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_OU, SQ_2B);
        CHECK_EQUAL(8, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(B_OU, SQ_2B);
        CHECK_EQUAL(SQ_3A, actual[0]);
        CHECK_EQUAL(SQ_2A, actual[1]);
        CHECK_EQUAL(SQ_1A, actual[2]);
        CHECK_EQUAL(SQ_3B, actual[3]);
        CHECK_EQUAL(SQ_1B, actual[4]);
        CHECK_EQUAL(SQ_3C, actual[5]);
        CHECK_EQUAL(SQ_2C, actual[6]);
        CHECK_EQUAL(SQ_1C, actual[7]);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_OU, SQ_6F);
        CHECK_TRUE((bb_6e | bb_5e | bb_5f) == actual);
    }
    {
        const auto actual = BitBoard::get_attacks_by_non_ranging(W_OU, SQ_6F);
        CHECK_EQUAL(SQ_6E, actual[0]);
        CHECK_EQUAL(SQ_5E, actual[1]);
        CHECK_EQUAL(SQ_5F, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
    }
}

} // namespace test_vshogi::test_judkins_shogi
