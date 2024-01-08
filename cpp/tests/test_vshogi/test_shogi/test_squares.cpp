#include "vshogi/shogi/squares.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_shogi/test_shogi.hpp"

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(squares){};

TEST(squares, to_rank)
{
    CHECK_EQUAL(RANK1, to_rank(SQ_1A));
    CHECK_EQUAL(RANK1, to_rank(SQ_2A));
    CHECK_EQUAL(RANK1, to_rank(SQ_3A));
    CHECK_EQUAL(RANK1, to_rank(SQ_4A));
    CHECK_EQUAL(RANK1, to_rank(SQ_5A));
    CHECK_EQUAL(RANK1, to_rank(SQ_6A));
    CHECK_EQUAL(RANK1, to_rank(SQ_7A));
    CHECK_EQUAL(RANK1, to_rank(SQ_8A));
    CHECK_EQUAL(RANK1, to_rank(SQ_9A));
    CHECK_EQUAL(RANK2, to_rank(SQ_1B));
    CHECK_EQUAL(RANK2, to_rank(SQ_2B));
    CHECK_EQUAL(RANK2, to_rank(SQ_3B));
    CHECK_EQUAL(RANK2, to_rank(SQ_4B));
    CHECK_EQUAL(RANK2, to_rank(SQ_5B));
    CHECK_EQUAL(RANK2, to_rank(SQ_6B));
    CHECK_EQUAL(RANK2, to_rank(SQ_7B));
    CHECK_EQUAL(RANK2, to_rank(SQ_8B));
    CHECK_EQUAL(RANK2, to_rank(SQ_9B));
    CHECK_EQUAL(RANK3, to_rank(SQ_1C));
    CHECK_EQUAL(RANK3, to_rank(SQ_2C));
    CHECK_EQUAL(RANK3, to_rank(SQ_3C));
    CHECK_EQUAL(RANK3, to_rank(SQ_4C));
    CHECK_EQUAL(RANK3, to_rank(SQ_5C));
    CHECK_EQUAL(RANK3, to_rank(SQ_6C));
    CHECK_EQUAL(RANK3, to_rank(SQ_7C));
    CHECK_EQUAL(RANK3, to_rank(SQ_8C));
    CHECK_EQUAL(RANK3, to_rank(SQ_9C));
    CHECK_EQUAL(RANK4, to_rank(SQ_1D));
    CHECK_EQUAL(RANK4, to_rank(SQ_2D));
    CHECK_EQUAL(RANK4, to_rank(SQ_3D));
    CHECK_EQUAL(RANK4, to_rank(SQ_4D));
    CHECK_EQUAL(RANK4, to_rank(SQ_5D));
    CHECK_EQUAL(RANK4, to_rank(SQ_6D));
    CHECK_EQUAL(RANK4, to_rank(SQ_7D));
    CHECK_EQUAL(RANK4, to_rank(SQ_8D));
    CHECK_EQUAL(RANK4, to_rank(SQ_9D));
    CHECK_EQUAL(RANK5, to_rank(SQ_1E));
    CHECK_EQUAL(RANK5, to_rank(SQ_2E));
    CHECK_EQUAL(RANK5, to_rank(SQ_3E));
    CHECK_EQUAL(RANK5, to_rank(SQ_4E));
    CHECK_EQUAL(RANK5, to_rank(SQ_5E));
    CHECK_EQUAL(RANK5, to_rank(SQ_6E));
    CHECK_EQUAL(RANK5, to_rank(SQ_7E));
    CHECK_EQUAL(RANK5, to_rank(SQ_8E));
    CHECK_EQUAL(RANK5, to_rank(SQ_9E));
    CHECK_EQUAL(RANK6, to_rank(SQ_1F));
    CHECK_EQUAL(RANK6, to_rank(SQ_2F));
    CHECK_EQUAL(RANK6, to_rank(SQ_3F));
    CHECK_EQUAL(RANK6, to_rank(SQ_4F));
    CHECK_EQUAL(RANK6, to_rank(SQ_5F));
    CHECK_EQUAL(RANK6, to_rank(SQ_6F));
    CHECK_EQUAL(RANK6, to_rank(SQ_7F));
    CHECK_EQUAL(RANK6, to_rank(SQ_8F));
    CHECK_EQUAL(RANK6, to_rank(SQ_9F));
    CHECK_EQUAL(RANK7, to_rank(SQ_1G));
    CHECK_EQUAL(RANK7, to_rank(SQ_2G));
    CHECK_EQUAL(RANK7, to_rank(SQ_3G));
    CHECK_EQUAL(RANK7, to_rank(SQ_4G));
    CHECK_EQUAL(RANK7, to_rank(SQ_5G));
    CHECK_EQUAL(RANK7, to_rank(SQ_6G));
    CHECK_EQUAL(RANK7, to_rank(SQ_7G));
    CHECK_EQUAL(RANK7, to_rank(SQ_8G));
    CHECK_EQUAL(RANK7, to_rank(SQ_9G));
    CHECK_EQUAL(RANK8, to_rank(SQ_1H));
    CHECK_EQUAL(RANK8, to_rank(SQ_2H));
    CHECK_EQUAL(RANK8, to_rank(SQ_3H));
    CHECK_EQUAL(RANK8, to_rank(SQ_4H));
    CHECK_EQUAL(RANK8, to_rank(SQ_5H));
    CHECK_EQUAL(RANK8, to_rank(SQ_6H));
    CHECK_EQUAL(RANK8, to_rank(SQ_7H));
    CHECK_EQUAL(RANK8, to_rank(SQ_8H));
    CHECK_EQUAL(RANK8, to_rank(SQ_9H));
    CHECK_EQUAL(RANK9, to_rank(SQ_1I));
    CHECK_EQUAL(RANK9, to_rank(SQ_2I));
    CHECK_EQUAL(RANK9, to_rank(SQ_3I));
    CHECK_EQUAL(RANK9, to_rank(SQ_4I));
    CHECK_EQUAL(RANK9, to_rank(SQ_5I));
    CHECK_EQUAL(RANK9, to_rank(SQ_6I));
    CHECK_EQUAL(RANK9, to_rank(SQ_7I));
    CHECK_EQUAL(RANK9, to_rank(SQ_8I));
    CHECK_EQUAL(RANK9, to_rank(SQ_9I));
}

