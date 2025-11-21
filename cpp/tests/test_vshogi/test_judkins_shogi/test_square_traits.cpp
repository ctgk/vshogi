#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP (test_judkins_shogi_square_traits) {
};

TEST(test_judkins_shogi_square_traits, shift)
{
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_4A, SquareTraits::shift(SQ_3C, vshogi::DIR_NNW));
    CHECK_EQUAL(SQ_2A, SquareTraits::shift(SQ_3C, vshogi::DIR_NNE));
    CHECK_EQUAL(SQ_4B, SquareTraits::shift(SQ_3C, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_3B, SquareTraits::shift(SQ_3C, vshogi::DIR_N));
    CHECK_EQUAL(SQ_2B, SquareTraits::shift(SQ_3C, vshogi::DIR_NE));
    CHECK_EQUAL(SQ_4C, SquareTraits::shift(SQ_3C, vshogi::DIR_W));
    CHECK_EQUAL(SQ_2C, SquareTraits::shift(SQ_3C, vshogi::DIR_E));
    CHECK_EQUAL(SQ_4D, SquareTraits::shift(SQ_3C, vshogi::DIR_SW));
    CHECK_EQUAL(SQ_3D, SquareTraits::shift(SQ_3C, vshogi::DIR_S));
    CHECK_EQUAL(SQ_2D, SquareTraits::shift(SQ_3C, vshogi::DIR_SE));
    CHECK_EQUAL(SQ_4E, SquareTraits::shift(SQ_3C, vshogi::DIR_SSW));
    CHECK_EQUAL(SQ_2E, SquareTraits::shift(SQ_3C, vshogi::DIR_SSE));
}

TEST(test_judkins_shogi_square_traits, to_rank)
{
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_1A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_1B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_1C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_1D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_1E));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_1F));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_2A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_2B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_2C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_2D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_2E));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_2F));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_3A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_3B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_3C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_3D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_3E));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_3F));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_4A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_4B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_4C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_4D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_4E));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_4F));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_5A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_5B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_5C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_5D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_5E));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_5F));
}

TEST(test_judkins_shogi_square_traits, to_file)
{
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1A));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1B));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1C));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1D));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1E));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1F));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2A));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2B));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2C));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2D));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2E));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2F));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3A));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3B));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3C));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3D));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3E));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3F));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4A));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4B));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4C));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4D));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4E));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4F));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5A));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5B));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5C));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5D));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5E));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5F));
}

TEST(test_judkins_shogi_square_traits, to_square)
{
    CHECK_EQUAL(SQ_1A, SquareTraits::to_square("1a"));
    CHECK_EQUAL(SQ_1B, SquareTraits::to_square("1b"));
    CHECK_EQUAL(SQ_1C, SquareTraits::to_square("1c"));
    CHECK_EQUAL(SQ_1D, SquareTraits::to_square("1d"));
    CHECK_EQUAL(SQ_1E, SquareTraits::to_square("1e"));
    CHECK_EQUAL(SQ_1F, SquareTraits::to_square("1f"));
    CHECK_EQUAL(SQ_2A, SquareTraits::to_square("2a"));
    CHECK_EQUAL(SQ_2B, SquareTraits::to_square("2b"));
    CHECK_EQUAL(SQ_2C, SquareTraits::to_square("2c"));
    CHECK_EQUAL(SQ_2D, SquareTraits::to_square("2d"));
    CHECK_EQUAL(SQ_2E, SquareTraits::to_square("2e"));
    CHECK_EQUAL(SQ_2F, SquareTraits::to_square("2f"));
    CHECK_EQUAL(SQ_3A, SquareTraits::to_square("3a"));
    CHECK_EQUAL(SQ_3B, SquareTraits::to_square("3b"));
    CHECK_EQUAL(SQ_3C, SquareTraits::to_square("3c"));
    CHECK_EQUAL(SQ_3D, SquareTraits::to_square("3d"));
    CHECK_EQUAL(SQ_3E, SquareTraits::to_square("3e"));
    CHECK_EQUAL(SQ_3F, SquareTraits::to_square("3f"));
    CHECK_EQUAL(SQ_4A, SquareTraits::to_square("4a"));
    CHECK_EQUAL(SQ_4B, SquareTraits::to_square("4b"));
    CHECK_EQUAL(SQ_4C, SquareTraits::to_square("4c"));
    CHECK_EQUAL(SQ_4D, SquareTraits::to_square("4d"));
    CHECK_EQUAL(SQ_4E, SquareTraits::to_square("4e"));
    CHECK_EQUAL(SQ_4F, SquareTraits::to_square("4f"));
    CHECK_EQUAL(SQ_5A, SquareTraits::to_square("5a"));
    CHECK_EQUAL(SQ_5B, SquareTraits::to_square("5b"));
    CHECK_EQUAL(SQ_5C, SquareTraits::to_square("5c"));
    CHECK_EQUAL(SQ_5D, SquareTraits::to_square("5d"));
    CHECK_EQUAL(SQ_5E, SquareTraits::to_square("5e"));
    CHECK_EQUAL(SQ_5F, SquareTraits::to_square("5f"));
    CHECK_EQUAL(SQ_6A, SquareTraits::to_square("6a"));
    CHECK_EQUAL(SQ_6B, SquareTraits::to_square("6b"));
    CHECK_EQUAL(SQ_6C, SquareTraits::to_square("6c"));
    CHECK_EQUAL(SQ_6D, SquareTraits::to_square("6d"));
    CHECK_EQUAL(SQ_6E, SquareTraits::to_square("6e"));
    CHECK_EQUAL(SQ_6F, SquareTraits::to_square("6f"));
}

