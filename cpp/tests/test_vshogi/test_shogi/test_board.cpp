#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (test_shogi_board) {
};

TEST(test_shogi_board, get)
{
    const auto b = Board();
    CHECK_EQUAL(VOID, b[SQ_5E]);
    CHECK_EQUAL(W_GI, b[SQ_3A]);
    CHECK_EQUAL(B_KA, b[SQ_8H]);
    CHECK_EQUAL(SQ_5A, b.get_king_square(vshogi::WHITE));
}

TEST(test_shogi_board, place_at_pop_from)
{
    auto b = Board();
    b.place_at(SQ_5E, B_HI);
    b.place_at(SQ_3A, VOID);
    b.place_at(SQ_8H, W_UM);
    CHECK_EQUAL(B_HI, b[SQ_5E]);
    CHECK_EQUAL(VOID, b[SQ_3A]);
    CHECK_EQUAL(W_UM, b[SQ_8H]);
    CHECK_EQUAL(SQ_5I, b.get_king_square(vshogi::BLACK));
    b.place_at(SQ_4H, b.pop_from(SQ_5I));
    CHECK_EQUAL(SQ_4H, b.get_king_square(vshogi::BLACK));
}

TEST(test_shogi_board, set_sfen)
{
    auto b = Board();
    const auto sfen = "8k/8P/9/9/9/9/9/4K4/9 b - 1";
    const auto actual = b.set_sfen(sfen);
    CHECK_EQUAL('b', actual[0]);
    CHECK_EQUAL(' ', actual[1]);
    CHECK_EQUAL(VOID, b[SQ_5A]);
    CHECK_EQUAL(W_OU, b[SQ_1A]);
    CHECK_EQUAL(B_OU, b[SQ_5H]);
    CHECK_EQUAL(VOID, b[SQ_5I]);
    CHECK_EQUAL(SQ_5H, b.get_king_square(vshogi::BLACK));
}

TEST(test_shogi_board, append_sfen)
{
    const auto b = Board();
    auto actual = std::string();
    b.append_sfen(actual);
    STRCMP_EQUAL(
        "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL",
        actual.c_str());
}

TEST(test_shogi_board, find_pinned)
{
    {
        // not pinned because KA does not attack to the north direction.
        const auto b = Board("8k/8g/9/9/9/9/9/9/8B");
        const auto actual = b.find_pinned(vshogi::WHITE);
        const auto expect = bb_na;
        CHECK_TRUE(expect == actual);
    }
    {
        // pinned because KY does attack to the north direction.
        const auto b = Board("8k/8g/9/9/9/9/9/9/8L");
        const auto actual = b.find_pinned(vshogi::WHITE);
        const auto expect = bb_1b;
        CHECK_TRUE(expect == actual);
    }
    {
        // no pinned pieces because there is no pieces in between
        const auto b = Board("8k/9/9/9/9/9/9/9/8L");
        const auto actual = b.find_pinned(vshogi::WHITE);
        const auto expect = bb_na;
        CHECK_TRUE(expect == actual);
    }
    {
        // no pinned pieces because there are two ally pieces in between
        const auto b = Board("8k/8g/8g/9/9/9/9/9/8L");
        const auto actual = b.find_pinned(vshogi::WHITE);
        const auto expect = bb_na;
        CHECK_TRUE(expect == actual);
    }
    {
        // no pinned pieces because there are one ally and one enemy piece
        const auto b = Board("8k/8g/8G/9/9/9/9/9/8L");
        const auto actual = b.find_pinned(vshogi::WHITE);
        const auto expect = bb_na;
        CHECK_TRUE(expect == actual);
    }
    {
        const auto b = Board("4b3l/9/6P1P/9/4r1P1K/9/9/9/9");
        const auto actual = b.find_pinned(vshogi::BLACK);
        const auto expect = bb_3c | bb_1c | bb_3e;
        CHECK_TRUE(expect.value() == actual.value());
    }
}

