#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(shogi_pieces){};

TEST(shogi_pieces, is_promotable)
{
    CHECK_TRUE(Pieces::is_promotable(FU));
    CHECK_TRUE(Pieces::is_promotable(KY));
    CHECK_TRUE(Pieces::is_promotable(KE));
    CHECK_TRUE(Pieces::is_promotable(GI));
    CHECK_TRUE(Pieces::is_promotable(KA));
    CHECK_TRUE(Pieces::is_promotable(HI));
    CHECK_FALSE(Pieces::is_promotable(KI));
    CHECK_FALSE(Pieces::is_promotable(OU));
    CHECK_FALSE(Pieces::is_promotable(TO));
    CHECK_FALSE(Pieces::is_promotable(NY));
    CHECK_FALSE(Pieces::is_promotable(NK));
    CHECK_FALSE(Pieces::is_promotable(NG));
    CHECK_FALSE(Pieces::is_promotable(UM));
    CHECK_FALSE(Pieces::is_promotable(RY));

    CHECK_TRUE(Pieces::is_promotable(B_FU));
    CHECK_TRUE(Pieces::is_promotable(B_KY));
    CHECK_TRUE(Pieces::is_promotable(B_KE));
    CHECK_TRUE(Pieces::is_promotable(B_GI));
    CHECK_TRUE(Pieces::is_promotable(B_KA));
    CHECK_TRUE(Pieces::is_promotable(B_HI));
    CHECK_FALSE(Pieces::is_promotable(B_KI));
    CHECK_FALSE(Pieces::is_promotable(B_OU));
    CHECK_FALSE(Pieces::is_promotable(B_TO));
    CHECK_FALSE(Pieces::is_promotable(B_NY));
    CHECK_FALSE(Pieces::is_promotable(B_NK));
    CHECK_FALSE(Pieces::is_promotable(B_NG));
    CHECK_FALSE(Pieces::is_promotable(B_UM));
    CHECK_FALSE(Pieces::is_promotable(B_RY));

    CHECK_TRUE(Pieces::is_promotable(W_FU));
    CHECK_TRUE(Pieces::is_promotable(W_KY));
    CHECK_TRUE(Pieces::is_promotable(W_KE));
    CHECK_TRUE(Pieces::is_promotable(W_GI));
    CHECK_TRUE(Pieces::is_promotable(W_KA));
    CHECK_TRUE(Pieces::is_promotable(W_HI));
    CHECK_FALSE(Pieces::is_promotable(W_KI));
    CHECK_FALSE(Pieces::is_promotable(W_OU));
    CHECK_FALSE(Pieces::is_promotable(W_TO));
    CHECK_FALSE(Pieces::is_promotable(W_NY));
    CHECK_FALSE(Pieces::is_promotable(W_NK));
    CHECK_FALSE(Pieces::is_promotable(W_NG));
    CHECK_FALSE(Pieces::is_promotable(W_UM));
    CHECK_FALSE(Pieces::is_promotable(W_RY));
}

