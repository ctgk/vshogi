#include <algorithm>

#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(state){};

TEST(state, set_sfen)
{
    {
        auto s = State();
        s.set_sfen("8+L/9/9/9/4k4/9/9/2K6/9 w 2br10PR");
        CHECK_EQUAL(B_NY, s.get_board()[SQ_1A]);
        CHECK_EQUAL(W_OU, s.get_board()[SQ_5E]);
        CHECK_EQUAL(B_OU, s.get_board()[SQ_7H]);
        CHECK_EQUAL(2, s.get_stand(vshogi::WHITE).count(KA));
        CHECK_EQUAL(1, s.get_stand(vshogi::WHITE).count(HI));
        CHECK_EQUAL(10, s.get_stand(vshogi::BLACK).count(FU));
        CHECK_EQUAL(1, s.get_stand(vshogi::BLACK).count(HI));
        CHECK_EQUAL(vshogi::WHITE, s.get_turn());
    }
    {
        const auto s = State("8+L/9/9/9/4k4/9/9/2K6/9 w 2br10PR");
        CHECK_EQUAL(B_NY, s.get_board()[SQ_1A]);
        CHECK_EQUAL(W_OU, s.get_board()[SQ_5E]);
        CHECK_EQUAL(B_OU, s.get_board()[SQ_7H]);
        CHECK_EQUAL(2, s.get_stand(vshogi::WHITE).count(KA));
        CHECK_EQUAL(1, s.get_stand(vshogi::WHITE).count(HI));
        CHECK_EQUAL(10, s.get_stand(vshogi::BLACK).count(FU));
        CHECK_EQUAL(1, s.get_stand(vshogi::BLACK).count(HI));
        CHECK_EQUAL(vshogi::WHITE, s.get_turn());
    }
}

TEST(state, apply)
{
    {
        auto s = State();
        s.set_sfen("8+L/8g/9/9/4k4/9/9/2K6/9 w 2br10PR");
        CHECK_EQUAL(0, s.get_stand(vshogi::WHITE).count(KY));
        CHECK_EQUAL(W_KI, s.get_board()[SQ_1B]);
        CHECK_EQUAL(B_NY, s.get_board()[SQ_1A]);
        s.apply(Move(SQ_1A, SQ_1B));
        CHECK_EQUAL(1, s.get_stand(vshogi::WHITE).count(KY));
        CHECK_EQUAL(VOID, s.get_board()[SQ_1B]);
        CHECK_EQUAL(W_KI, s.get_board()[SQ_1A]);
        CHECK_EQUAL(vshogi::BLACK, s.get_turn());
    }
}

TEST(state, to_sfen)
{
    {
        const auto s = State();
        const auto actual = s.to_sfen();
        STRCMP_EQUAL(
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b -",
            actual.c_str());
    }
}

} // namespace test_vshogi::test_shogi