TEST(squares, to_file)
{
    CHECK_EQUAL(FILE1, to_file(SQ_1A));
    CHECK_EQUAL(FILE2, to_file(SQ_2A));
    CHECK_EQUAL(FILE3, to_file(SQ_3A));
    CHECK_EQUAL(FILE4, to_file(SQ_4A));
    CHECK_EQUAL(FILE5, to_file(SQ_5A));
    CHECK_EQUAL(FILE6, to_file(SQ_6A));
    CHECK_EQUAL(FILE7, to_file(SQ_7A));
    CHECK_EQUAL(FILE8, to_file(SQ_8A));
    CHECK_EQUAL(FILE9, to_file(SQ_9A));
    CHECK_EQUAL(FILE1, to_file(SQ_1B));
    CHECK_EQUAL(FILE2, to_file(SQ_2B));
    CHECK_EQUAL(FILE3, to_file(SQ_3B));
    CHECK_EQUAL(FILE4, to_file(SQ_4B));
    CHECK_EQUAL(FILE5, to_file(SQ_5B));
    CHECK_EQUAL(FILE6, to_file(SQ_6B));
    CHECK_EQUAL(FILE7, to_file(SQ_7B));
    CHECK_EQUAL(FILE8, to_file(SQ_8B));
    CHECK_EQUAL(FILE9, to_file(SQ_9B));
    CHECK_EQUAL(FILE1, to_file(SQ_1C));
    CHECK_EQUAL(FILE2, to_file(SQ_2C));
    CHECK_EQUAL(FILE3, to_file(SQ_3C));
    CHECK_EQUAL(FILE4, to_file(SQ_4C));
    CHECK_EQUAL(FILE5, to_file(SQ_5C));
    CHECK_EQUAL(FILE6, to_file(SQ_6C));
    CHECK_EQUAL(FILE7, to_file(SQ_7C));
    CHECK_EQUAL(FILE8, to_file(SQ_8C));
    CHECK_EQUAL(FILE9, to_file(SQ_9C));
    CHECK_EQUAL(FILE1, to_file(SQ_1D));
    CHECK_EQUAL(FILE2, to_file(SQ_2D));
    CHECK_EQUAL(FILE3, to_file(SQ_3D));
    CHECK_EQUAL(FILE4, to_file(SQ_4D));
    CHECK_EQUAL(FILE5, to_file(SQ_5D));
    CHECK_EQUAL(FILE6, to_file(SQ_6D));
    CHECK_EQUAL(FILE7, to_file(SQ_7D));
    CHECK_EQUAL(FILE8, to_file(SQ_8D));
    CHECK_EQUAL(FILE9, to_file(SQ_9D));
    CHECK_EQUAL(FILE1, to_file(SQ_1E));
    CHECK_EQUAL(FILE2, to_file(SQ_2E));
    CHECK_EQUAL(FILE3, to_file(SQ_3E));
    CHECK_EQUAL(FILE4, to_file(SQ_4E));
    CHECK_EQUAL(FILE5, to_file(SQ_5E));
    CHECK_EQUAL(FILE6, to_file(SQ_6E));
    CHECK_EQUAL(FILE7, to_file(SQ_7E));
    CHECK_EQUAL(FILE8, to_file(SQ_8E));
    CHECK_EQUAL(FILE9, to_file(SQ_9E));
    CHECK_EQUAL(FILE1, to_file(SQ_1F));
    CHECK_EQUAL(FILE2, to_file(SQ_2F));
    CHECK_EQUAL(FILE3, to_file(SQ_3F));
    CHECK_EQUAL(FILE4, to_file(SQ_4F));
    CHECK_EQUAL(FILE5, to_file(SQ_5F));
    CHECK_EQUAL(FILE6, to_file(SQ_6F));
    CHECK_EQUAL(FILE7, to_file(SQ_7F));
    CHECK_EQUAL(FILE8, to_file(SQ_8F));
    CHECK_EQUAL(FILE9, to_file(SQ_9F));
    CHECK_EQUAL(FILE1, to_file(SQ_1G));
    CHECK_EQUAL(FILE2, to_file(SQ_2G));
    CHECK_EQUAL(FILE3, to_file(SQ_3G));
    CHECK_EQUAL(FILE4, to_file(SQ_4G));
    CHECK_EQUAL(FILE5, to_file(SQ_5G));
    CHECK_EQUAL(FILE6, to_file(SQ_6G));
    CHECK_EQUAL(FILE7, to_file(SQ_7G));
    CHECK_EQUAL(FILE8, to_file(SQ_8G));
    CHECK_EQUAL(FILE9, to_file(SQ_9G));
    CHECK_EQUAL(FILE1, to_file(SQ_1H));
    CHECK_EQUAL(FILE2, to_file(SQ_2H));
    CHECK_EQUAL(FILE3, to_file(SQ_3H));
    CHECK_EQUAL(FILE4, to_file(SQ_4H));
    CHECK_EQUAL(FILE5, to_file(SQ_5H));
    CHECK_EQUAL(FILE6, to_file(SQ_6H));
    CHECK_EQUAL(FILE7, to_file(SQ_7H));
    CHECK_EQUAL(FILE8, to_file(SQ_8H));
    CHECK_EQUAL(FILE9, to_file(SQ_9H));
    CHECK_EQUAL(FILE1, to_file(SQ_1I));
    CHECK_EQUAL(FILE2, to_file(SQ_2I));
    CHECK_EQUAL(FILE3, to_file(SQ_3I));
    CHECK_EQUAL(FILE4, to_file(SQ_4I));
    CHECK_EQUAL(FILE5, to_file(SQ_5I));
    CHECK_EQUAL(FILE6, to_file(SQ_6I));
    CHECK_EQUAL(FILE7, to_file(SQ_7I));
    CHECK_EQUAL(FILE8, to_file(SQ_8I));
    CHECK_EQUAL(FILE9, to_file(SQ_9I));
}

