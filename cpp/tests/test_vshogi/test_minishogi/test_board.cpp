#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;
using namespace vshogi;
using Board = vshogi::minishogi::Board;
using BT = vshogi::BitboardTraits<Parameters>;

TEST_GROUP (test_minishogi_board) {
};

TEST(test_minishogi_board, get)
{
    auto b = Board();
    CHECK_EQUAL(W_HI, b[SQ_5A]);
    CHECK_EQUAL(B_GI, b[SQ_3E]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(test_minishogi_board, place_at)
{
    auto b = Board();
    b.place_at(SQ_2D, W_GI);
    CHECK_EQUAL(W_GI, b[SQ_2D]);
    CHECK_EQUAL(W_HI, b[SQ_5A]);
    CHECK_EQUAL(B_GI, b[SQ_3E]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(test_minishogi_board, hflip)
{
    const auto b = Board();
    const auto actual = b.hflip();
    CHECK_EQUAL(W_OU, actual[SQ_5A]);
    CHECK_EQUAL(VOID, actual[SQ_1B]);
    CHECK_EQUAL(B_FU, actual[SQ_1D]);
    CHECK_EQUAL(B_HI, actual[SQ_5E]);
}

TEST(test_minishogi_board, set_sfen)
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

TEST(test_minishogi_board, append_sfen)
{
    const char sfen[] = "2+S1k/1r2+P/2K2/5/5 b 2bP2GSR 1";
    auto b = Board();
    b.set_sfen(sfen);
    CHECK_EQUAL(W_OU, b[SQ_1A]);

    const char expected[] = "2+S1k/1r2+P/2K2/5/5";
    auto actual = std::string();
    b.append_sfen(actual);
    STRCMP_EQUAL(expected, actual.c_str());
}

TEST(test_minishogi_board, pop_from)
{
    auto b = Board();
    CHECK_TRUE(b[SQ_5D] != VOID);
    b.pop_from(SQ_5D);
    CHECK_TRUE(b[SQ_5D] == VOID);
}

TEST(test_minishogi_board, get_occupied)
{
    {
        const auto b = Board("4k/5/5/5/K4 b");
        CHECK_EQUAL(0b1000000000000000000000001u, b.get_occupied());
        CHECK_EQUAL(0b1000000000000000000000000u, b.get_occupied(BLACK));
        CHECK_EQUAL(0b0000000000000000000000001u, b.get_occupied(WHITE));
        CHECK_EQUAL(0, b.get_occupied(FU));
        CHECK_EQUAL(0, b.get_occupied(B_FU));
        CHECK_EQUAL(0, b.get_occupied(W_FU));
        CHECK_EQUAL(0b1000000000000000000000001u, b.get_occupied(OU));
        CHECK_EQUAL(0b1000000000000000000000000u, b.get_occupied(B_OU));
        CHECK_EQUAL(0b0000000000000000000000001u, b.get_occupied(W_OU));
    }
    {
        auto b = Board("4k/5/5/5/K4 b");
        b.place_at(SQ_1A, b.pop_from(SQ_5E));
        CHECK_EQUAL(0b0000000000000000000000001u, b.get_occupied());
        CHECK_EQUAL(0b0000000000000000000000001u, b.get_occupied(BLACK));
        CHECK_EQUAL(0, b.get_occupied(WHITE));
        CHECK_EQUAL(0, b.get_occupied(FU));
        CHECK_EQUAL(0, b.get_occupied(B_FU));
        CHECK_EQUAL(0, b.get_occupied(W_FU));
        CHECK_EQUAL(0b0000000000000000000000001u, b.get_occupied(OU));
        CHECK_EQUAL(0b0000000000000000000000001u, b.get_occupied(B_OU));
        CHECK_EQUAL(0, b.get_occupied(W_OU));
    }
    {
        auto b = Board("4k/5/5/5/K4 b");
        b.place_at(SQ_1A, B_FU);
        CHECK_EQUAL(0b1000000000000000000000001u, b.get_occupied());
        CHECK_EQUAL(0b1000000000000000000000001u, b.get_occupied(BLACK));
        CHECK_EQUAL(0, b.get_occupied(WHITE));
        CHECK_EQUAL(0b0000000000000000000000001u, b.get_occupied(FU));
        CHECK_EQUAL(0b0000000000000000000000001u, b.get_occupied(B_FU));
        CHECK_EQUAL(0, b.get_occupied(W_FU));
        CHECK_EQUAL(0b1000000000000000000000000u, b.get_occupied(OU));
        CHECK_EQUAL(0b1000000000000000000000000u, b.get_occupied(B_OU));
        CHECK_EQUAL(0, b.get_occupied(W_OU));
    }
    {
        // W_GI VOID VOID VOID VOID
        // W_OU W_GI VOID B_RY VOID
        // VOID VOID VOID B_KI B_FU
        // B_FU B_OU VOID VOID VOID
        // VOID VOID VOID W_KA B_HI
        auto b = Board("s4/ks1+R1/3GP/PK3/3bR w -");
        CHECK_EQUAL(
            0b0000000000000000001010000u, b.get_occupied_by_slider(BLACK));
        CHECK_EQUAL(
            0b0000000000000001000000000u, b.get_occupied_by_slider(WHITE));
    }
}

TEST(test_minishogi_board, find_pinned)
{
    {
        const auto b = Board("b3+r/5/2P1P/5/r1P1K");
        CHECK_EQUAL(0b0000000000101000000000100u, b.find_pinned(BLACK));
        CHECK_EQUAL(0, b.find_pinned(WHITE));
    }
    {
        const auto b = Board("2s1R/2k2/4G/5/4K");
        CHECK_EQUAL(0, b.find_pinned(BLACK));
        CHECK_EQUAL(0, b.find_pinned(WHITE));
    }
}

TEST(test_minishogi_board, find_sliding_attacker)
{
    {
        const auto b = Board("5/5/5/5/4R");
        const auto actual
            = b.find_sliding_attacker(BLACK, SQ_1A, vshogi::DIR_S);
        CHECK_EQUAL(SQ_1E, actual);
    }
    {
        const auto b = Board("5/5/4P/5/4R");
        const auto actual
            = b.find_sliding_attacker(vshogi::BLACK, SQ_1A, vshogi::DIR_S);
        CHECK_EQUAL(SQ_NA, actual);
    }
    {
        const auto b = Board("5/5/4P/5/4R");
        const auto actual = b.find_sliding_attacker(
            vshogi::BLACK, SQ_1A, vshogi::DIR_S, SQ_1C);
        CHECK_EQUAL(SQ_1E, actual);
    }
    {
        const auto b = Board("5/5/4P/4B/4R");
        const auto actual = b.find_sliding_attacker(
            vshogi::BLACK, SQ_1A, vshogi::DIR_S, SQ_1C);
        CHECK_EQUAL(SQ_NA, actual);
    }
}

TEST(test_minishogi_board, compute_movable_to)
{
    {
        const auto b = Board("5/5/5/5/5");
        const auto actual = b.compute_movable_to(SQ_1A, BLACK, BT::full());
        CHECK_EQUAL(0, actual);
    }
    {
        const auto b = Board("5/5/5/5/4R");
        const auto actual = b.compute_movable_to(SQ_1A, BLACK, BT::full());
        CHECK_EQUAL(0b0000000000000000000010000u, actual);
    }
    {
        const auto b = Board("5/4P/5/5/4R");
        const auto actual = b.compute_movable_to(SQ_1A, BLACK, BT::full());
        CHECK_EQUAL(0b0000000000000000000000010u, actual);
    }
    {
        // Turn: WHITE
        // White: GI
        //     5   4   3   2   1
        //   +---+---+---+---+---+
        // A |   |   |-FU|-OU|+RY|
        //   +---+---+---+---+---+
        // B |   |   |   |-KA|   |
        //   +---+---+---+---+---+
        // C |   |   |-UM|+KI|   |
        //   +---+---+---+---+---+
        // D |   |   |   |   |   |
        //   +---+---+---+---+---+
        // E |   |   |   |   |   |
        //   +---+---+---+---+---+
        // Black: GI
        const auto b = Board("2pk+R/3b1/2+bG1/5/5");
        const auto actual = b.compute_movable_to(
            SQ_1A, WHITE, BT::invert(BT::from_square(SQ_2A)));
        CHECK_EQUAL(0b0000000000000000001000000u, actual);
    }
}

TEST(test_minishogi_board, get_occupied_by_slider)
{
    {
        const auto b = Board("4k/5/5/3S1/K3r");
        CHECK_EQUAL(0, b.get_occupied_by_slider(vshogi::BLACK));
    }
}

TEST(test_minishogi_board, rotate)
{
    const auto b = Board("4k/5/5/3S1/r3K");
    const auto actual = b.rotate();
    CHECK_EQUAL(B_OU, actual[SQ_5E]);
    CHECK_EQUAL(W_GI, actual[SQ_4B]);
    CHECK_EQUAL(VOID, actual[SQ_3C]);
}

TEST(test_minishogi_board, compute_king_movable)
{
    {
        // in single check
        const auto b = Board("2k1+R/5/5/5/5 w -");
        CHECK_EQUAL(
            0b0000000010000100000000000u, b.compute_king_movable(WHITE));
    }
    {
        // double check
        const auto b = Board("2k1+R/5/B4/5/5 w -");
        CHECK_EQUAL(
            0b0000000000000100000000000u, b.compute_king_movable(WHITE));
    }
    {
        // no check
        const auto b = Board("2k2/5/3+R1/5/5 w -");
        CHECK_EQUAL(
            0b0000000011000000000000000u, b.compute_king_movable(WHITE));
    }
}

} // namespace test_vshogi::test_minishogi
