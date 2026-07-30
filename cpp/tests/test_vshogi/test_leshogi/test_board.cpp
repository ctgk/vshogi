#include "vshogi/common/notation.hpp"
#include "vshogi/variants/leshogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_leshogi
{

using namespace vshogi::leshogi;
using namespace vshogi;
using Board = vshogi::leshogi::Board;
using BT = vshogi::BitboardTraits<Parameters>;
using NT = vshogi::Notation<Parameters>;

TEST_GROUP (test_leshogi_board) {
};

TEST(test_leshogi_board, test_get)
{
    auto b = Board();
    CHECK_EQUAL(W_GI, b[SQ_4A]);
    CHECK_EQUAL(B_KA, b[SQ_3E]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(test_leshogi_board, test_place_at)
{
    auto b = Board();
    b.place_at(SQ_2D, W_GI);
    CHECK_EQUAL(W_GI, b[SQ_2D]);
    CHECK_EQUAL(W_GI, b[SQ_4A]);
    CHECK_EQUAL(B_KA, b[SQ_3E]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(test_leshogi_board, test_hflip)
{
    const auto b = Board();
    const auto actual = b.hflip();
    CHECK_EQUAL(W_OU, actual[SQ_2A]);
    CHECK_EQUAL(W_FU, actual[SQ_1B]);
    CHECK_EQUAL(VOID, actual[SQ_1D]);
    CHECK_EQUAL(B_FU, actual[SQ_4E]);
}

TEST(test_leshogi_board, test_set_sfen)
{
    {
        const char sfen[] = "2k1/g3/4/4/P3/1K2 b - 1";
        auto b = Board();
        const auto actual = b.set_sfen(sfen);
        CHECK_EQUAL(VOID, b[SQ_4A]);
        CHECK_EQUAL(VOID, b[SQ_3A]);
        CHECK_EQUAL(W_OU, b[SQ_2A]);
        CHECK_EQUAL(VOID, b[SQ_1A]);
        CHECK_EQUAL(W_KI, b[SQ_4B]);
        CHECK_EQUAL(VOID, b[SQ_3B]);
        CHECK_EQUAL(VOID, b[SQ_2B]);
        CHECK_EQUAL(VOID, b[SQ_1B]);
        CHECK_EQUAL(VOID, b[SQ_4C]);
        CHECK_EQUAL(VOID, b[SQ_3C]);
        CHECK_EQUAL(VOID, b[SQ_2C]);
        CHECK_EQUAL(VOID, b[SQ_1C]);
        CHECK_EQUAL(VOID, b[SQ_4D]);
        CHECK_EQUAL(VOID, b[SQ_3D]);
        CHECK_EQUAL(VOID, b[SQ_2D]);
        CHECK_EQUAL(VOID, b[SQ_1D]);
        CHECK_EQUAL(B_FU, b[SQ_4E]);
        CHECK_EQUAL(VOID, b[SQ_3E]);
        CHECK_EQUAL(VOID, b[SQ_2E]);
        CHECK_EQUAL(VOID, b[SQ_1E]);
        CHECK_EQUAL(VOID, b[SQ_4F]);
        CHECK_EQUAL(B_OU, b[SQ_3F]);
        CHECK_EQUAL(VOID, b[SQ_2F]);
        CHECK_EQUAL(VOID, b[SQ_1F]);
        CHECK_EQUAL('b', actual[0]);
        CHECK_EQUAL(' ', actual[1]);
    }
    {
        const char sfen[] = "+r+b+s+P/+B+S2/3k/4/K3/4 b -";
        auto b = Board();
        const auto actual = b.set_sfen(sfen);
        CHECK_EQUAL(W_RY, b[SQ_4A]);
        CHECK_EQUAL(W_UM, b[SQ_3A]);
        CHECK_EQUAL(W_NG, b[SQ_2A]);
        CHECK_EQUAL(B_TO, b[SQ_1A]);
        CHECK_EQUAL(B_UM, b[SQ_4B]);
        CHECK_EQUAL(B_NG, b[SQ_3B]);
        CHECK_EQUAL(VOID, b[SQ_2B]);
        CHECK_EQUAL(VOID, b[SQ_1B]);
        CHECK_EQUAL(VOID, b[SQ_4C]);
        CHECK_EQUAL(VOID, b[SQ_3C]);
        CHECK_EQUAL(VOID, b[SQ_2C]);
        CHECK_EQUAL(W_OU, b[SQ_1C]);
        CHECK_EQUAL(VOID, b[SQ_4D]);
        CHECK_EQUAL(VOID, b[SQ_3D]);
        CHECK_EQUAL(VOID, b[SQ_2D]);
        CHECK_EQUAL(VOID, b[SQ_1D]);
        CHECK_EQUAL(B_OU, b[SQ_4E]);
        CHECK_EQUAL(VOID, b[SQ_3E]);
        CHECK_EQUAL(VOID, b[SQ_2E]);
        CHECK_EQUAL(VOID, b[SQ_1E]);
        CHECK_EQUAL(VOID, b[SQ_4F]);
        CHECK_EQUAL(VOID, b[SQ_3F]);
        CHECK_EQUAL(VOID, b[SQ_2F]);
        CHECK_EQUAL(VOID, b[SQ_1F]);
        CHECK_EQUAL('b', actual[0]);
        CHECK_EQUAL(' ', actual[1]);
    }
}

TEST(test_leshogi_board, test_to_sfen)
{
    const auto b = Board();
    const char expected[] = "sknl/prbg/4/4/GBRP/LNKS";
    const auto actual = NT::to_sfen(b);
    STRCMP_EQUAL(expected, actual.c_str());
}

TEST(test_leshogi_board, test_pop_from)
{
    auto b = Board();
    CHECK_TRUE(b[SQ_1A] != VOID);
    const auto actual = b.pop_from(SQ_1A);
    CHECK_EQUAL(W_KY, actual);
    CHECK_TRUE(b[SQ_1A] == VOID);
}

TEST(test_leshogi_board, test_get_occupied)
{
    {
        const auto b = Board("3k/4/4/4/4/K3 b");
        CHECK_EQUAL(0b100000000000000000000001u, b.get_occupied());
        CHECK_EQUAL(0b100000000000000000000000u, b.get_occupied(BLACK));
        CHECK_EQUAL(0b000000000000000000000001u, b.get_occupied(WHITE));
        CHECK_EQUAL(0, b.get_occupied(FU));
        CHECK_EQUAL(0, b.get_occupied(B_FU));
        CHECK_EQUAL(0, b.get_occupied(W_FU));
        CHECK_EQUAL(0b100000000000000000000001u, b.get_occupied(OU));
        CHECK_EQUAL(0b100000000000000000000000u, b.get_occupied(B_OU));
        CHECK_EQUAL(0b000000000000000000000001u, b.get_occupied(W_OU));
    }
    {
        auto b = Board("3k/4/4/4/4/K3 b");
        b.place_at(SQ_1A, b.pop_from(SQ_4F));
        CHECK_EQUAL(0b000000000000000000000001u, b.get_occupied());
        CHECK_EQUAL(0b000000000000000000000001u, b.get_occupied(BLACK));
        CHECK_EQUAL(0, b.get_occupied(WHITE));
        CHECK_EQUAL(0, b.get_occupied(FU));
        CHECK_EQUAL(0, b.get_occupied(B_FU));
        CHECK_EQUAL(0, b.get_occupied(W_FU));
        CHECK_EQUAL(0b000000000000000000000001u, b.get_occupied(OU));
        CHECK_EQUAL(0b000000000000000000000001u, b.get_occupied(B_OU));
        CHECK_EQUAL(0, b.get_occupied(W_OU));
    }
    {
        auto b = Board("3k/4/4/4/4/K3 b");
        b.place_at(SQ_1A, B_FU);
        CHECK_EQUAL(0b100000000000000000000001u, b.get_occupied());
        CHECK_EQUAL(0b100000000000000000000001u, b.get_occupied(BLACK));
        CHECK_EQUAL(0, b.get_occupied(WHITE));
        CHECK_EQUAL(0b000000000000000000000001u, b.get_occupied(FU));
        CHECK_EQUAL(0b000000000000000000000001u, b.get_occupied(B_FU));
        CHECK_EQUAL(0, b.get_occupied(W_FU));
        CHECK_EQUAL(0b100000000000000000000000u, b.get_occupied(OU));
        CHECK_EQUAL(0b100000000000000000000000u, b.get_occupied(B_OU));
        CHECK_EQUAL(0, b.get_occupied(W_OU));
    }
    {
        // VOID VOID VOID VOID
        // W_GI VOID B_RY VOID
        // VOID VOID B_KI B_FU
        // B_OU VOID VOID VOID
        // VOID VOID W_KA B_HI
        // VOID VOID VOID VOID
        auto b = Board("4/s1+R1/2GP/K3/2bR/4 w -");
        CHECK_EQUAL(
            0b000000000000000010010000u, b.get_occupied_by_slider(BLACK));
        CHECK_EQUAL(
            0b000000000000010000000000u, b.get_occupied_by_slider(WHITE));
    }
}

TEST(test_leshogi_board, test_find_pinned)
{
    {
        const auto b = Board("3+r/b3/1P1P/4/1P1K/4");
        CHECK_EQUAL(0b000000000100000000000100u, b.find_pinned(BLACK));
        CHECK_EQUAL(0, b.find_pinned(WHITE));
    }
    {
        const auto b = Board("1s1R/1k2/3G/4/3K/4");
        CHECK_EQUAL(0, b.find_pinned(BLACK));
        CHECK_EQUAL(0, b.find_pinned(WHITE));
    }
}

TEST(test_leshogi_board, test_find_sliding_attacker)
{
    {
        const auto b = Board("4/4/4/4/3R/4");
        const auto actual
            = b.find_sliding_attacker(BLACK, SQ_1A, vshogi::DIR_S);
        CHECK_EQUAL(SQ_1E, actual);
    }
    {
        const auto b = Board("4/4/3P/4/3R/4");
        const auto actual
            = b.find_sliding_attacker(vshogi::BLACK, SQ_1A, vshogi::DIR_S);
        CHECK_EQUAL(SQ_NA, actual);
    }
    {
        const auto b = Board("4/4/3P/4/3R/4");
        const auto actual = b.find_sliding_attacker(
            vshogi::BLACK, SQ_1A, vshogi::DIR_S, SQ_1C);
        CHECK_EQUAL(SQ_1E, actual);
    }
    {
        const auto b = Board("4/4/3P/3B/3R/4");
        const auto actual = b.find_sliding_attacker(
            vshogi::BLACK, SQ_1A, vshogi::DIR_S, SQ_1C);
        CHECK_EQUAL(SQ_NA, actual);
    }
}

TEST(test_leshogi_board, test_compute_movable_to)
{
    {
        const auto b = Board("4/4/4/4/4/4");
        const auto actual = b.compute_movable_to(SQ_1A, BLACK, BT::full());
        CHECK_EQUAL(0, actual);
    }
    {
        const auto b = Board("4/4/4/4/3R/4");
        const auto actual = b.compute_movable_to(SQ_1A, BLACK, BT::full());
        CHECK_EQUAL(0b000000000000000000010000u, actual);
    }
    {
        const auto b = Board("4/3P/4/4/3R/4");
        const auto actual = b.compute_movable_to(SQ_1A, BLACK, BT::full());
        CHECK_EQUAL(0b000000000000000000000010u, actual);
    }
    {
        // Turn: WHITE
        // White: GI
        //     4   3   2   1
        //   +---+---+---+---+
        // A |   |-FU|-OU|+RY|
        //   +---+---+---+---+
        // B |   |   |-KA|   |
        //   +---+---+---+---+
        // C |   |-UM|+KI|   |
        //   +---+---+---+---+
        // D |   |   |   |   |
        //   +---+---+---+---+
        // E |   |   |   |   |
        //   +---+---+---+---+
        // F |   |   |   |   |
        //   +---+---+---+---+
        // Black: GI
        const auto b = Board("1pk+R/2b1/1+bG1/4/4/4");
        const auto actual = b.compute_movable_to(
            SQ_1A, WHITE, BT::invert(BT::from_square(SQ_2A)));
        CHECK_EQUAL(0b000000000000000010000000u, actual);
    }
}

TEST(test_leshogi_board, test_get_occupied_by_slider)
{
    {
        const auto b = Board("3k/4/4/2S1/K2r/4");
        CHECK_EQUAL(0, b.get_occupied_by_slider(vshogi::BLACK));
        CHECK_EQUAL(
            0b000000000000000000010000u,
            b.get_occupied_by_slider(vshogi::WHITE));
    }
}

TEST(test_leshogi_board, test_rotate)
{
    const auto b = Board("3k/4/4/2S1/r2K/4");
    const auto actual = b.rotate();
    CHECK_EQUAL(B_OU, actual[SQ_4F]);
    CHECK_EQUAL(W_GI, actual[SQ_3C]);
    CHECK_EQUAL(VOID, actual[SQ_1E]);
}

TEST(test_leshogi_board, test_compute_king_movable)
{
    {
        // in single check
        const auto b = Board("1k1+R/4/4/4/4/4 w -");
        CHECK_EQUAL(0b000010000010000000000000u, b.compute_king_movable(WHITE));
    }
    {
        // double check
        const auto b = Board("1k2/4/3B/4/4/1R2 w -");
        CHECK_EQUAL(0b000011000000000001000000u, b.compute_king_movable(WHITE));
    }
    {
        // no check
        const auto b = Board("1k2/4/2+R1/4/4/4 w -");
        CHECK_EQUAL(0b000011000000000000000000u, b.compute_king_movable(WHITE));
    }
}

} // namespace test_vshogi::test_leshogi