TEST(squares, shift)
{
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, vshogi::DIR_NNW));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, vshogi::DIR_NNE));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, vshogi::DIR_N));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, vshogi::DIR_NE));
    CHECK_EQUAL(SQ_2A, shift(SQ_1A, vshogi::DIR_W));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, vshogi::DIR_E));
    CHECK_EQUAL(SQ_2B, shift(SQ_1A, vshogi::DIR_SW));
    CHECK_EQUAL(SQ_1B, shift(SQ_1A, vshogi::DIR_S));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, vshogi::DIR_SE));
    CHECK_EQUAL(SQ_2C, shift(SQ_1A, vshogi::DIR_SSW));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, vshogi::DIR_SSE));

    CHECK_EQUAL(SQ_6C, shift(SQ_5E, vshogi::DIR_NNW));
    CHECK_EQUAL(SQ_4C, shift(SQ_5E, vshogi::DIR_NNE));
    CHECK_EQUAL(SQ_6D, shift(SQ_5E, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_5D, shift(SQ_5E, vshogi::DIR_N));
    CHECK_EQUAL(SQ_4D, shift(SQ_5E, vshogi::DIR_NE));
    CHECK_EQUAL(SQ_6E, shift(SQ_5E, vshogi::DIR_W));
    CHECK_EQUAL(SQ_4E, shift(SQ_5E, vshogi::DIR_E));
    CHECK_EQUAL(SQ_6F, shift(SQ_5E, vshogi::DIR_SW));
    CHECK_EQUAL(SQ_5F, shift(SQ_5E, vshogi::DIR_S));
    CHECK_EQUAL(SQ_4F, shift(SQ_5E, vshogi::DIR_SE));
    CHECK_EQUAL(SQ_6G, shift(SQ_5E, vshogi::DIR_SSW));
    CHECK_EQUAL(SQ_4G, shift(SQ_5E, vshogi::DIR_SSE));

    CHECK_EQUAL(SQ_NA, shift(SQ_9I, vshogi::DIR_NNW));
    CHECK_EQUAL(SQ_8G, shift(SQ_9I, vshogi::DIR_NNE));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_9H, shift(SQ_9I, vshogi::DIR_N));
    CHECK_EQUAL(SQ_8H, shift(SQ_9I, vshogi::DIR_NE));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, vshogi::DIR_W));
    CHECK_EQUAL(SQ_8I, shift(SQ_9I, vshogi::DIR_E));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, vshogi::DIR_SW));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, vshogi::DIR_S));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, vshogi::DIR_SE));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, vshogi::DIR_SSW));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, vshogi::DIR_SSE));
}

