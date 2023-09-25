#include "vshogi/judkins_shogi/squares.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_judkins_shogi/test_judkins_shogi.hpp"

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi;
using namespace vshogi::judkins_shogi;

TEST_GROUP(squares){};

TEST(squares, shift)
{
    CHECK_EQUAL(SQ_4A, shift(SQ_3C, DIR_NNW));
    CHECK_EQUAL(SQ_2A, shift(SQ_3C, DIR_NNE));
    CHECK_EQUAL(SQ_4B, shift(SQ_3C, DIR_NW));
    CHECK_EQUAL(SQ_3B, shift(SQ_3C, DIR_N));
    CHECK_EQUAL(SQ_2B, shift(SQ_3C, DIR_NE));
    CHECK_EQUAL(SQ_4C, shift(SQ_3C, DIR_W));
    CHECK_EQUAL(SQ_2C, shift(SQ_3C, DIR_E));
    CHECK_EQUAL(SQ_4D, shift(SQ_3C, DIR_SW));
    CHECK_EQUAL(SQ_3D, shift(SQ_3C, DIR_S));
    CHECK_EQUAL(SQ_2D, shift(SQ_3C, DIR_SE));
    CHECK_EQUAL(SQ_4E, shift(SQ_3C, DIR_SSW));
    CHECK_EQUAL(SQ_2E, shift(SQ_3C, DIR_SSE));
}

TEST(squares, to_rank)
{
    CHECK_EQUAL(RANK1, to_rank(SQ_1A));
    CHECK_EQUAL(RANK2, to_rank(SQ_1B));
    CHECK_EQUAL(RANK3, to_rank(SQ_1C));
    CHECK_EQUAL(RANK4, to_rank(SQ_1D));
    CHECK_EQUAL(RANK5, to_rank(SQ_1E));
    CHECK_EQUAL(RANK6, to_rank(SQ_1F));
    CHECK_EQUAL(RANK1, to_rank(SQ_2A));
    CHECK_EQUAL(RANK2, to_rank(SQ_2B));
    CHECK_EQUAL(RANK3, to_rank(SQ_2C));
    CHECK_EQUAL(RANK4, to_rank(SQ_2D));
    CHECK_EQUAL(RANK5, to_rank(SQ_2E));
    CHECK_EQUAL(RANK6, to_rank(SQ_2F));
    CHECK_EQUAL(RANK1, to_rank(SQ_3A));
    CHECK_EQUAL(RANK2, to_rank(SQ_3B));
    CHECK_EQUAL(RANK3, to_rank(SQ_3C));
    CHECK_EQUAL(RANK4, to_rank(SQ_3D));
    CHECK_EQUAL(RANK5, to_rank(SQ_3E));
    CHECK_EQUAL(RANK6, to_rank(SQ_3F));
    CHECK_EQUAL(RANK1, to_rank(SQ_4A));
    CHECK_EQUAL(RANK2, to_rank(SQ_4B));
    CHECK_EQUAL(RANK3, to_rank(SQ_4C));
    CHECK_EQUAL(RANK4, to_rank(SQ_4D));
    CHECK_EQUAL(RANK5, to_rank(SQ_4E));
    CHECK_EQUAL(RANK6, to_rank(SQ_4F));
    CHECK_EQUAL(RANK1, to_rank(SQ_5A));
    CHECK_EQUAL(RANK2, to_rank(SQ_5B));
    CHECK_EQUAL(RANK3, to_rank(SQ_5C));
    CHECK_EQUAL(RANK4, to_rank(SQ_5D));
    CHECK_EQUAL(RANK5, to_rank(SQ_5E));
    CHECK_EQUAL(RANK6, to_rank(SQ_5F));
}

