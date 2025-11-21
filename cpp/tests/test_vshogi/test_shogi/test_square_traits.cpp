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
    CHECK_EQUAL(SQ_1A, SquareTraits::to_square("1a"));
    CHECK_EQUAL(SQ_1B, SquareTraits::to_square("1b"));
    CHECK_EQUAL(SQ_1C, SquareTraits::to_square("1c"));
    CHECK_EQUAL(SQ_1D, SquareTraits::to_square("1d"));
    CHECK_EQUAL(SQ_1E, SquareTraits::to_square("1e"));
    CHECK_EQUAL(SQ_1F, SquareTraits::to_square("1f"));
    CHECK_EQUAL(SQ_1G, SquareTraits::to_square("1g"));
    CHECK_EQUAL(SQ_1H, SquareTraits::to_square("1h"));
    CHECK_EQUAL(SQ_1I, SquareTraits::to_square("1i"));
    CHECK_EQUAL(SQ_2A, SquareTraits::to_square("2a"));
    CHECK_EQUAL(SQ_2B, SquareTraits::to_square("2b"));
    CHECK_EQUAL(SQ_2C, SquareTraits::to_square("2c"));
    CHECK_EQUAL(SQ_2D, SquareTraits::to_square("2d"));
    CHECK_EQUAL(SQ_2E, SquareTraits::to_square("2e"));
    CHECK_EQUAL(SQ_2F, SquareTraits::to_square("2f"));
    CHECK_EQUAL(SQ_2G, SquareTraits::to_square("2g"));
    CHECK_EQUAL(SQ_2H, SquareTraits::to_square("2h"));
    CHECK_EQUAL(SQ_2I, SquareTraits::to_square("2i"));
    CHECK_EQUAL(SQ_3A, SquareTraits::to_square("3a"));
    CHECK_EQUAL(SQ_3B, SquareTraits::to_square("3b"));
    CHECK_EQUAL(SQ_3C, SquareTraits::to_square("3c"));
    CHECK_EQUAL(SQ_3D, SquareTraits::to_square("3d"));
    CHECK_EQUAL(SQ_3E, SquareTraits::to_square("3e"));
    CHECK_EQUAL(SQ_3F, SquareTraits::to_square("3f"));
    CHECK_EQUAL(SQ_3G, SquareTraits::to_square("3g"));
    CHECK_EQUAL(SQ_3H, SquareTraits::to_square("3h"));
    CHECK_EQUAL(SQ_3I, SquareTraits::to_square("3i"));
    CHECK_EQUAL(SQ_4A, SquareTraits::to_square("4a"));
    CHECK_EQUAL(SQ_4B, SquareTraits::to_square("4b"));
    CHECK_EQUAL(SQ_4C, SquareTraits::to_square("4c"));
    CHECK_EQUAL(SQ_4D, SquareTraits::to_square("4d"));
    CHECK_EQUAL(SQ_4E, SquareTraits::to_square("4e"));
    CHECK_EQUAL(SQ_4F, SquareTraits::to_square("4f"));
    CHECK_EQUAL(SQ_4G, SquareTraits::to_square("4g"));
    CHECK_EQUAL(SQ_4H, SquareTraits::to_square("4h"));
    CHECK_EQUAL(SQ_4I, SquareTraits::to_square("4i"));
    CHECK_EQUAL(SQ_5A, SquareTraits::to_square("5a"));
    CHECK_EQUAL(SQ_5B, SquareTraits::to_square("5b"));
    CHECK_EQUAL(SQ_5C, SquareTraits::to_square("5c"));
    CHECK_EQUAL(SQ_5D, SquareTraits::to_square("5d"));
    CHECK_EQUAL(SQ_5E, SquareTraits::to_square("5e"));
    CHECK_EQUAL(SQ_5F, SquareTraits::to_square("5f"));
    CHECK_EQUAL(SQ_5G, SquareTraits::to_square("5g"));
    CHECK_EQUAL(SQ_5H, SquareTraits::to_square("5h"));
    CHECK_EQUAL(SQ_5I, SquareTraits::to_square("5i"));
    CHECK_EQUAL(SQ_6A, SquareTraits::to_square("6a"));
    CHECK_EQUAL(SQ_6B, SquareTraits::to_square("6b"));
    CHECK_EQUAL(SQ_6C, SquareTraits::to_square("6c"));
    CHECK_EQUAL(SQ_6D, SquareTraits::to_square("6d"));
    CHECK_EQUAL(SQ_6E, SquareTraits::to_square("6e"));
    CHECK_EQUAL(SQ_6F, SquareTraits::to_square("6f"));
    CHECK_EQUAL(SQ_6G, SquareTraits::to_square("6g"));
    CHECK_EQUAL(SQ_6H, SquareTraits::to_square("6h"));
    CHECK_EQUAL(SQ_6I, SquareTraits::to_square("6i"));
    CHECK_EQUAL(SQ_7A, SquareTraits::to_square("7a"));
    CHECK_EQUAL(SQ_7B, SquareTraits::to_square("7b"));
    CHECK_EQUAL(SQ_7C, SquareTraits::to_square("7c"));
    CHECK_EQUAL(SQ_7D, SquareTraits::to_square("7d"));
    CHECK_EQUAL(SQ_7E, SquareTraits::to_square("7e"));
    CHECK_EQUAL(SQ_7F, SquareTraits::to_square("7f"));
    CHECK_EQUAL(SQ_7G, SquareTraits::to_square("7g"));
    CHECK_EQUAL(SQ_7H, SquareTraits::to_square("7h"));
    CHECK_EQUAL(SQ_7I, SquareTraits::to_square("7i"));
    CHECK_EQUAL(SQ_8A, SquareTraits::to_square("8a"));
    CHECK_EQUAL(SQ_8B, SquareTraits::to_square("8b"));
    CHECK_EQUAL(SQ_8C, SquareTraits::to_square("8c"));
    CHECK_EQUAL(SQ_8D, SquareTraits::to_square("8d"));
    CHECK_EQUAL(SQ_8E, SquareTraits::to_square("8e"));
    CHECK_EQUAL(SQ_8F, SquareTraits::to_square("8f"));
    CHECK_EQUAL(SQ_8G, SquareTraits::to_square("8g"));
    CHECK_EQUAL(SQ_8H, SquareTraits::to_square("8h"));
    CHECK_EQUAL(SQ_8I, SquareTraits::to_square("8i"));
    CHECK_EQUAL(SQ_9A, SquareTraits::to_square("9a"));
    CHECK_EQUAL(SQ_9B, SquareTraits::to_square("9b"));
    CHECK_EQUAL(SQ_9C, SquareTraits::to_square("9c"));
    CHECK_EQUAL(SQ_9D, SquareTraits::to_square("9d"));
    CHECK_EQUAL(SQ_9E, SquareTraits::to_square("9e"));
    CHECK_EQUAL(SQ_9F, SquareTraits::to_square("9f"));
    CHECK_EQUAL(SQ_9G, SquareTraits::to_square("9g"));
    CHECK_EQUAL(SQ_9H, SquareTraits::to_square("9h"));
    CHECK_EQUAL(SQ_9I, SquareTraits::to_square("9i"));
}

