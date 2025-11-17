#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (shogi_pieces) {
};

TEST(shogi_pieces, is_promotable)
{
    CHECK_TRUE(PieceTraits::is_promotable(FU));
    CHECK_TRUE(PieceTraits::is_promotable(KY));
    CHECK_TRUE(PieceTraits::is_promotable(KE));
    CHECK_TRUE(PieceTraits::is_promotable(GI));
    CHECK_TRUE(PieceTraits::is_promotable(KA));
    CHECK_TRUE(PieceTraits::is_promotable(HI));
    CHECK_FALSE(PieceTraits::is_promotable(KI));
    CHECK_FALSE(PieceTraits::is_promotable(OU));
    CHECK_FALSE(PieceTraits::is_promotable(TO));
    CHECK_FALSE(PieceTraits::is_promotable(NY));
    CHECK_FALSE(PieceTraits::is_promotable(NK));
    CHECK_FALSE(PieceTraits::is_promotable(NG));
    CHECK_FALSE(PieceTraits::is_promotable(UM));
    CHECK_FALSE(PieceTraits::is_promotable(RY));

    CHECK_TRUE(PieceTraits::is_promotable(B_FU));
    CHECK_TRUE(PieceTraits::is_promotable(B_KY));
    CHECK_TRUE(PieceTraits::is_promotable(B_KE));
    CHECK_TRUE(PieceTraits::is_promotable(B_GI));
    CHECK_TRUE(PieceTraits::is_promotable(B_KA));
    CHECK_TRUE(PieceTraits::is_promotable(B_HI));
    CHECK_FALSE(PieceTraits::is_promotable(B_KI));
    CHECK_FALSE(PieceTraits::is_promotable(B_OU));
    CHECK_FALSE(PieceTraits::is_promotable(B_TO));
    CHECK_FALSE(PieceTraits::is_promotable(B_NY));
    CHECK_FALSE(PieceTraits::is_promotable(B_NK));
    CHECK_FALSE(PieceTraits::is_promotable(B_NG));
    CHECK_FALSE(PieceTraits::is_promotable(B_UM));
    CHECK_FALSE(PieceTraits::is_promotable(B_RY));

    CHECK_TRUE(PieceTraits::is_promotable(W_FU));
    CHECK_TRUE(PieceTraits::is_promotable(W_KY));
    CHECK_TRUE(PieceTraits::is_promotable(W_KE));
    CHECK_TRUE(PieceTraits::is_promotable(W_GI));
    CHECK_TRUE(PieceTraits::is_promotable(W_KA));
    CHECK_TRUE(PieceTraits::is_promotable(W_HI));
    CHECK_FALSE(PieceTraits::is_promotable(W_KI));
    CHECK_FALSE(PieceTraits::is_promotable(W_OU));
    CHECK_FALSE(PieceTraits::is_promotable(W_TO));
    CHECK_FALSE(PieceTraits::is_promotable(W_NY));
    CHECK_FALSE(PieceTraits::is_promotable(W_NK));
    CHECK_FALSE(PieceTraits::is_promotable(W_NG));
    CHECK_FALSE(PieceTraits::is_promotable(W_UM));
    CHECK_FALSE(PieceTraits::is_promotable(W_RY));
}

