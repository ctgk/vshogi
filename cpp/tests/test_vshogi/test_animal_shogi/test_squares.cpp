#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP (animal_shogi_squares) {
};

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

TEST(animal_shogi_squares, in_promotion_zone)
{
    CHECK_TRUE(Squares::in_promotion_zone(RANK1, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(RANK2, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(RANK3, vshogi::BLACK));
    CHECK_FALSE(Squares::in_promotion_zone(RANK4, vshogi::BLACK));

    CHECK_FALSE(Squares::in_promotion_zone(RANK1, vshogi::WHITE));
    CHECK_FALSE(Squares::in_promotion_zone(RANK2, vshogi::WHITE));
    CHECK_FALSE(Squares::in_promotion_zone(RANK3, vshogi::WHITE));
    CHECK_TRUE(Squares::in_promotion_zone(RANK4, vshogi::WHITE));
}

TEST(animal_shogi_squares, get_direction)
{
    CHECK_EQUAL(vshogi::DIR_NW, Squares::get_direction(SQ_A1, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_N, Squares::get_direction(SQ_B1, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_NE, Squares::get_direction(SQ_C1, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_W, Squares::get_direction(SQ_A2, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_E, Squares::get_direction(SQ_C2, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_SW, Squares::get_direction(SQ_A3, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_S, Squares::get_direction(SQ_B3, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_SE, Squares::get_direction(SQ_C3, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_S, Squares::get_direction(SQ_B4, SQ_B2));

    CHECK_EQUAL(vshogi::DIR_NA, Squares::get_direction(SQ_C4, SQ_B2));
    CHECK_EQUAL(vshogi::DIR_NA, Squares::get_direction(SQ_A4, SQ_B1));
    CHECK_EQUAL(vshogi::DIR_NA, Squares::get_direction(SQ_A1, SQ_C2));
}

} // namespace test_vshogi::test_animal_shogi
