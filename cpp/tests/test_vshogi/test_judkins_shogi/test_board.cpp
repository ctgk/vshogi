#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP (judkins_shogi_board) {
};

TEST(judkins_shogi_board, get)
{
    auto b = Board();
    CHECK_EQUAL(W_HI, b[SQ_6A]);
    CHECK_EQUAL(B_GI, b[SQ_4F]);
    CHECK_EQUAL(VOID, b[SQ_1D]);
}

TEST(judkins_shogi_board, set)
{
    auto b = Board();
    CHECK_EQUAL(VOID, b[SQ_2D]);
    b.apply(SQ_2D, W_GI);
    CHECK_EQUAL(W_GI, b[SQ_2D]);
}

TEST(judkins_shogi_board, set_sfen)
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

TEST(judkins_shogi_board, append_sfen)
{
    const char sfen[] = "+r+b+n+s+p+P/+S+N+B+RGg/6/5k/6/K5 b - 1";
    auto b = Board();
    b.set_sfen(sfen);

    const char expected[] = "+r+b+n+s+p+P/+S+N+B+RGg/6/5k/6/K5";
    auto actual = std::string();
    b.append_sfen(actual);
    STRCMP_EQUAL(expected, actual.c_str());
}

TEST(judkins_shogi_board, is_square_attacked)
{
    {
        // by FU.
        const auto b = Board("6/6/6/3p2/6/6 b");
        CHECK_TRUE(b.is_square_attacked(SQ_3E, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_3C, vshogi::BLACK));
    }
    {
        // by KE.
        const auto b = Board("1n4/6/6/6/6/6");
        CHECK_TRUE(b.is_square_attacked(SQ_6C, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_5C, vshogi::WHITE));
    }
    {
        // by GI.
        const auto b = Board("6/6/2s3/6/6/6 b");
        CHECK_TRUE(b.is_square_attacked(SQ_5B, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_4B, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_5C, vshogi::WHITE));
    }
    {
        // by gold like piece.
        const auto b = Board("6/6/3+n2/6/6 b");
        CHECK_TRUE(b.is_square_attacked(SQ_3B, vshogi::WHITE));
        CHECK_TRUE(b.is_square_attacked(SQ_2D, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_4B, vshogi::WHITE));
    }
    {
        // by king like piece.
        const auto b = Board("6/6/6/6/6/5+b b");
        CHECK_TRUE(b.is_square_attacked(SQ_2E, vshogi::WHITE));
        CHECK_FALSE(b.is_square_attacked(SQ_2C, vshogi::WHITE));
    }
    {
        // by a ranging piece.
        const auto b = Board("6/6/6/6/1p4/b5 b");
        CHECK_FALSE(b.is_square_attacked(SQ_1A, vshogi::WHITE));
        CHECK_TRUE(b.is_square_attacked(SQ_5E, vshogi::WHITE));
    }
}

} // namespace test_vshogi::test_judkins_shogi
