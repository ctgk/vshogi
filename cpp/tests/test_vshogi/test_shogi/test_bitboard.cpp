#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(bitboard){};

TEST(bitboard, from_square)
{
    {
        const auto bb = BitBoard::from_square(SQ_9H);
        CHECK_EQUAL(1, bb.hamming_weight());
        CHECK_TRUE(bb.is_one(SQ_9H));
    }
    {
        const auto bb = BitBoard::from_square(SQ_8H);
        CHECK_EQUAL(1, bb.hamming_weight());
        CHECK_TRUE(bb.is_one(SQ_8H));
    }
}

TEST(bitboard, bitshift)
{
    {
        const auto bb = BitBoard::from_square(SQ_9H) << 1;
        CHECK_EQUAL(1, bb.hamming_weight());
        CHECK_TRUE(bb.is_one(SQ_8H));
    }
}

TEST(bitboard, shift)
{
    {
        const auto bb = BitBoard::from_square(SQ_9A);
        const auto actual = bb.shift<vshogi::DIR_E>();
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_8A));
    }
    {
        const auto bb = BitBoard::from_square(SQ_9A);
        const auto actual = bb.shift<vshogi::DIR_SSE>();
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_8C));
    }
    {
        const auto bb = BitBoard::from_square(SQ_1A);
        const auto actual = bb.shift<vshogi::DIR_E>();
        CHECK_FALSE(actual.any());
    }
}

TEST(bitboard, get_attacks_by_fu)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_FU, SQ_2G);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2F));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_FU, SQ_2G);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2H));
    }
}

TEST(bitboard, get_attacks_by_ky)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_KY, SQ_1I);
        CHECK_EQUAL(8, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1A));
        CHECK_TRUE(actual.is_one(SQ_1B));
        CHECK_TRUE(actual.is_one(SQ_1C));
        CHECK_TRUE(actual.is_one(SQ_1D));
        CHECK_TRUE(actual.is_one(SQ_1E));
        CHECK_TRUE(actual.is_one(SQ_1F));
        CHECK_TRUE(actual.is_one(SQ_1G));
        CHECK_TRUE(actual.is_one(SQ_1H));
    }
    {
        const auto actual = BitBoard::get_attacks_by(
            B_KY, SQ_1I, BitBoard::from_square(SQ_1G));
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_FALSE(actual.is_one(SQ_1F));
        CHECK_TRUE(actual.is_one(SQ_1G));
        CHECK_TRUE(actual.is_one(SQ_1H));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_KY, SQ_9A);
        CHECK_EQUAL(8, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_9B));
        CHECK_TRUE(actual.is_one(SQ_9C));
        CHECK_TRUE(actual.is_one(SQ_9D));
        CHECK_TRUE(actual.is_one(SQ_9E));
        CHECK_TRUE(actual.is_one(SQ_9F));
        CHECK_TRUE(actual.is_one(SQ_9G));
        CHECK_TRUE(actual.is_one(SQ_9H));
        CHECK_TRUE(actual.is_one(SQ_9I));
    }
    {
        const auto actual = BitBoard::get_attacks_by(
            W_KY, SQ_9A, BitBoard::from_square(SQ_9F));
        CHECK_EQUAL(5, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_9B));
        CHECK_TRUE(actual.is_one(SQ_9C));
        CHECK_TRUE(actual.is_one(SQ_9D));
        CHECK_TRUE(actual.is_one(SQ_9E));
        CHECK_TRUE(actual.is_one(SQ_9F));
        CHECK_FALSE(actual.is_one(SQ_9G));
    }
}

TEST(bitboard, get_attacks_by_ke)
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
        const auto actual = BitBoard::get_attacks_by(W_KE, SQ_5E);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4G));
        CHECK_TRUE(actual.is_one(SQ_6G));
    }
}

TEST(bitboard, get_attacks_by_gi)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_GI, SQ_9D);
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_8C));
        CHECK_TRUE(actual.is_one(SQ_9C));
        CHECK_TRUE(actual.is_one(SQ_8E));
    }
    {
        const auto actual = BitBoard::get_attacks_by(B_GI, SQ_1A);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2B));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_GI, SQ_1A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1B));
        CHECK_TRUE(actual.is_one(SQ_2B));
    }
}

TEST(bitboard, get_attacks_by_ki)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_KI, SQ_9D);
        CHECK_EQUAL(4, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_8C));
        CHECK_TRUE(actual.is_one(SQ_9C));
        CHECK_TRUE(actual.is_one(SQ_8D));
        CHECK_TRUE(actual.is_one(SQ_9E));
    }
    {
        const auto actual = BitBoard::get_attacks_by(B_NY, SQ_1A);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2A));
        CHECK_TRUE(actual.is_one(SQ_1B));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_TO, SQ_1A);
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1B));
        CHECK_TRUE(actual.is_one(SQ_2B));
        CHECK_TRUE(actual.is_one(SQ_2A));
    }
}

