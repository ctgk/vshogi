#include <algorithm>

#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP(state){};

TEST(state, set_sfen)
{
    {
        auto s = State();
        // W: -KAx2
        // +---+---+---+---+---+---+
        // |   |   |+NG|   |-OU|   |
        // +---+---+---+---+---+---+
        // |   |-HI|   |   |+TO|   |
        // +---+---+---+---+---+---+
        // |   |   |+OU|   |   |+NK|
        // +---+---+---+---+---+---+
        // |   |   |   |   |-KE|   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // B: +FU +GI +KIx2 +HI
        s.set_sfen("2+S1k1/1r2+P1/2K2+N/4n1/6/6 w R2GSP2b 1");
        CHECK_EQUAL(W_OU, s.get_board()[SQ_2A]);
        CHECK_EQUAL(B_TO, s.get_board()[SQ_2B]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_2C]);
        CHECK_EQUAL(1, s.get_stand(vshogi::BLACK).count(FU));
        CHECK_EQUAL(2, s.get_stand(vshogi::BLACK).count(KI));
        CHECK_EQUAL(0, s.get_stand(vshogi::BLACK).count(KA));
        CHECK_EQUAL(0, s.get_stand(vshogi::WHITE).count(FU));
        CHECK_EQUAL(2, s.get_stand(vshogi::WHITE).count(KA));
        CHECK_EQUAL(vshogi::WHITE, s.get_turn());
    }
}

TEST(state, to_sfen)
{
    auto s = State();
    s.set_sfen("2+S1k1/1r2+P1/2K2+N/4n1/6/6 w R2GSP2b 1");
    const auto actual = s.to_sfen();
    STRCMP_EQUAL("2+S1k1/1r2+P1/2K2+N/4n1/6/6 w R2GSP2b", actual.c_str());
}

TEST(state, apply)
{
    {
        auto s = State();
        CHECK_EQUAL(VOID, s.get_board()[SQ_6D]);
        CHECK_EQUAL(B_FU, s.get_board()[SQ_6E]);
        s.apply(Move(SQ_6D, SQ_6E));
        CHECK_EQUAL(B_FU, s.get_board()[SQ_6D]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_6E]);
    }
    {
        auto s = State()
                     .apply(Move(SQ_4D, SQ_3F))
                     .apply(Move(SQ_1C, SQ_1B))
                     .apply(Move(SQ_5B, SQ_4D, true));
        CHECK_EQUAL(B_NK, s.get_board()[SQ_5B]);
    }
}

} // namespace test_vshogi::test_judkins_shogi
