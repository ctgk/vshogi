#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP(judkins_shogi_pieces){};

TEST(judkins_shogi_pieces, is_promotable)
{
    CHECK_TRUE(Pieces::is_promotable(FU));
    CHECK_TRUE(Pieces::is_promotable(KE));
    CHECK_TRUE(Pieces::is_promotable(GI));
    CHECK_TRUE(Pieces::is_promotable(KA));
    CHECK_TRUE(Pieces::is_promotable(HI));
    CHECK_FALSE(Pieces::is_promotable(KI));
    CHECK_FALSE(Pieces::is_promotable(OU));
    CHECK_FALSE(Pieces::is_promotable(TO));
    CHECK_FALSE(Pieces::is_promotable(NK));
    CHECK_FALSE(Pieces::is_promotable(NG));
    CHECK_FALSE(Pieces::is_promotable(UM));
    CHECK_FALSE(Pieces::is_promotable(RY));
    CHECK_FALSE(Pieces::is_promotable(NA));

    CHECK_TRUE(Pieces::is_promotable(B_FU));
    CHECK_TRUE(Pieces::is_promotable(B_KE));
    CHECK_TRUE(Pieces::is_promotable(B_GI));
    CHECK_TRUE(Pieces::is_promotable(B_KA));
    CHECK_TRUE(Pieces::is_promotable(B_HI));
    CHECK_FALSE(Pieces::is_promotable(B_KI));
    CHECK_FALSE(Pieces::is_promotable(B_OU));
    CHECK_FALSE(Pieces::is_promotable(B_TO));
    CHECK_FALSE(Pieces::is_promotable(B_NK));
    CHECK_FALSE(Pieces::is_promotable(B_NG));
    CHECK_FALSE(Pieces::is_promotable(B_UM));
    CHECK_FALSE(Pieces::is_promotable(B_RY));

    CHECK_TRUE(Pieces::is_promotable(W_FU));
    CHECK_TRUE(Pieces::is_promotable(W_KE));
    CHECK_TRUE(Pieces::is_promotable(W_GI));
    CHECK_TRUE(Pieces::is_promotable(W_KA));
    CHECK_TRUE(Pieces::is_promotable(W_HI));
    CHECK_FALSE(Pieces::is_promotable(W_KI));
    CHECK_FALSE(Pieces::is_promotable(W_OU));
    CHECK_FALSE(Pieces::is_promotable(W_TO));
    CHECK_FALSE(Pieces::is_promotable(W_NK));
    CHECK_FALSE(Pieces::is_promotable(W_NG));
    CHECK_FALSE(Pieces::is_promotable(W_UM));
    CHECK_FALSE(Pieces::is_promotable(W_RY));

    CHECK_FALSE(Pieces::is_promotable(VOID));
}

