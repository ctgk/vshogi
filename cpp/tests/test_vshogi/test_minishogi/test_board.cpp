#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP (minishogi_board) {
};

TEST(minishogi_board, get)
{
    auto b = Board();
    CHECK_EQUAL(W_HI, b[SQ_5A]);
    CHECK_EQUAL(B_GI, b[SQ_3E]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(minishogi_board, set)
{
    auto b = Board();
    b.apply(SQ_2D, W_GI);
    CHECK_EQUAL(W_GI, b[SQ_2D]);
    CHECK_EQUAL(W_HI, b[SQ_5A]);
    CHECK_EQUAL(B_GI, b[SQ_3E]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(minishogi_board, hflip)
{
    const auto b = Board();
    const auto actual = b.hflip();
    CHECK_EQUAL(W_OU, actual[SQ_5A]);
    CHECK_EQUAL(VOID, actual[SQ_1B]);
    CHECK_EQUAL(B_FU, actual[SQ_1D]);
    CHECK_EQUAL(B_HI, actual[SQ_5E]);
}

TEST(minishogi_board, set_sfen)
{
    {
        const char sfen[] = "2+S1k/1r2+P/2K2/5/5 b 2bP2GSR 1";
        auto b = Board();
        const auto actual = b.set_sfen(sfen);
        CHECK_EQUAL(VOID, b[SQ_5A]);
        CHECK_EQUAL(VOID, b[SQ_4A]);
        CHECK_EQUAL(B_NG, b[SQ_3A]);
        CHECK_EQUAL(VOID, b[SQ_2A]);
        CHECK_EQUAL(W_OU, b[SQ_1A]);
        CHECK_EQUAL(VOID, b[SQ_5B]);
        CHECK_EQUAL(W_HI, b[SQ_4B]);
        CHECK_EQUAL(VOID, b[SQ_3B]);
        CHECK_EQUAL(VOID, b[SQ_2B]);
        CHECK_EQUAL(B_TO, b[SQ_1B]);
        CHECK_EQUAL(VOID, b[SQ_5C]);
        CHECK_EQUAL(VOID, b[SQ_4C]);
        CHECK_EQUAL(B_OU, b[SQ_3C]);
        CHECK_EQUAL(VOID, b[SQ_2C]);
        CHECK_EQUAL(VOID, b[SQ_1C]);
        CHECK_EQUAL(VOID, b[SQ_5D]);
        CHECK_EQUAL(VOID, b[SQ_4D]);
        CHECK_EQUAL(VOID, b[SQ_3D]);
        CHECK_EQUAL(VOID, b[SQ_2D]);
        CHECK_EQUAL(VOID, b[SQ_1D]);
        CHECK_EQUAL(VOID, b[SQ_5E]);
        CHECK_EQUAL(VOID, b[SQ_4E]);
        CHECK_EQUAL(VOID, b[SQ_3E]);
        CHECK_EQUAL(VOID, b[SQ_2E]);
        CHECK_EQUAL(VOID, b[SQ_1E]);
        CHECK_EQUAL('b', actual[0]);
        CHECK_EQUAL(' ', actual[1]);
    }
    {
        const char sfen[] = "+p+r+b+s+P/+R+B+S2/4k/5/K4 b -";
        auto b = Board();
        const auto actual = b.set_sfen(sfen);
        CHECK_EQUAL(W_TO, b[SQ_5A]);
        CHECK_EQUAL(W_RY, b[SQ_4A]);
        CHECK_EQUAL(W_UM, b[SQ_3A]);
        CHECK_EQUAL(W_NG, b[SQ_2A]);
        CHECK_EQUAL(B_TO, b[SQ_1A]);
        CHECK_EQUAL(B_RY, b[SQ_5B]);
        CHECK_EQUAL(B_UM, b[SQ_4B]);
        CHECK_EQUAL(B_NG, b[SQ_3B]);
        CHECK_EQUAL(VOID, b[SQ_2B]);
        CHECK_EQUAL(VOID, b[SQ_1B]);
        CHECK_EQUAL(VOID, b[SQ_5C]);
        CHECK_EQUAL(VOID, b[SQ_4C]);
        CHECK_EQUAL(VOID, b[SQ_3C]);
        CHECK_EQUAL(VOID, b[SQ_2C]);
        CHECK_EQUAL(W_OU, b[SQ_1C]);
        CHECK_EQUAL(VOID, b[SQ_5D]);
        CHECK_EQUAL(VOID, b[SQ_4D]);
        CHECK_EQUAL(VOID, b[SQ_3D]);
        CHECK_EQUAL(VOID, b[SQ_2D]);
        CHECK_EQUAL(VOID, b[SQ_1D]);
        CHECK_EQUAL(B_OU, b[SQ_5E]);
        CHECK_EQUAL(VOID, b[SQ_4E]);
        CHECK_EQUAL(VOID, b[SQ_3E]);
        CHECK_EQUAL(VOID, b[SQ_2E]);
        CHECK_EQUAL(VOID, b[SQ_1E]);
        CHECK_EQUAL('b', actual[0]);
        CHECK_EQUAL(' ', actual[1]);
    }
}

TEST(minishogi_board, append_sfen)
{
    const char sfen[] = "2+S1k/1r2+P/2K2/5/5 b 2bP2GSR 1";
    auto b = Board();
    b.set_sfen(sfen);

    const char expected[] = "2+S1k/1r2+P/2K2/5/5";
    auto actual = std::string();
    b.append_sfen(actual);
    STRCMP_EQUAL(expected, actual.c_str());
}

TEST(minishogi_board, apply)
{
    {
        auto b = Board();
        b.apply(SQ_5D, VOID);
        CHECK_EQUAL(
            (bb_5e | bb_4e | bb_3e | bb_2e | bb_1e).value(),
            b.get_occupied(vshogi::BLACK).value());
    }
    {
        auto b = Board();
        b.apply(SQ_5D, SQ_5A);
        CHECK_EQUAL(
            (bb_4a | bb_3a | bb_2a | bb_1a | bb_1b | bb_5d).value(),
            b.get_occupied(vshogi::WHITE).value());
    }
}

TEST(minishogi_board, get_occupied)
{
    {
        const auto b = Board("4k/5/5/5/K4 b");
        CHECK_EQUAL((bb_1a | bb_5e).value(), b.get_occupied().value());
        CHECK_EQUAL(bb_5e.value(), b.get_occupied(vshogi::BLACK).value());
        CHECK_EQUAL(bb_1a.value(), b.get_occupied(vshogi::WHITE).value());
        CHECK_EQUAL(0, b.get_occupied(FU).value());
        CHECK_EQUAL(0, b.get_occupied(B_FU).value());
        CHECK_EQUAL(0, b.get_occupied(W_FU).value());
        CHECK_EQUAL((bb_1a | bb_5e).value(), b.get_occupied(OU).value());
        CHECK_EQUAL(bb_5e.value(), b.get_occupied(B_OU).value());
        CHECK_EQUAL(bb_1a.value(), b.get_occupied(W_OU).value());
    }
    {
        auto b = Board("4k/5/5/5/K4 b");
        b.apply(SQ_1A, SQ_5E);
        CHECK_EQUAL(bb_1a.value(), b.get_occupied().value());
        CHECK_EQUAL(bb_1a.value(), b.get_occupied(vshogi::BLACK).value());
        CHECK_EQUAL(0, b.get_occupied(vshogi::WHITE).value());
        CHECK_EQUAL(0, b.get_occupied(FU).value());
        CHECK_EQUAL(0, b.get_occupied(B_FU).value());
        CHECK_EQUAL(0, b.get_occupied(W_FU).value());
        CHECK_EQUAL(bb_1a.value(), b.get_occupied(OU).value());
        CHECK_EQUAL(bb_1a.value(), b.get_occupied(B_OU).value());
        CHECK_EQUAL(0, b.get_occupied(W_OU).value());
    }
    {
        auto b = Board("4k/5/5/5/K4 b");
        b.apply(SQ_1A, B_FU);
        CHECK_EQUAL((bb_1a | bb_5e).value(), b.get_occupied().value());
        CHECK_EQUAL(
            (bb_1a | bb_5e).value(), b.get_occupied(vshogi::BLACK).value());
        CHECK_EQUAL(0, b.get_occupied(vshogi::WHITE).value());
        CHECK_EQUAL(bb_1a.value(), b.get_occupied(FU).value());
        CHECK_EQUAL(bb_1a.value(), b.get_occupied(B_FU).value());
        CHECK_EQUAL(0, b.get_occupied(W_FU).value());
        CHECK_EQUAL(bb_5e.value(), b.get_occupied(OU).value());
        CHECK_EQUAL(bb_5e.value(), b.get_occupied(B_OU).value());
        CHECK_EQUAL(0, b.get_occupied(W_OU).value());
    }
    {
        // W_GI VOID VOID VOID VOID
        // W_OU W_GI VOID B_RY VOID
        // VOID VOID VOID B_KI B_FU
        // B_FU B_OU VOID VOID VOID
        // VOID VOID VOID W_KA B_HI
        auto b = Board("s4/ks1+R1/3GP/PK3/3bR w -");
        const auto actual = b.get_occupied<KA, HI, UM, RY>(vshogi::WHITE);
        CHECK_EQUAL(bb_2e.value(), actual.value());
    }
}

TEST(minishogi_board, is_square_attacked)
{
    {
        // by FU.
        const auto b = Board("5/5/2P2/5/5 b");
        CHECK_TRUE(b.is_square_attacked(vshogi::BLACK, SQ_3B, SQ_3B));
        CHECK_FALSE(b.is_square_attacked(vshogi::BLACK, SQ_2B, SQ_2B));
    }
    {
        // by GI.
        const auto b = Board("5/5/2S2/5/5 b");
        CHECK_TRUE(b.is_square_attacked(vshogi::BLACK, SQ_3B, SQ_3B));
        CHECK_FALSE(b.is_square_attacked(vshogi::BLACK, SQ_3D, SQ_3D));
    }
    {
        // by gold like piece.
        const auto b = Board("5/5/2+P2/5/5 b");
        CHECK_TRUE(b.is_square_attacked(vshogi::BLACK, SQ_3B, SQ_3B));
        CHECK_FALSE(b.is_square_attacked(vshogi::BLACK, SQ_2D, SQ_2D));
    }
    {
        // by king like piece.
        const auto b = Board("5/5/5/5/4+R b");
        CHECK_TRUE(b.is_square_attacked(vshogi::BLACK, SQ_2D, SQ_2D));
        CHECK_FALSE(b.is_square_attacked(vshogi::BLACK, SQ_2C, SQ_2C));
    }
    {
        // by a ranging piece.
        const auto b = Board("5/5/5/5/1P2R b");
        CHECK_TRUE(b.is_square_attacked(vshogi::BLACK, SQ_1A, SQ_1A));
        CHECK_FALSE(b.is_square_attacked(vshogi::BLACK, SQ_2A, SQ_2A));
        CHECK_TRUE(b.is_square_attacked(vshogi::BLACK, SQ_4E, SQ_4E));
        CHECK_FALSE(b.is_square_attacked(vshogi::BLACK, SQ_5E, SQ_5E));
    }
    {
        // Turn: WHITE
        // White: HI
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |+HI|   |+NG|   |   |
        //   *---*---*---*---*---*
        // B |   |   |   |   |-OU|
        //   *---*---*---*---*---*
        // C |+GI|   |   |-UM|-KI|
        //   *---*---*---*---*---*
        // D |+FU|+OU|   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |+KA|   |
        //   *---*---*---*---*---*
        // Black: -
        const auto b = Board("R1+S2/4k/S2+bg/PK3/3B1 w r 40");
        CHECK_TRUE(b.is_square_attacked(vshogi::BLACK, SQ_2A, SQ_2A));
    }
    {
        const auto b = Board("k3b/5/2K2/5/5 b");
        CHECK_TRUE(b.is_square_attacked(vshogi::WHITE, SQ_4D, SQ_3C));
        CHECK_FALSE(b.is_square_attacked(vshogi::WHITE, SQ_2D, SQ_3C));
    }
}

TEST(minishogi_board, find_pinned)
{
    const auto b = Board("b3+r/5/2P1P/5/r1P1K");
    const auto actual = b.find_pinned(vshogi::BLACK);
    const auto expect = bb_3c | bb_1c | bb_3e;
    CHECK_EQUAL(expect.value(), actual.value());
}

} // namespace test_vshogi::test_minishogi
