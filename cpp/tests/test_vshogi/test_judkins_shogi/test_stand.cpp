#include "vshogi/judkins_shogi/stand.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_judkins_shogi/test_judkins_shogi.hpp"

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi;
using namespace vshogi::judkins_shogi;

TEST_GROUP(judkins_shogi_stand){};

TEST(judkins_shogi_stand, count)
{
    {
        const auto s = Stand(0, 2, 0, 1, 2, 1);
        CHECK_EQUAL(0, s.count(FU));
        CHECK_EQUAL(2, s.count(KE));
        CHECK_EQUAL(0, s.count(GI));
        CHECK_EQUAL(1, s.count(KA));
        CHECK_EQUAL(2, s.count(HI));
        CHECK_EQUAL(1, s.count(KI));
    }
    {
        const auto s = Stand(2, 2, 2, 2, 2, 2);
        CHECK_EQUAL(2, s.count(FU));
        CHECK_EQUAL(2, s.count(KE));
        CHECK_EQUAL(2, s.count(GI));
        CHECK_EQUAL(2, s.count(KA));
        CHECK_EQUAL(2, s.count(HI));
        CHECK_EQUAL(2, s.count(KI));
    }
}

TEST(judkins_shogi_stand, exist)
{
    {
        const auto s = Stand(0, 2, 0, 1, 2, 1);
        CHECK_FALSE(s.exist(FU));
        CHECK_TRUE(s.exist(KE));
        CHECK_FALSE(s.exist(GI));
        CHECK_TRUE(s.exist(KA));
        CHECK_TRUE(s.exist(HI));
        CHECK_TRUE(s.exist(KI));
    }
    {
        const auto s = Stand(2, 2, 2, 2, 2, 2);
        CHECK_TRUE(s.exist(FU));
        CHECK_TRUE(s.exist(KE));
        CHECK_TRUE(s.exist(GI));
        CHECK_TRUE(s.exist(KA));
        CHECK_TRUE(s.exist(HI));
        CHECK_TRUE(s.exist(KI));
    }
}

TEST(judkins_shogi_stand, any)
{
    CHECK_TRUE(Stand(1, 0, 0, 0, 0, 0).any());
    CHECK_FALSE(Stand(0, 0, 0, 0, 0, 0).any());
}

TEST(judkins_shogi_stand, add)
{
    CHECK_EQUAL(1, Stand().add(FU).count(FU));
    CHECK_EQUAL(0, Stand().add(GI).count(FU));
}

TEST(judkins_shogi_stand, subtract)
{
    CHECK_EQUAL(1, Stand(2, 0, 0, 0, 0, 0).subtract(FU).count(FU));
}

TEST(judkins_shogi_stand, set_sfen_holdings)
{
    {
        const char sfen_holdings[] = "RBGSNPrbgsnp 10";
        auto s = BlackWhiteStands();
        const auto actual = s.set_sfen_holdings(sfen_holdings);
        CHECK_EQUAL(1, s[BLACK].count(FU));
        CHECK_EQUAL(1, s[BLACK].count(KE));
        CHECK_EQUAL(1, s[BLACK].count(GI));
        CHECK_EQUAL(1, s[BLACK].count(KA));
        CHECK_EQUAL(1, s[BLACK].count(HI));
        CHECK_EQUAL(1, s[BLACK].count(KI));
        CHECK_EQUAL(1, s[WHITE].count(FU));
        CHECK_EQUAL(1, s[WHITE].count(KE));
        CHECK_EQUAL(1, s[WHITE].count(GI));
        CHECK_EQUAL(1, s[WHITE].count(KA));
        CHECK_EQUAL(1, s[WHITE].count(HI));
        CHECK_EQUAL(1, s[WHITE].count(KI));
        CHECK_EQUAL('1', actual[0]);
        CHECK_EQUAL('0', actual[1]);
    }
    {
        const char sfen_holdings[] = "2R2B2G2S2N2P 5";
        auto s = BlackWhiteStands();
        const auto actual = s.set_sfen_holdings(sfen_holdings);
        CHECK_EQUAL(2, s[BLACK].count(FU));
        CHECK_EQUAL(2, s[BLACK].count(KE));
        CHECK_EQUAL(2, s[BLACK].count(GI));
        CHECK_EQUAL(2, s[BLACK].count(KA));
        CHECK_EQUAL(2, s[BLACK].count(HI));
        CHECK_EQUAL(2, s[BLACK].count(KI));
        CHECK_EQUAL(0, s[WHITE].count(FU));
        CHECK_EQUAL(0, s[WHITE].count(KE));
        CHECK_EQUAL(0, s[WHITE].count(GI));
        CHECK_EQUAL(0, s[WHITE].count(KA));
        CHECK_EQUAL(0, s[WHITE].count(HI));
        CHECK_EQUAL(0, s[WHITE].count(KI));
        CHECK_EQUAL('5', actual[0]);
        CHECK_EQUAL('\0', actual[1]);
    }
    {
        const char sfen_holdings[] = "2r2b2g2s2n2p 5";
        auto s = BlackWhiteStands();
        const auto actual = s.set_sfen_holdings(sfen_holdings);
        CHECK_EQUAL(0, s[BLACK].count(FU));
        CHECK_EQUAL(0, s[BLACK].count(KE));
        CHECK_EQUAL(0, s[BLACK].count(GI));
        CHECK_EQUAL(0, s[BLACK].count(KA));
        CHECK_EQUAL(0, s[BLACK].count(HI));
        CHECK_EQUAL(0, s[BLACK].count(KI));
        CHECK_EQUAL(2, s[WHITE].count(FU));
        CHECK_EQUAL(2, s[WHITE].count(KE));
        CHECK_EQUAL(2, s[WHITE].count(GI));
        CHECK_EQUAL(2, s[WHITE].count(KA));
        CHECK_EQUAL(2, s[WHITE].count(HI));
        CHECK_EQUAL(2, s[WHITE].count(KI));
        CHECK_EQUAL('5', actual[0]);
        CHECK_EQUAL('\0', actual[1]);
    }
}

TEST(judkins_shogi_stand, append_sfen)
{
    {
        auto s = BlackWhiteStands();
        s.set_sfen_holdings("-");
        auto actual = std::string();
        s.append_sfen(actual);
        STRCMP_EQUAL("-", actual.c_str());
    }
    {
        const char sfen_holdings[] = "RBGSNPrbgsnp 10";
        auto s = BlackWhiteStands();
        s.set_sfen_holdings(sfen_holdings);
        auto actual = std::string();
        s.append_sfen(actual);
        STRCMP_EQUAL("RBGSNPrbgsnp", actual.c_str());
    }
}

} // namespace test_vshogi::test_judkins_shogi