TEST(shogi_pieces, is_promoted)
{
    CHECK_FALSE(Pieces::is_promoted(FU));
    CHECK_FALSE(Pieces::is_promoted(KY));
    CHECK_FALSE(Pieces::is_promoted(KE));
    CHECK_FALSE(Pieces::is_promoted(GI));
    CHECK_FALSE(Pieces::is_promoted(KA));
    CHECK_FALSE(Pieces::is_promoted(HI));
    CHECK_FALSE(Pieces::is_promoted(KI));
    CHECK_FALSE(Pieces::is_promoted(OU));
    CHECK_TRUE(Pieces::is_promoted(TO));
    CHECK_TRUE(Pieces::is_promoted(NY));
    CHECK_TRUE(Pieces::is_promoted(NK));
    CHECK_TRUE(Pieces::is_promoted(NG));
    CHECK_TRUE(Pieces::is_promoted(UM));
    CHECK_TRUE(Pieces::is_promoted(RY));

    CHECK_FALSE(Pieces::is_promoted(B_FU));
    CHECK_FALSE(Pieces::is_promoted(B_KY));
    CHECK_FALSE(Pieces::is_promoted(B_KE));
    CHECK_FALSE(Pieces::is_promoted(B_GI));
    CHECK_FALSE(Pieces::is_promoted(B_KA));
    CHECK_FALSE(Pieces::is_promoted(B_HI));
    CHECK_FALSE(Pieces::is_promoted(B_KI));
    CHECK_FALSE(Pieces::is_promoted(B_OU));
    CHECK_TRUE(Pieces::is_promoted(B_TO));
    CHECK_TRUE(Pieces::is_promoted(B_NY));
    CHECK_TRUE(Pieces::is_promoted(B_NK));
    CHECK_TRUE(Pieces::is_promoted(B_NG));
    CHECK_TRUE(Pieces::is_promoted(B_UM));
    CHECK_TRUE(Pieces::is_promoted(B_RY));

    CHECK_FALSE(Pieces::is_promoted(W_FU));
    CHECK_FALSE(Pieces::is_promoted(W_KY));
    CHECK_FALSE(Pieces::is_promoted(W_KE));
    CHECK_FALSE(Pieces::is_promoted(W_GI));
    CHECK_FALSE(Pieces::is_promoted(W_KA));
    CHECK_FALSE(Pieces::is_promoted(W_HI));
    CHECK_FALSE(Pieces::is_promoted(W_KI));
    CHECK_FALSE(Pieces::is_promoted(W_OU));
    CHECK_TRUE(Pieces::is_promoted(W_TO));
    CHECK_TRUE(Pieces::is_promoted(W_NY));
    CHECK_TRUE(Pieces::is_promoted(W_NK));
    CHECK_TRUE(Pieces::is_promoted(W_NG));
    CHECK_TRUE(Pieces::is_promoted(W_UM));
    CHECK_TRUE(Pieces::is_promoted(W_RY));
}

TEST(shogi_pieces, promote_nocheck)
{
    CHECK_EQUAL(TO, Pieces::promote_nocheck(FU));
    CHECK_EQUAL(NY, Pieces::promote_nocheck(KY));
    CHECK_EQUAL(NK, Pieces::promote_nocheck(KE));
    CHECK_EQUAL(NG, Pieces::promote_nocheck(GI));
    CHECK_EQUAL(UM, Pieces::promote_nocheck(KA));
    CHECK_EQUAL(RY, Pieces::promote_nocheck(HI));

    CHECK_EQUAL(B_TO, Pieces::promote_nocheck(B_FU));
    CHECK_EQUAL(B_NY, Pieces::promote_nocheck(B_KY));
    CHECK_EQUAL(B_NK, Pieces::promote_nocheck(B_KE));
    CHECK_EQUAL(B_NG, Pieces::promote_nocheck(B_GI));
    CHECK_EQUAL(B_UM, Pieces::promote_nocheck(B_KA));
    CHECK_EQUAL(B_RY, Pieces::promote_nocheck(B_HI));

    CHECK_EQUAL(W_TO, Pieces::promote_nocheck(W_FU));
    CHECK_EQUAL(W_NY, Pieces::promote_nocheck(W_KY));
    CHECK_EQUAL(W_NK, Pieces::promote_nocheck(W_KE));
    CHECK_EQUAL(W_NG, Pieces::promote_nocheck(W_GI));
    CHECK_EQUAL(W_UM, Pieces::promote_nocheck(W_KA));
    CHECK_EQUAL(W_RY, Pieces::promote_nocheck(W_HI));
}

