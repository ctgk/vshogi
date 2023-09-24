#include "vshogi/judkins_shogi/piece.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_judkins_shogi/test_judkins_shogi.hpp"

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi;
using namespace vshogi::judkins_shogi;

TEST_GROUP(piece){};

TEST(piece, is_promotable)
{
    CHECK_TRUE(is_promotable(FU));
    CHECK_TRUE(is_promotable(KE));
    CHECK_TRUE(is_promotable(GI));
    CHECK_TRUE(is_promotable(KA));
    CHECK_TRUE(is_promotable(HI));
    CHECK_FALSE(is_promotable(KI));
    CHECK_FALSE(is_promotable(OU));
    CHECK_FALSE(is_promotable(TO));
    CHECK_FALSE(is_promotable(NK));
    CHECK_FALSE(is_promotable(NG));
    CHECK_FALSE(is_promotable(UM));
    CHECK_FALSE(is_promotable(RY));
    CHECK_FALSE(is_promotable(NA));

    CHECK_TRUE(is_promotable(B_FU));
    CHECK_TRUE(is_promotable(B_KE));
    CHECK_TRUE(is_promotable(B_GI));
    CHECK_TRUE(is_promotable(B_KA));
    CHECK_TRUE(is_promotable(B_HI));
    CHECK_FALSE(is_promotable(B_KI));
    CHECK_FALSE(is_promotable(B_OU));
    CHECK_FALSE(is_promotable(B_TO));
    CHECK_FALSE(is_promotable(B_NK));
    CHECK_FALSE(is_promotable(B_NG));
    CHECK_FALSE(is_promotable(B_UM));
    CHECK_FALSE(is_promotable(B_RY));

    CHECK_TRUE(is_promotable(W_FU));
    CHECK_TRUE(is_promotable(W_KE));
    CHECK_TRUE(is_promotable(W_GI));
    CHECK_TRUE(is_promotable(W_KA));
    CHECK_TRUE(is_promotable(W_HI));
    CHECK_FALSE(is_promotable(W_KI));
    CHECK_FALSE(is_promotable(W_OU));
    CHECK_FALSE(is_promotable(W_TO));
    CHECK_FALSE(is_promotable(W_NK));
    CHECK_FALSE(is_promotable(W_NG));
    CHECK_FALSE(is_promotable(W_UM));
    CHECK_FALSE(is_promotable(W_RY));

    CHECK_FALSE(is_promotable(VOID));
}

TEST(piece, is_promoted)
{
    CHECK_FALSE(is_promoted(FU));
    CHECK_FALSE(is_promoted(KE));
    CHECK_FALSE(is_promoted(GI));
    CHECK_FALSE(is_promoted(KA));
    CHECK_FALSE(is_promoted(HI));
    CHECK_FALSE(is_promoted(KI));
    CHECK_FALSE(is_promoted(OU));
    CHECK_TRUE(is_promoted(TO));
    CHECK_TRUE(is_promoted(NK));
    CHECK_TRUE(is_promoted(NG));
    CHECK_TRUE(is_promoted(UM));
    CHECK_TRUE(is_promoted(RY));
    CHECK_TRUE(is_promoted(NA));

    CHECK_FALSE(is_promoted(B_FU));
    CHECK_FALSE(is_promoted(B_KE));
    CHECK_FALSE(is_promoted(B_GI));
    CHECK_FALSE(is_promoted(B_KA));
    CHECK_FALSE(is_promoted(B_HI));
    CHECK_FALSE(is_promoted(B_KI));
    CHECK_FALSE(is_promoted(B_OU));
    CHECK_TRUE(is_promoted(B_TO));
    CHECK_TRUE(is_promoted(B_NK));
    CHECK_TRUE(is_promoted(B_NG));
    CHECK_TRUE(is_promoted(B_UM));
    CHECK_TRUE(is_promoted(B_RY));

    CHECK_FALSE(is_promoted(W_FU));
    CHECK_FALSE(is_promoted(W_KE));
    CHECK_FALSE(is_promoted(W_GI));
    CHECK_FALSE(is_promoted(W_KA));
    CHECK_FALSE(is_promoted(W_HI));
    CHECK_FALSE(is_promoted(W_KI));
    CHECK_FALSE(is_promoted(W_OU));
    CHECK_TRUE(is_promoted(W_TO));
    CHECK_TRUE(is_promoted(W_NK));
    CHECK_TRUE(is_promoted(W_NG));
    CHECK_TRUE(is_promoted(W_UM));
    CHECK_TRUE(is_promoted(W_RY));
}

