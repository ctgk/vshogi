#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP(judkins_shogi_squares){};

TEST(judkins_shogi_squares, shift)
{
    CHECK_EQUAL(SQ_4A, Squares::shift(SQ_3C, vshogi::DIR_NNW));
    CHECK_EQUAL(SQ_2A, Squares::shift(SQ_3C, vshogi::DIR_NNE));
    CHECK_EQUAL(SQ_4B, Squares::shift(SQ_3C, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_3B, Squares::shift(SQ_3C, vshogi::DIR_N));
    CHECK_EQUAL(SQ_2B, Squares::shift(SQ_3C, vshogi::DIR_NE));
    CHECK_EQUAL(SQ_4C, Squares::shift(SQ_3C, vshogi::DIR_W));
    CHECK_EQUAL(SQ_2C, Squares::shift(SQ_3C, vshogi::DIR_E));
    CHECK_EQUAL(SQ_4D, Squares::shift(SQ_3C, vshogi::DIR_SW));
    CHECK_EQUAL(SQ_3D, Squares::shift(SQ_3C, vshogi::DIR_S));
    CHECK_EQUAL(SQ_2D, Squares::shift(SQ_3C, vshogi::DIR_SE));
    CHECK_EQUAL(SQ_4E, Squares::shift(SQ_3C, vshogi::DIR_SSW));
    CHECK_EQUAL(SQ_2E, Squares::shift(SQ_3C, vshogi::DIR_SSE));
}

TEST(judkins_shogi_squares, to_rank)
{
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_1A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_1B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_1C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_1D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_1E));
    CHECK_EQUAL(RANK6, Squares::to_rank(SQ_1F));
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_2A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_2B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_2C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_2D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_2E));
    CHECK_EQUAL(RANK6, Squares::to_rank(SQ_2F));
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_3A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_3B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_3C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_3D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_3E));
    CHECK_EQUAL(RANK6, Squares::to_rank(SQ_3F));
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_4A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_4B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_4C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_4D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_4E));
    CHECK_EQUAL(RANK6, Squares::to_rank(SQ_4F));
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_5A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_5B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_5C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_5D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_5E));
    CHECK_EQUAL(RANK6, Squares::to_rank(SQ_5F));
}

TEST(judkins_shogi_squares, to_file)
{
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1A));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1B));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1C));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1D));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1E));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1F));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2A));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2B));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2C));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2D));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2E));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2F));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3A));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3B));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3C));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3D));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3E));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3F));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4A));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4B));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4C));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4D));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4E));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4F));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5A));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5B));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5C));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5D));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5E));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5F));
}

TEST(judkins_shogi_squares, to_square)
{
    CHECK_EQUAL(SQ_1A, Squares::to_square("1a"));
    CHECK_EQUAL(SQ_1B, Squares::to_square("1b"));
    CHECK_EQUAL(SQ_1C, Squares::to_square("1c"));
    CHECK_EQUAL(SQ_1D, Squares::to_square("1d"));
    CHECK_EQUAL(SQ_1E, Squares::to_square("1e"));
    CHECK_EQUAL(SQ_1F, Squares::to_square("1f"));
    CHECK_EQUAL(SQ_2A, Squares::to_square("2a"));
    CHECK_EQUAL(SQ_2B, Squares::to_square("2b"));
    CHECK_EQUAL(SQ_2C, Squares::to_square("2c"));
    CHECK_EQUAL(SQ_2D, Squares::to_square("2d"));
    CHECK_EQUAL(SQ_2E, Squares::to_square("2e"));
    CHECK_EQUAL(SQ_2F, Squares::to_square("2f"));
    CHECK_EQUAL(SQ_3A, Squares::to_square("3a"));
    CHECK_EQUAL(SQ_3B, Squares::to_square("3b"));
    CHECK_EQUAL(SQ_3C, Squares::to_square("3c"));
    CHECK_EQUAL(SQ_3D, Squares::to_square("3d"));
    CHECK_EQUAL(SQ_3E, Squares::to_square("3e"));
    CHECK_EQUAL(SQ_3F, Squares::to_square("3f"));
    CHECK_EQUAL(SQ_4A, Squares::to_square("4a"));
    CHECK_EQUAL(SQ_4B, Squares::to_square("4b"));
    CHECK_EQUAL(SQ_4C, Squares::to_square("4c"));
    CHECK_EQUAL(SQ_4D, Squares::to_square("4d"));
    CHECK_EQUAL(SQ_4E, Squares::to_square("4e"));
    CHECK_EQUAL(SQ_4F, Squares::to_square("4f"));
    CHECK_EQUAL(SQ_5A, Squares::to_square("5a"));
    CHECK_EQUAL(SQ_5B, Squares::to_square("5b"));
    CHECK_EQUAL(SQ_5C, Squares::to_square("5c"));
    CHECK_EQUAL(SQ_5D, Squares::to_square("5d"));
    CHECK_EQUAL(SQ_5E, Squares::to_square("5e"));
    CHECK_EQUAL(SQ_5F, Squares::to_square("5f"));
    CHECK_EQUAL(SQ_6A, Squares::to_square("6a"));
    CHECK_EQUAL(SQ_6B, Squares::to_square("6b"));
    CHECK_EQUAL(SQ_6C, Squares::to_square("6c"));
    CHECK_EQUAL(SQ_6D, Squares::to_square("6d"));
    CHECK_EQUAL(SQ_6E, Squares::to_square("6e"));
    CHECK_EQUAL(SQ_6F, Squares::to_square("6f"));
}

