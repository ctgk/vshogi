#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP (judkins_shogi_squares) {
};

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

TEST(judkins_shogi_squares, in_promotion_zone)
{
    CHECK_TRUE(Squares::in_promotion_zone(SQ_6A, vshogi::BLACK));
    CHECK_TRUE(Squares::in_promotion_zone(SQ_5B, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(SQ_4C, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(SQ_3D, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(SQ_2E, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(SQ_1F, vshogi::BLACK));

    CHECK_FALSE(Squares::in_promotion_zone(SQ_1A, vshogi::WHITE));
    CHECK_FALSE(Squares::in_promotion_zone(SQ_2B, vshogi::WHITE));
    CHECK_FALSE(Squares::in_promotion_zone(SQ_3C, vshogi::WHITE));
    CHECK_FALSE(Squares::in_promotion_zone(SQ_4D, vshogi::WHITE));
    CHECK_TRUE(Squares::in_promotion_zone(SQ_5E, vshogi::WHITE));
    CHECK_TRUE(Squares::in_promotion_zone(SQ_6F, vshogi::WHITE));

    CHECK_TRUE(Squares::in_promotion_zone(RANK1, vshogi::BLACK));
    CHECK_TRUE(Squares::in_promotion_zone(RANK2, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(RANK3, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(RANK4, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(RANK5, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(RANK6, vshogi::BLACK));

    CHECK_FALSE(Squares::in_promotion_zone(RANK1, vshogi::WHITE));
    CHECK_FALSE(Squares::in_promotion_zone(RANK2, vshogi::WHITE));
    CHECK_FALSE(Squares::in_promotion_zone(RANK3, vshogi::WHITE));
    CHECK_FALSE(Squares::in_promotion_zone(RANK4, vshogi::WHITE));
    CHECK_TRUE(Squares::in_promotion_zone(RANK5, vshogi::WHITE));
    CHECK_TRUE(Squares::in_promotion_zone(RANK6, vshogi::WHITE));
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

} // namespace test_vshogi::test_judkins_shogi