TEST(shogi_pieces, demote)
{
    CHECK_EQUAL(FU, Pieces::demote(FU));
    CHECK_EQUAL(KY, Pieces::demote(KY));
    CHECK_EQUAL(KE, Pieces::demote(KE));
    CHECK_EQUAL(GI, Pieces::demote(GI));
    CHECK_EQUAL(KA, Pieces::demote(KA));
    CHECK_EQUAL(HI, Pieces::demote(HI));
    CHECK_EQUAL(KI, Pieces::demote(KI));
    CHECK_EQUAL(OU, Pieces::demote(OU));
    CHECK_EQUAL(FU, Pieces::demote(TO));
    CHECK_EQUAL(KY, Pieces::demote(NY));
    CHECK_EQUAL(KE, Pieces::demote(NK));
    CHECK_EQUAL(GI, Pieces::demote(NG));
    CHECK_EQUAL(KA, Pieces::demote(UM));
    CHECK_EQUAL(HI, Pieces::demote(RY));

    CHECK_EQUAL(B_FU, Pieces::demote(B_FU));
    CHECK_EQUAL(B_KY, Pieces::demote(B_KY));
    CHECK_EQUAL(B_KE, Pieces::demote(B_KE));
    CHECK_EQUAL(B_GI, Pieces::demote(B_GI));
    CHECK_EQUAL(B_KA, Pieces::demote(B_KA));
    CHECK_EQUAL(B_HI, Pieces::demote(B_HI));
    CHECK_EQUAL(B_KI, Pieces::demote(B_KI));
    CHECK_EQUAL(B_OU, Pieces::demote(B_OU));
    CHECK_EQUAL(B_FU, Pieces::demote(B_TO));
    CHECK_EQUAL(B_KY, Pieces::demote(B_NY));
    CHECK_EQUAL(B_KE, Pieces::demote(B_NK));
    CHECK_EQUAL(B_GI, Pieces::demote(B_NG));
    CHECK_EQUAL(B_KA, Pieces::demote(B_UM));
    CHECK_EQUAL(B_HI, Pieces::demote(B_RY));

    CHECK_EQUAL(W_FU, Pieces::demote(W_FU));
    CHECK_EQUAL(W_KY, Pieces::demote(W_KY));
    CHECK_EQUAL(W_KE, Pieces::demote(W_KE));
    CHECK_EQUAL(W_GI, Pieces::demote(W_GI));
    CHECK_EQUAL(W_KA, Pieces::demote(W_KA));
    CHECK_EQUAL(W_HI, Pieces::demote(W_HI));
    CHECK_EQUAL(W_KI, Pieces::demote(W_KI));
    CHECK_EQUAL(W_OU, Pieces::demote(W_OU));
    CHECK_EQUAL(W_FU, Pieces::demote(W_TO));
    CHECK_EQUAL(W_KY, Pieces::demote(W_NY));
    CHECK_EQUAL(W_KE, Pieces::demote(W_NK));
    CHECK_EQUAL(W_GI, Pieces::demote(W_NG));
    CHECK_EQUAL(W_KA, Pieces::demote(W_UM));
    CHECK_EQUAL(W_HI, Pieces::demote(W_RY));
}

TEST(shogi_pieces, get_color)
{
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_FU));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KY));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KE));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_GI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KA));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_HI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_OU));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_TO));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_NY));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_NK));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_NG));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_UM));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_RY));

    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_FU));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KY));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KE));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_GI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KA));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_HI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_OU));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_TO));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_NY));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_NK));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_NG));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_UM));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_RY));
}