TEST(bitboard, get_attacks_by_ka)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_KA, SQ_5E);
        CHECK_EQUAL(16, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1A));
        CHECK_TRUE(actual.is_one(SQ_2B));
        CHECK_TRUE(actual.is_one(SQ_3C));
        CHECK_TRUE(actual.is_one(SQ_4D));
        CHECK_TRUE(actual.is_one(SQ_9A));
        CHECK_TRUE(actual.is_one(SQ_8B));
        CHECK_TRUE(actual.is_one(SQ_7C));
        CHECK_TRUE(actual.is_one(SQ_6D));
        CHECK_TRUE(actual.is_one(SQ_1I));
        CHECK_TRUE(actual.is_one(SQ_2H));
        CHECK_TRUE(actual.is_one(SQ_3G));
        CHECK_TRUE(actual.is_one(SQ_4F));
        CHECK_TRUE(actual.is_one(SQ_9I));
        CHECK_TRUE(actual.is_one(SQ_8H));
        CHECK_TRUE(actual.is_one(SQ_7G));
        CHECK_TRUE(actual.is_one(SQ_6F));
    }
    {
        const auto actual = BitBoard::get_attacks_by(
            W_KA, SQ_1B, BitBoard::from_square(SQ_3D));
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2A));
        CHECK_TRUE(actual.is_one(SQ_2C));
        CHECK_TRUE(actual.is_one(SQ_3D));
        CHECK_FALSE(actual.is_one(SQ_4E));
    }
}

TEST(bitboard, get_attacks_by_hi)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_HI, SQ_5E);
        CHECK_EQUAL(16, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_5A));
        CHECK_TRUE(actual.is_one(SQ_5B));
        CHECK_TRUE(actual.is_one(SQ_5C));
        CHECK_TRUE(actual.is_one(SQ_5D));
        CHECK_TRUE(actual.is_one(SQ_5F));
        CHECK_TRUE(actual.is_one(SQ_5G));
        CHECK_TRUE(actual.is_one(SQ_5H));
        CHECK_TRUE(actual.is_one(SQ_5I));
        CHECK_TRUE(actual.is_one(SQ_1E));
        CHECK_TRUE(actual.is_one(SQ_2E));
        CHECK_TRUE(actual.is_one(SQ_3E));
        CHECK_TRUE(actual.is_one(SQ_4E));
        CHECK_TRUE(actual.is_one(SQ_6E));
        CHECK_TRUE(actual.is_one(SQ_7E));
        CHECK_TRUE(actual.is_one(SQ_8E));
        CHECK_TRUE(actual.is_one(SQ_9E));
    }
    {
        const auto actual = BitBoard::get_attacks_by(
            W_HI,
            SQ_5E,
            BitBoard::from_square(SQ_5C) | BitBoard::from_square(SQ_6E)
                | BitBoard::from_square(SQ_1E));
        CHECK_EQUAL(11, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_5C));
        CHECK_TRUE(actual.is_one(SQ_5D));
        CHECK_TRUE(actual.is_one(SQ_5F));
        CHECK_TRUE(actual.is_one(SQ_5G));
        CHECK_TRUE(actual.is_one(SQ_5H));
        CHECK_TRUE(actual.is_one(SQ_5I));
        CHECK_TRUE(actual.is_one(SQ_1E));
        CHECK_TRUE(actual.is_one(SQ_2E));
        CHECK_TRUE(actual.is_one(SQ_3E));
        CHECK_TRUE(actual.is_one(SQ_4E));
        CHECK_TRUE(actual.is_one(SQ_6E));
    }
}

TEST(bitboard, get_attacks_by_um)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_UM, SQ_5E);
        CHECK_EQUAL(20, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1A));
        CHECK_TRUE(actual.is_one(SQ_2B));
        CHECK_TRUE(actual.is_one(SQ_3C));
        CHECK_TRUE(actual.is_one(SQ_4D));
        CHECK_TRUE(actual.is_one(SQ_9A));
        CHECK_TRUE(actual.is_one(SQ_8B));
        CHECK_TRUE(actual.is_one(SQ_7C));
        CHECK_TRUE(actual.is_one(SQ_6D));
        CHECK_TRUE(actual.is_one(SQ_1I));
        CHECK_TRUE(actual.is_one(SQ_2H));
        CHECK_TRUE(actual.is_one(SQ_3G));
        CHECK_TRUE(actual.is_one(SQ_4F));
        CHECK_TRUE(actual.is_one(SQ_9I));
        CHECK_TRUE(actual.is_one(SQ_8H));
        CHECK_TRUE(actual.is_one(SQ_7G));
        CHECK_TRUE(actual.is_one(SQ_6F));
        CHECK_TRUE(actual.is_one(SQ_5D));
        CHECK_TRUE(actual.is_one(SQ_4E));
        CHECK_TRUE(actual.is_one(SQ_6E));
        CHECK_TRUE(actual.is_one(SQ_5F));
    }
    {
        const auto actual = BitBoard::get_attacks_by(
            W_UM, SQ_1B, BitBoard::from_square(SQ_3D));
        CHECK_EQUAL(6, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2A));
        CHECK_TRUE(actual.is_one(SQ_2C));
        CHECK_TRUE(actual.is_one(SQ_3D));
        CHECK_FALSE(actual.is_one(SQ_4E));
        CHECK_TRUE(actual.is_one(SQ_1A));
        CHECK_TRUE(actual.is_one(SQ_2B));
        CHECK_TRUE(actual.is_one(SQ_1C));
    }
}