TEST(piece, promote)
{
    CHECK_EQUAL(TO, promote(FU));
    CHECK_EQUAL(NK, promote(KE));
    CHECK_EQUAL(NG, promote(GI));
    CHECK_EQUAL(UM, promote(KA));
    CHECK_EQUAL(RY, promote(HI));
    CHECK_EQUAL(KI, promote(KI));
    CHECK_EQUAL(OU, promote(OU));
    CHECK_EQUAL(TO, promote(TO));
    CHECK_EQUAL(NK, promote(NK));
    CHECK_EQUAL(NG, promote(NG));
    CHECK_EQUAL(UM, promote(UM));
    CHECK_EQUAL(RY, promote(RY));
    CHECK_EQUAL(NA, promote(NA));

    CHECK_EQUAL(B_TO, promote(B_FU));
    CHECK_EQUAL(B_NK, promote(B_KE));
    CHECK_EQUAL(B_NG, promote(B_GI));
    CHECK_EQUAL(B_UM, promote(B_KA));
    CHECK_EQUAL(B_RY, promote(B_HI));
    CHECK_EQUAL(B_KI, promote(B_KI));
    CHECK_EQUAL(B_OU, promote(B_OU));
    CHECK_EQUAL(B_TO, promote(B_TO));
    CHECK_EQUAL(B_NK, promote(B_NK));
    CHECK_EQUAL(B_NG, promote(B_NG));
    CHECK_EQUAL(B_UM, promote(B_UM));
    CHECK_EQUAL(B_RY, promote(B_RY));

    CHECK_EQUAL(W_TO, promote(W_FU));
    CHECK_EQUAL(W_NK, promote(W_KE));
    CHECK_EQUAL(W_NG, promote(W_GI));
    CHECK_EQUAL(W_UM, promote(W_KA));
    CHECK_EQUAL(W_RY, promote(W_HI));
    CHECK_EQUAL(W_KI, promote(W_KI));
    CHECK_EQUAL(W_OU, promote(W_OU));
    CHECK_EQUAL(W_TO, promote(W_TO));
    CHECK_EQUAL(W_NK, promote(W_NK));
    CHECK_EQUAL(W_NG, promote(W_NG));
    CHECK_EQUAL(W_UM, promote(W_UM));
    CHECK_EQUAL(W_RY, promote(W_RY));
}

TEST(piece, demote)
{
    CHECK_EQUAL(FU, demote(FU));
    CHECK_EQUAL(KE, demote(KE));
    CHECK_EQUAL(GI, demote(GI));
    CHECK_EQUAL(KA, demote(KA));
    CHECK_EQUAL(HI, demote(HI));
    CHECK_EQUAL(KI, demote(KI));
    CHECK_EQUAL(OU, demote(OU));
    CHECK_EQUAL(FU, demote(TO));
    CHECK_EQUAL(KE, demote(NK));
    CHECK_EQUAL(GI, demote(NG));
    CHECK_EQUAL(KA, demote(UM));
    CHECK_EQUAL(HI, demote(RY));

    CHECK_EQUAL(B_FU, demote(B_FU));
    CHECK_EQUAL(B_KE, demote(B_KE));
    CHECK_EQUAL(B_GI, demote(B_GI));
    CHECK_EQUAL(B_KA, demote(B_KA));
    CHECK_EQUAL(B_HI, demote(B_HI));
    CHECK_EQUAL(B_KI, demote(B_KI));
    CHECK_EQUAL(B_OU, demote(B_OU));
    CHECK_EQUAL(B_FU, demote(B_TO));
    CHECK_EQUAL(B_KE, demote(B_NK));
    CHECK_EQUAL(B_GI, demote(B_NG));
    CHECK_EQUAL(B_KA, demote(B_UM));
    CHECK_EQUAL(B_HI, demote(B_RY));

    CHECK_EQUAL(W_FU, demote(W_FU));
    CHECK_EQUAL(W_KE, demote(W_KE));
    CHECK_EQUAL(W_GI, demote(W_GI));
    CHECK_EQUAL(W_KA, demote(W_KA));
    CHECK_EQUAL(W_HI, demote(W_HI));
    CHECK_EQUAL(W_KI, demote(W_KI));
    CHECK_EQUAL(W_OU, demote(W_OU));
    CHECK_EQUAL(W_FU, demote(W_TO));
    CHECK_EQUAL(W_KE, demote(W_NK));
    CHECK_EQUAL(W_GI, demote(W_NG));
    CHECK_EQUAL(W_KA, demote(W_UM));
    CHECK_EQUAL(W_HI, demote(W_RY));
}

