#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP (test_minishogi_square_traits) {
};

TEST(test_minishogi_square_traits, shift)
{
    CHECK_EQUAL(SQ_1A, SquareTraits::shift(SQ_1B, vshogi::DIR_N));
    CHECK_EQUAL(SQ_4B, SquareTraits::shift(SQ_3C, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_5A, SquareTraits::shift(SQ_4B, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_1C, SquareTraits::shift(SQ_1B, vshogi::DIR_S));
}

TEST(test_minishogi_square_traits, to_rank)
{
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_1A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_1B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_1C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_1D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_1E));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_2A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_2B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_2C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_2D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_2E));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_3A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_3B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_3C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_3D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_3E));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_4A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_4B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_4C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_4D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_4E));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_5A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_5B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_5C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_5D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_5E));
}

TEST(test_minishogi_square_traits, to_file)
{
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1A));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1B));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1C));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1D));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1E));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2A));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2B));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2C));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2D));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2E));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3A));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3B));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3C));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3D));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3E));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4A));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4B));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4C));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4D));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4E));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5A));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5B));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5C));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5D));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5E));
}

TEST(test_minishogi_square_traits, to_square)
{
    CHECK_EQUAL(SQ_1A, SquareTraits::to_square(FILE1, RANK1));
    CHECK_EQUAL(SQ_1B, SquareTraits::to_square(FILE1, RANK2));
    CHECK_EQUAL(SQ_1C, SquareTraits::to_square(FILE1, RANK3));
    CHECK_EQUAL(SQ_1D, SquareTraits::to_square(FILE1, RANK4));
    CHECK_EQUAL(SQ_1E, SquareTraits::to_square(FILE1, RANK5));
    CHECK_EQUAL(SQ_2A, SquareTraits::to_square(FILE2, RANK1));
    CHECK_EQUAL(SQ_2B, SquareTraits::to_square(FILE2, RANK2));
    CHECK_EQUAL(SQ_2C, SquareTraits::to_square(FILE2, RANK3));
    CHECK_EQUAL(SQ_2D, SquareTraits::to_square(FILE2, RANK4));
    CHECK_EQUAL(SQ_2E, SquareTraits::to_square(FILE2, RANK5));
    CHECK_EQUAL(SQ_3A, SquareTraits::to_square(FILE3, RANK1));
    CHECK_EQUAL(SQ_3B, SquareTraits::to_square(FILE3, RANK2));
    CHECK_EQUAL(SQ_3C, SquareTraits::to_square(FILE3, RANK3));
    CHECK_EQUAL(SQ_3D, SquareTraits::to_square(FILE3, RANK4));
    CHECK_EQUAL(SQ_3E, SquareTraits::to_square(FILE3, RANK5));
    CHECK_EQUAL(SQ_4A, SquareTraits::to_square(FILE4, RANK1));
    CHECK_EQUAL(SQ_4B, SquareTraits::to_square(FILE4, RANK2));
    CHECK_EQUAL(SQ_4C, SquareTraits::to_square(FILE4, RANK3));
    CHECK_EQUAL(SQ_4D, SquareTraits::to_square(FILE4, RANK4));
    CHECK_EQUAL(SQ_4E, SquareTraits::to_square(FILE4, RANK5));
    CHECK_EQUAL(SQ_5A, SquareTraits::to_square(FILE5, RANK1));
    CHECK_EQUAL(SQ_5B, SquareTraits::to_square(FILE5, RANK2));
    CHECK_EQUAL(SQ_5C, SquareTraits::to_square(FILE5, RANK3));
    CHECK_EQUAL(SQ_5D, SquareTraits::to_square(FILE5, RANK4));
    CHECK_EQUAL(SQ_5E, SquareTraits::to_square(FILE5, RANK5));
}

TEST(test_minishogi_square_traits, in_promotion_zone)
{
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_5A, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_4B, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_3C, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_2D, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_1E, vshogi::BLACK));

    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_1A, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_2B, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_3C, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_4D, vshogi::WHITE));
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_5E, vshogi::WHITE));
}

TEST(test_minishogi_square_traits, direction)
{
    CHECK_EQUAL(vshogi::DIR_NW, SquareTraits::direction(SQ_3C, SQ_5A));

    // note that there is no knight move in Minishogi.
    CHECK_EQUAL(vshogi::DIR_NA, SquareTraits::direction(SQ_4C, SQ_5A));

    CHECK_EQUAL(vshogi::DIR_NE, SquareTraits::direction(SQ_4D, SQ_1A));
}

TEST(test_minishogi_square_traits, chebyshev_distance)
{
    CHECK_EQUAL(1u, SquareTraits::chebyshev_distance(SQ_1A, SQ_1B));
    CHECK_EQUAL(2u, SquareTraits::chebyshev_distance(SQ_3A, SQ_1A));
    CHECK_EQUAL(3u, SquareTraits::chebyshev_distance(SQ_1B, SQ_4D));
}

TEST(test_minishogi_square_traits, ray_from)
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
        const auto actual = SquareTraits::ray_from(SQ_1D, vshogi::DIR_S);
        CHECK_EQUAL(SQ_1E, actual[0]);
        CHECK_EQUAL(SQ_NA, actual[1]);
    }
}

} // namespace test_vshogi::test_minishogi
