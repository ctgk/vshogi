#include "vshogi/shogi/squares.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_shogi/test_shogi.hpp"

namespace test_vshogi::test_shogi
{

using namespace vshogi;
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

TEST(squares, is_edge)
{
    CHECK_TRUE(is_edge(RANK1));
    CHECK_FALSE(is_edge(RANK2));
    CHECK_FALSE(is_edge(RANK3));
    CHECK_FALSE(is_edge(RANK4));
    CHECK_FALSE(is_edge(RANK5));
    CHECK_FALSE(is_edge(RANK6));
    CHECK_FALSE(is_edge(RANK7));
    CHECK_FALSE(is_edge(RANK8));
    CHECK_TRUE(is_edge(RANK9));

    CHECK_TRUE(is_edge(FILE1));
    CHECK_FALSE(is_edge(FILE2));
    CHECK_FALSE(is_edge(FILE3));
    CHECK_FALSE(is_edge(FILE4));
    CHECK_FALSE(is_edge(FILE5));
    CHECK_FALSE(is_edge(FILE6));
    CHECK_FALSE(is_edge(FILE7));
    CHECK_FALSE(is_edge(FILE8));
    CHECK_TRUE(is_edge(FILE9));

    CHECK_TRUE(is_edge(SQ_1A));
    CHECK_TRUE(is_edge(SQ_2A));
    CHECK_TRUE(is_edge(SQ_3A));
    CHECK_TRUE(is_edge(SQ_4A));
    CHECK_TRUE(is_edge(SQ_5A));
    CHECK_TRUE(is_edge(SQ_6A));
    CHECK_TRUE(is_edge(SQ_7A));
    CHECK_TRUE(is_edge(SQ_8A));
    CHECK_TRUE(is_edge(SQ_9A));
    CHECK_TRUE(is_edge(SQ_1B));
    CHECK_FALSE(is_edge(SQ_2B));
    CHECK_FALSE(is_edge(SQ_3B));
    CHECK_FALSE(is_edge(SQ_4B));
    CHECK_FALSE(is_edge(SQ_5B));
    CHECK_FALSE(is_edge(SQ_6B));
    CHECK_FALSE(is_edge(SQ_7B));
    CHECK_FALSE(is_edge(SQ_8B));
    CHECK_TRUE(is_edge(SQ_9B));
    CHECK_TRUE(is_edge(SQ_1C));
    CHECK_FALSE(is_edge(SQ_2C));
    CHECK_FALSE(is_edge(SQ_3C));
    CHECK_FALSE(is_edge(SQ_4C));
    CHECK_FALSE(is_edge(SQ_5C));
    CHECK_FALSE(is_edge(SQ_6C));
    CHECK_FALSE(is_edge(SQ_7C));
    CHECK_FALSE(is_edge(SQ_8C));
    CHECK_TRUE(is_edge(SQ_9C));
    CHECK_TRUE(is_edge(SQ_1D));
    CHECK_FALSE(is_edge(SQ_2D));
    CHECK_FALSE(is_edge(SQ_3D));
    CHECK_FALSE(is_edge(SQ_4D));
    CHECK_FALSE(is_edge(SQ_5D));
    CHECK_FALSE(is_edge(SQ_6D));
    CHECK_FALSE(is_edge(SQ_7D));
    CHECK_FALSE(is_edge(SQ_8D));
    CHECK_TRUE(is_edge(SQ_9D));
    CHECK_TRUE(is_edge(SQ_1E));
    CHECK_FALSE(is_edge(SQ_2E));
    CHECK_FALSE(is_edge(SQ_3E));
    CHECK_FALSE(is_edge(SQ_4E));
    CHECK_FALSE(is_edge(SQ_5E));
    CHECK_FALSE(is_edge(SQ_6E));
    CHECK_FALSE(is_edge(SQ_7E));
    CHECK_FALSE(is_edge(SQ_8E));
    CHECK_TRUE(is_edge(SQ_9E));
    CHECK_TRUE(is_edge(SQ_1F));
    CHECK_FALSE(is_edge(SQ_2F));
    CHECK_FALSE(is_edge(SQ_3F));
    CHECK_FALSE(is_edge(SQ_4F));
    CHECK_FALSE(is_edge(SQ_5F));
    CHECK_FALSE(is_edge(SQ_6F));
    CHECK_FALSE(is_edge(SQ_7F));
    CHECK_FALSE(is_edge(SQ_8F));
    CHECK_TRUE(is_edge(SQ_9F));
    CHECK_TRUE(is_edge(SQ_1G));
    CHECK_FALSE(is_edge(SQ_2G));
    CHECK_FALSE(is_edge(SQ_3G));
    CHECK_FALSE(is_edge(SQ_4G));
    CHECK_FALSE(is_edge(SQ_5G));
    CHECK_FALSE(is_edge(SQ_6G));
    CHECK_FALSE(is_edge(SQ_7G));
    CHECK_FALSE(is_edge(SQ_8G));
    CHECK_TRUE(is_edge(SQ_9G));
    CHECK_TRUE(is_edge(SQ_1H));
    CHECK_FALSE(is_edge(SQ_2H));
    CHECK_FALSE(is_edge(SQ_3H));
    CHECK_FALSE(is_edge(SQ_4H));
    CHECK_FALSE(is_edge(SQ_5H));
    CHECK_FALSE(is_edge(SQ_6H));
    CHECK_FALSE(is_edge(SQ_7H));
    CHECK_FALSE(is_edge(SQ_8H));
    CHECK_TRUE(is_edge(SQ_9H));
    CHECK_TRUE(is_edge(SQ_1I));
    CHECK_TRUE(is_edge(SQ_2I));
    CHECK_TRUE(is_edge(SQ_3I));
    CHECK_TRUE(is_edge(SQ_4I));
    CHECK_TRUE(is_edge(SQ_5I));
    CHECK_TRUE(is_edge(SQ_6I));
    CHECK_TRUE(is_edge(SQ_7I));
    CHECK_TRUE(is_edge(SQ_8I));
    CHECK_TRUE(is_edge(SQ_9I));
}

TEST(squares, shift)
{
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, DIR_NNW));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, DIR_NNE));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, DIR_NW));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, DIR_N));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, DIR_NE));
    CHECK_EQUAL(SQ_2A, shift(SQ_1A, DIR_W));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, DIR_E));
    CHECK_EQUAL(SQ_2B, shift(SQ_1A, DIR_SW));
    CHECK_EQUAL(SQ_1B, shift(SQ_1A, DIR_S));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, DIR_SE));
    CHECK_EQUAL(SQ_2C, shift(SQ_1A, DIR_SSW));
    CHECK_EQUAL(SQ_NA, shift(SQ_1A, DIR_SSE));

    CHECK_EQUAL(SQ_6C, shift(SQ_5E, DIR_NNW));
    CHECK_EQUAL(SQ_4C, shift(SQ_5E, DIR_NNE));
    CHECK_EQUAL(SQ_6D, shift(SQ_5E, DIR_NW));
    CHECK_EQUAL(SQ_5D, shift(SQ_5E, DIR_N));
    CHECK_EQUAL(SQ_4D, shift(SQ_5E, DIR_NE));
    CHECK_EQUAL(SQ_6E, shift(SQ_5E, DIR_W));
    CHECK_EQUAL(SQ_4E, shift(SQ_5E, DIR_E));
    CHECK_EQUAL(SQ_6F, shift(SQ_5E, DIR_SW));
    CHECK_EQUAL(SQ_5F, shift(SQ_5E, DIR_S));
    CHECK_EQUAL(SQ_4F, shift(SQ_5E, DIR_SE));
    CHECK_EQUAL(SQ_6G, shift(SQ_5E, DIR_SSW));
    CHECK_EQUAL(SQ_4G, shift(SQ_5E, DIR_SSE));

    CHECK_EQUAL(SQ_NA, shift(SQ_9I, DIR_NNW));
    CHECK_EQUAL(SQ_8G, shift(SQ_9I, DIR_NNE));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, DIR_NW));
    CHECK_EQUAL(SQ_9H, shift(SQ_9I, DIR_N));
    CHECK_EQUAL(SQ_8H, shift(SQ_9I, DIR_NE));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, DIR_W));
    CHECK_EQUAL(SQ_8I, shift(SQ_9I, DIR_E));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, DIR_SW));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, DIR_S));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, DIR_SE));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, DIR_SSW));
    CHECK_EQUAL(SQ_NA, shift(SQ_9I, DIR_SSE));
}

TEST(squares, get_direction_of_src)
{
    CHECK_EQUAL(DIR_NW, get_direction_of_src(SQ_1B, SQ_2A));
    CHECK_EQUAL(DIR_N, get_direction_of_src(SQ_8F, SQ_8B));
    CHECK_EQUAL(DIR_NE, get_direction_of_src(SQ_6F, SQ_1A));
    CHECK_EQUAL(DIR_W, get_direction_of_src(SQ_1D, SQ_9D));
    CHECK_EQUAL(DIR_E, get_direction_of_src(SQ_9D, SQ_1D));
    CHECK_EQUAL(DIR_SW, get_direction_of_src(SQ_3A, SQ_8F));
    CHECK_EQUAL(DIR_S, get_direction_of_src(SQ_1A, SQ_1I));
    CHECK_EQUAL(DIR_SE, get_direction_of_src(SQ_9A, SQ_1I));
    CHECK_EQUAL(DIR_SSW, get_direction_of_src(SQ_7G, SQ_8I));
    CHECK_EQUAL(DIR_SSE, get_direction_of_src(SQ_3G, SQ_2I));
}

} // namespace test_vshogi::test_shogi