TEST(shogi_pieces, is_promoted)
{
    CHECK_FALSE(PieceTraits::is_promoted(FU));
    CHECK_FALSE(PieceTraits::is_promoted(KY));
    CHECK_FALSE(PieceTraits::is_promoted(KE));
    CHECK_FALSE(PieceTraits::is_promoted(GI));
    CHECK_FALSE(PieceTraits::is_promoted(KA));
    CHECK_FALSE(PieceTraits::is_promoted(HI));
    CHECK_FALSE(PieceTraits::is_promoted(KI));
    CHECK_FALSE(PieceTraits::is_promoted(OU));
    CHECK_TRUE(PieceTraits::is_promoted(TO));
    CHECK_TRUE(PieceTraits::is_promoted(NY));
    CHECK_TRUE(PieceTraits::is_promoted(NK));
    CHECK_TRUE(PieceTraits::is_promoted(NG));
    CHECK_TRUE(PieceTraits::is_promoted(UM));
    CHECK_TRUE(PieceTraits::is_promoted(RY));

    CHECK_FALSE(PieceTraits::is_promoted(B_FU));
    CHECK_FALSE(PieceTraits::is_promoted(B_KY));
    CHECK_FALSE(PieceTraits::is_promoted(B_KE));
    CHECK_FALSE(PieceTraits::is_promoted(B_GI));
    CHECK_FALSE(PieceTraits::is_promoted(B_KA));
    CHECK_FALSE(PieceTraits::is_promoted(B_HI));
    CHECK_FALSE(PieceTraits::is_promoted(B_KI));
    CHECK_FALSE(PieceTraits::is_promoted(B_OU));
    CHECK_TRUE(PieceTraits::is_promoted(B_TO));
    CHECK_TRUE(PieceTraits::is_promoted(B_NY));
    CHECK_TRUE(PieceTraits::is_promoted(B_NK));
    CHECK_TRUE(PieceTraits::is_promoted(B_NG));
    CHECK_TRUE(PieceTraits::is_promoted(B_UM));
    CHECK_TRUE(PieceTraits::is_promoted(B_RY));

    CHECK_FALSE(PieceTraits::is_promoted(W_FU));
    CHECK_FALSE(PieceTraits::is_promoted(W_KY));
    CHECK_FALSE(PieceTraits::is_promoted(W_KE));
    CHECK_FALSE(PieceTraits::is_promoted(W_GI));
    CHECK_FALSE(PieceTraits::is_promoted(W_KA));
    CHECK_FALSE(PieceTraits::is_promoted(W_HI));
    CHECK_FALSE(PieceTraits::is_promoted(W_KI));
    CHECK_FALSE(PieceTraits::is_promoted(W_OU));
    CHECK_TRUE(PieceTraits::is_promoted(W_TO));
    CHECK_TRUE(PieceTraits::is_promoted(W_NY));
    CHECK_TRUE(PieceTraits::is_promoted(W_NK));
    CHECK_TRUE(PieceTraits::is_promoted(W_NG));
    CHECK_TRUE(PieceTraits::is_promoted(W_UM));
    CHECK_TRUE(PieceTraits::is_promoted(W_RY));
}

TEST(shogi_pieces, is_promotion_fully_superior)
{
    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(FU));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(KY));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(KE));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(GI));
    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(KA));
    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(HI));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(KI));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(OU));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(TO));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(NY));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(NK));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(NG));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(UM));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(RY));

    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(B_FU));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_KY));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_KE));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_GI));
    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(B_KA));
    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(B_HI));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_KI));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_OU));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_TO));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_NY));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_NK));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_NG));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_UM));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(B_RY));

    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(W_FU));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_KY));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_KE));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_GI));
    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(W_KA));
    CHECK_TRUE(PieceTraits::is_promotion_fully_superior(W_HI));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_KI));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_OU));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_TO));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_NY));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_NK));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_NG));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_UM));
    CHECK_FALSE(PieceTraits::is_promotion_fully_superior(W_RY));
}

TEST(shogi_pieces, promote_nocheck)
{
    CHECK_EQUAL(TO, PieceTraits::promote_nocheck(FU));
    CHECK_EQUAL(NY, PieceTraits::promote_nocheck(KY));
    CHECK_EQUAL(NK, PieceTraits::promote_nocheck(KE));
    CHECK_EQUAL(NG, PieceTraits::promote_nocheck(GI));
    CHECK_EQUAL(UM, PieceTraits::promote_nocheck(KA));
    CHECK_EQUAL(RY, PieceTraits::promote_nocheck(HI));

    CHECK_EQUAL(B_TO, PieceTraits::promote_nocheck(B_FU));
    CHECK_EQUAL(B_NY, PieceTraits::promote_nocheck(B_KY));
    CHECK_EQUAL(B_NK, PieceTraits::promote_nocheck(B_KE));
    CHECK_EQUAL(B_NG, PieceTraits::promote_nocheck(B_GI));
    CHECK_EQUAL(B_UM, PieceTraits::promote_nocheck(B_KA));
    CHECK_EQUAL(B_RY, PieceTraits::promote_nocheck(B_HI));

    CHECK_EQUAL(W_TO, PieceTraits::promote_nocheck(W_FU));
    CHECK_EQUAL(W_NY, PieceTraits::promote_nocheck(W_KY));
    CHECK_EQUAL(W_NK, PieceTraits::promote_nocheck(W_KE));
    CHECK_EQUAL(W_NG, PieceTraits::promote_nocheck(W_GI));
    CHECK_EQUAL(W_UM, PieceTraits::promote_nocheck(W_KA));
    CHECK_EQUAL(W_RY, PieceTraits::promote_nocheck(W_HI));
}