TEST(piece, get_color)
{
    CHECK_EQUAL(BLACK, get_color(B_FU));
    CHECK_EQUAL(BLACK, get_color(B_KE));
    CHECK_EQUAL(BLACK, get_color(B_GI));
    CHECK_EQUAL(BLACK, get_color(B_KA));
    CHECK_EQUAL(BLACK, get_color(B_HI));
    CHECK_EQUAL(BLACK, get_color(B_KI));
    CHECK_EQUAL(BLACK, get_color(B_OU));
    CHECK_EQUAL(BLACK, get_color(B_TO));
    CHECK_EQUAL(BLACK, get_color(B_NK));
    CHECK_EQUAL(BLACK, get_color(B_NG));
    CHECK_EQUAL(BLACK, get_color(B_UM));
    CHECK_EQUAL(BLACK, get_color(B_RY));

    CHECK_EQUAL(WHITE, get_color(W_FU));
    CHECK_EQUAL(WHITE, get_color(W_KE));
    CHECK_EQUAL(WHITE, get_color(W_GI));
    CHECK_EQUAL(WHITE, get_color(W_KA));
    CHECK_EQUAL(WHITE, get_color(W_HI));
    CHECK_EQUAL(WHITE, get_color(W_KI));
    CHECK_EQUAL(WHITE, get_color(W_OU));
    CHECK_EQUAL(WHITE, get_color(W_TO));
    CHECK_EQUAL(WHITE, get_color(W_NK));
    CHECK_EQUAL(WHITE, get_color(W_NG));
    CHECK_EQUAL(WHITE, get_color(W_UM));
    CHECK_EQUAL(WHITE, get_color(W_RY));
}

TEST(piece, to_piece_type)
{
    CHECK_EQUAL(FU, to_piece_type(B_FU));
    CHECK_EQUAL(KE, to_piece_type(B_KE));
    CHECK_EQUAL(GI, to_piece_type(B_GI));
    CHECK_EQUAL(KA, to_piece_type(B_KA));
    CHECK_EQUAL(HI, to_piece_type(B_HI));
    CHECK_EQUAL(KI, to_piece_type(B_KI));
    CHECK_EQUAL(OU, to_piece_type(B_OU));
    CHECK_EQUAL(TO, to_piece_type(B_TO));
    CHECK_EQUAL(NG, to_piece_type(B_NG));
    CHECK_EQUAL(NG, to_piece_type(B_NG));
    CHECK_EQUAL(UM, to_piece_type(B_UM));
    CHECK_EQUAL(RY, to_piece_type(B_RY));

    CHECK_EQUAL(FU, to_piece_type(W_FU));
    CHECK_EQUAL(KE, to_piece_type(W_KE));
    CHECK_EQUAL(GI, to_piece_type(W_GI));
    CHECK_EQUAL(KA, to_piece_type(W_KA));
    CHECK_EQUAL(HI, to_piece_type(W_HI));
    CHECK_EQUAL(KI, to_piece_type(W_KI));
    CHECK_EQUAL(OU, to_piece_type(W_OU));
    CHECK_EQUAL(TO, to_piece_type(W_TO));
    CHECK_EQUAL(NK, to_piece_type(W_NK));
    CHECK_EQUAL(NG, to_piece_type(W_NG));
    CHECK_EQUAL(UM, to_piece_type(W_UM));
    CHECK_EQUAL(RY, to_piece_type(W_RY));
}

