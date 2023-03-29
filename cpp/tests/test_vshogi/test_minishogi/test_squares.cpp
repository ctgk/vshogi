#include "vshogi/minishogi/squares.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(squares){};

TEST(squares, to_rank)
{
    CHECK_EQUAL(RANK1, to_rank(SQ_11));
    CHECK_EQUAL(RANK2, to_rank(SQ_12));
    CHECK_EQUAL(RANK3, to_rank(SQ_13));
    CHECK_EQUAL(RANK4, to_rank(SQ_14));
    CHECK_EQUAL(RANK5, to_rank(SQ_15));
    CHECK_EQUAL(RANK1, to_rank(SQ_21));
    CHECK_EQUAL(RANK2, to_rank(SQ_22));
    CHECK_EQUAL(RANK3, to_rank(SQ_23));
    CHECK_EQUAL(RANK4, to_rank(SQ_24));
    CHECK_EQUAL(RANK5, to_rank(SQ_25));
    CHECK_EQUAL(RANK1, to_rank(SQ_31));
    CHECK_EQUAL(RANK2, to_rank(SQ_32));
    CHECK_EQUAL(RANK3, to_rank(SQ_33));
    CHECK_EQUAL(RANK4, to_rank(SQ_34));
    CHECK_EQUAL(RANK5, to_rank(SQ_35));
    CHECK_EQUAL(RANK1, to_rank(SQ_41));
    CHECK_EQUAL(RANK2, to_rank(SQ_42));
    CHECK_EQUAL(RANK3, to_rank(SQ_43));
    CHECK_EQUAL(RANK4, to_rank(SQ_44));
    CHECK_EQUAL(RANK5, to_rank(SQ_45));
    CHECK_EQUAL(RANK1, to_rank(SQ_51));
    CHECK_EQUAL(RANK2, to_rank(SQ_52));
    CHECK_EQUAL(RANK3, to_rank(SQ_53));
    CHECK_EQUAL(RANK4, to_rank(SQ_54));
    CHECK_EQUAL(RANK5, to_rank(SQ_55));
}

TEST(squares, to_file)
{
    CHECK_EQUAL(FILE1, to_file(SQ_11));
    CHECK_EQUAL(FILE1, to_file(SQ_12));
    CHECK_EQUAL(FILE1, to_file(SQ_13));
    CHECK_EQUAL(FILE1, to_file(SQ_14));
    CHECK_EQUAL(FILE1, to_file(SQ_15));
    CHECK_EQUAL(FILE2, to_file(SQ_21));
    CHECK_EQUAL(FILE2, to_file(SQ_22));
    CHECK_EQUAL(FILE2, to_file(SQ_23));
    CHECK_EQUAL(FILE2, to_file(SQ_24));
    CHECK_EQUAL(FILE2, to_file(SQ_25));
    CHECK_EQUAL(FILE3, to_file(SQ_31));
    CHECK_EQUAL(FILE3, to_file(SQ_32));
    CHECK_EQUAL(FILE3, to_file(SQ_33));
    CHECK_EQUAL(FILE3, to_file(SQ_34));
    CHECK_EQUAL(FILE3, to_file(SQ_35));
    CHECK_EQUAL(FILE4, to_file(SQ_41));
    CHECK_EQUAL(FILE4, to_file(SQ_42));
    CHECK_EQUAL(FILE4, to_file(SQ_43));
    CHECK_EQUAL(FILE4, to_file(SQ_44));
    CHECK_EQUAL(FILE4, to_file(SQ_45));
    CHECK_EQUAL(FILE5, to_file(SQ_51));
    CHECK_EQUAL(FILE5, to_file(SQ_52));
    CHECK_EQUAL(FILE5, to_file(SQ_53));
    CHECK_EQUAL(FILE5, to_file(SQ_54));
    CHECK_EQUAL(FILE5, to_file(SQ_55));
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
