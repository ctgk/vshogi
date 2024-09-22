#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (shogi_board) {
};

TEST(shogi_board, get)
{
    const auto b = Board();
    CHECK_EQUAL(VOID, b[SQ_5E]);
    CHECK_EQUAL(W_GI, b[SQ_3A]);
    CHECK_EQUAL(B_KA, b[SQ_8H]);
    CHECK_EQUAL(SQ_5A, b.get_king_location(vshogi::WHITE));
}

TEST(shogi_board, set)
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

TEST(shogi_board, set_sfen)
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

TEST(shogi_board, append_sfen)
{
    const auto b = Board();
    auto actual = std::string();
    b.append_sfen(actual);
    STRCMP_EQUAL(
        "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL",
        actual.c_str());
}

TEST(shogi_board, is_square_attacked)
{
    {
        // by FU.
        const auto b = Board("9/9/9/6p2/9/9/9/9/9 b");
        CHECK_TRUE(b.is_square_attacked(SQ_3E, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_3C, vshogi::BLACK));
    }
    {
        // by KE.
        const auto b = Board("4n4/9/9/9/9/9/9/9/9");
        CHECK_TRUE(b.is_square_attacked(SQ_6C, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_5C, vshogi::WHITE));
    }
    {
        // by GI.
        const auto b = Board("9/9/5s3/9/9/9/9/9/9 b");
        CHECK_TRUE(b.is_square_attacked(SQ_5B, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_4B, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_5C, vshogi::WHITE));
    }
    {
        // by gold like piece.
        const auto b = Board("9/9/6+l2/9/9/9/9/9 b");
        CHECK_TRUE(b.is_square_attacked(SQ_3B, vshogi::WHITE));
        CHECK_TRUE(b.is_square_attacked(SQ_2D, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_4B, vshogi::WHITE));
    }
    {
        // by king like piece.
        const auto b = Board("9/9/9/9/9/8+b/9/9/9 b");
        CHECK_TRUE(b.is_square_attacked(SQ_2E, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_2C, vshogi::WHITE));
    }
    {
        // by a ranging piece.
        const auto b = Board("9/9/8p/9/9/9/9/9/8L b");
        CHECK_FALSE(b.is_square_attacked(SQ_1A, vshogi::BLACK));
        CHECK_TRUE(b.is_square_attacked(SQ_1E, vshogi::BLACK));
    }
}

} // namespace test_vshogi::test_shogi