TEST(shogi_pieces, demote)
{
    CHECK_EQUAL(FU, PieceTraits::demote(FU));
    CHECK_EQUAL(KY, PieceTraits::demote(KY));
    CHECK_EQUAL(KE, PieceTraits::demote(KE));
    CHECK_EQUAL(GI, PieceTraits::demote(GI));
    CHECK_EQUAL(KA, PieceTraits::demote(KA));
    CHECK_EQUAL(HI, PieceTraits::demote(HI));
    CHECK_EQUAL(KI, PieceTraits::demote(KI));
    CHECK_EQUAL(OU, PieceTraits::demote(OU));
    CHECK_EQUAL(FU, PieceTraits::demote(TO));
    CHECK_EQUAL(KY, PieceTraits::demote(NY));
    CHECK_EQUAL(KE, PieceTraits::demote(NK));
    CHECK_EQUAL(GI, PieceTraits::demote(NG));
    CHECK_EQUAL(KA, PieceTraits::demote(UM));
    CHECK_EQUAL(HI, PieceTraits::demote(RY));

    CHECK_EQUAL(B_FU, PieceTraits::demote(B_FU));
    CHECK_EQUAL(B_KY, PieceTraits::demote(B_KY));
    CHECK_EQUAL(B_KE, PieceTraits::demote(B_KE));
    CHECK_EQUAL(B_GI, PieceTraits::demote(B_GI));
    CHECK_EQUAL(B_KA, PieceTraits::demote(B_KA));
    CHECK_EQUAL(B_HI, PieceTraits::demote(B_HI));
    CHECK_EQUAL(B_KI, PieceTraits::demote(B_KI));
    CHECK_EQUAL(B_OU, PieceTraits::demote(B_OU));
    CHECK_EQUAL(B_FU, PieceTraits::demote(B_TO));
    CHECK_EQUAL(B_KY, PieceTraits::demote(B_NY));
    CHECK_EQUAL(B_KE, PieceTraits::demote(B_NK));
    CHECK_EQUAL(B_GI, PieceTraits::demote(B_NG));
    CHECK_EQUAL(B_KA, PieceTraits::demote(B_UM));
    CHECK_EQUAL(B_HI, PieceTraits::demote(B_RY));

    CHECK_EQUAL(W_FU, PieceTraits::demote(W_FU));
    CHECK_EQUAL(W_KY, PieceTraits::demote(W_KY));
    CHECK_EQUAL(W_KE, PieceTraits::demote(W_KE));
    CHECK_EQUAL(W_GI, PieceTraits::demote(W_GI));
    CHECK_EQUAL(W_KA, PieceTraits::demote(W_KA));
    CHECK_EQUAL(W_HI, PieceTraits::demote(W_HI));
    CHECK_EQUAL(W_KI, PieceTraits::demote(W_KI));
    CHECK_EQUAL(W_OU, PieceTraits::demote(W_OU));
    CHECK_EQUAL(W_FU, PieceTraits::demote(W_TO));
    CHECK_EQUAL(W_KY, PieceTraits::demote(W_NY));
    CHECK_EQUAL(W_KE, PieceTraits::demote(W_NK));
    CHECK_EQUAL(W_GI, PieceTraits::demote(W_NG));
    CHECK_EQUAL(W_KA, PieceTraits::demote(W_UM));
    CHECK_EQUAL(W_HI, PieceTraits::demote(W_RY));
}

TEST(shogi_pieces, get_color)
{
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_FU));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_KY));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_KE));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_GI));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_KA));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_HI));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_KI));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_OU));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_TO));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_NY));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_NK));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_NG));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_UM));
    CHECK_EQUAL(vshogi::BLACK, PieceTraits::get_color(B_RY));

    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_FU));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_KY));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_KE));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_GI));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_KA));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_HI));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_KI));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_OU));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_TO));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_NY));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_NK));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_NG));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_UM));
    CHECK_EQUAL(vshogi::WHITE, PieceTraits::get_color(W_RY));
}

