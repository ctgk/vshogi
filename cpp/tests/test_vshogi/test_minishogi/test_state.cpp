#include <algorithm>

#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(state){};

TEST(state, set_sfen)
{
    {
        auto s = State();
        // W: -KAx2
        // +---+---+---+---+---+
        // |   |   |+NG|   |-OU|
        // +---+---+---+---+---+
        // |   |-HI|   |   |+TO|
        // +---+---+---+---+---+
        // |   |   |+OU|   |   |
        // +---+---+---+---+---+
        // |   |   |   |   |   |
        // +---+---+---+---+---+
        // |   |   |   |   |   |
        // +---+---+---+---+---+
        // B: +FU +KIx2 +GI +HI
        s.set_sfen("2+S1k/1r2+P/2K2/5/5 b 2bP2GSR 1");
        CHECK_EQUAL(W_OU, s.get_board()[SQ_1A]);
        CHECK_EQUAL(B_TO, s.get_board()[SQ_1B]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_1C]);
        CHECK_EQUAL(1, s.get_stand(vshogi::BLACK).count(FU));
        CHECK_EQUAL(2, s.get_stand(vshogi::BLACK).count(KI));
        CHECK_EQUAL(0, s.get_stand(vshogi::BLACK).count(KA));
        CHECK_EQUAL(0, s.get_stand(vshogi::WHITE).count(FU));
        CHECK_EQUAL(2, s.get_stand(vshogi::WHITE).count(KA));
        CHECK_EQUAL(vshogi::BLACK, s.get_turn());
    }
}

TEST(state, to_sfen)
{
    auto s = State();
    s.set_sfen("2+S1k/1r2+P/2K2/5/5 b R2GSP2b 1");
    const auto actual = s.to_sfen();
    STRCMP_EQUAL("2+S1k/1r2+P/2K2/5/5 b R2GSP2b", actual.c_str());
}

TEST(state, apply)
{
    {
        auto s = State();
        CHECK_EQUAL(VOID, s.get_board()[SQ_5C]);
        CHECK_EQUAL(B_FU, s.get_board()[SQ_5D]);
        s.apply(Move(SQ_5C, SQ_5D));
        CHECK_EQUAL(B_FU, s.get_board()[SQ_5C]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_5D]);
    }
    {
        auto s = State();
        s.apply(Move(SQ_4C, SQ_2E));
        s.apply(Move(SQ_1C, SQ_1B));
        s.apply(Move(SQ_2A, SQ_4C, true));
        CHECK_EQUAL(B_UM, s.get_board()[SQ_2A]);
    }
}

TEST(state, check)
{
    {
        auto s = State();
        CHECK_FALSE(s.in_check());
        CHECK_FALSE(s.in_double_check());
    }
    {
        auto s = State("4k/3B1/5/5/K3R w -");
        CHECK_TRUE(s.in_check());
        CHECK_TRUE(s.in_double_check());
    }
    {
        auto s = State("4k/5/4B/5/K3R b -");
        CHECK_FALSE(s.in_check());
        CHECK_FALSE(s.in_double_check());
        s.apply(Move(SQ_2B, SQ_1C));
        CHECK_TRUE(s.in_check());
        CHECK_TRUE(s.in_double_check());
        CHECK_EQUAL(SQ_2B, s.get_checker_location());
        CHECK_EQUAL(SQ_1E, s.get_checker_location(1));
    }
    {
        auto s = State("4k/5/5/5/K4 b R");
        CHECK_FALSE(s.in_check());
        CHECK_FALSE(s.in_double_check());
        s.apply(Move(SQ_1B, HI));
        CHECK_TRUE(s.in_check());
        CHECK_FALSE(s.in_double_check());
        CHECK_EQUAL(SQ_1B, s.get_checker_location());
        CHECK_EQUAL(SQ_NA, s.get_checker_location(1));
    }
}

} // namespace test_vshogi::test_minishogi
