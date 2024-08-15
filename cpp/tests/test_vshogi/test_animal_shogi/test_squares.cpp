#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_squares){};

TEST(animal_shogi_squares, to_square)
{
    CHECK_EQUAL(SQ_A1, Squares::to_square(FILE1, RANK1));

    CHECK_EQUAL(SQ_A1, Squares::to_square("a1"));
    CHECK_EQUAL(SQ_A2, Squares::to_square("a2"));
    CHECK_EQUAL(SQ_A3, Squares::to_square("a3"));
    CHECK_EQUAL(SQ_A4, Squares::to_square("a4"));
    CHECK_EQUAL(SQ_B1, Squares::to_square("b1"));
    CHECK_EQUAL(SQ_B2, Squares::to_square("b2"));
    CHECK_EQUAL(SQ_B3, Squares::to_square("b3"));
    CHECK_EQUAL(SQ_B4, Squares::to_square("b4"));
    CHECK_EQUAL(SQ_C1, Squares::to_square("c1"));
    CHECK_EQUAL(SQ_C2, Squares::to_square("c2"));
    CHECK_EQUAL(SQ_C3, Squares::to_square("c3"));
    CHECK_EQUAL(SQ_C4, Squares::to_square("c4"));
}

} // namespace test_vshogi::test_animal_shogi
