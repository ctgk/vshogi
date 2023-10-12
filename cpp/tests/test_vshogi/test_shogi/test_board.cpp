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

TEST(board, set_sfen)
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
