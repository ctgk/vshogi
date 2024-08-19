#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(minishogi_stand){};

TEST(minishogi_stand, count)
{
    CHECK_EQUAL(0, Stand(0, 1, 2, 0, 2).count(FU));
    CHECK_EQUAL(1, Stand(0, 1, 2, 0, 2).count(GI));
    CHECK_EQUAL(2, Stand(0, 1, 2, 0, 2).count(KA));
    CHECK_EQUAL(0, Stand(0, 1, 2, 0, 2).count(HI));
    CHECK_EQUAL(2, Stand(0, 1, 2, 0, 2).count(KI));
}

TEST(minishogi_stand, exist)
{
    CHECK_FALSE(Stand(0, 1, 2, 0, 2).exist(FU));
    CHECK_TRUE(Stand(0, 1, 2, 0, 2).exist(GI));
    CHECK_TRUE(Stand(0, 1, 2, 0, 2).exist(KA));
    CHECK_FALSE(Stand(0, 1, 2, 0, 2).exist(HI));
    CHECK_TRUE(Stand(0, 1, 2, 0, 2).exist(KI));
}

TEST(minishogi_stand, any)
{
    CHECK_TRUE(Stand(0, 0, 0, 1, 0).any());
    CHECK_FALSE(Stand(0, 0, 0, 0, 0).any());
}

TEST(minishogi_stand, add)
{
    CHECK_EQUAL(1, Stand().add(FU).count(FU));
    CHECK_EQUAL(0, Stand().add(GI).count(FU));
}

TEST(minishogi_stand, subtract)
{
    CHECK_EQUAL(0, Stand(1, 0, 0, 0, 0).subtract(FU).count(FU));
}

TEST(minishogi_stand, set_sfen)
{
    const char sfen_holdings[] = "2bP2GSR 3";
    auto s = BlackWhiteStands();
    const auto actual = s.set_sfen(sfen_holdings);
    CHECK_EQUAL(0, s.white().count(FU));
    CHECK_EQUAL(0, s.white().count(GI));
    CHECK_EQUAL(0, s.white().count(KI));
    CHECK_EQUAL(2, s.white().count(KA));
    CHECK_EQUAL(0, s.white().count(HI));
    CHECK_EQUAL(1, s.black().count(FU));
    CHECK_EQUAL(1, s.black().count(GI));
    CHECK_EQUAL(2, s.black().count(KI));
    CHECK_EQUAL(0, s.black().count(KA));
    CHECK_EQUAL(1, s.black().count(HI));
    CHECK_EQUAL('3', actual[0]);
    CHECK_EQUAL('\0', actual[1]);
}

TEST(minishogi_stand, append_sfen)
{
    {
        auto s = BlackWhiteStands();
        s.set_sfen("-");
        auto actual = std::string();
        s.append_sfen(actual);
        STRCMP_EQUAL("-", actual.c_str());
    }
    {
        const char sfen_holdings[] = "2bP2GSR 3";
        auto s = BlackWhiteStands();
        s.set_sfen(sfen_holdings);
        auto actual = std::string();
        s.append_sfen(actual);
        STRCMP_EQUAL("R2GSP2b", actual.c_str());
    }
}

} // namespace test_vshogi::test_minishogi
