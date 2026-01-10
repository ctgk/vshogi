#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (test_shogi_square_traits) {
};

TEST(test_shogi_square_traits, to_rank)
{
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_1A));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_2A));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_3A));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_4A));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_5A));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_6A));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_7A));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_8A));
    CHECK_EQUAL(RANK1, SquareTraits::to_rank(SQ_9A));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_1B));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_2B));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_3B));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_4B));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_5B));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_6B));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_7B));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_8B));
    CHECK_EQUAL(RANK2, SquareTraits::to_rank(SQ_9B));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_1C));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_2C));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_3C));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_4C));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_5C));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_6C));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_7C));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_8C));
    CHECK_EQUAL(RANK3, SquareTraits::to_rank(SQ_9C));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_1D));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_2D));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_3D));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_4D));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_5D));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_6D));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_7D));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_8D));
    CHECK_EQUAL(RANK4, SquareTraits::to_rank(SQ_9D));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_1E));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_2E));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_3E));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_4E));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_5E));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_6E));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_7E));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_8E));
    CHECK_EQUAL(RANK5, SquareTraits::to_rank(SQ_9E));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_1F));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_2F));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_3F));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_4F));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_5F));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_6F));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_7F));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_8F));
    CHECK_EQUAL(RANK6, SquareTraits::to_rank(SQ_9F));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_1G));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_2G));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_3G));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_4G));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_5G));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_6G));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_7G));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_8G));
    CHECK_EQUAL(RANK7, SquareTraits::to_rank(SQ_9G));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_1H));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_2H));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_3H));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_4H));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_5H));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_6H));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_7H));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_8H));
    CHECK_EQUAL(RANK8, SquareTraits::to_rank(SQ_9H));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_1I));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_2I));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_3I));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_4I));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_5I));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_6I));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_7I));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_8I));
    CHECK_EQUAL(RANK9, SquareTraits::to_rank(SQ_9I));
}

TEST(test_shogi_square_traits, to_file)
{
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1A));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2A));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3A));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4A));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5A));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6A));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7A));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8A));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9A));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1B));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2B));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3B));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4B));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5B));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6B));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7B));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8B));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9B));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1C));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2C));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3C));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4C));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5C));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6C));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7C));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8C));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9C));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1D));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2D));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3D));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4D));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5D));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6D));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7D));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8D));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9D));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1E));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2E));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3E));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4E));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5E));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6E));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7E));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8E));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9E));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1F));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2F));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3F));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4F));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5F));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6F));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7F));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8F));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9F));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1G));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2G));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3G));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4G));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5G));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6G));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7G));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8G));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9G));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1H));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2H));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3H));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4H));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5H));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6H));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7H));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8H));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9H));
    CHECK_EQUAL(FILE1, SquareTraits::to_file(SQ_1I));
    CHECK_EQUAL(FILE2, SquareTraits::to_file(SQ_2I));
    CHECK_EQUAL(FILE3, SquareTraits::to_file(SQ_3I));
    CHECK_EQUAL(FILE4, SquareTraits::to_file(SQ_4I));
    CHECK_EQUAL(FILE5, SquareTraits::to_file(SQ_5I));
    CHECK_EQUAL(FILE6, SquareTraits::to_file(SQ_6I));
    CHECK_EQUAL(FILE7, SquareTraits::to_file(SQ_7I));
    CHECK_EQUAL(FILE8, SquareTraits::to_file(SQ_8I));
    CHECK_EQUAL(FILE9, SquareTraits::to_file(SQ_9I));
}