TEST(shogi_pieces, to_piece_type)
{
    CHECK_EQUAL(FU, PieceTraits::to_piece_type(B_FU));
    CHECK_EQUAL(KY, PieceTraits::to_piece_type(B_KY));
    CHECK_EQUAL(KE, PieceTraits::to_piece_type(B_KE));
    CHECK_EQUAL(GI, PieceTraits::to_piece_type(B_GI));
    CHECK_EQUAL(KA, PieceTraits::to_piece_type(B_KA));
    CHECK_EQUAL(HI, PieceTraits::to_piece_type(B_HI));
    CHECK_EQUAL(KI, PieceTraits::to_piece_type(B_KI));
    CHECK_EQUAL(OU, PieceTraits::to_piece_type(B_OU));
    CHECK_EQUAL(TO, PieceTraits::to_piece_type(B_TO));
    CHECK_EQUAL(NY, PieceTraits::to_piece_type(B_NY));
    CHECK_EQUAL(NK, PieceTraits::to_piece_type(B_NK));
    CHECK_EQUAL(NG, PieceTraits::to_piece_type(B_NG));
    CHECK_EQUAL(UM, PieceTraits::to_piece_type(B_UM));
    CHECK_EQUAL(RY, PieceTraits::to_piece_type(B_RY));

    CHECK_EQUAL(FU, PieceTraits::to_piece_type(W_FU));
    CHECK_EQUAL(KY, PieceTraits::to_piece_type(W_KY));
    CHECK_EQUAL(KE, PieceTraits::to_piece_type(W_KE));
    CHECK_EQUAL(GI, PieceTraits::to_piece_type(W_GI));
    CHECK_EQUAL(KA, PieceTraits::to_piece_type(W_KA));
    CHECK_EQUAL(HI, PieceTraits::to_piece_type(W_HI));
    CHECK_EQUAL(KI, PieceTraits::to_piece_type(W_KI));
    CHECK_EQUAL(OU, PieceTraits::to_piece_type(W_OU));
    CHECK_EQUAL(TO, PieceTraits::to_piece_type(W_TO));
    CHECK_EQUAL(NY, PieceTraits::to_piece_type(W_NY));
    CHECK_EQUAL(NK, PieceTraits::to_piece_type(W_NK));
    CHECK_EQUAL(NG, PieceTraits::to_piece_type(W_NG));
    CHECK_EQUAL(UM, PieceTraits::to_piece_type(W_UM));
    CHECK_EQUAL(RY, PieceTraits::to_piece_type(W_RY));

    CHECK_EQUAL(NA, PieceTraits::to_piece_type(VOID));

    CHECK_EQUAL(FU, PieceTraits::to_piece_type('p'));
    CHECK_EQUAL(KY, PieceTraits::to_piece_type('l'));
    CHECK_EQUAL(KE, PieceTraits::to_piece_type('n'));
    CHECK_EQUAL(GI, PieceTraits::to_piece_type('s'));
    CHECK_EQUAL(KA, PieceTraits::to_piece_type('b'));
    CHECK_EQUAL(HI, PieceTraits::to_piece_type('r'));
    CHECK_EQUAL(KI, PieceTraits::to_piece_type('g'));
    CHECK_EQUAL(OU, PieceTraits::to_piece_type('k'));
    CHECK_EQUAL(NA, PieceTraits::to_piece_type('a'));

    CHECK_EQUAL(FU, PieceTraits::to_piece_type('P'));
    CHECK_EQUAL(KY, PieceTraits::to_piece_type('L'));
    CHECK_EQUAL(KE, PieceTraits::to_piece_type('N'));
    CHECK_EQUAL(GI, PieceTraits::to_piece_type('S'));
    CHECK_EQUAL(KA, PieceTraits::to_piece_type('B'));
    CHECK_EQUAL(HI, PieceTraits::to_piece_type('R'));
    CHECK_EQUAL(KI, PieceTraits::to_piece_type('G'));
    CHECK_EQUAL(OU, PieceTraits::to_piece_type('K'));
    CHECK_EQUAL(NA, PieceTraits::to_piece_type('A'));
}

