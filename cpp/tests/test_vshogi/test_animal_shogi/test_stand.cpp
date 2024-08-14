#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_stand){};

TEST(animal_shogi_stand, sizeof)
{
    CHECK_EQUAL(sizeof(std::uint8_t), sizeof(Stand));
}

TEST(animal_shogi_stand, count)
{
    {
        const auto s = Stand();
        CHECK_EQUAL(0, s.count(CH));
        CHECK_EQUAL(0, s.count(EL));
        CHECK_EQUAL(0, s.count(GI));
    }
    {
        CHECK_EQUAL(1, Stand(1).count(CH));
        CHECK_EQUAL(0, Stand(1).count(GI));
        CHECK_EQUAL(0, Stand(1).count(EL));
    }
    {
        const auto s = Stand(0, 0, 0);
        CHECK_EQUAL(0, s.count(CH));
        CHECK_EQUAL(0, s.count(EL));
        CHECK_EQUAL(0, s.count(GI));
    }
    {
        const auto s = Stand(2, 0, 0);
        CHECK_EQUAL(2, s.count(CH));
        CHECK_EQUAL(0, s.count(EL));
        CHECK_EQUAL(0, s.count(GI));
    }
    {
        const auto s = Stand(0, 1, 2);
        CHECK_EQUAL(0, s.count(CH));
        CHECK_EQUAL(1, s.count(EL));
        CHECK_EQUAL(2, s.count(GI));
    }
}

TEST(animal_shogi_stand, add)
{
    {
        const auto s = Stand().add(EL);
        CHECK_EQUAL(0, s.count(CH));
        CHECK_EQUAL(1, s.count(EL));
        CHECK_EQUAL(0, s.count(GI));
    }
    {
        const auto s = Stand().add(EL).add(GI);
        CHECK_EQUAL(0, s.count(CH));
        CHECK_EQUAL(1, s.count(EL));
        CHECK_EQUAL(1, s.count(GI));
    }
    {
        const auto s = Stand().add(CH).add(CH);
        CHECK_EQUAL(2, s.count(CH));
        CHECK_EQUAL(0, s.count(EL));
        CHECK_EQUAL(0, s.count(GI));
    }
    {
        const auto s = Stand().add(HE);
        CHECK_EQUAL(1, s.count(CH));
        CHECK_EQUAL(0, s.count(EL));
        CHECK_EQUAL(0, s.count(GI));
    }
}

TEST(animal_shogi_stand, subtract)
{
    {
        const auto s = Stand().add(EL).subtract(EL);
        CHECK_EQUAL(0, s.count(CH));
        CHECK_EQUAL(0, s.count(EL));
        CHECK_EQUAL(0, s.count(GI));
    }
    {
        const auto s = Stand().add(EL).add(EL).subtract(EL);
        CHECK_EQUAL(0, s.count(CH));
        CHECK_EQUAL(1, s.count(EL));
        CHECK_EQUAL(0, s.count(GI));
    }
}

TEST(animal_shogi_stand, operator)
{
    auto a = Stand(1);
    auto b = Stand(2);
    CHECK_FALSE(a == b);
    CHECK_TRUE(a != b);
}

TEST_GROUP(animal_shogi_black_white_stands){};

TEST(animal_shogi_black_white_stands, sizeof)
{
    CHECK_EQUAL(sizeof(std::uint16_t), sizeof(BlackWhiteStands));
}

TEST(animal_shogi_black_white_stands, set_sfen)
{
    {
        const auto s = "- 1";
        auto stands = BlackWhiteStands();

        const auto actual = stands.set_sfen(s);

        CHECK_EQUAL(0, stands.black().count(CH));
        CHECK_EQUAL(0, stands.black().count(EL));
        CHECK_EQUAL(0, stands.black().count(GI));
        CHECK_EQUAL(0, stands.white().count(CH));
        CHECK_EQUAL(0, stands.white().count(EL));
        CHECK_EQUAL(0, stands.white().count(GI));
        CHECK_EQUAL('1', *actual);
    }
    {
        const auto s = "2C2E2G2c2e2g 7";
        auto stands = BlackWhiteStands();

        const auto actual = stands.set_sfen(s);

        CHECK_EQUAL(2, stands.black().count(CH));
        CHECK_EQUAL(2, stands.black().count(EL));
        CHECK_EQUAL(2, stands.black().count(GI));
        CHECK_EQUAL(2, stands.white().count(CH));
        CHECK_EQUAL(2, stands.white().count(EL));
        CHECK_EQUAL(2, stands.white().count(GI));
        CHECK_EQUAL('7', *actual);
    }
    {
        const auto s = "C2E2cg\0aabbbccc";
        auto stands = BlackWhiteStands();

        const auto actual = stands.set_sfen(s);

        CHECK_EQUAL(1, stands.black().count(CH));
        CHECK_EQUAL(2, stands.black().count(EL));
        CHECK_EQUAL(0, stands.black().count(GI));
        CHECK_EQUAL(2, stands.white().count(CH));
        CHECK_EQUAL(0, stands.white().count(EL));
        CHECK_EQUAL(1, stands.white().count(GI));
        CHECK_EQUAL('\0', *actual);
    }
}

TEST(animal_shogi_black_white_stands, append_sfen)
{
    {
        auto s = BlackWhiteStands();
        s.set_sfen("C2E2cg");
        auto actual = std::string();
        s.append_sfen(actual);
        STRCMP_EQUAL("2ECg2c", actual.c_str());
    }
}

TEST(animal_shogi_black_white_stands, zobrist_hash)
{
    auto s = BlackWhiteStands();
    auto hash = s.zobrist_hash();
    s.add_piece_to(vshogi::BLACK, CH, &hash);
    s.add_piece_to(vshogi::BLACK, CH, &hash);
    s.add_piece_to(vshogi::WHITE, EL, &hash);
    s.subtract_piece_from(vshogi::BLACK, CH, &hash);
    const auto expect = BlackWhiteStands("Ce").zobrist_hash();
    CHECK_EQUAL(expect, hash);
    CHECK_TRUE(BlackWhiteStands("").zobrist_hash() != hash);
    CHECK_TRUE(BlackWhiteStands("CE").zobrist_hash() != hash);
    CHECK_TRUE(BlackWhiteStands("Ec").zobrist_hash() != hash);
    CHECK_TRUE(BlackWhiteStands("C").zobrist_hash() != hash);
    CHECK_TRUE(BlackWhiteStands("e").zobrist_hash() != hash);
}

} // namespace test_vshogi::test_animal_shogi
