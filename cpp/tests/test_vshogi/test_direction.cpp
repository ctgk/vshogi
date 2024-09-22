#include "vshogi/common/direction.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi
{

using namespace vshogi;

TEST_GROUP (test_direction) {
};

TEST(test_direction, rotate)
{
    CHECK_EQUAL(DIR_SE, rotate(DIR_NW));
    CHECK_EQUAL(DIR_S, rotate(DIR_N));
    CHECK_EQUAL(DIR_SW, rotate(DIR_NE));
    CHECK_EQUAL(DIR_E, rotate(DIR_W));
    CHECK_EQUAL(DIR_W, rotate(DIR_E));
    CHECK_EQUAL(DIR_NE, rotate(DIR_SW));
    CHECK_EQUAL(DIR_N, rotate(DIR_S));
    CHECK_EQUAL(DIR_NW, rotate(DIR_SE));
    CHECK_EQUAL(DIR_NNE, rotate(DIR_SSW));
    CHECK_EQUAL(DIR_NNW, rotate(DIR_SSE));
    CHECK_EQUAL(DIR_SSE, rotate(DIR_NNW));
    CHECK_EQUAL(DIR_SSW, rotate(DIR_NNE));
    CHECK_EQUAL(DIR_NA, rotate(DIR_NA));
}

TEST(test_direction, is_adjacent_direction)
{
    CHECK_FALSE(is_adjacent_direction(DIR_NW));
    CHECK_TRUE(is_adjacent_direction(DIR_N));
    CHECK_FALSE(is_adjacent_direction(DIR_NE));
    CHECK_TRUE(is_adjacent_direction(DIR_W));
    CHECK_TRUE(is_adjacent_direction(DIR_E));
    CHECK_FALSE(is_adjacent_direction(DIR_SW));
    CHECK_TRUE(is_adjacent_direction(DIR_S));
    CHECK_FALSE(is_adjacent_direction(DIR_SE));
    CHECK_FALSE(is_adjacent_direction(DIR_SSW));
    CHECK_FALSE(is_adjacent_direction(DIR_SSE));
    CHECK_FALSE(is_adjacent_direction(DIR_NNW));
    CHECK_FALSE(is_adjacent_direction(DIR_NNE));
    CHECK_FALSE(is_adjacent_direction(DIR_NA));
}

TEST(test_direction, is_diagonal_direction)
{
    CHECK_TRUE(is_diagonal_direction(DIR_NW));
    CHECK_FALSE(is_diagonal_direction(DIR_N));
    CHECK_TRUE(is_diagonal_direction(DIR_NE));
    CHECK_FALSE(is_diagonal_direction(DIR_W));
    CHECK_FALSE(is_diagonal_direction(DIR_E));
    CHECK_TRUE(is_diagonal_direction(DIR_SW));
    CHECK_FALSE(is_diagonal_direction(DIR_S));
    CHECK_TRUE(is_diagonal_direction(DIR_SE));
    CHECK_FALSE(is_diagonal_direction(DIR_SSW));
    CHECK_FALSE(is_diagonal_direction(DIR_SSE));
    CHECK_FALSE(is_diagonal_direction(DIR_NNW));
    CHECK_FALSE(is_diagonal_direction(DIR_NNE));
    CHECK_FALSE(is_diagonal_direction(DIR_NA));
}

TEST(test_direction, is_knight_direction)
{
    CHECK_FALSE(is_knight_direction(DIR_NW));
    CHECK_FALSE(is_knight_direction(DIR_N));
    CHECK_FALSE(is_knight_direction(DIR_NE));
    CHECK_FALSE(is_knight_direction(DIR_W));
    CHECK_FALSE(is_knight_direction(DIR_E));
    CHECK_FALSE(is_knight_direction(DIR_SW));
    CHECK_FALSE(is_knight_direction(DIR_S));
    CHECK_FALSE(is_knight_direction(DIR_SE));
    CHECK_TRUE(is_knight_direction(DIR_SSW));
    CHECK_TRUE(is_knight_direction(DIR_SSE));
    CHECK_TRUE(is_knight_direction(DIR_NNW));
    CHECK_TRUE(is_knight_direction(DIR_NNE));
    CHECK_FALSE(is_knight_direction(DIR_NA));
}

} // namespace test_vshogi