TEST(test_shogi_board, find_cover)
{
    {
        const auto b = Board("8k/9/9/9/9/9/9/8G/8L");
        const auto actual = b.find_cover(vshogi::BLACK);
        CHECK_TRUE(bb_1h == actual);
    }
    {
        // no cover because no pieces in between king and the attacker
        const auto b = Board("8k/9/9/9/9/9/9/9/8L");
        const auto actual = b.find_cover(vshogi::BLACK);
        CHECK_TRUE(bb_na == actual);
    }
    {
        // no cover because the blocker is not ally piece
        const auto b = Board("8k/9/9/9/9/9/9/8g/8L");
        const auto actual = b.find_cover(vshogi::BLACK);
        CHECK_TRUE(bb_na == actual);
    }
    {
        // no cover because two ally blockers
        const auto b = Board("8k/9/9/9/9/9/8S/8G/8L");
        const auto actual = b.find_cover(vshogi::BLACK);
        CHECK_TRUE(bb_na == actual);
    }
    {
        // no cover because one ally and one enemy blocker
        const auto b = Board("8k/9/9/9/9/9/8s/8G/8L");
        const auto actual = b.find_cover(vshogi::BLACK);
        CHECK_TRUE(bb_na == actual);
    }
}

TEST(test_shogi_board, find_ranging_attacker)
{
    {
        const auto b = Board("9/9/9/9/9/9/9/9/9");
        const auto actual
            = b.find_ranging_attacker(vshogi::BLACK, SQ_1A, vshogi::DIR_S);
        CHECK_EQUAL(SQ_NA, actual);
    }
    {
        const auto b = Board("9/9/9/9/9/9/9/9/8L");
        const auto actual
            = b.find_ranging_attacker(vshogi::BLACK, SQ_1A, vshogi::DIR_S);
        CHECK_EQUAL(SQ_1I, actual);
    }
    {
        const auto b = Board("9/9/9/8P/9/9/9/9/8L");
        const auto actual
            = b.find_ranging_attacker(vshogi::BLACK, SQ_1A, vshogi::DIR_S);
        CHECK_EQUAL(SQ_NA, actual);
    }
    {
        const auto b = Board("9/9/9/8P/9/9/9/9/8L");
        const auto actual = b.find_ranging_attacker(
            vshogi::BLACK, SQ_1A, vshogi::DIR_S, SQ_1D);
        CHECK_EQUAL(SQ_1I, actual);
    }
    {
        const auto b = Board("9/9/9/8P/9/9/9/8l/8L");
        const auto actual = b.find_ranging_attacker(
            vshogi::BLACK, SQ_1A, vshogi::DIR_S, SQ_1D);
        CHECK_EQUAL(SQ_NA, actual);
    }
}

TEST(test_shogi_board, compute_droppable)
{
    const auto b = Board("9/9/9/9/9/9/P1P1P1P1P/9/9");
    const auto actual = b.compute_droppable<false>(B_FU);
    CHECK_TRUE(
        ((bb_file2 | bb_file4 | bb_file6 | bb_file8) & ~bb_ranka) == actual);
}

TEST(test_shogi_board, get_occupied_by_ranging)
{
    {
        const auto b = Board();
        CHECK_TRUE(
            (bb_1a | bb_9a | bb_2b | bb_8b)
            == b.get_occupied_by_ranging(vshogi::WHITE));
    }
    {
        auto b = Board("9/9/9/9/9/9/9/9/8L");
        CHECK_TRUE(bb_1i == b.get_occupied_by_ranging(vshogi::BLACK));
        b.place_at(SQ_1A, Pieces::promote_nocheck(b.pop_from(SQ_1I)));
        CHECK_TRUE(bb_na == b.get_occupied_by_ranging(vshogi::BLACK));
    }
    {
        auto b = Board("9/9/9/9/9/9/9/9/8L");
        CHECK_TRUE(bb_1i == b.get_occupied_by_ranging(vshogi::BLACK));
        b.place_at(SQ_1D, b.pop_from(SQ_1I));
        CHECK_TRUE(bb_1d == b.get_occupied_by_ranging(vshogi::BLACK));
    }
}

} // namespace test_vshogi::test_shogi
