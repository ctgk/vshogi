#include "vshogi/minishogi/bitboard.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(bitboard){};

TEST(bitboard, fu)
{
    {
        const auto actual = get_attacks_by(B_FU, SQ_31);
        CHECK_EQUAL(0, actual.hamming_weight());
    }
    {
        const auto actual = get_attacks_by(B_FU, SQ_42);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_41));
    }
    {
        const auto actual = get_attacks_by(W_FU, SQ_42);
        CHECK_EQUAL(1, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_43));
    }
}

TEST(bitboard, gi)
{
    {
        const auto actual = get_attacks_by(B_GI, SQ_25);
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_14));
        CHECK_TRUE(actual.is_one(SQ_24));
        CHECK_TRUE(actual.is_one(SQ_34));
    }
    {
        const auto actual = get_attacks_by(W_GI, SQ_25);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_14));
        CHECK_TRUE(actual.is_one(SQ_34));
    }
}

TEST(bitboard, ki)
{
    {
        const auto actual = get_attacks_by(B_KI, SQ_51);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_41));
        CHECK_TRUE(actual.is_one(SQ_52));
    }
    {
        const auto actual = get_attacks_by(W_KI, SQ_33);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(bitboard, to)
{
    {
        const auto actual = get_attacks_by(B_TO, SQ_51);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_41));
        CHECK_TRUE(actual.is_one(SQ_52));
    }
    {
        const auto actual = get_attacks_by(W_TO, SQ_33);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(bitboard, ng)
{
    {
        const auto actual = get_attacks_by(B_NG, SQ_51);
        CHECK_EQUAL(2, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_41));
        CHECK_TRUE(actual.is_one(SQ_52));
    }
    {
        const auto actual = get_attacks_by(W_NG, SQ_33);
        CHECK_EQUAL(6, actual.hamming_weight());
    }
}

TEST(bitboard, ka)
{
    CHECK_EQUAL(8, get_attacks_by(B_KA, SQ_33).hamming_weight());
    CHECK_TRUE((bb_22 | bb_13 | bb_42 | bb_53) == get_attacks_by(W_KA, SQ_31));
    CHECK_EQUAL(7, get_attacks_by(B_KA, SQ_33, bb_22).hamming_weight());
    CHECK_EQUAL(7, get_attacks_by(B_KA, SQ_33, bb_22 | bb_55).hamming_weight());
    CHECK_EQUAL(6, get_attacks_by(B_KA, SQ_33, bb_22 | bb_44).hamming_weight());
}

TEST(bitboard, um)
{
    CHECK_EQUAL(12, get_attacks_by(B_UM, SQ_33).hamming_weight());
    CHECK_EQUAL(7, get_attacks_by(W_UM, SQ_31).hamming_weight());
    CHECK_EQUAL(11, get_attacks_by(B_UM, SQ_33, bb_22).hamming_weight());
    CHECK_EQUAL(
        11, get_attacks_by(B_UM, SQ_33, bb_22 | bb_55).hamming_weight());
    CHECK_EQUAL(
        10, get_attacks_by(B_UM, SQ_33, bb_22 | bb_44).hamming_weight());
}

TEST(bitboard, hi)
{
    CHECK_EQUAL(8, get_attacks_by(B_HI, SQ_33).hamming_weight());
    CHECK_EQUAL(8, get_attacks_by(W_HI, SQ_31).hamming_weight());
    CHECK_EQUAL(6, get_attacks_by(B_HI, SQ_22, bb_32).hamming_weight());
}

TEST(bitboard, ry)
{
    CHECK_EQUAL(12, get_attacks_by(B_RY, SQ_33).hamming_weight());
    CHECK_EQUAL(10, get_attacks_by(W_RY, SQ_31).hamming_weight());
    CHECK_EQUAL(10, get_attacks_by(B_RY, SQ_22, bb_32).hamming_weight());
}

TEST(bitboard, ou)
{
    {
        const auto actual = get_attacks_by(B_OU, SQ_22);
        CHECK_EQUAL(8, actual.hamming_weight());
    }
    {
        const auto actual = get_attacks_by(W_OU, SQ_55);
        CHECK_EQUAL(3, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_44));
        CHECK_TRUE(actual.is_one(SQ_45));
        CHECK_TRUE(actual.is_one(SQ_54));
    }
}

} // namespace test_vshogi::test_minishogi
