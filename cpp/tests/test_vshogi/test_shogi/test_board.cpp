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
    CHECK_EQUAL(SQ_5A, b.get_king_location(vshogi::WHITE));
}

TEST(test_shogi_board, set)
{
    auto b = Board();
    b.apply(SQ_5E, B_HI);
    b.apply(SQ_3A, VOID);
    b.apply(SQ_8H, W_UM);
    CHECK_EQUAL(B_HI, b[SQ_5E]);
    CHECK_EQUAL(VOID, b[SQ_3A]);
    CHECK_EQUAL(W_UM, b[SQ_8H]);
    CHECK_EQUAL(SQ_5I, b.get_king_location(vshogi::BLACK));
    b.apply(SQ_4H, SQ_5I);
    CHECK_EQUAL(SQ_4H, b.get_king_location(vshogi::BLACK));
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
    CHECK_EQUAL(SQ_5H, b.get_king_location(vshogi::BLACK));
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

TEST(test_shogi_board, is_square_attacked)
{
    {
        // by FU.
        const auto b = Board("9/9/9/6p2/9/9/9/9/9 b");
        CHECK_TRUE(b.is_square_attacked(vshogi::WHITE, SQ_3E, SQ_3E));
        CHECK_FALSE(b.is_square_attacked(vshogi::BLACK, SQ_3C, SQ_3C));
    }
    {
        // by KE.
        const auto b = Board("4n4/9/9/9/9/9/9/9/9");
        CHECK_TRUE(b.is_square_attacked(vshogi::WHITE, SQ_6C, SQ_6C));
        CHECK_FALSE(b.is_square_attacked(vshogi::WHITE, SQ_5C, SQ_5C));
    }
    {
        // by GI.
        const auto b = Board("9/9/5s3/9/9/9/9/9/9 b");
        CHECK_TRUE(b.is_square_attacked(vshogi::WHITE, SQ_5B, SQ_5B));
        CHECK_FALSE(b.is_square_attacked(vshogi::WHITE, SQ_4B, SQ_4B));
        CHECK_FALSE(b.is_square_attacked(vshogi::WHITE, SQ_5C, SQ_5C));
    }
    {
        // by gold like piece.
        const auto b = Board("9/9/6+l2/9/9/9/9/9/9 b");
        CHECK_TRUE(b.is_square_attacked(vshogi::WHITE, SQ_3B, SQ_3B));
        CHECK_TRUE(b.is_square_attacked(vshogi::WHITE, SQ_2D, SQ_2D));
        CHECK_FALSE(b.is_square_attacked(vshogi::WHITE, SQ_4B, SQ_4B));
    }
    {
        // by king like piece.
        const auto b = Board("9/9/9/9/9/8+b/9/9/9 b");
        CHECK_TRUE(b.is_square_attacked(vshogi::WHITE, SQ_2E, SQ_2E));
        CHECK_FALSE(b.is_square_attacked(vshogi::WHITE, SQ_2C, SQ_2C));
    }
    {
        // by a ranging piece.
        const auto b = Board("9/9/8p/9/9/9/9/9/8L b");
        CHECK_FALSE(b.is_square_attacked(vshogi::BLACK, SQ_1A, SQ_1A));
        CHECK_TRUE(b.is_square_attacked(vshogi::BLACK, SQ_1E, SQ_1E));
    }
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

} // namespace test_vshogi::test_shogi
