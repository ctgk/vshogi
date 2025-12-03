#include <algorithm>

#include "vshogi/common/notation.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;
using MT = vshogi::MoveTraits<Parameters>;
using NT = vshogi::Notation<Parameters>;

TEST_GROUP (test_minishogi_state) {
};

TEST(test_minishogi_state, set_sfen)
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

TEST(test_minishogi_state, to_sfen)
{
    auto s = State();
    s.set_sfen("2+S1k/1r2+P/2K2/5/5 b R2GSP2b 1");
    const auto actual = NT::to_sfen(s);
    STRCMP_EQUAL("2+S1k/1r2+P/2K2/5/5 b R2GSP2b", actual.c_str());
}

TEST(test_minishogi_state, apply)
{
    {
        auto s = State();
        CHECK_EQUAL(VOID, s.get_board()[SQ_5C]);
        CHECK_EQUAL(B_FU, s.get_board()[SQ_5D]);
        s.apply(MT::make_move(SQ_5D, SQ_5C));
        CHECK_EQUAL(B_FU, s.get_board()[SQ_5C]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_5D]);
    }
    {
        auto s = State();
        s.apply(MT::make_move(SQ_2E, SQ_4C));
        s.apply(MT::make_move(SQ_1B, SQ_1C));
        s.apply(MT::make_move(SQ_4C, SQ_2A, true));
        CHECK_EQUAL(B_UM, s.get_board()[SQ_2A]);
    }
}

TEST(test_minishogi_state, check)
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
        s.apply(MT::make_move(SQ_1C, SQ_2B));
        CHECK_TRUE(s.in_check());
        CHECK_TRUE(s.in_double_check());
        CHECK_EQUAL(SQ_2B, s.find_checker_square());
        CHECK_EQUAL(SQ_1E, s.find_checker_square(1));
    }
    {
        auto s = State("4k/5/5/5/K4 b R");
        CHECK_FALSE(s.in_check());
        CHECK_FALSE(s.in_double_check());
        s.apply(MT::make_move(HI, SQ_1B));
        CHECK_TRUE(s.in_check());
        CHECK_FALSE(s.in_double_check());
        CHECK_EQUAL(SQ_1B, s.find_checker_square());
        CHECK_EQUAL(SQ_NA, s.find_checker_square(1));
    }
}

TEST(test_minishogi_state, zobrist_hash)
{
    {
        const auto s1 = State("4k/5/P4/5/K4 b -");
        const auto s2 = State("4k/5/5/5/K4 w -");
        CHECK_TRUE(s1.zobrist_hash() != s2.zobrist_hash());
    }
    {
        const auto s1 = State("4k/5/P4/5/K4 b -");
        auto s2 = State("3k1/5/P4/5/K4 w -");
        auto hash_s2 = s2.zobrist_hash();
        CHECK_TRUE(s1.zobrist_hash() != hash_s2);
        s2.apply(MT::make_move(SQ_2A, SQ_1A), &hash_s2);
        CHECK_EQUAL(s1.zobrist_hash(), hash_s2);
    }
}

} // namespace test_vshogi::test_minishogi
