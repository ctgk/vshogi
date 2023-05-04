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

TEST(state, is_legal)
{
    {
        auto s = State();
        CHECK_TRUE(s.is_legal(Move(SQ_5C, SQ_5D)));
        CHECK_FALSE(s.is_legal(Move(SQ_5C, SQ_5D, true)));
        CHECK_FALSE(s.is_legal(Move(SQ_5B, SQ_5D)));
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

        CHECK_TRUE(s.is_legal(Move(SQ_1C, FU)));
        CHECK_FALSE(s.is_legal(Move(SQ_5A, FU))); // Unmovable
        CHECK_FALSE(s.is_legal(Move(SQ_1B, FU))); // pawn drop checkmate

        CHECK_TRUE(s.is_legal(Move(SQ_4A, SQ_4B)));
        CHECK_TRUE(s.is_legal(Move(SQ_4A, SQ_4B, true)));
        CHECK_FALSE(s.is_legal(Move(SQ_5A, SQ_4B)));
        CHECK_FALSE(s.is_legal(Move(SQ_5B, SQ_4B))); // discovered check
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
            4 // -OU (SQ_2A, SQ_1B, SQ_3C, SQ_2C)
            + 1 // -GI (SQ_2D)
            + 6 // -HI (SQ_5D, SQ_4D, SQ_3D, SQ_2D, SQ_1E, SQ_1E+)
            + 3 // -KI (SQ_3D, SQ_4E, SQ_2E)
            + 10, // -FU (41, 31, 21, 32, 12, 33, 23, 44, 34, 24)
            // clang-format on
            actual.size());
    }
    {
        auto s = State();
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |   |-OU|
        //   *---*---*---*---*---*
        // B |+OU|   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |+KA|   |   |   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: -
        s.set_sfen("4k/K4/1B3/5/5 b - 1");

        const auto actual = s.get_legal_moves();
        CHECK_EQUAL(
            // clang-format off
            6 // +KA (SQ_2A, SQ_2A+, SQ_3B, SQ_5D, SQ_3D, SQ_2E)
            + 4, // +OU (SQ_5A, 4A, 4B, 5C)
            // clang-format on
            actual.size());
    }
}

} // namespace test_vshogi::test_minishogi