TEST(judkins_shogi_pieces, is_promoted)
{
    CHECK_FALSE(Pieces::is_promoted(FU));
    CHECK_FALSE(Pieces::is_promoted(KE));
    CHECK_FALSE(Pieces::is_promoted(GI));
    CHECK_FALSE(Pieces::is_promoted(KA));
    CHECK_FALSE(Pieces::is_promoted(HI));
    CHECK_FALSE(Pieces::is_promoted(KI));
    CHECK_FALSE(Pieces::is_promoted(OU));
    CHECK_TRUE(Pieces::is_promoted(TO));
    CHECK_TRUE(Pieces::is_promoted(NK));
    CHECK_TRUE(Pieces::is_promoted(NG));
    CHECK_TRUE(Pieces::is_promoted(UM));
    CHECK_TRUE(Pieces::is_promoted(RY));
    CHECK_TRUE(Pieces::is_promoted(NA));

    CHECK_FALSE(Pieces::is_promoted(B_FU));
    CHECK_FALSE(Pieces::is_promoted(B_KE));
    CHECK_FALSE(Pieces::is_promoted(B_GI));
    CHECK_FALSE(Pieces::is_promoted(B_KA));
    CHECK_FALSE(Pieces::is_promoted(B_HI));
    CHECK_FALSE(Pieces::is_promoted(B_KI));
    CHECK_FALSE(Pieces::is_promoted(B_OU));
    CHECK_TRUE(Pieces::is_promoted(B_TO));
    CHECK_TRUE(Pieces::is_promoted(B_NK));
    CHECK_TRUE(Pieces::is_promoted(B_NG));
    CHECK_TRUE(Pieces::is_promoted(B_UM));
    CHECK_TRUE(Pieces::is_promoted(B_RY));

    CHECK_FALSE(Pieces::is_promoted(W_FU));
    CHECK_FALSE(Pieces::is_promoted(W_KE));
    CHECK_FALSE(Pieces::is_promoted(W_GI));
    CHECK_FALSE(Pieces::is_promoted(W_KA));
    CHECK_FALSE(Pieces::is_promoted(W_HI));
    CHECK_FALSE(Pieces::is_promoted(W_KI));
    CHECK_FALSE(Pieces::is_promoted(W_OU));
    CHECK_TRUE(Pieces::is_promoted(W_TO));
    CHECK_TRUE(Pieces::is_promoted(W_NK));
    CHECK_TRUE(Pieces::is_promoted(W_NG));
    CHECK_TRUE(Pieces::is_promoted(W_UM));
    CHECK_TRUE(Pieces::is_promoted(W_RY));
}

TEST(judkins_shogi_pieces, promote)
{
    CHECK_EQUAL(TO, Pieces::promote(FU));
    CHECK_EQUAL(NK, Pieces::promote(KE));
    CHECK_EQUAL(NG, Pieces::promote(GI));
    CHECK_EQUAL(UM, Pieces::promote(KA));
    CHECK_EQUAL(RY, Pieces::promote(HI));
    // CHECK_EQUAL(KI, Pieces::promote(KI));
    // CHECK_EQUAL(OU, Pieces::promote(OU));
    CHECK_EQUAL(TO, Pieces::promote(TO));
    CHECK_EQUAL(NK, Pieces::promote(NK));
    CHECK_EQUAL(NG, Pieces::promote(NG));
    CHECK_EQUAL(UM, Pieces::promote(UM));
    CHECK_EQUAL(RY, Pieces::promote(RY));
    CHECK_EQUAL(NA, Pieces::promote(NA));

    CHECK_EQUAL(B_TO, Pieces::promote(B_FU));
    CHECK_EQUAL(B_NK, Pieces::promote(B_KE));
    CHECK_EQUAL(B_NG, Pieces::promote(B_GI));
    CHECK_EQUAL(B_UM, Pieces::promote(B_KA));
    CHECK_EQUAL(B_RY, Pieces::promote(B_HI));
    // CHECK_EQUAL(B_KI, Pieces::promote(B_KI));
    // CHECK_EQUAL(B_OU, Pieces::promote(B_OU));
    CHECK_EQUAL(B_TO, Pieces::promote(B_TO));
    CHECK_EQUAL(B_NK, Pieces::promote(B_NK));
    CHECK_EQUAL(B_NG, Pieces::promote(B_NG));
    CHECK_EQUAL(B_UM, Pieces::promote(B_UM));
    CHECK_EQUAL(B_RY, Pieces::promote(B_RY));

    CHECK_EQUAL(W_TO, Pieces::promote(W_FU));
    CHECK_EQUAL(W_NK, Pieces::promote(W_KE));
    CHECK_EQUAL(W_NG, Pieces::promote(W_GI));
    CHECK_EQUAL(W_UM, Pieces::promote(W_KA));
    CHECK_EQUAL(W_RY, Pieces::promote(W_HI));
    // CHECK_EQUAL(W_KI, Pieces::promote(W_KI));
    // CHECK_EQUAL(W_OU, Pieces::promote(W_OU));
    CHECK_EQUAL(W_TO, Pieces::promote(W_TO));
    CHECK_EQUAL(W_NK, Pieces::promote(W_NK));
    CHECK_EQUAL(W_NG, Pieces::promote(W_NG));
    CHECK_EQUAL(W_UM, Pieces::promote(W_UM));
    CHECK_EQUAL(W_RY, Pieces::promote(W_RY));
}

