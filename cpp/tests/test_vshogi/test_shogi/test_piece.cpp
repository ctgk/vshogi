#include "vshogi/shogi/piece.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_shogi/test_shogi.hpp"

namespace test_vshogi::test_shogi
{

using namespace vshogi;
using namespace vshogi::shogi;

TEST_GROUP(shogi_piece){};

TEST(shogi_piece, is_promotable)
{
    CHECK_TRUE(is_promotable(FU));
    CHECK_TRUE(is_promotable(KY));
    CHECK_TRUE(is_promotable(KE));
    CHECK_TRUE(is_promotable(GI));
    CHECK_TRUE(is_promotable(KA));
    CHECK_TRUE(is_promotable(HI));
    CHECK_FALSE(is_promotable(KI));
    CHECK_FALSE(is_promotable(OU));
    CHECK_FALSE(is_promotable(TO));
    CHECK_FALSE(is_promotable(NY));
    CHECK_FALSE(is_promotable(NK));
    CHECK_FALSE(is_promotable(NG));
    CHECK_FALSE(is_promotable(UM));
    CHECK_FALSE(is_promotable(RY));

    CHECK_TRUE(is_promotable(B_FU));
    CHECK_TRUE(is_promotable(B_KY));
    CHECK_TRUE(is_promotable(B_KE));
    CHECK_TRUE(is_promotable(B_GI));
    CHECK_TRUE(is_promotable(B_KA));
    CHECK_TRUE(is_promotable(B_HI));
    CHECK_FALSE(is_promotable(B_KI));
    CHECK_FALSE(is_promotable(B_OU));
    CHECK_FALSE(is_promotable(B_TO));
    CHECK_FALSE(is_promotable(B_NY));
    CHECK_FALSE(is_promotable(B_NK));
    CHECK_FALSE(is_promotable(B_NG));
    CHECK_FALSE(is_promotable(B_UM));
    CHECK_FALSE(is_promotable(B_RY));

    CHECK_TRUE(is_promotable(W_FU));
    CHECK_TRUE(is_promotable(W_KY));
    CHECK_TRUE(is_promotable(W_KE));
    CHECK_TRUE(is_promotable(W_GI));
    CHECK_TRUE(is_promotable(W_KA));
    CHECK_TRUE(is_promotable(W_HI));
    CHECK_FALSE(is_promotable(W_KI));
    CHECK_FALSE(is_promotable(W_OU));
    CHECK_FALSE(is_promotable(W_TO));
    CHECK_FALSE(is_promotable(W_NY));
    CHECK_FALSE(is_promotable(W_NK));
    CHECK_FALSE(is_promotable(W_NG));
    CHECK_FALSE(is_promotable(W_UM));
    CHECK_FALSE(is_promotable(W_RY));
}

TEST(shogi_piece, is_promoted)
{
    CHECK_FALSE(is_promoted(FU));
    CHECK_FALSE(is_promoted(KY));
    CHECK_FALSE(is_promoted(KE));
    CHECK_FALSE(is_promoted(GI));
    CHECK_FALSE(is_promoted(KA));
    CHECK_FALSE(is_promoted(HI));
    CHECK_FALSE(is_promoted(KI));
    CHECK_FALSE(is_promoted(OU));
    CHECK_TRUE(is_promoted(TO));
    CHECK_TRUE(is_promoted(NY));
    CHECK_TRUE(is_promoted(NK));
    CHECK_TRUE(is_promoted(NG));
    CHECK_TRUE(is_promoted(UM));
    CHECK_TRUE(is_promoted(RY));

    CHECK_FALSE(is_promoted(B_FU));
    CHECK_FALSE(is_promoted(B_KY));
    CHECK_FALSE(is_promoted(B_KE));
    CHECK_FALSE(is_promoted(B_GI));
    CHECK_FALSE(is_promoted(B_KA));
    CHECK_FALSE(is_promoted(B_HI));
    CHECK_FALSE(is_promoted(B_KI));
    CHECK_FALSE(is_promoted(B_OU));
    CHECK_TRUE(is_promoted(B_TO));
    CHECK_TRUE(is_promoted(B_NY));
    CHECK_TRUE(is_promoted(B_NK));
    CHECK_TRUE(is_promoted(B_NG));
    CHECK_TRUE(is_promoted(B_UM));
    CHECK_TRUE(is_promoted(B_RY));

    CHECK_FALSE(is_promoted(W_FU));
    CHECK_FALSE(is_promoted(W_KY));
    CHECK_FALSE(is_promoted(W_KE));
    CHECK_FALSE(is_promoted(W_GI));
    CHECK_FALSE(is_promoted(W_KA));
    CHECK_FALSE(is_promoted(W_HI));
    CHECK_FALSE(is_promoted(W_KI));
    CHECK_FALSE(is_promoted(W_OU));
    CHECK_TRUE(is_promoted(W_TO));
    CHECK_TRUE(is_promoted(W_NY));
    CHECK_TRUE(is_promoted(W_NK));
    CHECK_TRUE(is_promoted(W_NG));
    CHECK_TRUE(is_promoted(W_UM));
    CHECK_TRUE(is_promoted(W_RY));
}

TEST(shogi_piece, promote)
{
    CHECK_EQUAL(TO, promote(FU));
    CHECK_EQUAL(NY, promote(KY));
    CHECK_EQUAL(NK, promote(KE));
    CHECK_EQUAL(NG, promote(GI));
    CHECK_EQUAL(UM, promote(KA));
    CHECK_EQUAL(RY, promote(HI));

    CHECK_EQUAL(B_TO, promote(B_FU));
    CHECK_EQUAL(B_NY, promote(B_KY));
    CHECK_EQUAL(B_NK, promote(B_KE));
    CHECK_EQUAL(B_NG, promote(B_GI));
    CHECK_EQUAL(B_UM, promote(B_KA));
    CHECK_EQUAL(B_RY, promote(B_HI));

    CHECK_EQUAL(W_TO, promote(W_FU));
    CHECK_EQUAL(W_NY, promote(W_KY));
    CHECK_EQUAL(W_NK, promote(W_KE));
    CHECK_EQUAL(W_NG, promote(W_GI));
    CHECK_EQUAL(W_UM, promote(W_KA));
    CHECK_EQUAL(W_RY, promote(W_HI));
}

TEST(shogi_piece, demote)
{
    CHECK_EQUAL(FU, demote(FU));
    CHECK_EQUAL(KY, demote(KY));
    CHECK_EQUAL(KE, demote(KE));
    CHECK_EQUAL(GI, demote(GI));
    CHECK_EQUAL(KA, demote(KA));
    CHECK_EQUAL(HI, demote(HI));
    CHECK_EQUAL(KI, demote(KI));
    CHECK_EQUAL(OU, demote(OU));
    CHECK_EQUAL(FU, demote(TO));
    CHECK_EQUAL(KY, demote(NY));
    CHECK_EQUAL(KE, demote(NK));
    CHECK_EQUAL(GI, demote(NG));
    CHECK_EQUAL(KA, demote(UM));
    CHECK_EQUAL(HI, demote(RY));

    CHECK_EQUAL(B_FU, demote(B_FU));
    CHECK_EQUAL(B_KY, demote(B_KY));
    CHECK_EQUAL(B_KE, demote(B_KE));
    CHECK_EQUAL(B_GI, demote(B_GI));
    CHECK_EQUAL(B_KA, demote(B_KA));
    CHECK_EQUAL(B_HI, demote(B_HI));
    CHECK_EQUAL(B_KI, demote(B_KI));
    CHECK_EQUAL(B_OU, demote(B_OU));
    CHECK_EQUAL(B_FU, demote(B_TO));
    CHECK_EQUAL(B_KY, demote(B_NY));
    CHECK_EQUAL(B_KE, demote(B_NK));
    CHECK_EQUAL(B_GI, demote(B_NG));
    CHECK_EQUAL(B_KA, demote(B_UM));
    CHECK_EQUAL(B_HI, demote(B_RY));

    CHECK_EQUAL(W_FU, demote(W_FU));
    CHECK_EQUAL(W_KY, demote(W_KY));
    CHECK_EQUAL(W_KE, demote(W_KE));
    CHECK_EQUAL(W_GI, demote(W_GI));
    CHECK_EQUAL(W_KA, demote(W_KA));
    CHECK_EQUAL(W_HI, demote(W_HI));
    CHECK_EQUAL(W_KI, demote(W_KI));
    CHECK_EQUAL(W_OU, demote(W_OU));
    CHECK_EQUAL(W_FU, demote(W_TO));
    CHECK_EQUAL(W_KY, demote(W_NY));
    CHECK_EQUAL(W_KE, demote(W_NK));
    CHECK_EQUAL(W_GI, demote(W_NG));
    CHECK_EQUAL(W_KA, demote(W_UM));
    CHECK_EQUAL(W_HI, demote(W_RY));
}

TEST(shogi_piece, get_color)
{
    CHECK_EQUAL(BLACK, get_color(B_FU));
    CHECK_EQUAL(BLACK, get_color(B_KY));
    CHECK_EQUAL(BLACK, get_color(B_KE));
    CHECK_EQUAL(BLACK, get_color(B_GI));
    CHECK_EQUAL(BLACK, get_color(B_KA));
    CHECK_EQUAL(BLACK, get_color(B_HI));
    CHECK_EQUAL(BLACK, get_color(B_KI));
    CHECK_EQUAL(BLACK, get_color(B_OU));
    CHECK_EQUAL(BLACK, get_color(B_TO));
    CHECK_EQUAL(BLACK, get_color(B_NY));
    CHECK_EQUAL(BLACK, get_color(B_NK));
    CHECK_EQUAL(BLACK, get_color(B_NG));
    CHECK_EQUAL(BLACK, get_color(B_UM));
    CHECK_EQUAL(BLACK, get_color(B_RY));

    CHECK_EQUAL(WHITE, get_color(W_FU));
    CHECK_EQUAL(WHITE, get_color(W_KY));
    CHECK_EQUAL(WHITE, get_color(W_KE));
    CHECK_EQUAL(WHITE, get_color(W_GI));
    CHECK_EQUAL(WHITE, get_color(W_KA));
    CHECK_EQUAL(WHITE, get_color(W_HI));
    CHECK_EQUAL(WHITE, get_color(W_KI));
    CHECK_EQUAL(WHITE, get_color(W_OU));
    CHECK_EQUAL(WHITE, get_color(W_TO));
    CHECK_EQUAL(WHITE, get_color(W_NY));
    CHECK_EQUAL(WHITE, get_color(W_NK));
    CHECK_EQUAL(WHITE, get_color(W_NG));
    CHECK_EQUAL(WHITE, get_color(W_UM));
    CHECK_EQUAL(WHITE, get_color(W_RY));
}

TEST(shogi_piece, to_piece_type)
{
    CHECK_EQUAL(FU, to_piece_type(B_FU));
    CHECK_EQUAL(KY, to_piece_type(B_KY));
    CHECK_EQUAL(KE, to_piece_type(B_KE));
    CHECK_EQUAL(GI, to_piece_type(B_GI));
    CHECK_EQUAL(KA, to_piece_type(B_KA));
    CHECK_EQUAL(HI, to_piece_type(B_HI));
    CHECK_EQUAL(KI, to_piece_type(B_KI));
    CHECK_EQUAL(OU, to_piece_type(B_OU));
    CHECK_EQUAL(TO, to_piece_type(B_TO));
    CHECK_EQUAL(NY, to_piece_type(B_NY));
    CHECK_EQUAL(NK, to_piece_type(B_NK));
    CHECK_EQUAL(NG, to_piece_type(B_NG));
    CHECK_EQUAL(UM, to_piece_type(B_UM));
    CHECK_EQUAL(RY, to_piece_type(B_RY));

    CHECK_EQUAL(FU, to_piece_type(W_FU));
    CHECK_EQUAL(KY, to_piece_type(W_KY));
    CHECK_EQUAL(KE, to_piece_type(W_KE));
    CHECK_EQUAL(GI, to_piece_type(W_GI));
    CHECK_EQUAL(KA, to_piece_type(W_KA));
    CHECK_EQUAL(HI, to_piece_type(W_HI));
    CHECK_EQUAL(KI, to_piece_type(W_KI));
    CHECK_EQUAL(OU, to_piece_type(W_OU));
    CHECK_EQUAL(TO, to_piece_type(W_TO));
    CHECK_EQUAL(NY, to_piece_type(W_NY));
    CHECK_EQUAL(NK, to_piece_type(W_NK));
    CHECK_EQUAL(NG, to_piece_type(W_NG));
    CHECK_EQUAL(UM, to_piece_type(W_UM));
    CHECK_EQUAL(RY, to_piece_type(W_RY));

    CHECK_EQUAL(NA, to_piece_type(VOID));
}

TEST(shogi_piece, to_board_piece)
{
    CHECK_EQUAL(B_FU, to_board_piece(BLACK, FU));
    CHECK_EQUAL(B_KY, to_board_piece(BLACK, KY));
    CHECK_EQUAL(B_KE, to_board_piece(BLACK, KE));
    CHECK_EQUAL(B_GI, to_board_piece(BLACK, GI));
    CHECK_EQUAL(B_KA, to_board_piece(BLACK, KA));
    CHECK_EQUAL(B_HI, to_board_piece(BLACK, HI));
    CHECK_EQUAL(B_KI, to_board_piece(BLACK, KI));
    CHECK_EQUAL(B_OU, to_board_piece(BLACK, OU));
    CHECK_EQUAL(B_TO, to_board_piece(BLACK, TO));
    CHECK_EQUAL(B_NY, to_board_piece(BLACK, NY));
    CHECK_EQUAL(B_NK, to_board_piece(BLACK, NK));
    CHECK_EQUAL(B_NG, to_board_piece(BLACK, NG));
    CHECK_EQUAL(B_UM, to_board_piece(BLACK, UM));
    CHECK_EQUAL(B_RY, to_board_piece(BLACK, RY));

    CHECK_EQUAL(W_FU, to_board_piece(WHITE, FU));
    CHECK_EQUAL(W_KY, to_board_piece(WHITE, KY));
    CHECK_EQUAL(W_KE, to_board_piece(WHITE, KE));
    CHECK_EQUAL(W_GI, to_board_piece(WHITE, GI));
    CHECK_EQUAL(W_KA, to_board_piece(WHITE, KA));
    CHECK_EQUAL(W_HI, to_board_piece(WHITE, HI));
    CHECK_EQUAL(W_KI, to_board_piece(WHITE, KI));
    CHECK_EQUAL(W_OU, to_board_piece(WHITE, OU));
    CHECK_EQUAL(W_TO, to_board_piece(WHITE, TO));
    CHECK_EQUAL(W_NY, to_board_piece(WHITE, NY));
    CHECK_EQUAL(W_NK, to_board_piece(WHITE, NK));
    CHECK_EQUAL(W_NG, to_board_piece(WHITE, NG));
    CHECK_EQUAL(W_UM, to_board_piece(WHITE, UM));
    CHECK_EQUAL(W_RY, to_board_piece(WHITE, RY));

    CHECK_EQUAL(VOID, to_board_piece(BLACK, NA));
    CHECK_EQUAL(VOID, to_board_piece(WHITE, NA));
}

TEST(shogi_piece, append_sfen)
{
    // clang-format off
    {auto actual = std::string(); append_sfen(B_FU, actual); STRCMP_EQUAL("P", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_KY, actual); STRCMP_EQUAL("L", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_KE, actual); STRCMP_EQUAL("N", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_GI, actual); STRCMP_EQUAL("S", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_KA, actual); STRCMP_EQUAL("B", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_HI, actual); STRCMP_EQUAL("R", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_KI, actual); STRCMP_EQUAL("G", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_OU, actual); STRCMP_EQUAL("K", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_TO, actual); STRCMP_EQUAL("+P", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_NY, actual); STRCMP_EQUAL("+L", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_NK, actual); STRCMP_EQUAL("+N", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_NG, actual); STRCMP_EQUAL("+S", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_UM, actual); STRCMP_EQUAL("+B", actual.c_str());}
    {auto actual = std::string(); append_sfen(B_RY, actual); STRCMP_EQUAL("+R", actual.c_str());}

    {auto actual = std::string(); append_sfen(W_FU, actual); STRCMP_EQUAL("p", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_KY, actual); STRCMP_EQUAL("l", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_KE, actual); STRCMP_EQUAL("n", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_GI, actual); STRCMP_EQUAL("s", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_KA, actual); STRCMP_EQUAL("b", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_HI, actual); STRCMP_EQUAL("r", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_KI, actual); STRCMP_EQUAL("g", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_OU, actual); STRCMP_EQUAL("k", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_TO, actual); STRCMP_EQUAL("+p", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_NY, actual); STRCMP_EQUAL("+l", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_NK, actual); STRCMP_EQUAL("+n", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_NG, actual); STRCMP_EQUAL("+s", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_UM, actual); STRCMP_EQUAL("+b", actual.c_str());}
    {auto actual = std::string(); append_sfen(W_RY, actual); STRCMP_EQUAL("+r", actual.c_str());}
    // clang-format on
}

} // namespace test_vshogi::test_shogi