TEST(test_shogi_square_traits, to_sfen)
{
    // clang-format off
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1A); STRCMP_EQUAL("1a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1B); STRCMP_EQUAL("1b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1C); STRCMP_EQUAL("1c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1D); STRCMP_EQUAL("1d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1E); STRCMP_EQUAL("1e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1F); STRCMP_EQUAL("1f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1G); STRCMP_EQUAL("1g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1H); STRCMP_EQUAL("1h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_1I); STRCMP_EQUAL("1i", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2A); STRCMP_EQUAL("2a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2B); STRCMP_EQUAL("2b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2C); STRCMP_EQUAL("2c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2D); STRCMP_EQUAL("2d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2E); STRCMP_EQUAL("2e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2F); STRCMP_EQUAL("2f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2G); STRCMP_EQUAL("2g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2H); STRCMP_EQUAL("2h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_2I); STRCMP_EQUAL("2i", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3A); STRCMP_EQUAL("3a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3B); STRCMP_EQUAL("3b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3C); STRCMP_EQUAL("3c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3D); STRCMP_EQUAL("3d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3E); STRCMP_EQUAL("3e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3F); STRCMP_EQUAL("3f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3G); STRCMP_EQUAL("3g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3H); STRCMP_EQUAL("3h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_3I); STRCMP_EQUAL("3i", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4A); STRCMP_EQUAL("4a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4B); STRCMP_EQUAL("4b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4C); STRCMP_EQUAL("4c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4D); STRCMP_EQUAL("4d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4E); STRCMP_EQUAL("4e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4F); STRCMP_EQUAL("4f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4G); STRCMP_EQUAL("4g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4H); STRCMP_EQUAL("4h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_4I); STRCMP_EQUAL("4i", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5A); STRCMP_EQUAL("5a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5B); STRCMP_EQUAL("5b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5C); STRCMP_EQUAL("5c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5D); STRCMP_EQUAL("5d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5E); STRCMP_EQUAL("5e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5F); STRCMP_EQUAL("5f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5G); STRCMP_EQUAL("5g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5H); STRCMP_EQUAL("5h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_5I); STRCMP_EQUAL("5i", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6A); STRCMP_EQUAL("6a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6B); STRCMP_EQUAL("6b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6C); STRCMP_EQUAL("6c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6D); STRCMP_EQUAL("6d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6E); STRCMP_EQUAL("6e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6F); STRCMP_EQUAL("6f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6G); STRCMP_EQUAL("6g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6H); STRCMP_EQUAL("6h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_6I); STRCMP_EQUAL("6i", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7A); STRCMP_EQUAL("7a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7B); STRCMP_EQUAL("7b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7C); STRCMP_EQUAL("7c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7D); STRCMP_EQUAL("7d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7E); STRCMP_EQUAL("7e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7F); STRCMP_EQUAL("7f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7G); STRCMP_EQUAL("7g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7H); STRCMP_EQUAL("7h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_7I); STRCMP_EQUAL("7i", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8A); STRCMP_EQUAL("8a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8B); STRCMP_EQUAL("8b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8C); STRCMP_EQUAL("8c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8D); STRCMP_EQUAL("8d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8E); STRCMP_EQUAL("8e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8F); STRCMP_EQUAL("8f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8G); STRCMP_EQUAL("8g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8H); STRCMP_EQUAL("8h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_8I); STRCMP_EQUAL("8i", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9A); STRCMP_EQUAL("9a", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9B); STRCMP_EQUAL("9b", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9C); STRCMP_EQUAL("9c", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9D); STRCMP_EQUAL("9d", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9E); STRCMP_EQUAL("9e", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9F); STRCMP_EQUAL("9f", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9G); STRCMP_EQUAL("9g", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9H); STRCMP_EQUAL("9h", actual); }
    { char actual[3] = {}; SquareTraits::to_sfen(actual, SQ_9I); STRCMP_EQUAL("9i", actual); }
    // clang-format on
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