TEST(judkins_shogi_pieces, demote)
{
    CHECK_EQUAL(FU, Pieces::demote(FU));
    CHECK_EQUAL(KE, Pieces::demote(KE));
    CHECK_EQUAL(GI, Pieces::demote(GI));
    CHECK_EQUAL(KA, Pieces::demote(KA));
    CHECK_EQUAL(HI, Pieces::demote(HI));
    CHECK_EQUAL(KI, Pieces::demote(KI));
    CHECK_EQUAL(OU, Pieces::demote(OU));
    CHECK_EQUAL(FU, Pieces::demote(TO));
    CHECK_EQUAL(KE, Pieces::demote(NK));
    CHECK_EQUAL(GI, Pieces::demote(NG));
    CHECK_EQUAL(KA, Pieces::demote(UM));
    CHECK_EQUAL(HI, Pieces::demote(RY));

    CHECK_EQUAL(B_FU, Pieces::demote(B_FU));
    CHECK_EQUAL(B_KE, Pieces::demote(B_KE));
    CHECK_EQUAL(B_GI, Pieces::demote(B_GI));
    CHECK_EQUAL(B_KA, Pieces::demote(B_KA));
    CHECK_EQUAL(B_HI, Pieces::demote(B_HI));
    CHECK_EQUAL(B_KI, Pieces::demote(B_KI));
    CHECK_EQUAL(B_OU, Pieces::demote(B_OU));
    CHECK_EQUAL(B_FU, Pieces::demote(B_TO));
    CHECK_EQUAL(B_KE, Pieces::demote(B_NK));
    CHECK_EQUAL(B_GI, Pieces::demote(B_NG));
    CHECK_EQUAL(B_KA, Pieces::demote(B_UM));
    CHECK_EQUAL(B_HI, Pieces::demote(B_RY));

    CHECK_EQUAL(W_FU, Pieces::demote(W_FU));
    CHECK_EQUAL(W_KE, Pieces::demote(W_KE));
    CHECK_EQUAL(W_GI, Pieces::demote(W_GI));
    CHECK_EQUAL(W_KA, Pieces::demote(W_KA));
    CHECK_EQUAL(W_HI, Pieces::demote(W_HI));
    CHECK_EQUAL(W_KI, Pieces::demote(W_KI));
    CHECK_EQUAL(W_OU, Pieces::demote(W_OU));
    CHECK_EQUAL(W_FU, Pieces::demote(W_TO));
    CHECK_EQUAL(W_KE, Pieces::demote(W_NK));
    CHECK_EQUAL(W_GI, Pieces::demote(W_NG));
    CHECK_EQUAL(W_KA, Pieces::demote(W_UM));
    CHECK_EQUAL(W_HI, Pieces::demote(W_RY));
}

TEST(judkins_shogi_pieces, get_color)
{
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_FU));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KE));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_GI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KA));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_HI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_OU));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_TO));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_NK));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_NG));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_UM));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_RY));

    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_FU));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KE));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_GI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KA));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_HI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_OU));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_TO));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_NK));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_NG));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_UM));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_RY));
}