TEST(shogi_pieces, to_piece_type)
{
    CHECK_EQUAL(FU, Pieces::to_piece_type(B_FU));
    CHECK_EQUAL(KY, Pieces::to_piece_type(B_KY));
    CHECK_EQUAL(KE, Pieces::to_piece_type(B_KE));
    CHECK_EQUAL(GI, Pieces::to_piece_type(B_GI));
    CHECK_EQUAL(KA, Pieces::to_piece_type(B_KA));
    CHECK_EQUAL(HI, Pieces::to_piece_type(B_HI));
    CHECK_EQUAL(KI, Pieces::to_piece_type(B_KI));
    CHECK_EQUAL(OU, Pieces::to_piece_type(B_OU));
    CHECK_EQUAL(TO, Pieces::to_piece_type(B_TO));
    CHECK_EQUAL(NY, Pieces::to_piece_type(B_NY));
    CHECK_EQUAL(NK, Pieces::to_piece_type(B_NK));
    CHECK_EQUAL(NG, Pieces::to_piece_type(B_NG));
    CHECK_EQUAL(UM, Pieces::to_piece_type(B_UM));
    CHECK_EQUAL(RY, Pieces::to_piece_type(B_RY));

    CHECK_EQUAL(FU, Pieces::to_piece_type(W_FU));
    CHECK_EQUAL(KY, Pieces::to_piece_type(W_KY));
    CHECK_EQUAL(KE, Pieces::to_piece_type(W_KE));
    CHECK_EQUAL(GI, Pieces::to_piece_type(W_GI));
    CHECK_EQUAL(KA, Pieces::to_piece_type(W_KA));
    CHECK_EQUAL(HI, Pieces::to_piece_type(W_HI));
    CHECK_EQUAL(KI, Pieces::to_piece_type(W_KI));
    CHECK_EQUAL(OU, Pieces::to_piece_type(W_OU));
    CHECK_EQUAL(TO, Pieces::to_piece_type(W_TO));
    CHECK_EQUAL(NY, Pieces::to_piece_type(W_NY));
    CHECK_EQUAL(NK, Pieces::to_piece_type(W_NK));
    CHECK_EQUAL(NG, Pieces::to_piece_type(W_NG));
    CHECK_EQUAL(UM, Pieces::to_piece_type(W_UM));
    CHECK_EQUAL(RY, Pieces::to_piece_type(W_RY));

    CHECK_EQUAL(NA, Pieces::to_piece_type(VOID));

    CHECK_EQUAL(FU, Pieces::to_piece_type('p'));
    CHECK_EQUAL(KY, Pieces::to_piece_type('l'));
    CHECK_EQUAL(KE, Pieces::to_piece_type('n'));
    CHECK_EQUAL(GI, Pieces::to_piece_type('s'));
    CHECK_EQUAL(KA, Pieces::to_piece_type('b'));
    CHECK_EQUAL(HI, Pieces::to_piece_type('r'));
    CHECK_EQUAL(KI, Pieces::to_piece_type('g'));
    CHECK_EQUAL(OU, Pieces::to_piece_type('k'));
    CHECK_EQUAL(NA, Pieces::to_piece_type('a'));

    CHECK_EQUAL(FU, Pieces::to_piece_type('P'));
    CHECK_EQUAL(KY, Pieces::to_piece_type('L'));
    CHECK_EQUAL(KE, Pieces::to_piece_type('N'));
    CHECK_EQUAL(GI, Pieces::to_piece_type('S'));
    CHECK_EQUAL(KA, Pieces::to_piece_type('B'));
    CHECK_EQUAL(HI, Pieces::to_piece_type('R'));
    CHECK_EQUAL(KI, Pieces::to_piece_type('G'));
    CHECK_EQUAL(OU, Pieces::to_piece_type('K'));
    CHECK_EQUAL(NA, Pieces::to_piece_type('A'));
}