TEST(piece, to_board_piece)
{
    CHECK_EQUAL(B_FU, to_board_piece(BLACK, FU));
    CHECK_EQUAL(B_KE, to_board_piece(BLACK, KE));
    CHECK_EQUAL(B_GI, to_board_piece(BLACK, GI));
    CHECK_EQUAL(B_KA, to_board_piece(BLACK, KA));
    CHECK_EQUAL(B_HI, to_board_piece(BLACK, HI));
    CHECK_EQUAL(B_KI, to_board_piece(BLACK, KI));
    CHECK_EQUAL(B_OU, to_board_piece(BLACK, OU));
    CHECK_EQUAL(B_TO, to_board_piece(BLACK, TO));
    CHECK_EQUAL(B_NK, to_board_piece(BLACK, NK));
    CHECK_EQUAL(B_NG, to_board_piece(BLACK, NG));
    CHECK_EQUAL(B_UM, to_board_piece(BLACK, UM));
    CHECK_EQUAL(B_RY, to_board_piece(BLACK, RY));

    CHECK_EQUAL(W_FU, to_board_piece(WHITE, FU));
    CHECK_EQUAL(W_KE, to_board_piece(WHITE, KE));
    CHECK_EQUAL(W_GI, to_board_piece(WHITE, GI));
    CHECK_EQUAL(W_KA, to_board_piece(WHITE, KA));
    CHECK_EQUAL(W_HI, to_board_piece(WHITE, HI));
    CHECK_EQUAL(W_KI, to_board_piece(WHITE, KI));
    CHECK_EQUAL(W_OU, to_board_piece(WHITE, OU));
    CHECK_EQUAL(W_TO, to_board_piece(WHITE, TO));
    CHECK_EQUAL(W_NK, to_board_piece(WHITE, NK));
    CHECK_EQUAL(W_NG, to_board_piece(WHITE, NG));
    CHECK_EQUAL(W_UM, to_board_piece(WHITE, UM));
    CHECK_EQUAL(W_RY, to_board_piece(WHITE, RY));

    CHECK_EQUAL(VOID, to_board_piece(BLACK, NA));
    CHECK_EQUAL(VOID, to_board_piece(WHITE, NA));
}

TEST(piece, to_sfen)
{
    STRCMP_EQUAL("P", to_sfen(B_FU).c_str());
    STRCMP_EQUAL("N", to_sfen(B_KE).c_str());
    STRCMP_EQUAL("S", to_sfen(B_GI).c_str());
    STRCMP_EQUAL("B", to_sfen(B_KA).c_str());
    STRCMP_EQUAL("R", to_sfen(B_HI).c_str());
    STRCMP_EQUAL("G", to_sfen(B_KI).c_str());
    STRCMP_EQUAL("K", to_sfen(B_OU).c_str());
    STRCMP_EQUAL("+P", to_sfen(B_TO).c_str());
    STRCMP_EQUAL("+N", to_sfen(B_NK).c_str());
    STRCMP_EQUAL("+S", to_sfen(B_NG).c_str());
    STRCMP_EQUAL("+B", to_sfen(B_UM).c_str());
    STRCMP_EQUAL("+R", to_sfen(B_RY).c_str());

    STRCMP_EQUAL("p", to_sfen(W_FU).c_str());
    STRCMP_EQUAL("n", to_sfen(W_KE).c_str());
    STRCMP_EQUAL("s", to_sfen(W_GI).c_str());
    STRCMP_EQUAL("b", to_sfen(W_KA).c_str());
    STRCMP_EQUAL("r", to_sfen(W_HI).c_str());
    STRCMP_EQUAL("g", to_sfen(W_KI).c_str());
    STRCMP_EQUAL("k", to_sfen(W_OU).c_str());
    STRCMP_EQUAL("+p", to_sfen(W_TO).c_str());
    STRCMP_EQUAL("+n", to_sfen(W_NK).c_str());
    STRCMP_EQUAL("+s", to_sfen(W_NG).c_str());
    STRCMP_EQUAL("+b", to_sfen(W_UM).c_str());
    STRCMP_EQUAL("+r", to_sfen(W_RY).c_str());
}

} // namespace test_vshogi::test_judkins_shogi