TEST(judkins_shogi_pieces, to_piece_type)
{
    CHECK_EQUAL(FU, Pieces::to_piece_type(B_FU));
    CHECK_EQUAL(KE, Pieces::to_piece_type(B_KE));
    CHECK_EQUAL(GI, Pieces::to_piece_type(B_GI));
    CHECK_EQUAL(KA, Pieces::to_piece_type(B_KA));
    CHECK_EQUAL(HI, Pieces::to_piece_type(B_HI));
    CHECK_EQUAL(KI, Pieces::to_piece_type(B_KI));
    CHECK_EQUAL(OU, Pieces::to_piece_type(B_OU));
    CHECK_EQUAL(TO, Pieces::to_piece_type(B_TO));
    CHECK_EQUAL(NG, Pieces::to_piece_type(B_NG));
    CHECK_EQUAL(NG, Pieces::to_piece_type(B_NG));
    CHECK_EQUAL(UM, Pieces::to_piece_type(B_UM));
    CHECK_EQUAL(RY, Pieces::to_piece_type(B_RY));

    CHECK_EQUAL(FU, Pieces::to_piece_type(W_FU));
    CHECK_EQUAL(KE, Pieces::to_piece_type(W_KE));
    CHECK_EQUAL(GI, Pieces::to_piece_type(W_GI));
    CHECK_EQUAL(KA, Pieces::to_piece_type(W_KA));
    CHECK_EQUAL(HI, Pieces::to_piece_type(W_HI));
    CHECK_EQUAL(KI, Pieces::to_piece_type(W_KI));
    CHECK_EQUAL(OU, Pieces::to_piece_type(W_OU));
    CHECK_EQUAL(TO, Pieces::to_piece_type(W_TO));
    CHECK_EQUAL(NK, Pieces::to_piece_type(W_NK));
    CHECK_EQUAL(NG, Pieces::to_piece_type(W_NG));
    CHECK_EQUAL(UM, Pieces::to_piece_type(W_UM));
    CHECK_EQUAL(RY, Pieces::to_piece_type(W_RY));
}

TEST(judkins_shogi_pieces, to_board_piece)
{
    CHECK_EQUAL(B_FU, Pieces::to_board_piece(vshogi::BLACK, FU));
    CHECK_EQUAL(B_KE, Pieces::to_board_piece(vshogi::BLACK, KE));
    CHECK_EQUAL(B_GI, Pieces::to_board_piece(vshogi::BLACK, GI));
    CHECK_EQUAL(B_KA, Pieces::to_board_piece(vshogi::BLACK, KA));
    CHECK_EQUAL(B_HI, Pieces::to_board_piece(vshogi::BLACK, HI));
    CHECK_EQUAL(B_KI, Pieces::to_board_piece(vshogi::BLACK, KI));
    CHECK_EQUAL(B_OU, Pieces::to_board_piece(vshogi::BLACK, OU));
    CHECK_EQUAL(B_TO, Pieces::to_board_piece(vshogi::BLACK, TO));
    CHECK_EQUAL(B_NK, Pieces::to_board_piece(vshogi::BLACK, NK));
    CHECK_EQUAL(B_NG, Pieces::to_board_piece(vshogi::BLACK, NG));
    CHECK_EQUAL(B_UM, Pieces::to_board_piece(vshogi::BLACK, UM));
    CHECK_EQUAL(B_RY, Pieces::to_board_piece(vshogi::BLACK, RY));

    CHECK_EQUAL(W_FU, Pieces::to_board_piece(vshogi::WHITE, FU));
    CHECK_EQUAL(W_KE, Pieces::to_board_piece(vshogi::WHITE, KE));
    CHECK_EQUAL(W_GI, Pieces::to_board_piece(vshogi::WHITE, GI));
    CHECK_EQUAL(W_KA, Pieces::to_board_piece(vshogi::WHITE, KA));
    CHECK_EQUAL(W_HI, Pieces::to_board_piece(vshogi::WHITE, HI));
    CHECK_EQUAL(W_KI, Pieces::to_board_piece(vshogi::WHITE, KI));
    CHECK_EQUAL(W_OU, Pieces::to_board_piece(vshogi::WHITE, OU));
    CHECK_EQUAL(W_TO, Pieces::to_board_piece(vshogi::WHITE, TO));
    CHECK_EQUAL(W_NK, Pieces::to_board_piece(vshogi::WHITE, NK));
    CHECK_EQUAL(W_NG, Pieces::to_board_piece(vshogi::WHITE, NG));
    CHECK_EQUAL(W_UM, Pieces::to_board_piece(vshogi::WHITE, UM));
    CHECK_EQUAL(W_RY, Pieces::to_board_piece(vshogi::WHITE, RY));

    CHECK_EQUAL(VOID, Pieces::to_board_piece(vshogi::BLACK, NA));
    CHECK_EQUAL(VOID, Pieces::to_board_piece(vshogi::WHITE, NA));
}

