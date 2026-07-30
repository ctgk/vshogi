#include "vshogi/variants/leshogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_leshogi
{

using namespace vshogi::leshogi;

TEST_GROUP (test_leshogi_square_traits) {
};

TEST(test_leshogi_square_traits, test_shift)
{
    CHECK_EQUAL(SQ_1A, SquareTraits::shift(SQ_1B, vshogi::DIR_N));
    CHECK_EQUAL(SQ_4B, SquareTraits::shift(SQ_3C, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_4B, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_1C, SquareTraits::shift(SQ_1B, vshogi::DIR_S));
}

TEST(test_leshogi_square_traits, test_to_rank)
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
}

TEST(test_leshogi_square_traits, test_to_file)
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
}

TEST(test_leshogi_square_traits, test_to_square)
{
    CHECK_EQUAL(SQ_1A, SquareTraits::to_square(FILE1, RANK1));
    CHECK_EQUAL(SQ_1B, SquareTraits::to_square(FILE1, RANK2));
    CHECK_EQUAL(SQ_1C, SquareTraits::to_square(FILE1, RANK3));
    CHECK_EQUAL(SQ_1D, SquareTraits::to_square(FILE1, RANK4));
    CHECK_EQUAL(SQ_1E, SquareTraits::to_square(FILE1, RANK5));
    CHECK_EQUAL(SQ_1F, SquareTraits::to_square(FILE1, RANK6));
    CHECK_EQUAL(SQ_2A, SquareTraits::to_square(FILE2, RANK1));
    CHECK_EQUAL(SQ_2B, SquareTraits::to_square(FILE2, RANK2));
    CHECK_EQUAL(SQ_2C, SquareTraits::to_square(FILE2, RANK3));
    CHECK_EQUAL(SQ_2D, SquareTraits::to_square(FILE2, RANK4));
    CHECK_EQUAL(SQ_2E, SquareTraits::to_square(FILE2, RANK5));
    CHECK_EQUAL(SQ_2F, SquareTraits::to_square(FILE2, RANK6));
    CHECK_EQUAL(SQ_3A, SquareTraits::to_square(FILE3, RANK1));
    CHECK_EQUAL(SQ_3B, SquareTraits::to_square(FILE3, RANK2));
    CHECK_EQUAL(SQ_3C, SquareTraits::to_square(FILE3, RANK3));
    CHECK_EQUAL(SQ_3D, SquareTraits::to_square(FILE3, RANK4));
    CHECK_EQUAL(SQ_3E, SquareTraits::to_square(FILE3, RANK5));
    CHECK_EQUAL(SQ_3F, SquareTraits::to_square(FILE3, RANK6));
    CHECK_EQUAL(SQ_4A, SquareTraits::to_square(FILE4, RANK1));
    CHECK_EQUAL(SQ_4B, SquareTraits::to_square(FILE4, RANK2));
    CHECK_EQUAL(SQ_4C, SquareTraits::to_square(FILE4, RANK3));
    CHECK_EQUAL(SQ_4D, SquareTraits::to_square(FILE4, RANK4));
    CHECK_EQUAL(SQ_4E, SquareTraits::to_square(FILE4, RANK5));
    CHECK_EQUAL(SQ_4F, SquareTraits::to_square(FILE4, RANK6));
}

TEST(test_leshogi_square_traits, test_in_promotion_zone)
{
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_4A, vshogi::BLACK));
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_4B, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_3C, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_2D, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_1E, vshogi::BLACK));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_1F, vshogi::BLACK));

    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_1A, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_2B, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_3C, vshogi::WHITE));
    CHECK_FALSE(SquareTraits::in_promotion_zone(SQ_4D, vshogi::WHITE));
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_4E, vshogi::WHITE));
    CHECK_TRUE(SquareTraits::in_promotion_zone(SQ_4F, vshogi::WHITE));
}

TEST(test_leshogi_square_traits, test_direction)
{
    CHECK_EQUAL(vshogi::DIR_NW, SquareTraits::direction(SQ_3C, SQ_4B));
    CHECK_EQUAL(vshogi::DIR_NA, SquareTraits::direction(SQ_4C, SQ_NA));
    CHECK_EQUAL(vshogi::DIR_SSE, SquareTraits::direction(SQ_4C, SQ_3E));
    CHECK_EQUAL(vshogi::DIR_NE, SquareTraits::direction(SQ_4D, SQ_1A));
}

TEST(test_leshogi_square_traits, test_chebyshev_distance)
{
    CHECK_EQUAL(1u, SquareTraits::chebyshev_distance(SQ_1A, SQ_1B));
    CHECK_EQUAL(2u, SquareTraits::chebyshev_distance(SQ_3A, SQ_1A));
    CHECK_EQUAL(3u, SquareTraits::chebyshev_distance(SQ_1B, SQ_4D));
}

TEST(test_leshogi_square_traits, test_ray_from)
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
        const auto actual = SquareTraits::ray_from(SQ_1A, vshogi::DIR_W);
        CHECK_EQUAL(SQ_2A, actual[0]);
        CHECK_EQUAL(SQ_3A, actual[1]);
        CHECK_EQUAL(SQ_4A, actual[2]);
        CHECK_EQUAL(SQ_NA, actual[3]);
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
        CHECK_EQUAL(SQ_1F, actual[1]);
        CHECK_EQUAL(SQ_NA, actual[2]);
    }
}

} // namespace test_vshogi::test_leshogi
