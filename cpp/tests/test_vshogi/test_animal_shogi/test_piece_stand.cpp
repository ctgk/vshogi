#include "vshogi/animal_shogi/piece_stand.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_piece_stand){};

TEST(animal_shogi_piece_stand, count)
{
    CHECK_EQUAL(3, PieceStand(3).count(PieceStand::FU));
    CHECK_EQUAL(0, PieceStand(3).count(PieceStand::GI));
}

TEST(animal_shogi_piece_stand, add)
{
    CHECK_EQUAL(1, PieceStand().add(PieceStand::KA).count(PieceStand::KA));
    CHECK_EQUAL(
        1,
        PieceStand()
            .add(PieceStand::KA)
            .add(PieceStand::GI)
            .count(PieceStand::KA));
    CHECK_EQUAL(
        2,
        PieceStand()
            .add(PieceStand::KA)
            .add(PieceStand::KA)
            .count(PieceStand::KA));
}

TEST(animal_shogi_piece_stand, subtract)
{
    CHECK_EQUAL(
        0,
        PieceStand()
            .add(PieceStand::KE)
            .subtract(PieceStand::KE)
            .count(PieceStand::KE));
    CHECK_EQUAL(
        1,
        PieceStand()
            .add(PieceStand::KE)
            .add(PieceStand::KE)
            .subtract(PieceStand::KE)
            .count(PieceStand::KE));
}

TEST(animal_shogi_piece_stand, operator)
{
    auto a = PieceStand(1);
    auto b = PieceStand(2);
    CHECK_FALSE(a == b);
    CHECK_TRUE(a != b);
}

} // namespace test_vshogi::test_animal_shogi
