#include "vshogi/animal_shogi/piece_stand.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_piece_stand){};

TEST(animal_shogi_piece_stand, sizeof)
{
    CHECK_EQUAL(sizeof(std::uint8_t), sizeof(PieceStand));
}

TEST(animal_shogi_piece_stand, count)
{
    {
        const auto ps = PieceStand();
        CHECK_EQUAL(0, ps.count(C_CH));
        CHECK_EQUAL(0, ps.count(C_EL));
        CHECK_EQUAL(0, ps.count(C_GI));
    }
    {
        CHECK_EQUAL(1, PieceStand(1).count(C_CH));
        CHECK_EQUAL(0, PieceStand(1).count(C_GI));
        CHECK_EQUAL(0, PieceStand(1).count(C_EL));
    }
    {
        const auto ps = PieceStand("-");
        CHECK_EQUAL(0, ps.count(C_CH));
        CHECK_EQUAL(0, ps.count(C_EL));
        CHECK_EQUAL(0, ps.count(C_GI));
    }
    {
        const auto ps = PieceStand("2c");
        CHECK_EQUAL(2, ps.count(C_CH));
        CHECK_EQUAL(0, ps.count(C_EL));
        CHECK_EQUAL(0, ps.count(C_GI));
    }
    {
        const auto ps = PieceStand("E2G");
        CHECK_EQUAL(0, ps.count(C_CH));
        CHECK_EQUAL(1, ps.count(C_EL));
        CHECK_EQUAL(2, ps.count(C_GI));
    }
}

TEST(animal_shogi_piece_stand, add)
{
    {
        const auto ps = PieceStand().add(C_EL);
        CHECK_EQUAL(0, ps.count(C_CH));
        CHECK_EQUAL(1, ps.count(C_EL));
        CHECK_EQUAL(0, ps.count(C_GI));
    }
    {
        const auto ps = PieceStand().add(C_EL).add(C_GI);
        CHECK_EQUAL(0, ps.count(C_CH));
        CHECK_EQUAL(1, ps.count(C_EL));
        CHECK_EQUAL(1, ps.count(C_GI));
    }
    {
        const auto ps = PieceStand().add(C_CH).add(C_CH);
        CHECK_EQUAL(2, ps.count(C_CH));
        CHECK_EQUAL(0, ps.count(C_EL));
        CHECK_EQUAL(0, ps.count(C_GI));
    }
}

TEST(animal_shogi_piece_stand, subtract)
{
    {
        const auto ps = PieceStand().add(C_EL).subtract(C_EL);
        CHECK_EQUAL(0, ps.count(C_CH));
        CHECK_EQUAL(0, ps.count(C_EL));
        CHECK_EQUAL(0, ps.count(C_GI));
    }
    {
        const auto ps = PieceStand().add(C_EL).add(C_EL).subtract(C_EL);
        CHECK_EQUAL(0, ps.count(C_CH));
        CHECK_EQUAL(1, ps.count(C_EL));
        CHECK_EQUAL(0, ps.count(C_GI));
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
