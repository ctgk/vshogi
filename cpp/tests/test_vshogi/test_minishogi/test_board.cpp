#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(minishogi_board){};

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

} // namespace test_vshogi::test_minishogi
