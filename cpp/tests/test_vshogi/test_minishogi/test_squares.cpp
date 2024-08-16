#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(minishogi_squares){};

TEST(minishogi_squares, shift)
{
    CHECK_EQUAL(SQ_1A, Squares::shift(SQ_1B, vshogi::DIR_N));
    CHECK_EQUAL(SQ_1C, Squares::shift(SQ_1B, vshogi::DIR_S));
}

TEST(minishogi_squares, to_rank)
{
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_1A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_1B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_1C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_1D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_1E));
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_2A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_2B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_2C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_2D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_2E));
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_3A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_3B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_3C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_3D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_3E));
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_4A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_4B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_4C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_4D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_4E));
    CHECK_EQUAL(RANK1, Squares::to_rank(SQ_5A));
    CHECK_EQUAL(RANK2, Squares::to_rank(SQ_5B));
    CHECK_EQUAL(RANK3, Squares::to_rank(SQ_5C));
    CHECK_EQUAL(RANK4, Squares::to_rank(SQ_5D));
    CHECK_EQUAL(RANK5, Squares::to_rank(SQ_5E));
}

TEST(minishogi_squares, to_file)
{
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1A));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1B));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1C));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1D));
    CHECK_EQUAL(FILE1, Squares::to_file(SQ_1E));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2A));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2B));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2C));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2D));
    CHECK_EQUAL(FILE2, Squares::to_file(SQ_2E));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3A));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3B));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3C));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3D));
    CHECK_EQUAL(FILE3, Squares::to_file(SQ_3E));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4A));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4B));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4C));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4D));
    CHECK_EQUAL(FILE4, Squares::to_file(SQ_4E));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5A));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5B));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5C));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5D));
    CHECK_EQUAL(FILE5, Squares::to_file(SQ_5E));
}

TEST(minishogi_squares, to_square)
{
    CHECK_EQUAL(SQ_1A, Squares::to_square("1a"));
    CHECK_EQUAL(SQ_1B, Squares::to_square("1b"));
    CHECK_EQUAL(SQ_1C, Squares::to_square("1c"));
    CHECK_EQUAL(SQ_1D, Squares::to_square("1d"));
    CHECK_EQUAL(SQ_1E, Squares::to_square("1e"));
    CHECK_EQUAL(SQ_2A, Squares::to_square("2a"));
    CHECK_EQUAL(SQ_2B, Squares::to_square("2b"));
    CHECK_EQUAL(SQ_2C, Squares::to_square("2c"));
    CHECK_EQUAL(SQ_2D, Squares::to_square("2d"));
    CHECK_EQUAL(SQ_2E, Squares::to_square("2e"));
    CHECK_EQUAL(SQ_3A, Squares::to_square("3a"));
    CHECK_EQUAL(SQ_3B, Squares::to_square("3b"));
    CHECK_EQUAL(SQ_3C, Squares::to_square("3c"));
    CHECK_EQUAL(SQ_3D, Squares::to_square("3d"));
    CHECK_EQUAL(SQ_3E, Squares::to_square("3e"));
    CHECK_EQUAL(SQ_4A, Squares::to_square("4a"));
    CHECK_EQUAL(SQ_4B, Squares::to_square("4b"));
    CHECK_EQUAL(SQ_4C, Squares::to_square("4c"));
    CHECK_EQUAL(SQ_4D, Squares::to_square("4d"));
    CHECK_EQUAL(SQ_4E, Squares::to_square("4e"));
    CHECK_EQUAL(SQ_5A, Squares::to_square("5a"));
    CHECK_EQUAL(SQ_5B, Squares::to_square("5b"));
    CHECK_EQUAL(SQ_5C, Squares::to_square("5c"));
    CHECK_EQUAL(SQ_5D, Squares::to_square("5d"));
    CHECK_EQUAL(SQ_5E, Squares::to_square("5e"));
}

TEST(minishogi_squares, get_direction)
{
    CHECK_EQUAL(vshogi::DIR_NW, Squares::get_direction(SQ_5A, SQ_3C));

    // note that there is no knight move in Minishogi.
    CHECK_EQUAL(vshogi::DIR_NA, Squares::get_direction(SQ_5A, SQ_4C));
}

TEST(minishogi_squares, get_non_ranging_attacks_by)
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
        const auto actual = Squares::get_non_ranging_attacks_by(B_GI, SQ_2E);
        CHECK_EQUAL(SQ_3D, actual[0]);
        CHECK_EQUAL(SQ_2D, actual[1]);
        CHECK_EQUAL(SQ_1D, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_GI, SQ_2E);
        CHECK_EQUAL(SQ_1D, actual[0]);
        CHECK_EQUAL(SQ_3D, actual[1]);
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
        const auto actual = Squares::get_non_ranging_attacks_by(B_KI, SQ_5A);
        CHECK_EQUAL(SQ_4A, actual[0]);
        CHECK_EQUAL(SQ_5B, actual[1]);
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
        const auto actual = Squares::get_non_ranging_attacks_by(B_TO, SQ_5A);
        CHECK_EQUAL(SQ_4A, actual[0]);
        CHECK_EQUAL(SQ_5B, actual[1]);
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
        const auto actual = Squares::get_non_ranging_attacks_by(B_NG, SQ_5A);
        CHECK_EQUAL(SQ_4A, actual[0]);
        CHECK_EQUAL(SQ_5B, actual[1]);
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
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(W_UM, SQ_3C));
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(B_HI, SQ_3C));
    CHECK_TRUE(nullptr == Squares::get_non_ranging_attacks_by(W_RY, SQ_3C));
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
        CHECK_EQUAL(SQ_NA, actual[8]);
    }
    {
        const auto actual = Squares::get_non_ranging_attacks_by(W_OU, SQ_5E);
        CHECK_EQUAL(SQ_4E, actual[0]);
        CHECK_EQUAL(SQ_4D, actual[1]);
        CHECK_EQUAL(SQ_5D, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
    }
}

} // namespace test_vshogi::test_minishogi
