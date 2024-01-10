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

TEST(squares, to_square)
{
    CHECK_EQUAL(SQ_1A, Squares::to_square("1a"));
    CHECK_EQUAL(SQ_1B, Squares::to_square("1b"));
    CHECK_EQUAL(SQ_1C, Squares::to_square("1c"));
    CHECK_EQUAL(SQ_1D, Squares::to_square("1d"));
    CHECK_EQUAL(SQ_1E, Squares::to_square("1e"));
    CHECK_EQUAL(SQ_1F, Squares::to_square("1f"));
    CHECK_EQUAL(SQ_1G, Squares::to_square("1g"));
    CHECK_EQUAL(SQ_1H, Squares::to_square("1h"));
    CHECK_EQUAL(SQ_1I, Squares::to_square("1i"));
    CHECK_EQUAL(SQ_2A, Squares::to_square("2a"));
    CHECK_EQUAL(SQ_2B, Squares::to_square("2b"));
    CHECK_EQUAL(SQ_2C, Squares::to_square("2c"));
    CHECK_EQUAL(SQ_2D, Squares::to_square("2d"));
    CHECK_EQUAL(SQ_2E, Squares::to_square("2e"));
    CHECK_EQUAL(SQ_2F, Squares::to_square("2f"));
    CHECK_EQUAL(SQ_2G, Squares::to_square("2g"));
    CHECK_EQUAL(SQ_2H, Squares::to_square("2h"));
    CHECK_EQUAL(SQ_2I, Squares::to_square("2i"));
    CHECK_EQUAL(SQ_3A, Squares::to_square("3a"));
    CHECK_EQUAL(SQ_3B, Squares::to_square("3b"));
    CHECK_EQUAL(SQ_3C, Squares::to_square("3c"));
    CHECK_EQUAL(SQ_3D, Squares::to_square("3d"));
    CHECK_EQUAL(SQ_3E, Squares::to_square("3e"));
    CHECK_EQUAL(SQ_3F, Squares::to_square("3f"));
    CHECK_EQUAL(SQ_3G, Squares::to_square("3g"));
    CHECK_EQUAL(SQ_3H, Squares::to_square("3h"));
    CHECK_EQUAL(SQ_3I, Squares::to_square("3i"));
    CHECK_EQUAL(SQ_4A, Squares::to_square("4a"));
    CHECK_EQUAL(SQ_4B, Squares::to_square("4b"));
    CHECK_EQUAL(SQ_4C, Squares::to_square("4c"));
    CHECK_EQUAL(SQ_4D, Squares::to_square("4d"));
    CHECK_EQUAL(SQ_4E, Squares::to_square("4e"));
    CHECK_EQUAL(SQ_4F, Squares::to_square("4f"));
    CHECK_EQUAL(SQ_4G, Squares::to_square("4g"));
    CHECK_EQUAL(SQ_4H, Squares::to_square("4h"));
    CHECK_EQUAL(SQ_4I, Squares::to_square("4i"));
    CHECK_EQUAL(SQ_5A, Squares::to_square("5a"));
    CHECK_EQUAL(SQ_5B, Squares::to_square("5b"));
    CHECK_EQUAL(SQ_5C, Squares::to_square("5c"));
    CHECK_EQUAL(SQ_5D, Squares::to_square("5d"));
    CHECK_EQUAL(SQ_5E, Squares::to_square("5e"));
    CHECK_EQUAL(SQ_5F, Squares::to_square("5f"));
    CHECK_EQUAL(SQ_5G, Squares::to_square("5g"));
    CHECK_EQUAL(SQ_5H, Squares::to_square("5h"));
    CHECK_EQUAL(SQ_5I, Squares::to_square("5i"));
    CHECK_EQUAL(SQ_6A, Squares::to_square("6a"));
    CHECK_EQUAL(SQ_6B, Squares::to_square("6b"));
    CHECK_EQUAL(SQ_6C, Squares::to_square("6c"));
    CHECK_EQUAL(SQ_6D, Squares::to_square("6d"));
    CHECK_EQUAL(SQ_6E, Squares::to_square("6e"));
    CHECK_EQUAL(SQ_6F, Squares::to_square("6f"));
    CHECK_EQUAL(SQ_6G, Squares::to_square("6g"));
    CHECK_EQUAL(SQ_6H, Squares::to_square("6h"));
    CHECK_EQUAL(SQ_6I, Squares::to_square("6i"));
    CHECK_EQUAL(SQ_7A, Squares::to_square("7a"));
    CHECK_EQUAL(SQ_7B, Squares::to_square("7b"));
    CHECK_EQUAL(SQ_7C, Squares::to_square("7c"));
    CHECK_EQUAL(SQ_7D, Squares::to_square("7d"));
    CHECK_EQUAL(SQ_7E, Squares::to_square("7e"));
    CHECK_EQUAL(SQ_7F, Squares::to_square("7f"));
    CHECK_EQUAL(SQ_7G, Squares::to_square("7g"));
    CHECK_EQUAL(SQ_7H, Squares::to_square("7h"));
    CHECK_EQUAL(SQ_7I, Squares::to_square("7i"));
    CHECK_EQUAL(SQ_8A, Squares::to_square("8a"));
    CHECK_EQUAL(SQ_8B, Squares::to_square("8b"));
    CHECK_EQUAL(SQ_8C, Squares::to_square("8c"));
    CHECK_EQUAL(SQ_8D, Squares::to_square("8d"));
    CHECK_EQUAL(SQ_8E, Squares::to_square("8e"));
    CHECK_EQUAL(SQ_8F, Squares::to_square("8f"));
    CHECK_EQUAL(SQ_8G, Squares::to_square("8g"));
    CHECK_EQUAL(SQ_8H, Squares::to_square("8h"));
    CHECK_EQUAL(SQ_8I, Squares::to_square("8i"));
    CHECK_EQUAL(SQ_9A, Squares::to_square("9a"));
    CHECK_EQUAL(SQ_9B, Squares::to_square("9b"));
    CHECK_EQUAL(SQ_9C, Squares::to_square("9c"));
    CHECK_EQUAL(SQ_9D, Squares::to_square("9d"));
    CHECK_EQUAL(SQ_9E, Squares::to_square("9e"));
    CHECK_EQUAL(SQ_9F, Squares::to_square("9f"));
    CHECK_EQUAL(SQ_9G, Squares::to_square("9g"));
    CHECK_EQUAL(SQ_9H, Squares::to_square("9h"));
    CHECK_EQUAL(SQ_9I, Squares::to_square("9i"));
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
