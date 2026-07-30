#include <algorithm>

#include "vshogi/common/notation.hpp"
#include "vshogi/variants/leshogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_leshogi
{

using namespace vshogi::leshogi;
using MT = vshogi::MoveTraits<Parameters>;
using NT = vshogi::Notation<Parameters>;

TEST_GROUP (test_leshogi_state) {
};

TEST(test_leshogi_state, set_sfen)
{
    {
        auto s = State();
        // W: -KAx2
        // +---+---+---+---+
        // |   |+NG|   |-OU|
        // +---+---+---+---+
        // |-HI|   |   |+TO|
        // +---+---+---+---+
        // |   |+OU|   |   |
        // +---+---+---+---+
        // |   |   |   |   |
        // +---+---+---+---+
        // |   |   |   |   |
        // +---+---+---+---+
        // |   |   |   |   |
        // +---+---+---+---+
        // B: +FU +KIx2 +GI +HI
        s.set_sfen("1+S1k/r2+P/1K2/4/4/4 b 2bP2GSR 1");
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

TEST(test_leshogi_state, to_sfen)
{
    auto s = State();
    s.set_sfen("1+S1k/r2+P/1K2/4/4/4 b 2bP2GSR 1");
    const auto actual = NT::to_sfen(s);
    STRCMP_EQUAL("1+S1k/r2+P/1K2/4/4/4 b R2GSP2b", actual.c_str());
}

TEST(test_leshogi_state, apply)
{
    {
        auto s = State();
        CHECK_EQUAL(VOID, s.get_board()[SQ_4C]);
        CHECK_EQUAL(W_FU, s.get_board()[SQ_4B]);
        s.apply(MT::make_move(SQ_4B, SQ_4C));
        CHECK_EQUAL(W_FU, s.get_board()[SQ_4C]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_4B]);
    }
    {
        auto s = State();
        s.apply(MT::make_move(SQ_2E, SQ_2B, true));
        CHECK_EQUAL(B_RY, s.get_board()[SQ_2B]);
        CHECK_EQUAL(1, s.get_stand(vshogi::BLACK).count(KA));
        CHECK_TRUE(s.in_check());
    }
}

TEST(test_leshogi_state, check)
{
    {
        auto s = State();
        CHECK_FALSE(s.in_check());
        CHECK_FALSE(s.in_double_check());
    }
    {
        auto s = State("3k/2B1/4/4/K2R/4 w -");
        CHECK_TRUE(s.in_check());
        CHECK_TRUE(s.in_double_check());
    }
    {
        auto s = State("3k/4/3B/4/K2R/4 b -");
        CHECK_FALSE(s.in_check());
        CHECK_FALSE(s.in_double_check());
        s.apply(MT::make_move(SQ_1C, SQ_2B));
        CHECK_TRUE(s.in_check());
        CHECK_TRUE(s.in_double_check());
        CHECK_EQUAL(SQ_2B, s.find_checker_square());
        CHECK_EQUAL(SQ_1E, s.find_checker_square(1));
    }
    {
        auto s = State("3k/4/4/4/K3/4 b R");
        CHECK_FALSE(s.in_check());
        CHECK_FALSE(s.in_double_check());
        s.apply(MT::make_move(HI, SQ_1B));
        CHECK_TRUE(s.in_check());
        CHECK_FALSE(s.in_double_check());
        CHECK_EQUAL(SQ_1B, s.find_checker_square());
        CHECK_EQUAL(SQ_NA, s.find_checker_square(1));
    }
}

TEST(test_leshogi_state, zobrist_hash)
{
    {
        const auto s1 = State("3k/4/P3/4/K3/4 b -");
        const auto s2 = State("3k/4/4/4/K3/4 w -");
        CHECK_TRUE(s1.zobrist_hash() != s2.zobrist_hash());
    }
    {
        const auto s1 = State("3k/4/P3/4/K3/4 b -");
        auto s2 = State("2k1/4/P3/4/K3/4 w -");
        auto hash_s2 = s2.zobrist_hash();
        CHECK_TRUE(s1.zobrist_hash() != hash_s2);
        s2.apply(MT::make_move(SQ_2A, SQ_1A), &hash_s2);
        CHECK_EQUAL(s1.zobrist_hash(), hash_s2);
    }
}

} // namespace test_vshogi::test_leshogi
