#include "vshogi/minishogi/board.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_minishogi/test_minishogi.hpp"

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(board){};

TEST(board, get)
{
    auto b = Board();
    CHECK_EQUAL(W_HI, b[SQ_5A]);
    CHECK_EQUAL(B_GI, b[SQ_3E]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(board, set)
{
    auto b = Board();
    b[SQ_2D] = W_GI;
    CHECK_EQUAL(W_GI, b[SQ_2D]);
    CHECK_EQUAL(W_HI, b[SQ_5A]);
    CHECK_EQUAL(B_GI, b[SQ_3E]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(board, to_piece_mask)
{
    auto b = Board();
    CHECK_TRUE(
        (bb_5a | bb_4a | bb_3a | bb_2a | bb_1a | bb_1b | bb_5d | bb_5e | bb_4e
         | bb_3e | bb_2e | bb_1e)
        == b.to_piece_mask());
    CHECK_TRUE(
        (bb_5a | bb_4a | bb_3a | bb_2a | bb_1a | bb_1b)
        == b.to_piece_mask(vshogi::WHITE));
    CHECK_TRUE(
        (bb_5d | bb_5e | bb_4e | bb_3e | bb_2e | bb_1e)
        == b.to_piece_mask(vshogi::BLACK));
}

TEST(board, to_attack_mask)
{
    auto b = Board();
    CHECK_TRUE(
        (bb_5b | bb_1b | bb_5c | bb_4c | bb_1c | bb_5d | bb_4d | bb_3d | bb_2d
         | bb_1d | bb_5e | bb_4e | bb_3e | bb_2e)
        == b.to_attack_mask(vshogi::BLACK));
}

TEST(board, king_location)
{
    auto b = Board();
    CHECK_EQUAL(SQ_5E, b.king_location(vshogi::BLACK));
    CHECK_EQUAL(SQ_1A, b.king_location(vshogi::WHITE));
    b[SQ_5E] = VOID;
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
    b[SQ_5D] = W_FU;
    CHECK_TRUE(b.in_check(vshogi::BLACK));
    CHECK_TRUE(b.in_check(vshogi::WHITE));
}

TEST(board, set_sfen)
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

TEST(board, to_sfen)
{
    const char sfen[] = "2+S1k/1r2+P/2K2/5/5 b 2bP2GSR 1";
    auto b = Board();
    b.set_sfen(sfen);

    const char expected[] = "2+S1k/1r2+P/2K2/5/5";
    STRCMP_EQUAL(expected, b.to_sfen().c_str());
}

} // namespace test_vshogi::test_minishogi