TEST(shogi_pieces, make_piece)
{
    CHECK_EQUAL(B_FU, PieceTraits::make_piece(vshogi::BLACK, FU));
    CHECK_EQUAL(B_KY, PieceTraits::make_piece(vshogi::BLACK, KY));
    CHECK_EQUAL(B_KE, PieceTraits::make_piece(vshogi::BLACK, KE));
    CHECK_EQUAL(B_GI, PieceTraits::make_piece(vshogi::BLACK, GI));
    CHECK_EQUAL(B_KA, PieceTraits::make_piece(vshogi::BLACK, KA));
    CHECK_EQUAL(B_HI, PieceTraits::make_piece(vshogi::BLACK, HI));
    CHECK_EQUAL(B_KI, PieceTraits::make_piece(vshogi::BLACK, KI));
    CHECK_EQUAL(B_OU, PieceTraits::make_piece(vshogi::BLACK, OU));
    CHECK_EQUAL(B_TO, PieceTraits::make_piece(vshogi::BLACK, TO));
    CHECK_EQUAL(B_NY, PieceTraits::make_piece(vshogi::BLACK, NY));
    CHECK_EQUAL(B_NK, PieceTraits::make_piece(vshogi::BLACK, NK));
    CHECK_EQUAL(B_NG, PieceTraits::make_piece(vshogi::BLACK, NG));
    CHECK_EQUAL(B_UM, PieceTraits::make_piece(vshogi::BLACK, UM));
    CHECK_EQUAL(B_RY, PieceTraits::make_piece(vshogi::BLACK, RY));

    CHECK_EQUAL(W_FU, PieceTraits::make_piece(vshogi::WHITE, FU));
    CHECK_EQUAL(W_KY, PieceTraits::make_piece(vshogi::WHITE, KY));
    CHECK_EQUAL(W_KE, PieceTraits::make_piece(vshogi::WHITE, KE));
    CHECK_EQUAL(W_GI, PieceTraits::make_piece(vshogi::WHITE, GI));
    CHECK_EQUAL(W_KA, PieceTraits::make_piece(vshogi::WHITE, KA));
    CHECK_EQUAL(W_HI, PieceTraits::make_piece(vshogi::WHITE, HI));
    CHECK_EQUAL(W_KI, PieceTraits::make_piece(vshogi::WHITE, KI));
    CHECK_EQUAL(W_OU, PieceTraits::make_piece(vshogi::WHITE, OU));
    CHECK_EQUAL(W_TO, PieceTraits::make_piece(vshogi::WHITE, TO));
    CHECK_EQUAL(W_NY, PieceTraits::make_piece(vshogi::WHITE, NY));
    CHECK_EQUAL(W_NK, PieceTraits::make_piece(vshogi::WHITE, NK));
    CHECK_EQUAL(W_NG, PieceTraits::make_piece(vshogi::WHITE, NG));
    CHECK_EQUAL(W_UM, PieceTraits::make_piece(vshogi::WHITE, UM));
    CHECK_EQUAL(W_RY, PieceTraits::make_piece(vshogi::WHITE, RY));

    CHECK_EQUAL(VOID, PieceTraits::make_piece(vshogi::BLACK, NA));
    CHECK_EQUAL(VOID, PieceTraits::make_piece(vshogi::WHITE, NA));
}

TEST(shogi_pieces, to_char)
{
    CHECK_EQUAL('p', PieceTraits::to_char(FU));
    CHECK_EQUAL('l', PieceTraits::to_char(KY));
    CHECK_EQUAL('n', PieceTraits::to_char(KE));
    CHECK_EQUAL('s', PieceTraits::to_char(GI));
    CHECK_EQUAL('b', PieceTraits::to_char(KA));
    CHECK_EQUAL('r', PieceTraits::to_char(HI));
    CHECK_EQUAL('g', PieceTraits::to_char(KI));
    CHECK_EQUAL('k', PieceTraits::to_char(OU));
}