TEST(judkins_shogi_pieces, append_sfen)
{
    // clang-format off
    { auto actual = std::string(); Pieces::append_sfen(B_FU, actual); STRCMP_EQUAL("P", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_KE, actual); STRCMP_EQUAL("N", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_GI, actual); STRCMP_EQUAL("S", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_KA, actual); STRCMP_EQUAL("B", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_HI, actual); STRCMP_EQUAL("R", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_KI, actual); STRCMP_EQUAL("G", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_OU, actual); STRCMP_EQUAL("K", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_TO, actual); STRCMP_EQUAL("+P", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_NK, actual); STRCMP_EQUAL("+N", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_NG, actual); STRCMP_EQUAL("+S", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_UM, actual); STRCMP_EQUAL("+B", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(B_RY, actual); STRCMP_EQUAL("+R", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_FU, actual); STRCMP_EQUAL("p", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_KE, actual); STRCMP_EQUAL("n", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_GI, actual); STRCMP_EQUAL("s", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_KA, actual); STRCMP_EQUAL("b", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_HI, actual); STRCMP_EQUAL("r", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_KI, actual); STRCMP_EQUAL("g", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_OU, actual); STRCMP_EQUAL("k", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_TO, actual); STRCMP_EQUAL("+p", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_NK, actual); STRCMP_EQUAL("+n", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_NG, actual); STRCMP_EQUAL("+s", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_UM, actual); STRCMP_EQUAL("+b", actual.c_str()); }
    { auto actual = std::string(); Pieces::append_sfen(W_RY, actual); STRCMP_EQUAL("+r", actual.c_str()); }
    // clang-format on
}

TEST(judkins_shogi_pieces, get_attack_directions)
{
    {
        const auto actual = Pieces::get_attack_directions(B_FU);
        CHECK_EQUAL(vshogi::DIR_N, actual[0]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[1]);
    }
    {
        const auto actual = Pieces::get_attack_directions(B_KE);
        CHECK_EQUAL(vshogi::DIR_NNW, actual[0]);
        CHECK_EQUAL(vshogi::DIR_NNE, actual[1]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[2]);
    }
    {
        const auto actual = Pieces::get_attack_directions(B_GI);
        CHECK_EQUAL(vshogi::DIR_NW, actual[0]);
        CHECK_EQUAL(vshogi::DIR_N, actual[1]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[2]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[3]);
        CHECK_EQUAL(vshogi::DIR_SE, actual[4]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[5]);
    }
    {
        const auto actual = Pieces::get_attack_directions(B_KA);
        CHECK_EQUAL(vshogi::DIR_NW, actual[0]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[1]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[2]);
        CHECK_EQUAL(vshogi::DIR_SE, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[4]);
    }
    {
        const auto actual = Pieces::get_attack_directions(B_HI);
        CHECK_EQUAL(vshogi::DIR_N, actual[0]);
        CHECK_EQUAL(vshogi::DIR_W, actual[1]);
        CHECK_EQUAL(vshogi::DIR_E, actual[2]);
        CHECK_EQUAL(vshogi::DIR_S, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[4]);
    }
    {
        for (auto&& p : {B_KI, B_TO, B_NK, B_NG}) {
            const auto actual = Pieces::get_attack_directions(p);
            CHECK_EQUAL(vshogi::DIR_NW, actual[0]);
            CHECK_EQUAL(vshogi::DIR_N, actual[1]);
            CHECK_EQUAL(vshogi::DIR_NE, actual[2]);
            CHECK_EQUAL(vshogi::DIR_W, actual[3]);
            CHECK_EQUAL(vshogi::DIR_E, actual[4]);
            CHECK_EQUAL(vshogi::DIR_S, actual[5]);
            CHECK_EQUAL(vshogi::DIR_NA, actual[6]);
        }
    }
    {
        for (auto&& p : {B_OU, B_UM, B_RY}) {
            const auto actual = Pieces::get_attack_directions(p);
            CHECK_EQUAL(vshogi::DIR_NW, actual[0]);
            CHECK_EQUAL(vshogi::DIR_N, actual[1]);
            CHECK_EQUAL(vshogi::DIR_NE, actual[2]);
            CHECK_EQUAL(vshogi::DIR_W, actual[3]);
            CHECK_EQUAL(vshogi::DIR_E, actual[4]);
            CHECK_EQUAL(vshogi::DIR_SW, actual[5]);
            CHECK_EQUAL(vshogi::DIR_S, actual[6]);
            CHECK_EQUAL(vshogi::DIR_SE, actual[7]);
            CHECK_EQUAL(vshogi::DIR_NA, actual[8]);
        }
    }
    {
        const auto actual = Pieces::get_attack_directions(W_FU);
        CHECK_EQUAL(vshogi::DIR_S, actual[0]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[1]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_KE);
        CHECK_EQUAL(vshogi::DIR_SSE, actual[0]);
        CHECK_EQUAL(vshogi::DIR_SSW, actual[1]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[2]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_GI);
        CHECK_EQUAL(vshogi::DIR_SE, actual[0]);
        CHECK_EQUAL(vshogi::DIR_S, actual[1]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[2]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NW, actual[4]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[5]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_KA);
        CHECK_EQUAL(vshogi::DIR_SE, actual[0]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[1]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[2]);
        CHECK_EQUAL(vshogi::DIR_NW, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[4]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_HI);
        CHECK_EQUAL(vshogi::DIR_S, actual[0]);
        CHECK_EQUAL(vshogi::DIR_E, actual[1]);
        CHECK_EQUAL(vshogi::DIR_W, actual[2]);
        CHECK_EQUAL(vshogi::DIR_N, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[4]);
    }
    {
        for (auto&& p : {W_KI, W_TO, W_NK, W_NG}) {
            const auto actual = Pieces::get_attack_directions(p);
            CHECK_EQUAL(vshogi::DIR_SE, actual[0]);
            CHECK_EQUAL(vshogi::DIR_S, actual[1]);
            CHECK_EQUAL(vshogi::DIR_SW, actual[2]);
            CHECK_EQUAL(vshogi::DIR_E, actual[3]);
            CHECK_EQUAL(vshogi::DIR_W, actual[4]);
            CHECK_EQUAL(vshogi::DIR_N, actual[5]);
            CHECK_EQUAL(vshogi::DIR_NA, actual[6]);
        }
    }
    {
        for (auto&& p : {W_OU, W_UM, W_RY}) {
            const auto actual = Pieces::get_attack_directions(p);
            CHECK_EQUAL(vshogi::DIR_SE, actual[0]);
            CHECK_EQUAL(vshogi::DIR_S, actual[1]);
            CHECK_EQUAL(vshogi::DIR_SW, actual[2]);
            CHECK_EQUAL(vshogi::DIR_E, actual[3]);
            CHECK_EQUAL(vshogi::DIR_W, actual[4]);
            CHECK_EQUAL(vshogi::DIR_NE, actual[5]);
            CHECK_EQUAL(vshogi::DIR_N, actual[6]);
            CHECK_EQUAL(vshogi::DIR_NW, actual[7]);
            CHECK_EQUAL(vshogi::DIR_NA, actual[8]);
        }
    }
}

} // namespace test_vshogi::test_judkins_shogi