TEST(squares, get_direction)
{
    CHECK_EQUAL(vshogi::DIR_NW, get_direction(SQ_2A, SQ_1B));
    CHECK_EQUAL(vshogi::DIR_N, get_direction(SQ_8B, SQ_8F));
    CHECK_EQUAL(vshogi::DIR_NE, get_direction(SQ_1A, SQ_6F));
    CHECK_EQUAL(vshogi::DIR_W, get_direction(SQ_9D, SQ_1D));
    CHECK_EQUAL(vshogi::DIR_E, get_direction(SQ_1D, SQ_9D));
    CHECK_EQUAL(vshogi::DIR_SW, get_direction(SQ_8F, SQ_3A));
    CHECK_EQUAL(vshogi::DIR_S, get_direction(SQ_1I, SQ_1A));
    CHECK_EQUAL(vshogi::DIR_SE, get_direction(SQ_1I, SQ_9A));
    CHECK_EQUAL(vshogi::DIR_SSW, get_direction(SQ_8I, SQ_7G));
    CHECK_EQUAL(vshogi::DIR_SSE, get_direction(SQ_2I, SQ_3G));
}

TEST(squares, get_attacks_by_fu)
{
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_FU, SQ_2G);
        CHECK_EQUAL(SQ_2F, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_FU, SQ_2G);
        CHECK_EQUAL(SQ_2H, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
}

