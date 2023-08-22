#include "vshogi/shogi/stand.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi;
using namespace vshogi::shogi;

TEST_GROUP(stand){};

TEST(stand, count)
{
    const auto s = Stand(0, 1, 2, 0, 1, 2, 1);
    CHECK_EQUAL(0, s.count(FU));
    CHECK_EQUAL(1, s.count(KY));
    CHECK_EQUAL(2, s.count(KE));
    CHECK_EQUAL(0, s.count(GI));
    CHECK_EQUAL(1, s.count(KA));
    CHECK_EQUAL(2, s.count(HI));
    CHECK_EQUAL(1, s.count(KI));
}

TEST(stand, exist)
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

TEST(stand, any)
{
    CHECK_TRUE(Stand(18, 0, 0, 0, 0, 0, 0).any());
    CHECK_FALSE(Stand(0, 0, 0, 0, 0, 0, 0).any());
}

TEST(stand, add)
{
    CHECK_EQUAL(1, Stand().add(FU).count(FU));
    CHECK_EQUAL(0, Stand().add(GI).count(FU));
}

TEST(stand, subtract)
{
    CHECK_EQUAL(7, Stand(8, 0, 0, 0, 0, 0, 0).subtract(FU).count(FU));
}

TEST(stand, set_sfen_holdings)
{
    const char sfen_holdings[] = "3P4NG10pl2r 5";
    auto s = BlackWhiteStands();
    const auto actual = s.set_sfen_holdings(sfen_holdings);
    CHECK_EQUAL(3, s[BLACK].count(FU));
    CHECK_EQUAL(0, s[BLACK].count(KY));
    CHECK_EQUAL(4, s[BLACK].count(KE));
    CHECK_EQUAL(0, s[BLACK].count(GI));
    CHECK_EQUAL(0, s[BLACK].count(KA));
    CHECK_EQUAL(0, s[BLACK].count(HI));
    CHECK_EQUAL(1, s[BLACK].count(KI));
    CHECK_EQUAL(10, s[WHITE].count(FU));
    CHECK_EQUAL(1, s[WHITE].count(KY));
    CHECK_EQUAL(0, s[WHITE].count(KE));
    CHECK_EQUAL(0, s[WHITE].count(GI));
    CHECK_EQUAL(0, s[WHITE].count(KA));
    CHECK_EQUAL(2, s[WHITE].count(HI));
    CHECK_EQUAL(0, s[WHITE].count(KI));
    CHECK_EQUAL('5', actual[0]);
    CHECK_EQUAL('\0', actual[1]);
}

TEST(stand, to_sfen_holdings)
{
    {
        auto s = BlackWhiteStands();
        s.set_sfen_holdings("-");
        const auto actual = s.to_sfen_holdings();
        STRCMP_EQUAL("-", actual.c_str());
    }
    {
        const char sfen_holdings[] = "3P4NG10pl2r 5";
        auto s = BlackWhiteStands();
        s.set_sfen_holdings(sfen_holdings);
        const auto actual = s.to_sfen_holdings();
        STRCMP_EQUAL("3P4NG10pl2r", actual.c_str());
    }
}

} // namespace test_vshogi::test_shogi