TEST(judkins_shogi_squares, get_direction)
{
    // Note that result will be different in Minishogi.
    CHECK_EQUAL(vshogi::DIR_NNW, Squares::get_direction(SQ_5A, SQ_4C));

    CHECK_EQUAL(vshogi::DIR_NW, Squares::get_direction(SQ_2A, SQ_1B));
    CHECK_EQUAL(vshogi::DIR_N, Squares::get_direction(SQ_6B, SQ_6F));
    CHECK_EQUAL(vshogi::DIR_NE, Squares::get_direction(SQ_1A, SQ_6F));
    CHECK_EQUAL(vshogi::DIR_W, Squares::get_direction(SQ_4D, SQ_1D));
    CHECK_EQUAL(vshogi::DIR_E, Squares::get_direction(SQ_1D, SQ_3D));
    CHECK_EQUAL(vshogi::DIR_SW, Squares::get_direction(SQ_4B, SQ_3A));
    CHECK_EQUAL(vshogi::DIR_S, Squares::get_direction(SQ_1E, SQ_1A));
    CHECK_EQUAL(vshogi::DIR_SE, Squares::get_direction(SQ_1F, SQ_6A));
    CHECK_EQUAL(vshogi::DIR_SSW, Squares::get_direction(SQ_6C, SQ_5A));
    CHECK_EQUAL(vshogi::DIR_SSE, Squares::get_direction(SQ_2D, SQ_3B));
    CHECK_EQUAL(vshogi::DIR_NA, Squares::get_direction(SQ_6A, SQ_1B));
}

TEST(judkins_shogi_squares, get_non_ranging_attacks_by)
{
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_FU, SQ_3A);
        CHECK_EQUAL(SQ_NA, actual[0]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_FU, SQ_4B);
        CHECK_EQUAL(SQ_4A, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_FU, SQ_4B);
        CHECK_EQUAL(SQ_4C, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
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
        const auto actual = Squares::get_non_ranging_attacks_by(W_KE, SQ_5B);
        CHECK_EQUAL(SQ_4D, actual[0]);
        CHECK_EQUAL(SQ_6D, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_GI, SQ_2E);
        CHECK_EQUAL(SQ_3D, actual[0]);
        CHECK_EQUAL(SQ_2D, actual[1]);
        CHECK_EQUAL(SQ_1D, actual[2]);
        CHECK_EQUAL(SQ_3F, actual[3]);
        CHECK_EQUAL(SQ_1F, actual[4]);
        CHECK_EQUAL(SQ_NA, actual[5]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_GI, SQ_2F);
        CHECK_EQUAL(SQ_1E, actual[0]);
        CHECK_EQUAL(SQ_3E, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_GI, SQ_3A);
        CHECK_EQUAL(SQ_2B, actual[0]);
        CHECK_EQUAL(SQ_3B, actual[1]);
        CHECK_EQUAL(SQ_4B, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_KI, SQ_6A);
        CHECK_EQUAL(SQ_5A, actual[0]);
        CHECK_EQUAL(SQ_6B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_KI, SQ_3C);
        constexpr SquareEnum expected[]
            = {SQ_2D, SQ_3D, SQ_4D, SQ_2C, SQ_4C, SQ_3B, SQ_NA};
        for (int ii = 0; ii < 7; ++ii) {
            CHECK_EQUAL(expected[ii], actual[ii]);
        }
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_TO, SQ_6A);
        CHECK_EQUAL(SQ_5A, actual[0]);
        CHECK_EQUAL(SQ_6B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_TO, SQ_3C);
        constexpr SquareEnum expected[]
            = {SQ_2D, SQ_3D, SQ_4D, SQ_2C, SQ_4C, SQ_3B, SQ_NA};
        for (int ii = 0; ii < 7; ++ii) {
            CHECK_EQUAL(expected[ii], actual[ii]);
        }
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_NK, SQ_6A);
        CHECK_EQUAL(SQ_5A, actual[0]);
        CHECK_EQUAL(SQ_6B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_NK, SQ_3C);
        constexpr SquareEnum expected[]
            = {SQ_2D, SQ_3D, SQ_4D, SQ_2C, SQ_4C, SQ_3B, SQ_NA};
        for (int ii = 0; ii < 7; ++ii) {
            CHECK_EQUAL(expected[ii], actual[ii]);
        }
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_NG, SQ_6A);
        CHECK_EQUAL(SQ_5A, actual[0]);
        CHECK_EQUAL(SQ_6B, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_NG, SQ_3C);
        constexpr SquareEnum expected[]
            = {SQ_2D, SQ_3D, SQ_4D, SQ_2C, SQ_4C, SQ_3B, SQ_NA};
        for (int ii = 0; ii < 7; ++ii) {
            CHECK_EQUAL(expected[ii], actual[ii]);
        }
    }
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(B_KA, SQ_3C));
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(W_UM, SQ_3A));
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(W_HI, SQ_3A));
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(B_RY, SQ_3C));
    {
        const auto actual = Squares::get_non_ranging_attacks_by(B_OU, SQ_2B);
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
        const auto actual = Squares::get_non_ranging_attacks_by(W_OU, SQ_6F);
        CHECK_EQUAL(SQ_5F, actual[0]);
        CHECK_EQUAL(SQ_5E, actual[1]);
        CHECK_EQUAL(SQ_6E, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
    }
}

} // namespace test_vshogi::test_judkins_shogi