TEST(squares, get_attacks_by_ky)
{
    {
        CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(B_KY, SQ_1I));
    }
}

TEST(squares, get_attacks_by_ke)
{
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_KE, SQ_5E);
        CHECK_EQUAL(SQ_6C, actual[0]);
        CHECK_EQUAL(SQ_4C, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_KE, SQ_1D);
        CHECK_EQUAL(SQ_2B, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_KE, SQ_5E);
        CHECK_EQUAL(SQ_6G, actual[0]);
        CHECK_EQUAL(SQ_4G, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
}

TEST(squares, get_attacks_by_gi)
{
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_GI, SQ_9D);
        CHECK_EQUAL(SQ_9C, actual[0]);
        CHECK_EQUAL(SQ_8C, actual[1]);
        CHECK_EQUAL(SQ_8E, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_GI, SQ_1A);
        CHECK_EQUAL(SQ_2B, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_GI, SQ_1A);
        CHECK_EQUAL(SQ_2B, actual[0]);
        CHECK_EQUAL(SQ_1B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
}

TEST(squares, get_attacks_by_ki)
{
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_KI, SQ_9D);
        CHECK_EQUAL(SQ_9C, actual[0]);
        CHECK_EQUAL(SQ_8C, actual[1]);
        CHECK_EQUAL(SQ_8D, actual[2]);
        CHECK_EQUAL(SQ_9E, actual[3]);
        CHECK_EQUAL(SQ_NA, actual[4]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_NY, SQ_1A);
        CHECK_EQUAL(SQ_2A, actual[0]);
        CHECK_EQUAL(SQ_1B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_TO, SQ_1A);
        CHECK_EQUAL(SQ_2A, actual[0]);
        CHECK_EQUAL(SQ_2B, actual[1]);
        CHECK_EQUAL(SQ_1B, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
    }
}

TEST(squares, get_attacks_by_ka)
{
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(W_KA, SQ_1B));
}

TEST(squares, get_attacks_by_hi)
{
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(B_HI, SQ_5E));
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(W_HI, SQ_5E));
}

TEST(squares, get_attacks_by_um)
{
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(B_UM, SQ_5E));
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(W_UM, SQ_1B));
}

TEST(squares, get_attacks_by_ry)
{
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(W_RY, SQ_5E));
}

TEST(squares, get_attacks_by_ou)
{
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_OU, SQ_5A);
        CHECK_EQUAL(SQ_6A, actual[0]);
        CHECK_EQUAL(SQ_4A, actual[1]);
        CHECK_EQUAL(SQ_6B, actual[2]);
        CHECK_EQUAL(SQ_5B, actual[3]);
        CHECK_EQUAL(SQ_4B, actual[4]);
        CHECK_EQUAL(SQ_NA, actual[5]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_OU, SQ_8H);
        CHECK_EQUAL(SQ_9G, actual[0]);
        CHECK_EQUAL(SQ_8G, actual[1]);
        CHECK_EQUAL(SQ_7G, actual[2]);
        CHECK_EQUAL(SQ_9H, actual[3]);
        CHECK_EQUAL(SQ_7H, actual[4]);
        CHECK_EQUAL(SQ_9I, actual[5]);
        CHECK_EQUAL(SQ_8I, actual[6]);
        CHECK_EQUAL(SQ_7I, actual[7]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_OU, SQ_1H);
        CHECK_EQUAL(SQ_2G, actual[0]);
        CHECK_EQUAL(SQ_1G, actual[1]);
        CHECK_EQUAL(SQ_2H, actual[2]);
        CHECK_EQUAL(SQ_2I, actual[3]);
        CHECK_EQUAL(SQ_1I, actual[4]);
    }
}

} // namespace test_vshogi::test_shogi
