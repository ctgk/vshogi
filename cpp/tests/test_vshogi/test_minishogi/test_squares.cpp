#include "vshogi/minishogi/squares.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(squares){};

TEST(squares, shift)
{
    CHECK_EQUAL(SQ_1A, shift(SQ_1B, DIR_N));
    CHECK_EQUAL(SQ_1C, shift(SQ_1B, DIR_S));
}

TEST(squares, to_rank)
{
    CHECK_EQUAL(RANK1, to_rank(SQ_1A));
    CHECK_EQUAL(RANK2, to_rank(SQ_1B));
    CHECK_EQUAL(RANK3, to_rank(SQ_1C));
    CHECK_EQUAL(RANK4, to_rank(SQ_1D));
    CHECK_EQUAL(RANK5, to_rank(SQ_1E));
    CHECK_EQUAL(RANK1, to_rank(SQ_2A));
    CHECK_EQUAL(RANK2, to_rank(SQ_2B));
    CHECK_EQUAL(RANK3, to_rank(SQ_2C));
    CHECK_EQUAL(RANK4, to_rank(SQ_2D));
    CHECK_EQUAL(RANK5, to_rank(SQ_2E));
    CHECK_EQUAL(RANK1, to_rank(SQ_3A));
    CHECK_EQUAL(RANK2, to_rank(SQ_3B));
    CHECK_EQUAL(RANK3, to_rank(SQ_3C));
    CHECK_EQUAL(RANK4, to_rank(SQ_3D));
    CHECK_EQUAL(RANK5, to_rank(SQ_3E));
    CHECK_EQUAL(RANK1, to_rank(SQ_4A));
    CHECK_EQUAL(RANK2, to_rank(SQ_4B));
    CHECK_EQUAL(RANK3, to_rank(SQ_4C));
    CHECK_EQUAL(RANK4, to_rank(SQ_4D));
    CHECK_EQUAL(RANK5, to_rank(SQ_4E));
    CHECK_EQUAL(RANK1, to_rank(SQ_5A));
    CHECK_EQUAL(RANK2, to_rank(SQ_5B));
    CHECK_EQUAL(RANK3, to_rank(SQ_5C));
    CHECK_EQUAL(RANK4, to_rank(SQ_5D));
    CHECK_EQUAL(RANK5, to_rank(SQ_5E));
}

TEST(squares, to_file)
{
    CHECK_EQUAL(FILE1, to_file(SQ_1A));
    CHECK_EQUAL(FILE1, to_file(SQ_1B));
    CHECK_EQUAL(FILE1, to_file(SQ_1C));
    CHECK_EQUAL(FILE1, to_file(SQ_1D));
    CHECK_EQUAL(FILE1, to_file(SQ_1E));
    CHECK_EQUAL(FILE2, to_file(SQ_2A));
    CHECK_EQUAL(FILE2, to_file(SQ_2B));
    CHECK_EQUAL(FILE2, to_file(SQ_2C));
    CHECK_EQUAL(FILE2, to_file(SQ_2D));
    CHECK_EQUAL(FILE2, to_file(SQ_2E));
    CHECK_EQUAL(FILE3, to_file(SQ_3A));
    CHECK_EQUAL(FILE3, to_file(SQ_3B));
    CHECK_EQUAL(FILE3, to_file(SQ_3C));
    CHECK_EQUAL(FILE3, to_file(SQ_3D));
    CHECK_EQUAL(FILE3, to_file(SQ_3E));
    CHECK_EQUAL(FILE4, to_file(SQ_4A));
    CHECK_EQUAL(FILE4, to_file(SQ_4B));
    CHECK_EQUAL(FILE4, to_file(SQ_4C));
    CHECK_EQUAL(FILE4, to_file(SQ_4D));
    CHECK_EQUAL(FILE4, to_file(SQ_4E));
    CHECK_EQUAL(FILE5, to_file(SQ_5A));
    CHECK_EQUAL(FILE5, to_file(SQ_5B));
    CHECK_EQUAL(FILE5, to_file(SQ_5C));
    CHECK_EQUAL(FILE5, to_file(SQ_5D));
    CHECK_EQUAL(FILE5, to_file(SQ_5E));
}

TEST(squares, is_edge)
{
    {
        CHECK_TRUE(is_edge(RANK1));
        CHECK_FALSE(is_edge(RANK2));
        CHECK_FALSE(is_edge(RANK3));
        CHECK_FALSE(is_edge(RANK4));
        CHECK_TRUE(is_edge(RANK5));
    }
    {
        CHECK_TRUE(is_edge(FILE1));
        CHECK_FALSE(is_edge(FILE2));
        CHECK_FALSE(is_edge(FILE3));
        CHECK_FALSE(is_edge(FILE4));
        CHECK_TRUE(is_edge(FILE5));
    }
}

TEST(squares, square_array)
{
    CHECK_EQUAL(num_squares, sizeof(square_array) / sizeof(square_array[0]));
    for (int i = num_squares; i--;) {
        CHECK_EQUAL(i, static_cast<int>(square_array[i]));
    }
}

} // namespace test_vshogi::test_minishogi
