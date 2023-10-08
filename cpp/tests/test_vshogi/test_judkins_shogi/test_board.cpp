#include "vshogi/judkins_shogi/board.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_judkins_shogi/test_judkins_shogi.hpp"

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP(board){};

TEST(board, get)
{
    auto b = Board();
    CHECK_EQUAL(W_HI, b[SQ_6A]);
    CHECK_EQUAL(B_GI, b[SQ_4F]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(board, set)
{
    auto b = Board();
    CHECK_EQUAL(VOID, b[SQ_2D]);
    b[SQ_2D] = W_GI;
    CHECK_EQUAL(W_GI, b[SQ_2D]);
}

TEST(board, to_piece_mask)
{
    auto b = Board();
    CHECK_TRUE(
        (bb_6a | bb_5a | bb_4a | bb_3a | bb_2a | bb_1a | bb_1b | bb_6e | bb_6f
         | bb_5f | bb_4f | bb_3f | bb_2f | bb_1f)
        == b.to_piece_mask());
    CHECK_TRUE(
        (bb_6a | bb_5a | bb_4a | bb_3a | bb_2a | bb_1a | bb_1b)
        == b.to_piece_mask(vshogi::WHITE));
    CHECK_TRUE(
        (bb_6f | bb_5f | bb_4f | bb_3f | bb_2f | bb_1f | bb_6e)
        == b.to_piece_mask(vshogi::BLACK));
}

TEST(board, king_location)
{
    auto b = Board();
    CHECK_EQUAL(SQ_6F, b.king_location(vshogi::BLACK));
    CHECK_EQUAL(SQ_1A, b.king_location(vshogi::WHITE));
    b[SQ_6F] = VOID;
    b[SQ_1A] = VOID;
    CHECK_EQUAL(SQ_NA, b.king_location(vshogi::BLACK));
    CHECK_EQUAL(SQ_NA, b.king_location(vshogi::WHITE));
}

TEST(board, in_check)
{
    auto b = Board();
    CHECK_FALSE(b.in_check(vshogi::BLACK));
    CHECK_FALSE(b.in_check(vshogi::WHITE));
    b[SQ_1B] = B_FU;
    b[SQ_6E] = W_FU;
    CHECK_TRUE(b.in_check(vshogi::BLACK));
    CHECK_TRUE(b.in_check(vshogi::WHITE));
}

TEST(board, set_sfen)
{
    {
        const char sfen[] = "+r+b+n+s+p+P/+S+N+B+RGg/6/5k/6/K5 b - 1";
        auto b = Board();
        const auto actual = b.set_sfen(sfen);
        CHECK_EQUAL(W_RY, b[SQ_6A]);
        CHECK_EQUAL(W_UM, b[SQ_5A]);
        CHECK_EQUAL(W_NK, b[SQ_4A]);
        CHECK_EQUAL(W_NG, b[SQ_3A]);
        CHECK_EQUAL(W_TO, b[SQ_2A]);
        CHECK_EQUAL(B_TO, b[SQ_1A]);
        CHECK_EQUAL(B_NG, b[SQ_6B]);
        CHECK_EQUAL(B_NK, b[SQ_5B]);
        CHECK_EQUAL(B_UM, b[SQ_4B]);
        CHECK_EQUAL(B_RY, b[SQ_3B]);
        CHECK_EQUAL(B_KI, b[SQ_2B]);
        CHECK_EQUAL(W_KI, b[SQ_1B]);
        CHECK_EQUAL(VOID, b[SQ_6C]);
        CHECK_EQUAL(VOID, b[SQ_5C]);
        CHECK_EQUAL(VOID, b[SQ_4C]);
        CHECK_EQUAL(VOID, b[SQ_3C]);
        CHECK_EQUAL(VOID, b[SQ_2C]);
        CHECK_EQUAL(VOID, b[SQ_1C]);
        CHECK_EQUAL(VOID, b[SQ_6D]);
        CHECK_EQUAL(VOID, b[SQ_5D]);
        CHECK_EQUAL(VOID, b[SQ_4D]);
        CHECK_EQUAL(VOID, b[SQ_3D]);
        CHECK_EQUAL(VOID, b[SQ_2D]);
        CHECK_EQUAL(W_OU, b[SQ_1D]);
        CHECK_EQUAL(VOID, b[SQ_6E]);
        CHECK_EQUAL(VOID, b[SQ_5E]);
        CHECK_EQUAL(VOID, b[SQ_4E]);
        CHECK_EQUAL(VOID, b[SQ_3E]);
        CHECK_EQUAL(VOID, b[SQ_2E]);
        CHECK_EQUAL(VOID, b[SQ_1E]);
        CHECK_EQUAL(B_OU, b[SQ_6F]);
        CHECK_EQUAL(VOID, b[SQ_5F]);
        CHECK_EQUAL(VOID, b[SQ_4F]);
        CHECK_EQUAL(VOID, b[SQ_3F]);
        CHECK_EQUAL(VOID, b[SQ_2F]);
        CHECK_EQUAL(VOID, b[SQ_1F]);
        CHECK_EQUAL('b', actual[0]);
        CHECK_EQUAL(' ', actual[1]);
    }
    {
        const char sfen[] = "k4K/6/6/6/6/6 w 2R2B2G2S2N2P 1";
        auto b = Board();
        const auto actual = b.set_sfen(sfen);
        CHECK_EQUAL(W_OU, b[SQ_6A]);
        CHECK_EQUAL(VOID, b[SQ_5A]);
        CHECK_EQUAL(VOID, b[SQ_4A]);
        CHECK_EQUAL(VOID, b[SQ_3A]);
        CHECK_EQUAL(VOID, b[SQ_2A]);
        CHECK_EQUAL(B_OU, b[SQ_1A]);
        CHECK_EQUAL(VOID, b[SQ_6B]);
        CHECK_EQUAL(VOID, b[SQ_5B]);
        CHECK_EQUAL(VOID, b[SQ_4B]);
        CHECK_EQUAL(VOID, b[SQ_3B]);
        CHECK_EQUAL(VOID, b[SQ_2B]);
        CHECK_EQUAL(VOID, b[SQ_1B]);
        CHECK_EQUAL(VOID, b[SQ_6C]);
        CHECK_EQUAL(VOID, b[SQ_5C]);
        CHECK_EQUAL(VOID, b[SQ_4C]);
        CHECK_EQUAL(VOID, b[SQ_3C]);
        CHECK_EQUAL(VOID, b[SQ_2C]);
        CHECK_EQUAL(VOID, b[SQ_1C]);
        CHECK_EQUAL(VOID, b[SQ_6D]);
        CHECK_EQUAL(VOID, b[SQ_5D]);
        CHECK_EQUAL(VOID, b[SQ_4D]);
        CHECK_EQUAL(VOID, b[SQ_3D]);
        CHECK_EQUAL(VOID, b[SQ_2D]);
        CHECK_EQUAL(VOID, b[SQ_1D]);
        CHECK_EQUAL(VOID, b[SQ_6E]);
        CHECK_EQUAL(VOID, b[SQ_5E]);
        CHECK_EQUAL(VOID, b[SQ_4E]);
        CHECK_EQUAL(VOID, b[SQ_3E]);
        CHECK_EQUAL(VOID, b[SQ_2E]);
        CHECK_EQUAL(VOID, b[SQ_1E]);
        CHECK_EQUAL(VOID, b[SQ_6F]);
        CHECK_EQUAL(VOID, b[SQ_5F]);
        CHECK_EQUAL(VOID, b[SQ_4F]);
        CHECK_EQUAL(VOID, b[SQ_3F]);
        CHECK_EQUAL(VOID, b[SQ_2F]);
        CHECK_EQUAL(VOID, b[SQ_1F]);
        CHECK_EQUAL('w', actual[0]);
        CHECK_EQUAL(' ', actual[1]);
    }
}

TEST(board, to_sfen)
{
    const char sfen[] = "+r+b+n+s+p+P/+S+N+B+RGg/6/5k/6/K5 b - 1";
    auto b = Board();
    b.set_sfen(sfen);

    const char expected[] = "+r+b+n+s+p+P/+S+N+B+RGg/6/5k/6/K5";
    STRCMP_EQUAL(expected, b.to_sfen().c_str());
}

} // namespace test_vshogi::test_judkins_shogi