TEST(shogi_pieces, is_slider)
{
    CHECK_FALSE(PieceTraits::is_slider(FU));
    CHECK_TRUE(PieceTraits::is_slider(KY));
    CHECK_FALSE(PieceTraits::is_slider(KE));
    CHECK_FALSE(PieceTraits::is_slider(GI));
    CHECK_TRUE(PieceTraits::is_slider(KA));
    CHECK_TRUE(PieceTraits::is_slider(HI));
    CHECK_FALSE(PieceTraits::is_slider(KI));
    CHECK_FALSE(PieceTraits::is_slider(OU));
    CHECK_FALSE(PieceTraits::is_slider(TO));
    CHECK_FALSE(PieceTraits::is_slider(NY));
    CHECK_FALSE(PieceTraits::is_slider(NK));
    CHECK_FALSE(PieceTraits::is_slider(NG));
    CHECK_TRUE(PieceTraits::is_slider(UM));
    CHECK_TRUE(PieceTraits::is_slider(RY));
    CHECK_FALSE(PieceTraits::is_slider(NA));

    CHECK_FALSE(PieceTraits::is_slider(B_FU));
    CHECK_TRUE(PieceTraits::is_slider(B_KY));
    CHECK_FALSE(PieceTraits::is_slider(B_KE));
    CHECK_FALSE(PieceTraits::is_slider(B_GI));
    CHECK_TRUE(PieceTraits::is_slider(B_KA));
    CHECK_TRUE(PieceTraits::is_slider(B_HI));
    CHECK_FALSE(PieceTraits::is_slider(B_KI));
    CHECK_FALSE(PieceTraits::is_slider(B_OU));
    CHECK_FALSE(PieceTraits::is_slider(B_TO));
    CHECK_FALSE(PieceTraits::is_slider(B_NY));
    CHECK_FALSE(PieceTraits::is_slider(B_NK));
    CHECK_FALSE(PieceTraits::is_slider(B_NG));
    CHECK_TRUE(PieceTraits::is_slider(B_UM));
    CHECK_TRUE(PieceTraits::is_slider(B_RY));

    CHECK_FALSE(PieceTraits::is_slider(W_FU));
    CHECK_TRUE(PieceTraits::is_slider(W_KY));
    CHECK_FALSE(PieceTraits::is_slider(W_KE));
    CHECK_FALSE(PieceTraits::is_slider(W_GI));
    CHECK_TRUE(PieceTraits::is_slider(W_KA));
    CHECK_TRUE(PieceTraits::is_slider(W_HI));
    CHECK_FALSE(PieceTraits::is_slider(W_KI));
    CHECK_FALSE(PieceTraits::is_slider(W_OU));
    CHECK_FALSE(PieceTraits::is_slider(W_TO));
    CHECK_FALSE(PieceTraits::is_slider(W_NY));
    CHECK_FALSE(PieceTraits::is_slider(W_NK));
    CHECK_FALSE(PieceTraits::is_slider(W_NG));
    CHECK_TRUE(PieceTraits::is_slider(W_UM));
    CHECK_TRUE(PieceTraits::is_slider(W_RY));

    CHECK_FALSE(PieceTraits::is_slider(VOID));
}

