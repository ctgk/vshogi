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

TEST(bitboard, ke)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_KE, SQ_5E);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4C));
        CHECK_TRUE(actual.is_one(SQ_6C));
    }
    {
        const auto actual = BitBoard::get_attacks_by(B_KE, SQ_1D);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2B));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_KE, SQ_5B);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4D));
        CHECK_TRUE(actual.is_one(SQ_6D));
    }
}

TEST(bitboard, gi)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_GI, SQ_2E);
        CHECK_TRUE((bb_3d | bb_2d | bb_1d | bb_3f | bb_1f) == actual);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_GI, SQ_2F);
        CHECK_TRUE((bb_3e | bb_1e) == actual);
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_GI, SQ_3A);
        CHECK_FALSE(actual.is_one(SQ_2D));
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
        const auto actual = BitBoard::get_attacks_by(W_KI, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
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
        const auto actual = BitBoard::get_attacks_by(W_TO, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
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
        const auto actual = BitBoard::get_attacks_by(W_NK, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
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
        const auto actual = BitBoard::get_attacks_by(W_NG, SQ_3C);
        CHECK_EQUAL(6, actual.hamming_weight());
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
    CHECK_TRUE(
        (bb_3a | bb_4b | bb_5c | bb_5e | bb_4f)
        == BitBoard::get_attacks_by(W_KA, SQ_6D));
    CHECK_TRUE(BitBoard::get_attacks_by(B_KA, SQ_6E).is_one(SQ_2A));
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
}

TEST(bitboard, hi)
{
    CHECK_EQUAL(10, BitBoard::get_attacks_by(B_HI, SQ_3C).hamming_weight());
    CHECK_EQUAL(10, BitBoard::get_attacks_by(W_HI, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        7, BitBoard::get_attacks_by(B_HI, SQ_2B, bb_3b).hamming_weight());
}

TEST(bitboard, ry)
{
    CHECK_EQUAL(14, BitBoard::get_attacks_by(B_RY, SQ_3C).hamming_weight());
    CHECK_EQUAL(12, BitBoard::get_attacks_by(W_RY, SQ_3A).hamming_weight());
    CHECK_EQUAL(
        11, BitBoard::get_attacks_by(B_RY, SQ_2B, bb_3b).hamming_weight());
}

TEST(bitboard, ou)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_OU, SQ_2B);
        CHECK_EQUAL(8, actual.hamming_weight());
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_OU, SQ_6F);
        CHECK_TRUE((bb_6e | bb_5e | bb_5f) == actual);
    }
}

} // namespace test_vshogi::test_judkins_shogi