TEST(test_shogi_square_traits, to_square)
{
    CHECK_EQUAL(SQ_1A, SquareTraits::to_square(FILE1, RANK1));
    CHECK_EQUAL(SQ_1B, SquareTraits::to_square(FILE1, RANK2));
    CHECK_EQUAL(SQ_1C, SquareTraits::to_square(FILE1, RANK3));
    CHECK_EQUAL(SQ_1D, SquareTraits::to_square(FILE1, RANK4));
    CHECK_EQUAL(SQ_1E, SquareTraits::to_square(FILE1, RANK5));
    CHECK_EQUAL(SQ_1F, SquareTraits::to_square(FILE1, RANK6));
    CHECK_EQUAL(SQ_1G, SquareTraits::to_square(FILE1, RANK7));
    CHECK_EQUAL(SQ_1H, SquareTraits::to_square(FILE1, RANK8));
    CHECK_EQUAL(SQ_1I, SquareTraits::to_square(FILE1, RANK9));
    CHECK_EQUAL(SQ_2A, SquareTraits::to_square(FILE2, RANK1));
    CHECK_EQUAL(SQ_2B, SquareTraits::to_square(FILE2, RANK2));
    CHECK_EQUAL(SQ_2C, SquareTraits::to_square(FILE2, RANK3));
    CHECK_EQUAL(SQ_2D, SquareTraits::to_square(FILE2, RANK4));
    CHECK_EQUAL(SQ_2E, SquareTraits::to_square(FILE2, RANK5));
    CHECK_EQUAL(SQ_2F, SquareTraits::to_square(FILE2, RANK6));
    CHECK_EQUAL(SQ_2G, SquareTraits::to_square(FILE2, RANK7));
    CHECK_EQUAL(SQ_2H, SquareTraits::to_square(FILE2, RANK8));
    CHECK_EQUAL(SQ_2I, SquareTraits::to_square(FILE2, RANK9));
    CHECK_EQUAL(SQ_3A, SquareTraits::to_square(FILE3, RANK1));
    CHECK_EQUAL(SQ_3B, SquareTraits::to_square(FILE3, RANK2));
    CHECK_EQUAL(SQ_3C, SquareTraits::to_square(FILE3, RANK3));
    CHECK_EQUAL(SQ_3D, SquareTraits::to_square(FILE3, RANK4));
    CHECK_EQUAL(SQ_3E, SquareTraits::to_square(FILE3, RANK5));
    CHECK_EQUAL(SQ_3F, SquareTraits::to_square(FILE3, RANK6));
    CHECK_EQUAL(SQ_3G, SquareTraits::to_square(FILE3, RANK7));
    CHECK_EQUAL(SQ_3H, SquareTraits::to_square(FILE3, RANK8));
    CHECK_EQUAL(SQ_3I, SquareTraits::to_square(FILE3, RANK9));
    CHECK_EQUAL(SQ_4A, SquareTraits::to_square(FILE4, RANK1));
    CHECK_EQUAL(SQ_4B, SquareTraits::to_square(FILE4, RANK2));
    CHECK_EQUAL(SQ_4C, SquareTraits::to_square(FILE4, RANK3));
    CHECK_EQUAL(SQ_4D, SquareTraits::to_square(FILE4, RANK4));
    CHECK_EQUAL(SQ_4E, SquareTraits::to_square(FILE4, RANK5));
    CHECK_EQUAL(SQ_4F, SquareTraits::to_square(FILE4, RANK6));
    CHECK_EQUAL(SQ_4G, SquareTraits::to_square(FILE4, RANK7));
    CHECK_EQUAL(SQ_4H, SquareTraits::to_square(FILE4, RANK8));
    CHECK_EQUAL(SQ_4I, SquareTraits::to_square(FILE4, RANK9));
    CHECK_EQUAL(SQ_5A, SquareTraits::to_square(FILE5, RANK1));
    CHECK_EQUAL(SQ_5B, SquareTraits::to_square(FILE5, RANK2));
    CHECK_EQUAL(SQ_5C, SquareTraits::to_square(FILE5, RANK3));
    CHECK_EQUAL(SQ_5D, SquareTraits::to_square(FILE5, RANK4));
    CHECK_EQUAL(SQ_5E, SquareTraits::to_square(FILE5, RANK5));
    CHECK_EQUAL(SQ_5F, SquareTraits::to_square(FILE5, RANK6));
    CHECK_EQUAL(SQ_5G, SquareTraits::to_square(FILE5, RANK7));
    CHECK_EQUAL(SQ_5H, SquareTraits::to_square(FILE5, RANK8));
    CHECK_EQUAL(SQ_5I, SquareTraits::to_square(FILE5, RANK9));
    CHECK_EQUAL(SQ_6A, SquareTraits::to_square(FILE6, RANK1));
    CHECK_EQUAL(SQ_6B, SquareTraits::to_square(FILE6, RANK2));
    CHECK_EQUAL(SQ_6C, SquareTraits::to_square(FILE6, RANK3));
    CHECK_EQUAL(SQ_6D, SquareTraits::to_square(FILE6, RANK4));
    CHECK_EQUAL(SQ_6E, SquareTraits::to_square(FILE6, RANK5));
    CHECK_EQUAL(SQ_6F, SquareTraits::to_square(FILE6, RANK6));
    CHECK_EQUAL(SQ_6G, SquareTraits::to_square(FILE6, RANK7));
    CHECK_EQUAL(SQ_6H, SquareTraits::to_square(FILE6, RANK8));
    CHECK_EQUAL(SQ_6I, SquareTraits::to_square(FILE6, RANK9));
    CHECK_EQUAL(SQ_7A, SquareTraits::to_square(FILE7, RANK1));
    CHECK_EQUAL(SQ_7B, SquareTraits::to_square(FILE7, RANK2));
    CHECK_EQUAL(SQ_7C, SquareTraits::to_square(FILE7, RANK3));
    CHECK_EQUAL(SQ_7D, SquareTraits::to_square(FILE7, RANK4));
    CHECK_EQUAL(SQ_7E, SquareTraits::to_square(FILE7, RANK5));
    CHECK_EQUAL(SQ_7F, SquareTraits::to_square(FILE7, RANK6));
    CHECK_EQUAL(SQ_7G, SquareTraits::to_square(FILE7, RANK7));
    CHECK_EQUAL(SQ_7H, SquareTraits::to_square(FILE7, RANK8));
    CHECK_EQUAL(SQ_7I, SquareTraits::to_square(FILE7, RANK9));
    CHECK_EQUAL(SQ_8A, SquareTraits::to_square(FILE8, RANK1));
    CHECK_EQUAL(SQ_8B, SquareTraits::to_square(FILE8, RANK2));
    CHECK_EQUAL(SQ_8C, SquareTraits::to_square(FILE8, RANK3));
    CHECK_EQUAL(SQ_8D, SquareTraits::to_square(FILE8, RANK4));
    CHECK_EQUAL(SQ_8E, SquareTraits::to_square(FILE8, RANK5));
    CHECK_EQUAL(SQ_8F, SquareTraits::to_square(FILE8, RANK6));
    CHECK_EQUAL(SQ_8G, SquareTraits::to_square(FILE8, RANK7));
    CHECK_EQUAL(SQ_8H, SquareTraits::to_square(FILE8, RANK8));
    CHECK_EQUAL(SQ_8I, SquareTraits::to_square(FILE8, RANK9));
    CHECK_EQUAL(SQ_9A, SquareTraits::to_square(FILE9, RANK1));
    CHECK_EQUAL(SQ_9B, SquareTraits::to_square(FILE9, RANK2));
    CHECK_EQUAL(SQ_9C, SquareTraits::to_square(FILE9, RANK3));
    CHECK_EQUAL(SQ_9D, SquareTraits::to_square(FILE9, RANK4));
    CHECK_EQUAL(SQ_9E, SquareTraits::to_square(FILE9, RANK5));
    CHECK_EQUAL(SQ_9F, SquareTraits::to_square(FILE9, RANK6));
    CHECK_EQUAL(SQ_9G, SquareTraits::to_square(FILE9, RANK7));
    CHECK_EQUAL(SQ_9H, SquareTraits::to_square(FILE9, RANK8));
    CHECK_EQUAL(SQ_9I, SquareTraits::to_square(FILE9, RANK9));
}