TEST(shogi_pieces, slidable_to)
{
    CHECK_FALSE(PieceTraits::slidable_to(B_FU, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(B_KY, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(B_KY, vshogi::DIR_E));
    CHECK_FALSE(PieceTraits::slidable_to(B_KE, vshogi::DIR_NNW));
    CHECK_FALSE(PieceTraits::slidable_to(B_GI, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(B_KA, vshogi::DIR_NW));
    CHECK_FALSE(PieceTraits::slidable_to(B_KA, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(B_HI, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(B_HI, vshogi::DIR_NE));
    CHECK_FALSE(PieceTraits::slidable_to(B_KI, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(B_OU, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(B_TO, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(B_NY, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(B_NK, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(B_NG, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(B_UM, vshogi::DIR_NW));
    CHECK_FALSE(PieceTraits::slidable_to(B_UM, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(B_RY, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(B_RY, vshogi::DIR_NE));

    CHECK_FALSE(PieceTraits::slidable_to(W_FU, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(W_KY, vshogi::DIR_S));
    CHECK_FALSE(PieceTraits::slidable_to(W_KY, vshogi::DIR_W));
    CHECK_FALSE(PieceTraits::slidable_to(W_KE, vshogi::DIR_SSE));
    CHECK_FALSE(PieceTraits::slidable_to(W_GI, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(W_KA, vshogi::DIR_SW));
    CHECK_FALSE(PieceTraits::slidable_to(W_KA, vshogi::DIR_S));
    CHECK_TRUE(PieceTraits::slidable_to(W_HI, vshogi::DIR_S));
    CHECK_FALSE(PieceTraits::slidable_to(W_HI, vshogi::DIR_SE));
    CHECK_FALSE(PieceTraits::slidable_to(W_KI, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(W_OU, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(W_TO, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(W_NY, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(W_NK, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(W_NG, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(W_UM, vshogi::DIR_NW));
    CHECK_FALSE(PieceTraits::slidable_to(W_UM, vshogi::DIR_N));
    CHECK_TRUE(PieceTraits::slidable_to(W_RY, vshogi::DIR_N));
    CHECK_FALSE(PieceTraits::slidable_to(W_RY, vshogi::DIR_NE));

    CHECK_FALSE(PieceTraits::slidable_to(VOID, vshogi::DIR_N));
}

TEST(shogi_pieces, append_sfen)
{
    // clang-format off
    {auto actual = std::string(); PieceTraits::append_sfen(B_FU, actual); STRCMP_EQUAL("P", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_KY, actual); STRCMP_EQUAL("L", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_KE, actual); STRCMP_EQUAL("N", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_GI, actual); STRCMP_EQUAL("S", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_KA, actual); STRCMP_EQUAL("B", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_HI, actual); STRCMP_EQUAL("R", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_KI, actual); STRCMP_EQUAL("G", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_OU, actual); STRCMP_EQUAL("K", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_TO, actual); STRCMP_EQUAL("+P", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_NY, actual); STRCMP_EQUAL("+L", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_NK, actual); STRCMP_EQUAL("+N", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_NG, actual); STRCMP_EQUAL("+S", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_UM, actual); STRCMP_EQUAL("+B", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(B_RY, actual); STRCMP_EQUAL("+R", actual.c_str());}

    {auto actual = std::string(); PieceTraits::append_sfen(W_FU, actual); STRCMP_EQUAL("p", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_KY, actual); STRCMP_EQUAL("l", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_KE, actual); STRCMP_EQUAL("n", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_GI, actual); STRCMP_EQUAL("s", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_KA, actual); STRCMP_EQUAL("b", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_HI, actual); STRCMP_EQUAL("r", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_KI, actual); STRCMP_EQUAL("g", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_OU, actual); STRCMP_EQUAL("k", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_TO, actual); STRCMP_EQUAL("+p", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_NY, actual); STRCMP_EQUAL("+l", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_NK, actual); STRCMP_EQUAL("+n", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_NG, actual); STRCMP_EQUAL("+s", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_UM, actual); STRCMP_EQUAL("+b", actual.c_str());}
    {auto actual = std::string(); PieceTraits::append_sfen(W_RY, actual); STRCMP_EQUAL("+r", actual.c_str());}
    // clang-format on
}

TEST(shogi_pieces, get_point)
{
    CHECK_EQUAL(1, PieceTraits::get_point(FU));
    CHECK_EQUAL(1, PieceTraits::get_point(KY));
    CHECK_EQUAL(1, PieceTraits::get_point(KE));
    CHECK_EQUAL(1, PieceTraits::get_point(GI));
    CHECK_EQUAL(5, PieceTraits::get_point(KA));
    CHECK_EQUAL(5, PieceTraits::get_point(HI));
    CHECK_EQUAL(1, PieceTraits::get_point(KI));
    CHECK_EQUAL(0, PieceTraits::get_point(OU));
    CHECK_EQUAL(1, PieceTraits::get_point(TO));
    CHECK_EQUAL(1, PieceTraits::get_point(NY));
    CHECK_EQUAL(1, PieceTraits::get_point(NK));
    CHECK_EQUAL(1, PieceTraits::get_point(NG));
    CHECK_EQUAL(5, PieceTraits::get_point(UM));
    CHECK_EQUAL(5, PieceTraits::get_point(RY));
    CHECK_EQUAL(0, PieceTraits::get_point(NA));

    CHECK_EQUAL(1, PieceTraits::get_point(B_FU));
    CHECK_EQUAL(1, PieceTraits::get_point(B_KY));
    CHECK_EQUAL(1, PieceTraits::get_point(B_KE));
    CHECK_EQUAL(1, PieceTraits::get_point(B_GI));
    CHECK_EQUAL(5, PieceTraits::get_point(B_KA));
    CHECK_EQUAL(5, PieceTraits::get_point(B_HI));
    CHECK_EQUAL(1, PieceTraits::get_point(B_KI));
    CHECK_EQUAL(0, PieceTraits::get_point(B_OU));
    CHECK_EQUAL(1, PieceTraits::get_point(B_TO));
    CHECK_EQUAL(1, PieceTraits::get_point(B_NY));
    CHECK_EQUAL(1, PieceTraits::get_point(B_NK));
    CHECK_EQUAL(1, PieceTraits::get_point(B_NG));
    CHECK_EQUAL(5, PieceTraits::get_point(B_UM));
    CHECK_EQUAL(5, PieceTraits::get_point(B_RY));

    CHECK_EQUAL(1, PieceTraits::get_point(W_FU));
    CHECK_EQUAL(1, PieceTraits::get_point(W_KY));
    CHECK_EQUAL(1, PieceTraits::get_point(W_KE));
    CHECK_EQUAL(1, PieceTraits::get_point(W_GI));
    CHECK_EQUAL(5, PieceTraits::get_point(W_KA));
    CHECK_EQUAL(5, PieceTraits::get_point(W_HI));
    CHECK_EQUAL(1, PieceTraits::get_point(W_KI));
    CHECK_EQUAL(0, PieceTraits::get_point(W_OU));
    CHECK_EQUAL(1, PieceTraits::get_point(W_TO));
    CHECK_EQUAL(1, PieceTraits::get_point(W_NY));
    CHECK_EQUAL(1, PieceTraits::get_point(W_NK));
    CHECK_EQUAL(1, PieceTraits::get_point(W_NG));
    CHECK_EQUAL(5, PieceTraits::get_point(W_UM));
    CHECK_EQUAL(5, PieceTraits::get_point(W_RY));

    CHECK_EQUAL(0, PieceTraits::get_point(VOID));
}

TEST(shogi_pieces, get_attack_directions)
{
    using PieceTraits = vshogi::shogi::PieceTraits;
    using namespace vshogi;
    constexpr DirectionEnum expected[][9] = {
        // clang-format off
        {DIR_N, DIR_NA}, // B_FU
        {DIR_N, DIR_NA}, // B_KY
        {DIR_NNW, DIR_NNE, DIR_NA}, // B_KE
        {DIR_NW, DIR_N, DIR_NE, DIR_SW, DIR_SE, DIR_NA}, // B_GI
        {DIR_NW, DIR_NE, DIR_SW, DIR_SE, DIR_NA}, // B_KA
        {DIR_N, DIR_W, DIR_E, DIR_S, DIR_NA}, // B_HI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S, DIR_NA}, // B_KI
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_OU
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S, DIR_NA}, // B_TO
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S, DIR_NA}, // B_NY
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S, DIR_NA}, // B_NK
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_S, DIR_NA}, // B_NG
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_UM
        {DIR_NW, DIR_N, DIR_NE, DIR_W, DIR_E, DIR_SW, DIR_S, DIR_SE, DIR_NA}, // B_RY
        // clang-format on
    };
    for (auto pt : Config::stand_piece_type_iterator()) {
        for (auto&& color : {BLACK, WHITE}) {
            const PieceEnum p = PieceTraits::make_piece(color, pt);
            const auto actual = PieceTraits::get_attack_directions(p);
            for (int jj = 0; jj < 9; ++jj) {
                if (color == BLACK) {
                    CHECK_EQUAL(expected[pt][jj], actual[jj]);
                } else {
                    CHECK_EQUAL(rotate(expected[pt][jj]), actual[jj]);
                }
                if (expected[pt][jj] == DIR_NA)
                    break;
            }
        }
    }
}

} // namespace test_vshogi::test_shogi
