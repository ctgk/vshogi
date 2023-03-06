#include "vshogi/animal_shogi/piece_stand.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_piece_stand){};

TEST(animal_shogi_piece_stand, count)
{
    {
        const auto ps = PieceStand();
        CHECK_EQUAL(0, ps.count(PieceStand::CH));
        CHECK_EQUAL(0, ps.count(PieceStand::EL));
        CHECK_EQUAL(0, ps.count(PieceStand::GI));
    }
    {
        CHECK_EQUAL(1, PieceStand(1).count(PieceStand::CH));
        CHECK_EQUAL(0, PieceStand(1).count(PieceStand::GI));
        CHECK_EQUAL(0, PieceStand(1).count(PieceStand::EL));
    }
}

TEST(animal_shogi_piece_stand, add)
{
    {
        const auto ps = PieceStand().add(PieceStand::EL);
        CHECK_EQUAL(0, ps.count(PieceStand::CH));
        CHECK_EQUAL(1, ps.count(PieceStand::EL));
        CHECK_EQUAL(0, ps.count(PieceStand::GI));
    }
    {
        const auto ps = PieceStand().add(PieceStand::EL).add(PieceStand::GI);
        CHECK_EQUAL(0, ps.count(PieceStand::CH));
        CHECK_EQUAL(1, ps.count(PieceStand::EL));
        CHECK_EQUAL(1, ps.count(PieceStand::GI));
    }
    {
        const auto ps = PieceStand().add(PieceStand::CH).add(PieceStand::CH);
        CHECK_EQUAL(2, ps.count(PieceStand::CH));
        CHECK_EQUAL(0, ps.count(PieceStand::EL));
        CHECK_EQUAL(0, ps.count(PieceStand::GI));
    }
}

TEST(animal_shogi_piece_stand, subtract)
{
    {
        const auto ps
            = PieceStand().add(PieceStand::EL).subtract(PieceStand::EL);
        CHECK_EQUAL(0, ps.count(PieceStand::CH));
        CHECK_EQUAL(0, ps.count(PieceStand::EL));
        CHECK_EQUAL(0, ps.count(PieceStand::GI));
    }
    {
        const auto ps = PieceStand()
                            .add(PieceStand::EL)
                            .add(PieceStand::EL)
                            .subtract(PieceStand::EL);
        CHECK_EQUAL(0, ps.count(PieceStand::CH));
        CHECK_EQUAL(1, ps.count(PieceStand::EL));
        CHECK_EQUAL(0, ps.count(PieceStand::GI));
    }
}

TEST(animal_shogi_piece_stand, operator)
{
    auto a = PieceStand(1);
    auto b = PieceStand(2);
    CHECK_FALSE(a == b);
    CHECK_TRUE(a != b);
}

} // namespace test_vshogi::test_animal_shogi
