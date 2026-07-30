#include "vshogi/common/notation.hpp"
#include "vshogi/variants/leshogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_leshogi
{

using namespace vshogi::leshogi;
using NT = vshogi::Notation<Parameters>;

TEST_GROUP (test_leshogi_stand) {
};

TEST(test_leshogi_stand, test_count)
{
    const auto s = Stand(0, 2, 2, 0, 1, 2, 1);
    CHECK_EQUAL(0, s.count(FU));
    CHECK_EQUAL(2, s.count(KY));
    CHECK_EQUAL(2, s.count(KE));
    CHECK_EQUAL(0, s.count(GI));
    CHECK_EQUAL(1, s.count(KA));
    CHECK_EQUAL(2, s.count(HI));
    CHECK_EQUAL(1, s.count(KI));
}

TEST(test_leshogi_stand, test_exist)
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

TEST(test_leshogi_stand, test_any)
{
    CHECK_TRUE(Stand(1, 0, 0, 0, 0, 0, 0).any());
    CHECK_FALSE(Stand(0, 0, 0, 0, 0, 0, 0).any());
}

TEST(test_leshogi_stand, test_add)
{
    CHECK_EQUAL(1, Stand().add(FU).count(FU));
    CHECK_EQUAL(0, Stand().add(GI).count(FU));
}

TEST(test_leshogi_stand, test_subtract)
{
    CHECK_EQUAL(1, Stand(2, 0, 0, 0, 0, 0, 0).subtract(FU).count(FU));
}

TEST(test_leshogi_stand, test_set_sfen)
{
    const char sfen_holdings[] = "2PNGl2r 5";
    auto s = BlackWhiteStands();
    const auto actual = s.set_sfen(sfen_holdings);
    CHECK_EQUAL(2, s[vshogi::BLACK].count(FU));
    CHECK_EQUAL(0, s[vshogi::BLACK].count(KY));
    CHECK_EQUAL(1, s[vshogi::BLACK].count(KE));
    CHECK_EQUAL(0, s[vshogi::BLACK].count(GI));
    CHECK_EQUAL(0, s[vshogi::BLACK].count(KA));
    CHECK_EQUAL(0, s[vshogi::BLACK].count(HI));
    CHECK_EQUAL(1, s[vshogi::BLACK].count(KI));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(FU));
    CHECK_EQUAL(1, s[vshogi::WHITE].count(KY));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(KE));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(GI));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(KA));
    CHECK_EQUAL(2, s[vshogi::WHITE].count(HI));
    CHECK_EQUAL(0, s[vshogi::WHITE].count(KI));
    CHECK_EQUAL('5', actual[0]);
    CHECK_EQUAL('\0', actual[1]);
}

TEST(test_leshogi_stand, test_to_sfen)
{
    {
        auto s = BlackWhiteStands();
        s.set_sfen("-");
        auto actual = NT::to_sfen(s);
        STRCMP_EQUAL("-", actual.c_str());
    }
    {
        const char sfen_holdings[] = "2PNGl2r 5";
        auto s = BlackWhiteStands();
        s.set_sfen(sfen_holdings);
        auto actual = NT::to_sfen(s);
        STRCMP_EQUAL("GN2P2rl", actual.c_str());
    }
}

TEST(test_leshogi_stand, test_operators)
{
    CHECK_TRUE(Stand(1, 0, 0, 0, 0, 0, 0) == Stand(1, 0, 0, 0, 0, 0, 0));
    CHECK_FALSE(Stand(1, 0, 0, 0, 0, 0, 0) != Stand(1, 0, 0, 0, 0, 0, 0));
    CHECK_TRUE(Stand(1, 0, 0, 0, 0, 0, 0) >= Stand(1, 0, 0, 0, 0, 0, 0));

    CHECK_TRUE(Stand(2, 0, 0, 0, 0, 0, 0) >= Stand(1, 0, 0, 0, 0, 0, 0));
    CHECK_FALSE(Stand(2, 0, 0, 0, 0, 0, 0) > Stand(2, 0, 0, 0, 0, 0, 0));

    CHECK_TRUE(Stand(2, 1, 1, 1, 0, 0, 1) >= Stand(2, 1, 1, 0, 0, 0, 1));
    CHECK_FALSE(Stand(2, 1, 1, 1, 0, 0, 1) >= Stand(2, 1, 1, 0, 1, 0, 1));
    CHECK_FALSE(Stand(2, 1, 1, 1, 0, 0, 1) >= Stand(2, 1, 1, 0, 0, 0, 2));
}

} // namespace test_vshogi::test_leshogi
