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
        CHECK_EQUAL(0, ps.count(CH));
        CHECK_EQUAL(0, ps.count(EL));
        CHECK_EQUAL(0, ps.count(GI));
    }
    {
        CHECK_EQUAL(1, PieceStand(1).count(CH));
        CHECK_EQUAL(0, PieceStand(1).count(GI));
        CHECK_EQUAL(0, PieceStand(1).count(EL));
    }
    {
        const auto ps = PieceStand(0, 0, 0);
        CHECK_EQUAL(0, ps.count(CH));
        CHECK_EQUAL(0, ps.count(EL));
        CHECK_EQUAL(0, ps.count(GI));
    }
    {
        const auto ps = PieceStand(2, 0, 0);
        CHECK_EQUAL(2, ps.count(CH));
        CHECK_EQUAL(0, ps.count(EL));
        CHECK_EQUAL(0, ps.count(GI));
    }
    {
        const auto ps = PieceStand(0, 1, 2);
        CHECK_EQUAL(0, ps.count(CH));
        CHECK_EQUAL(1, ps.count(EL));
        CHECK_EQUAL(2, ps.count(GI));
    }
}

TEST(animal_shogi_piece_stand, add)
{
    {
        const auto ps = PieceStand().add(EL);
        CHECK_EQUAL(0, ps.count(CH));
        CHECK_EQUAL(1, ps.count(EL));
        CHECK_EQUAL(0, ps.count(GI));
    }
    {
        const auto ps = PieceStand().add(EL).add(GI);
        CHECK_EQUAL(0, ps.count(CH));
        CHECK_EQUAL(1, ps.count(EL));
        CHECK_EQUAL(1, ps.count(GI));
    }
    {
        const auto ps = PieceStand().add(CH).add(CH);
        CHECK_EQUAL(2, ps.count(CH));
        CHECK_EQUAL(0, ps.count(EL));
        CHECK_EQUAL(0, ps.count(GI));
    }
    {
        const auto ps = PieceStand().add(HE);
        CHECK_EQUAL(1, ps.count(CH));
        CHECK_EQUAL(0, ps.count(EL));
        CHECK_EQUAL(0, ps.count(GI));
    }
}

TEST(animal_shogi_piece_stand, subtract)
{
    {
        const auto ps = PieceStand().add(EL).subtract(EL);
        CHECK_EQUAL(0, ps.count(CH));
        CHECK_EQUAL(0, ps.count(EL));
        CHECK_EQUAL(0, ps.count(GI));
    }
    {
        const auto ps = PieceStand().add(EL).add(EL).subtract(EL);
        CHECK_EQUAL(0, ps.count(CH));
        CHECK_EQUAL(1, ps.count(EL));
        CHECK_EQUAL(0, ps.count(GI));
    }
}

TEST(animal_shogi_piece_stand, operator)
{
    auto a = PieceStand(1);
    auto b = PieceStand(2);
    CHECK_FALSE(a == b);
    CHECK_TRUE(a != b);
}

TEST_GROUP(animal_shogi_two_piece_stands){};

TEST(animal_shogi_two_piece_stands, sizeof)
{
    CHECK_EQUAL(sizeof(std::uint16_t), sizeof(TwoPieceStands));
}

TEST(animal_shogi_two_piece_stands, set_sfen_holdings)
{
    {
        const auto s = "- 1";
        auto ps = TwoPieceStands();

        const auto actual = ps.set_sfen_holdings(s);

        CHECK_EQUAL(0, ps.black().count(CH));
        CHECK_EQUAL(0, ps.black().count(EL));
        CHECK_EQUAL(0, ps.black().count(GI));
        CHECK_EQUAL(0, ps.white().count(CH));
        CHECK_EQUAL(0, ps.white().count(EL));
        CHECK_EQUAL(0, ps.white().count(GI));
        CHECK_EQUAL('1', *actual);
    }
    {
        const auto s = "2C2E2G2c2e2g 7";
        auto ps = TwoPieceStands();

        const auto actual = ps.set_sfen_holdings(s);

        CHECK_EQUAL(2, ps.black().count(CH));
        CHECK_EQUAL(2, ps.black().count(EL));
        CHECK_EQUAL(2, ps.black().count(GI));
        CHECK_EQUAL(2, ps.white().count(CH));
        CHECK_EQUAL(2, ps.white().count(EL));
        CHECK_EQUAL(2, ps.white().count(GI));
        CHECK_EQUAL('7', *actual);
    }
    {
        const auto s = "C2E2cg\0aabbbccc";
        auto ps = TwoPieceStands();

        const auto actual = ps.set_sfen_holdings(s);

        CHECK_EQUAL(1, ps.black().count(CH));
        CHECK_EQUAL(2, ps.black().count(EL));
        CHECK_EQUAL(0, ps.black().count(GI));
        CHECK_EQUAL(2, ps.white().count(CH));
        CHECK_EQUAL(0, ps.white().count(EL));
        CHECK_EQUAL(1, ps.white().count(GI));
        CHECK_EQUAL('\0', *actual);
    }
}

} // namespace test_vshogi::test_animal_shogi