TEST(bitboard, get_attacks_by_ry)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_RY, SQ_5E);
        CHECK_EQUAL(20, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_5A));
        CHECK_TRUE(actual.is_one(SQ_5B));
        CHECK_TRUE(actual.is_one(SQ_5C));
        CHECK_TRUE(actual.is_one(SQ_5D));
        CHECK_TRUE(actual.is_one(SQ_5F));
        CHECK_TRUE(actual.is_one(SQ_5G));
        CHECK_TRUE(actual.is_one(SQ_5H));
        CHECK_TRUE(actual.is_one(SQ_5I));
        CHECK_TRUE(actual.is_one(SQ_1E));
        CHECK_TRUE(actual.is_one(SQ_2E));
        CHECK_TRUE(actual.is_one(SQ_3E));
        CHECK_TRUE(actual.is_one(SQ_4E));
        CHECK_TRUE(actual.is_one(SQ_6E));
        CHECK_TRUE(actual.is_one(SQ_7E));
        CHECK_TRUE(actual.is_one(SQ_8E));
        CHECK_TRUE(actual.is_one(SQ_9E));
        CHECK_TRUE(actual.is_one(SQ_4D));
        CHECK_TRUE(actual.is_one(SQ_6D));
        CHECK_TRUE(actual.is_one(SQ_4F));
        CHECK_TRUE(actual.is_one(SQ_6F));
    }
    {
        const auto actual = BitBoard::get_attacks_by(
            W_RY,
            SQ_5E,
            BitBoard::from_square(SQ_5C) | BitBoard::from_square(SQ_6E)
                | BitBoard::from_square(SQ_1E));
        CHECK_EQUAL(15, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_5C));
        CHECK_TRUE(actual.is_one(SQ_5D));
        CHECK_TRUE(actual.is_one(SQ_5F));
        CHECK_TRUE(actual.is_one(SQ_5G));
        CHECK_TRUE(actual.is_one(SQ_5H));
        CHECK_TRUE(actual.is_one(SQ_5I));
        CHECK_TRUE(actual.is_one(SQ_1E));
        CHECK_TRUE(actual.is_one(SQ_2E));
        CHECK_TRUE(actual.is_one(SQ_3E));
        CHECK_TRUE(actual.is_one(SQ_4E));
        CHECK_TRUE(actual.is_one(SQ_6E));
        CHECK_TRUE(actual.is_one(SQ_4D));
        CHECK_TRUE(actual.is_one(SQ_6D));
        CHECK_TRUE(actual.is_one(SQ_4F));
        CHECK_TRUE(actual.is_one(SQ_6F));
    }
}

TEST(bitboard, get_attacks_by_ou)
{
    {
        const auto actual = BitBoard::get_attacks_by(B_OU, SQ_5A);
        CHECK_EQUAL(5, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_4A));
        CHECK_TRUE(actual.is_one(SQ_6A));
        CHECK_TRUE(actual.is_one(SQ_4B));
        CHECK_TRUE(actual.is_one(SQ_5B));
        CHECK_TRUE(actual.is_one(SQ_6B));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_OU, SQ_8H);
        CHECK_EQUAL(8, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_7G));
        CHECK_TRUE(actual.is_one(SQ_8G));
        CHECK_TRUE(actual.is_one(SQ_9G));
        CHECK_TRUE(actual.is_one(SQ_7H));
        CHECK_TRUE(actual.is_one(SQ_9H));
        CHECK_TRUE(actual.is_one(SQ_7I));
        CHECK_TRUE(actual.is_one(SQ_8I));
        CHECK_TRUE(actual.is_one(SQ_9I));
    }
    {
        const auto actual = BitBoard::get_attacks_by(W_OU, SQ_1H);
        CHECK_EQUAL(5, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_2G));
        CHECK_TRUE(actual.is_one(SQ_1G));
        CHECK_TRUE(actual.is_one(SQ_2H));
        CHECK_TRUE(actual.is_one(SQ_2I));
        CHECK_TRUE(actual.is_one(SQ_1I));
    }
}

} // namespace test_vshogi::test_shogi
