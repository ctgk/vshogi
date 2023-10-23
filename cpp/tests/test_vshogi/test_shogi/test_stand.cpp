#include "vshogi/shogi/stand.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_shogi/test_shogi.hpp"

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(shogi_stand){};

TEST(shogi_stand, count)
{
    const auto s = Stand(0, 4, 2, 0, 1, 2, 1);
    CHECK_EQUAL(0, s.count(FU));
    CHECK_EQUAL(4, s.count(KY));
    CHECK_EQUAL(2, s.count(KE));
    CHECK_EQUAL(0, s.count(GI));
    CHECK_EQUAL(1, s.count(KA));
    CHECK_EQUAL(2, s.count(HI));
    CHECK_EQUAL(1, s.count(KI));
}

TEST(shogi_stand, exist)
{
    const auto s = Stand(0, 1, 2, 0, 1, 2, 1);
    CHECK_FALSE(s.exist(FU));
    CHECK_TRUE(s.exist(KY));
    CHECK_TRUE(s.exist(KE));
    CHECK_FALSE(s.exist(GI));
    CHECK_TRUE(s.exist(KA));
    CHECK_TRUE(s.exist(HI));
    CHECK_TRUE(s.exist(KI));
}

TEST(shogi_stand, any)
{
    CHECK_TRUE(Stand(18, 0, 0, 0, 0, 0, 0).any());
    CHECK_FALSE(Stand(0, 0, 0, 0, 0, 0, 0).any());
}

TEST(shogi_stand, add)
{
    CHECK_EQUAL(1, Stand().add(FU).count(FU));
    CHECK_EQUAL(0, Stand().add(GI).count(FU));
}

TEST(shogi_stand, subtract)
{
    CHECK_EQUAL(7, Stand(8, 0, 0, 0, 0, 0, 0).subtract(FU).count(FU));
}

TEST(shogi_stand, set_sfen)
{
    const char sfen_holdings[] = "3P4NG10pl2r 5";
    auto s = BlackWhiteStands();
    const auto actual = s.set_sfen(sfen_holdings);
    CHECK_EQUAL(3, s[vshogi::BLACK].count(FU));
    CHECK_EQUAL(0, s[vshogi::BLACK].count(KY));
    CHECK_EQUAL(4, s[vshogi::BLACK].count(KE));
    CHECK_EQUAL(0, s[vshogi::BLACK].count(GI));
    CHECK_EQUAL(0, s[vshogi::BLACK].count(KA));
    CHECK_EQUAL(0, s[vshogi::BLACK].count(HI));
    CHECK_EQUAL(1, s[vshogi::BLACK].count(KI));
    CHECK_EQUAL(10, s[vshogi::WHITE].count(FU));
    CHECK_EQUAL(1, s[vshogi::WHITE].count(KY));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(KE));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(GI));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(KA));
    CHECK_EQUAL(2, s[vshogi::WHITE].count(HI));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(KI));
    CHECK_EQUAL('5', actual[0]);
    CHECK_EQUAL('\0', actual[1]);
}

TEST(shogi_stand, append_sfen)
{
    {
        auto s = BlackWhiteStands();
        s.set_sfen("-");
        auto actual = std::string();
        s.append_sfen(actual);
        STRCMP_EQUAL("-", actual.c_str());
    }
    {
        const char sfen_holdings[] = "3P4NG10pl2r 5";
        auto s = BlackWhiteStands();
        s.set_sfen(sfen_holdings);
        auto actual = std::string();
        s.append_sfen(actual);
        STRCMP_EQUAL("G4N3P2rl10p", actual.c_str());
    }
}

} // namespace test_vshogi::test_shogi