TEST(test_shogi_square_traits, shift)
{
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_NNW));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_NNE));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_N));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_NE));
    CHECK_EQUAL(SQ_2A, SquareTraits::shift(SQ_1A, vshogi::DIR_W));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_E));
    CHECK_EQUAL(SQ_2B, SquareTraits::shift(SQ_1A, vshogi::DIR_SW));
    CHECK_EQUAL(SQ_1B, SquareTraits::shift(SQ_1A, vshogi::DIR_S));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_SE));
    CHECK_EQUAL(SQ_2C, SquareTraits::shift(SQ_1A, vshogi::DIR_SSW));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_1A, vshogi::DIR_SSE));

    CHECK_EQUAL(SQ_6C, SquareTraits::shift(SQ_5E, vshogi::DIR_NNW));
    CHECK_EQUAL(SQ_4C, SquareTraits::shift(SQ_5E, vshogi::DIR_NNE));
    CHECK_EQUAL(SQ_6D, SquareTraits::shift(SQ_5E, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_5D, SquareTraits::shift(SQ_5E, vshogi::DIR_N));
    CHECK_EQUAL(SQ_4D, SquareTraits::shift(SQ_5E, vshogi::DIR_NE));
    CHECK_EQUAL(SQ_6E, SquareTraits::shift(SQ_5E, vshogi::DIR_W));
    CHECK_EQUAL(SQ_4E, SquareTraits::shift(SQ_5E, vshogi::DIR_E));
    CHECK_EQUAL(SQ_6F, SquareTraits::shift(SQ_5E, vshogi::DIR_SW));
    CHECK_EQUAL(SQ_5F, SquareTraits::shift(SQ_5E, vshogi::DIR_S));
    CHECK_EQUAL(SQ_4F, SquareTraits::shift(SQ_5E, vshogi::DIR_SE));
    CHECK_EQUAL(SQ_6G, SquareTraits::shift(SQ_5E, vshogi::DIR_SSW));
    CHECK_EQUAL(SQ_4G, SquareTraits::shift(SQ_5E, vshogi::DIR_SSE));

    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_9I, vshogi::DIR_NNW));
    CHECK_EQUAL(SQ_8G, SquareTraits::shift(SQ_9I, vshogi::DIR_NNE));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_9I, vshogi::DIR_NW));
    CHECK_EQUAL(SQ_9H, SquareTraits::shift(SQ_9I, vshogi::DIR_N));
    CHECK_EQUAL(SQ_8H, SquareTraits::shift(SQ_9I, vshogi::DIR_NE));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_9I, vshogi::DIR_W));
    CHECK_EQUAL(SQ_8I, SquareTraits::shift(SQ_9I, vshogi::DIR_E));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_9I, vshogi::DIR_SW));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_9I, vshogi::DIR_S));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_9I, vshogi::DIR_SE));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_9I, vshogi::DIR_SSW));
    CHECK_EQUAL(SQ_NA, SquareTraits::shift(SQ_9I, vshogi::DIR_SSE));
}