TEST(squares, to_file)
{
    CHECK_EQUAL(FILE1, to_file(SQ_1A));
    CHECK_EQUAL(FILE1, to_file(SQ_1B));
    CHECK_EQUAL(FILE1, to_file(SQ_1C));
    CHECK_EQUAL(FILE1, to_file(SQ_1D));
    CHECK_EQUAL(FILE1, to_file(SQ_1E));
    CHECK_EQUAL(FILE1, to_file(SQ_1F));
    CHECK_EQUAL(FILE2, to_file(SQ_2A));
    CHECK_EQUAL(FILE2, to_file(SQ_2B));
    CHECK_EQUAL(FILE2, to_file(SQ_2C));
    CHECK_EQUAL(FILE2, to_file(SQ_2D));
    CHECK_EQUAL(FILE2, to_file(SQ_2E));
    CHECK_EQUAL(FILE2, to_file(SQ_2F));
    CHECK_EQUAL(FILE3, to_file(SQ_3A));
    CHECK_EQUAL(FILE3, to_file(SQ_3B));
    CHECK_EQUAL(FILE3, to_file(SQ_3C));
    CHECK_EQUAL(FILE3, to_file(SQ_3D));
    CHECK_EQUAL(FILE3, to_file(SQ_3E));
    CHECK_EQUAL(FILE3, to_file(SQ_3F));
    CHECK_EQUAL(FILE4, to_file(SQ_4A));
    CHECK_EQUAL(FILE4, to_file(SQ_4B));
    CHECK_EQUAL(FILE4, to_file(SQ_4C));
    CHECK_EQUAL(FILE4, to_file(SQ_4D));
    CHECK_EQUAL(FILE4, to_file(SQ_4E));
    CHECK_EQUAL(FILE4, to_file(SQ_4F));
    CHECK_EQUAL(FILE5, to_file(SQ_5A));
    CHECK_EQUAL(FILE5, to_file(SQ_5B));
    CHECK_EQUAL(FILE5, to_file(SQ_5C));
    CHECK_EQUAL(FILE5, to_file(SQ_5D));
    CHECK_EQUAL(FILE5, to_file(SQ_5E));
    CHECK_EQUAL(FILE5, to_file(SQ_5F));
}

TEST(squares, is_edge)
{
    {
        CHECK_TRUE(is_edge(RANK1));
        CHECK_FALSE(is_edge(RANK2));
        CHECK_FALSE(is_edge(RANK3));
        CHECK_FALSE(is_edge(RANK4));
        CHECK_FALSE(is_edge(RANK5));
        CHECK_TRUE(is_edge(RANK6));
    }
    {
        CHECK_TRUE(is_edge(FILE1));
        CHECK_FALSE(is_edge(FILE2));
        CHECK_FALSE(is_edge(FILE3));
        CHECK_FALSE(is_edge(FILE4));
        CHECK_FALSE(is_edge(FILE5));
        CHECK_TRUE(is_edge(FILE6));
    }
}

TEST(squares, square_array)
{
    CHECK_EQUAL(
        num_squares,
        sizeof(Squares::square_array) / sizeof(Squares::square_array[0]));
    for (int i = num_squares; i--;) {
        CHECK_EQUAL(i, static_cast<int>(Squares::square_array[i]));
    }
}

TEST(squares, get_direction_of_src)
{
    CHECK_EQUAL(DIR_NW, get_direction_of_src(SQ_1B, SQ_2A));
    CHECK_EQUAL(DIR_N, get_direction_of_src(SQ_6F, SQ_6B));
    CHECK_EQUAL(DIR_NE, get_direction_of_src(SQ_6F, SQ_1A));
    CHECK_EQUAL(DIR_W, get_direction_of_src(SQ_1D, SQ_4D));
    CHECK_EQUAL(DIR_E, get_direction_of_src(SQ_3D, SQ_1D));
    CHECK_EQUAL(DIR_SW, get_direction_of_src(SQ_3A, SQ_4B));
    CHECK_EQUAL(DIR_S, get_direction_of_src(SQ_1A, SQ_1E));
    CHECK_EQUAL(DIR_SE, get_direction_of_src(SQ_6A, SQ_1F));
    CHECK_EQUAL(DIR_SSW, get_direction_of_src(SQ_5A, SQ_6C));
    CHECK_EQUAL(DIR_SSE, get_direction_of_src(SQ_3B, SQ_2D));
}

} // namespace test_vshogi::test_judkins_shogi
