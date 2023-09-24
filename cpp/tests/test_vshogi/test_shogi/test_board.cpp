#include "vshogi/shogi/board.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_shogi/test_shogi.hpp"

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(board){};

TEST(board, get)
{
    const auto b = Board();
    CHECK_EQUAL(VOID, b[SQ_5E]);
    CHECK_EQUAL(W_GI, b[SQ_3A]);
    CHECK_EQUAL(B_KA, b[SQ_8H]);
}

TEST(board, set)
{
    auto b = Board();
    b[SQ_5E] = B_HI;
    b[SQ_3A] = VOID;
    b[SQ_8H] = W_UM;
    CHECK_EQUAL(B_HI, b[SQ_5E]);
    CHECK_EQUAL(VOID, b[SQ_3A]);
    CHECK_EQUAL(W_UM, b[SQ_8H]);
}

TEST(board, to_piece_mask)
{
    auto b = Board();
    b[SQ_5E] = B_RY;
    {
        const auto actual = b.to_piece_mask();
        CHECK_EQUAL(41, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1A));
        CHECK_TRUE(actual.is_one(SQ_9I));
    }
    {
        const auto actual = b.to_piece_mask(vshogi::BLACK);
        CHECK_EQUAL(21, actual.hamming_weight());
        CHECK_FALSE(actual.is_one(SQ_1A));
        CHECK_TRUE(actual.is_one(SQ_9I));
    }
    {
        const auto actual = b.to_piece_mask(vshogi::WHITE);
        CHECK_EQUAL(20, actual.hamming_weight());
        CHECK_TRUE(actual.is_one(SQ_1A));
        CHECK_FALSE(actual.is_one(SQ_9I));
    }
}

TEST(board, to_attack_mask)
{
    auto b = Board();
    {
        const auto actual = b.to_attack_mask(vshogi::BLACK);
        CHECK_FALSE(actual.is_one(SQ_1D));
        CHECK_TRUE(actual.is_one(SQ_1F));
    }
    {
        const auto actual = b.to_attack_mask(vshogi::WHITE);
        CHECK_TRUE(actual.is_one(SQ_1D));
        CHECK_FALSE(actual.is_one(SQ_1F));
    }
}

TEST(board, king_location)
{
    {
        const auto b = Board();
        const auto actual = b.king_location(vshogi::BLACK);
        CHECK_EQUAL(SQ_5I, actual);
    }
    {
        auto b = Board();
        b[SQ_5A] = VOID;
        b[SQ_1B] = W_OU;
        const auto actual = b.king_location(vshogi::WHITE);
        CHECK_EQUAL(SQ_1B, actual);
    }
    {
        auto b = Board();
        b[SQ_5I] = VOID;
        const auto actual = b.king_location(vshogi::BLACK);
        CHECK_EQUAL(SQ_NA, actual);
    }
}

TEST(board, in_check)
{
    {
        const auto b = Board();
        CHECK_FALSE(b.in_check(vshogi::BLACK));
        CHECK_FALSE(b.in_check(vshogi::WHITE));
    }
    {
        auto b = Board();
        b[SQ_5B] = B_FU;
        CHECK_FALSE(b.in_check(vshogi::BLACK));
        CHECK_TRUE(b.in_check(vshogi::WHITE));
    }
    {
        auto b = Board();
        b[SQ_5H] = W_FU;
        CHECK_TRUE(b.in_check(vshogi::BLACK));
        CHECK_FALSE(b.in_check(vshogi::WHITE));
    }
}

TEST(board, set_sfen)
{
    auto b = Board();
    const auto sfen = "8k/9/9/9/9/9/9/4K4/9 b - 1";
    const auto actual = b.set_sfen(sfen);
    CHECK_EQUAL('b', actual[0]);
    CHECK_EQUAL(' ', actual[1]);
    CHECK_EQUAL(VOID, b[SQ_5A]);
    CHECK_EQUAL(W_OU, b[SQ_1A]);
    CHECK_EQUAL(B_OU, b[SQ_5H]);
    CHECK_EQUAL(VOID, b[SQ_5I]);
}

TEST(board, to_sfen)
{
    const auto b = Board();
    const auto actual = b.to_sfen();
    STRCMP_EQUAL(
        "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL",
        actual.c_str());
}

} // namespace test_vshogi::test_shogi