TEST(shogi_pieces, to_board_piece)
{
    CHECK_EQUAL(B_FU, Pieces::to_board_piece(vshogi::BLACK, FU));
    CHECK_EQUAL(B_KY, Pieces::to_board_piece(vshogi::BLACK, KY));
    CHECK_EQUAL(B_KE, Pieces::to_board_piece(vshogi::BLACK, KE));
    CHECK_EQUAL(B_GI, Pieces::to_board_piece(vshogi::BLACK, GI));
    CHECK_EQUAL(B_KA, Pieces::to_board_piece(vshogi::BLACK, KA));
    CHECK_EQUAL(B_HI, Pieces::to_board_piece(vshogi::BLACK, HI));
    CHECK_EQUAL(B_KI, Pieces::to_board_piece(vshogi::BLACK, KI));
    CHECK_EQUAL(B_OU, Pieces::to_board_piece(vshogi::BLACK, OU));
    CHECK_EQUAL(B_TO, Pieces::to_board_piece(vshogi::BLACK, TO));
    CHECK_EQUAL(B_NY, Pieces::to_board_piece(vshogi::BLACK, NY));
    CHECK_EQUAL(B_NK, Pieces::to_board_piece(vshogi::BLACK, NK));
    CHECK_EQUAL(B_NG, Pieces::to_board_piece(vshogi::BLACK, NG));
    CHECK_EQUAL(B_UM, Pieces::to_board_piece(vshogi::BLACK, UM));
    CHECK_EQUAL(B_RY, Pieces::to_board_piece(vshogi::BLACK, RY));

    CHECK_EQUAL(W_FU, Pieces::to_board_piece(vshogi::WHITE, FU));
    CHECK_EQUAL(W_KY, Pieces::to_board_piece(vshogi::WHITE, KY));
    CHECK_EQUAL(W_KE, Pieces::to_board_piece(vshogi::WHITE, KE));
    CHECK_EQUAL(W_GI, Pieces::to_board_piece(vshogi::WHITE, GI));
    CHECK_EQUAL(W_KA, Pieces::to_board_piece(vshogi::WHITE, KA));
    CHECK_EQUAL(W_HI, Pieces::to_board_piece(vshogi::WHITE, HI));
    CHECK_EQUAL(W_KI, Pieces::to_board_piece(vshogi::WHITE, KI));
    CHECK_EQUAL(W_OU, Pieces::to_board_piece(vshogi::WHITE, OU));
    CHECK_EQUAL(W_TO, Pieces::to_board_piece(vshogi::WHITE, TO));
    CHECK_EQUAL(W_NY, Pieces::to_board_piece(vshogi::WHITE, NY));
    CHECK_EQUAL(W_NK, Pieces::to_board_piece(vshogi::WHITE, NK));
    CHECK_EQUAL(W_NG, Pieces::to_board_piece(vshogi::WHITE, NG));
    CHECK_EQUAL(W_UM, Pieces::to_board_piece(vshogi::WHITE, UM));
    CHECK_EQUAL(W_RY, Pieces::to_board_piece(vshogi::WHITE, RY));

    CHECK_EQUAL(VOID, Pieces::to_board_piece(vshogi::BLACK, NA));
    CHECK_EQUAL(VOID, Pieces::to_board_piece(vshogi::WHITE, NA));
}

TEST(shogi_pieces, to_char)
{
    CHECK_EQUAL('p', Pieces::to_char(FU));
    CHECK_EQUAL('l', Pieces::to_char(KY));
    CHECK_EQUAL('n', Pieces::to_char(KE));
    CHECK_EQUAL('s', Pieces::to_char(GI));
    CHECK_EQUAL('b', Pieces::to_char(KA));
    CHECK_EQUAL('r', Pieces::to_char(HI));
    CHECK_EQUAL('g', Pieces::to_char(KI));
    CHECK_EQUAL('k', Pieces::to_char(OU));
}

TEST(shogi_pieces, append_sfen)
{
    // clang-format off
    {auto actual = std::string(); Pieces::append_sfen(B_FU, actual); STRCMP_EQUAL("P", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_KY, actual); STRCMP_EQUAL("L", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_KE, actual); STRCMP_EQUAL("N", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_GI, actual); STRCMP_EQUAL("S", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_KA, actual); STRCMP_EQUAL("B", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_HI, actual); STRCMP_EQUAL("R", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_KI, actual); STRCMP_EQUAL("G", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_OU, actual); STRCMP_EQUAL("K", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_TO, actual); STRCMP_EQUAL("+P", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_NY, actual); STRCMP_EQUAL("+L", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_NK, actual); STRCMP_EQUAL("+N", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_NG, actual); STRCMP_EQUAL("+S", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_UM, actual); STRCMP_EQUAL("+B", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_RY, actual); STRCMP_EQUAL("+R", actual.c_str());}

    {auto actual = std::string(); Pieces::append_sfen(W_FU, actual); STRCMP_EQUAL("p", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_KY, actual); STRCMP_EQUAL("l", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_KE, actual); STRCMP_EQUAL("n", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_GI, actual); STRCMP_EQUAL("s", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_KA, actual); STRCMP_EQUAL("b", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_HI, actual); STRCMP_EQUAL("r", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_KI, actual); STRCMP_EQUAL("g", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_OU, actual); STRCMP_EQUAL("k", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_TO, actual); STRCMP_EQUAL("+p", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_NY, actual); STRCMP_EQUAL("+l", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_NK, actual); STRCMP_EQUAL("+n", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_NG, actual); STRCMP_EQUAL("+s", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_UM, actual); STRCMP_EQUAL("+b", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_RY, actual); STRCMP_EQUAL("+r", actual.c_str());}
    // clang-format on
}