TEST(test_shogi_square_traits, direction)
{
    CHECK_EQUAL(vshogi::DIR_NW, SquareTraits::direction(SQ_1B, SQ_2A));
    CHECK_EQUAL(vshogi::DIR_N, SquareTraits::direction(SQ_8F, SQ_8B));
    CHECK_EQUAL(vshogi::DIR_NE, SquareTraits::direction(SQ_6F, SQ_1A));
    CHECK_EQUAL(vshogi::DIR_W, SquareTraits::direction(SQ_1D, SQ_9D));
    CHECK_EQUAL(vshogi::DIR_E, SquareTraits::direction(SQ_9D, SQ_1D));
    CHECK_EQUAL(vshogi::DIR_SW, SquareTraits::direction(SQ_3A, SQ_8F));
    CHECK_EQUAL(vshogi::DIR_S, SquareTraits::direction(SQ_1A, SQ_1I));
    CHECK_EQUAL(vshogi::DIR_SE, SquareTraits::direction(SQ_9A, SQ_1I));
    CHECK_EQUAL(vshogi::DIR_SSW, SquareTraits::direction(SQ_7G, SQ_8I));
    CHECK_EQUAL(vshogi::DIR_SSE, SquareTraits::direction(SQ_3G, SQ_2I));
    CHECK_EQUAL(vshogi::DIR_NA, SquareTraits::direction(SQ_2B, SQ_9A));
    CHECK_EQUAL(vshogi::DIR_NA, SquareTraits::direction(SQ_1A, SQ_5C));
}

TEST(test_shogi_square_traits, ray_from)
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
        const auto actual = SquareTraits::ray_from(SQ_1H, vshogi::DIR_S);
        CHECK_EQUAL(SQ_1I, actual[0]);
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

} // namespace test_vshogi::test_shogi
