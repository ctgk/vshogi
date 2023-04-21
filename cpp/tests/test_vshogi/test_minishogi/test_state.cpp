#include "vshogi/minishogi/state.hpp"

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
        CHECK_EQUAL(W_OU, s.get_board()[SQ_11]);
        CHECK_EQUAL(B_TO, s.get_board()[SQ_12]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_13]);
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
        CHECK_EQUAL(VOID, s.get_board()[SQ_53]);
        CHECK_EQUAL(B_FU, s.get_board()[SQ_54]);
        s.apply(Move(SQ_53, SQ_54));
        CHECK_EQUAL(B_FU, s.get_board()[SQ_53]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_54]);
    }
}

TEST(state, is_legal)
{
    {
        auto s = State();
        CHECK_TRUE(s.is_legal(Move(SQ_53, SQ_54)));
        CHECK_FALSE(s.is_legal(Move(SQ_53, SQ_54, true)));
        CHECK_FALSE(s.is_legal(Move(SQ_52, SQ_54)));
    }
    {
        auto s = State();
        // W: -KAx2
        // +---+---+---+---+---+
        // |   |-HI|+NG|   |-OU|
        // +---+---+---+---+---+
        // |   |+HI|   |   |   |
        // +---+---+---+---+---+
        // |   |+OU|   |   |   |
        // +---+---+---+---+---+
        // |   |   |   |   |   |
        // +---+---+---+---+---+
        // |   |   |   |   |   |
        // +---+---+---+---+---+
        // B: +FUx2 +KIx2 +GI
        s.set_sfen("1r+S1k/1R3/1K3/5/5 b 2b2P2GS 1");

        CHECK_TRUE(s.is_legal(Move(SQ_13, FU)));
        CHECK_FALSE(s.is_legal(Move(SQ_51, FU))); // Unmovable
        CHECK_FALSE(s.is_legal(Move(SQ_12, FU))); // pawn drop checkmate

        CHECK_TRUE(s.is_legal(Move(SQ_41, SQ_42)));
        CHECK_TRUE(s.is_legal(Move(SQ_41, SQ_42, true)));
        CHECK_FALSE(s.is_legal(Move(SQ_51, SQ_42)));
        CHECK_FALSE(s.is_legal(Move(SQ_52, SQ_42))); // discovered check
    }
}

TEST(state, get_legal_moves)
{
    {
        auto s = State();
        // W:
        // +---+---+---+---+---+
        // |+KA|   |   |   |-OU|
        // +---+---+---+---+---+
        // |   |+KI|   |-FU|-HI|
        // +---+---+---+---+---+
        // |+GI|   |   |   |-GI|
        // +---+---+---+---+---+
        // |+HI|+FU|   |-KI|   |
        // +---+---+---+---+---+
        // |+OU|   |   |   |-KA|
        // +---+---+---+---+---+
        // B:
        s.set_sfen("B3k/1G1pr/S3s/RP1g1/K3b b - 1");

        const auto actual = s.get_legal_moves();
        CHECK_EQUAL(8, actual.size());
    }
    {
        auto s = State();
        // W: -FU
        // +---+---+---+---+---+
        // |+KA|   |   |   |-KA|
        // +---+---+---+---+---+
        // |+HI|+KI|   |-OU|   |
        // +---+---+---+---+---+
        // |+GI|+FU|   |   |-GI|
        // +---+---+---+---+---+
        // |   |   |   |   |-HI|
        // +---+---+---+---+---+
        // |+OU|   |-KI|   |   |
        // +---+---+---+---+---+
        // B:
        s.set_sfen("B3b/RG1k1/SP2s/4r/K1g2 w p 1");

        const auto actual = s.get_legal_moves();
        CHECK_EQUAL(
            // clang-format off
            4 // -OU (SQ_21, SQ_12, SQ_33, SQ_23)
            + 1 // -GI (SQ_24)
            + 5 // -HI (SQ_54, SQ_44, SQ_34, SQ_24, SQ_15)
            + 3 // -KI (SQ_34, SQ_45, SQ_25)
            + 10, // -FU (41, 31, 21, 32, 12, 33, 23, 44, 34, 24)
            // clang-format on
            actual.size());
    }
}

} // namespace test_vshogi::test_minishogi