TEST(shogi_pieces, get_point)
{
    CHECK_EQUAL(1, Pieces::get_point(FU));
    CHECK_EQUAL(1, Pieces::get_point(KY));
    CHECK_EQUAL(1, Pieces::get_point(KE));
    CHECK_EQUAL(1, Pieces::get_point(GI));
    CHECK_EQUAL(5, Pieces::get_point(KA));
    CHECK_EQUAL(5, Pieces::get_point(HI));
    CHECK_EQUAL(1, Pieces::get_point(KI));
    CHECK_EQUAL(0, Pieces::get_point(OU));
    CHECK_EQUAL(1, Pieces::get_point(TO));
    CHECK_EQUAL(1, Pieces::get_point(NY));
    CHECK_EQUAL(1, Pieces::get_point(NK));
    CHECK_EQUAL(1, Pieces::get_point(NG));
    CHECK_EQUAL(5, Pieces::get_point(UM));
    CHECK_EQUAL(5, Pieces::get_point(RY));
    CHECK_EQUAL(0, Pieces::get_point(NA));

    CHECK_EQUAL(1, Pieces::get_point(B_FU));
    CHECK_EQUAL(1, Pieces::get_point(B_KY));
    CHECK_EQUAL(1, Pieces::get_point(B_KE));
    CHECK_EQUAL(1, Pieces::get_point(B_GI));
    CHECK_EQUAL(5, Pieces::get_point(B_KA));
    CHECK_EQUAL(5, Pieces::get_point(B_HI));
    CHECK_EQUAL(1, Pieces::get_point(B_KI));
    CHECK_EQUAL(0, Pieces::get_point(B_OU));
    CHECK_EQUAL(1, Pieces::get_point(B_TO));
    CHECK_EQUAL(1, Pieces::get_point(B_NY));
    CHECK_EQUAL(1, Pieces::get_point(B_NK));
    CHECK_EQUAL(1, Pieces::get_point(B_NG));
    CHECK_EQUAL(5, Pieces::get_point(B_UM));
    CHECK_EQUAL(5, Pieces::get_point(B_RY));

    CHECK_EQUAL(1, Pieces::get_point(W_FU));
    CHECK_EQUAL(1, Pieces::get_point(W_KY));
    CHECK_EQUAL(1, Pieces::get_point(W_KE));
    CHECK_EQUAL(1, Pieces::get_point(W_GI));
    CHECK_EQUAL(5, Pieces::get_point(W_KA));
    CHECK_EQUAL(5, Pieces::get_point(W_HI));
    CHECK_EQUAL(1, Pieces::get_point(W_KI));
    CHECK_EQUAL(0, Pieces::get_point(W_OU));
    CHECK_EQUAL(1, Pieces::get_point(W_TO));
    CHECK_EQUAL(1, Pieces::get_point(W_NY));
    CHECK_EQUAL(1, Pieces::get_point(W_NK));
    CHECK_EQUAL(1, Pieces::get_point(W_NG));
    CHECK_EQUAL(5, Pieces::get_point(W_UM));
    CHECK_EQUAL(5, Pieces::get_point(W_RY));

    CHECK_EQUAL(0, Pieces::get_point(VOID));
}

TEST(shogi_pieces, get_attack_directions)
{
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
    for (auto pt : EnumIterator<PieceTypeEnum, Config::num_piece_types>()) {
        for (auto&& color : {BLACK, WHITE}) {
            const ColoredPieceEnum p
                = vshogi::shogi::Pieces::to_board_piece(color, pt);
            const auto actual = vshogi::shogi::Pieces::get_attack_directions(p);
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