TEST(test_judkins_shogi_square_traits, in_promotion_zone)
{
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_6A, vshogi::BLACK));
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_5B, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_4C, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_3D, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_2E, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_1F, vshogi::BLACK));

    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_1A, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_2B, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_3C, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_4D, vshogi::WHITE));
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_5E, vshogi::WHITE));
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_6F, vshogi::WHITE));
}

TEST(test_judkins_shogi_square_traits, direction)
{
    // Note that result will be different in Minishogi.
    CHECK_EQUAL(vshogi::DIR_NNW, SquareTraits::direction(SQ_4C, SQ_5A));

    CHECK_EQUAL(vshogi::DIR_NW, SquareTraits::direction(SQ_1B, SQ_2A));
    CHECK_EQUAL(vshogi::DIR_N, SquareTraits::direction(SQ_6F, SQ_6B));
    CHECK_EQUAL(vshogi::DIR_NE, SquareTraits::direction(SQ_6F, SQ_1A));
    CHECK_EQUAL(vshogi::DIR_W, SquareTraits::direction(SQ_1D, SQ_4D));
    CHECK_EQUAL(vshogi::DIR_E, SquareTraits::direction(SQ_3D, SQ_1D));
    CHECK_EQUAL(vshogi::DIR_SW, SquareTraits::direction(SQ_3A, SQ_4B));
    CHECK_EQUAL(vshogi::DIR_S, SquareTraits::direction(SQ_1A, SQ_1E));
    CHECK_EQUAL(vshogi::DIR_SE, SquareTraits::direction(SQ_6A, SQ_1F));
    CHECK_EQUAL(vshogi::DIR_SSW, SquareTraits::direction(SQ_5A, SQ_6C));
    CHECK_EQUAL(vshogi::DIR_SSE, SquareTraits::direction(SQ_3B, SQ_2D));
    CHECK_EQUAL(vshogi::DIR_NA, SquareTraits::direction(SQ_1B, SQ_6A));
}

TEST(test_judkins_shogi_square_traits, ray_from)
{
    {
        const auto actual = SquareTraits::ray_from(SQ_NA, vshogi::DIR_NA);
        CHECK_EQUAL(SQ_NA, actual[0]);
    }
    {
        const auto actual = SquareTraits::ray_from(SQ_1A, vshogi::DIR_NA);
        CHECK_EQUAL(SQ_NA, actual[0]);
    }
    {
        const auto actual = SquareTraits::ray_from(SQ_NA, vshogi::DIR_S);
        CHECK_EQUAL(SQ_NA, actual[0]);
    }
    {
        const auto actual = SquareTraits::ray_from(SQ_1A, vshogi::DIR_E);
        CHECK_EQUAL(SQ_NA, actual[0]);
    }
    {
        const auto actual = SquareTraits::ray_from(SQ_1E, vshogi::DIR_S);
        CHECK_EQUAL(SQ_1F, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
    {
        const auto actual = SquareTraits::ray_from(SQ_5E, vshogi::DIR_NNE);
        CHECK_EQUAL(SQ_4C, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
    {
        const auto actual = SquareTraits::ray_from(SQ_5B, vshogi::DIR_NNE);
        CHECK_EQUAL(SQ_NA, actual[0]);
    }
}

} // namespace test_vshogi::test_judkins_shogi
