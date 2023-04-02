#include "vshogi/minishogi/board.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(board){};

TEST(board, get)
{
    auto b = Board();
    CHECK_EQUAL(W_HI, b[SQ_51]);
    CHECK_EQUAL(B_GI, b[SQ_35]);
    CHECK_EQUAL(VOID, b[SQ_14]);
}

TEST(board, set)
{
    auto b = Board();
    b[SQ_24] = W_GI;
    CHECK_EQUAL(W_GI, b[SQ_24]);
    CHECK_EQUAL(W_HI, b[SQ_51]);
    CHECK_EQUAL(B_GI, b[SQ_35]);
    CHECK_EQUAL(VOID, b[SQ_14]);
}

TEST(board, to_piece_mask)
{
    auto b = Board();
    CHECK_TRUE(
        (bb_51 | bb_41 | bb_31 | bb_21 | bb_11 | bb_12 | bb_54 | bb_55 | bb_45
         | bb_35 | bb_25 | bb_15)
        == b.to_piece_mask());
    CHECK_TRUE(
        (bb_51 | bb_41 | bb_31 | bb_21 | bb_11 | bb_12)
        == b.to_piece_mask(vshogi::WHITE));
    CHECK_TRUE(
        (bb_54 | bb_55 | bb_45 | bb_35 | bb_25 | bb_15)
        == b.to_piece_mask(vshogi::BLACK));
}

TEST(board, set_sfen)
{
    const char sfen[] = "2+S1k/1r2+P/2K2/5/5 b 2bP2GSR 1";
    auto b = Board();
    const auto actual = b.set_sfen(sfen);
    CHECK_EQUAL(VOID, b[SQ_51]);
    CHECK_EQUAL(VOID, b[SQ_41]);
    CHECK_EQUAL(B_NG, b[SQ_31]);
    CHECK_EQUAL(VOID, b[SQ_21]);
    CHECK_EQUAL(W_OU, b[SQ_11]);
    CHECK_EQUAL(VOID, b[SQ_52]);
    CHECK_EQUAL(W_HI, b[SQ_42]);
    CHECK_EQUAL(VOID, b[SQ_32]);
    CHECK_EQUAL(VOID, b[SQ_22]);
    CHECK_EQUAL(B_TO, b[SQ_12]);
    CHECK_EQUAL(VOID, b[SQ_53]);
    CHECK_EQUAL(VOID, b[SQ_43]);
    CHECK_EQUAL(B_OU, b[SQ_33]);
    CHECK_EQUAL(VOID, b[SQ_23]);
    CHECK_EQUAL(VOID, b[SQ_13]);
    CHECK_EQUAL(VOID, b[SQ_54]);
    CHECK_EQUAL(VOID, b[SQ_44]);
    CHECK_EQUAL(VOID, b[SQ_34]);
    CHECK_EQUAL(VOID, b[SQ_24]);
    CHECK_EQUAL(VOID, b[SQ_14]);
    CHECK_EQUAL(VOID, b[SQ_55]);
    CHECK_EQUAL(VOID, b[SQ_45]);
    CHECK_EQUAL(VOID, b[SQ_35]);
    CHECK_EQUAL(VOID, b[SQ_25]);
    CHECK_EQUAL(VOID, b[SQ_15]);
    CHECK_EQUAL('b', actual[0]);
    CHECK_EQUAL(' ', actual[1]);
}

} // namespace test_vshogi::test_minishogi
