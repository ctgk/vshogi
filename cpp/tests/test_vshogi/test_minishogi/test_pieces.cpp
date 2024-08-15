#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(minishogi_pieces){};

TEST(minishogi_pieces, is_promotable)
{
    CHECK_TRUE(Pieces::is_promotable(FU));
    CHECK_TRUE(Pieces::is_promotable(GI));
    CHECK_TRUE(Pieces::is_promotable(KA));
    CHECK_TRUE(Pieces::is_promotable(HI));
    CHECK_FALSE(Pieces::is_promotable(KI));
    CHECK_FALSE(Pieces::is_promotable(OU));
    CHECK_FALSE(Pieces::is_promotable(TO));
    CHECK_FALSE(Pieces::is_promotable(NG));
    CHECK_FALSE(Pieces::is_promotable(UM));
    CHECK_FALSE(Pieces::is_promotable(RY));
    CHECK_FALSE(Pieces::is_promotable(NA));
}

TEST(minishogi_pieces, is_promoted)
{
    CHECK_FALSE(Pieces::is_promoted(FU));
    CHECK_FALSE(Pieces::is_promoted(GI));
    CHECK_FALSE(Pieces::is_promoted(KA));
    CHECK_FALSE(Pieces::is_promoted(HI));
    CHECK_FALSE(Pieces::is_promoted(KI));
    CHECK_FALSE(Pieces::is_promoted(OU));
    CHECK_TRUE(Pieces::is_promoted(TO));
    CHECK_TRUE(Pieces::is_promoted(NG));
    CHECK_TRUE(Pieces::is_promoted(UM));
    CHECK_TRUE(Pieces::is_promoted(RY));
    CHECK_TRUE(Pieces::is_promoted(NA));

    CHECK_FALSE(Pieces::is_promoted(B_FU));
    CHECK_FALSE(Pieces::is_promoted(B_GI));
    CHECK_FALSE(Pieces::is_promoted(B_KA));
    CHECK_FALSE(Pieces::is_promoted(B_HI));
    CHECK_FALSE(Pieces::is_promoted(B_KI));
    CHECK_FALSE(Pieces::is_promoted(B_OU));
    CHECK_TRUE(Pieces::is_promoted(B_TO));
    CHECK_TRUE(Pieces::is_promoted(B_NG));
    CHECK_TRUE(Pieces::is_promoted(B_UM));
    CHECK_TRUE(Pieces::is_promoted(B_RY));

    CHECK_FALSE(Pieces::is_promoted(W_FU));
    CHECK_FALSE(Pieces::is_promoted(W_GI));
    CHECK_FALSE(Pieces::is_promoted(W_KA));
    CHECK_FALSE(Pieces::is_promoted(W_HI));
    CHECK_FALSE(Pieces::is_promoted(W_KI));
    CHECK_FALSE(Pieces::is_promoted(W_OU));
    CHECK_TRUE(Pieces::is_promoted(W_TO));
    CHECK_TRUE(Pieces::is_promoted(W_NG));
    CHECK_TRUE(Pieces::is_promoted(W_UM));
    CHECK_TRUE(Pieces::is_promoted(W_RY));
}

TEST(minishogi_pieces, promote_nocheck)
{
    CHECK_EQUAL(TO, Pieces::promote_nocheck(FU));
    CHECK_EQUAL(NG, Pieces::promote_nocheck(GI));
    CHECK_EQUAL(UM, Pieces::promote_nocheck(KA));
    CHECK_EQUAL(RY, Pieces::promote_nocheck(HI));
    // CHECK_EQUAL(TO, Pieces::promote_nocheck(TO));
    // CHECK_EQUAL(NG, Pieces::promote_nocheck(NG));
    // CHECK_EQUAL(UM, Pieces::promote_nocheck(UM));
    // CHECK_EQUAL(RY, Pieces::promote_nocheck(RY));
}

TEST(minishogi_pieces, demote)
{
    CHECK_EQUAL(FU, Pieces::demote(FU));
    CHECK_EQUAL(GI, Pieces::demote(GI));
    CHECK_EQUAL(KA, Pieces::demote(KA));
    CHECK_EQUAL(HI, Pieces::demote(HI));
    CHECK_EQUAL(KI, Pieces::demote(KI));
    CHECK_EQUAL(FU, Pieces::demote(TO));
    CHECK_EQUAL(GI, Pieces::demote(NG));
    CHECK_EQUAL(KA, Pieces::demote(UM));
    CHECK_EQUAL(HI, Pieces::demote(RY));
}

TEST(minishogi_pieces, get_color)
{
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_FU));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_GI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KA));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_HI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_KI));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_OU));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_TO));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_NG));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_UM));
    CHECK_EQUAL(vshogi::BLACK, Pieces::get_color(B_RY));

    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_FU));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_GI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KA));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_HI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_KI));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_OU));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_TO));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_NG));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_UM));
    CHECK_EQUAL(vshogi::WHITE, Pieces::get_color(W_RY));
}

TEST(minishogi_pieces, to_piece_type)
{
    CHECK_EQUAL(FU, Pieces::to_piece_type(B_FU));
    CHECK_EQUAL(GI, Pieces::to_piece_type(B_GI));
    CHECK_EQUAL(KA, Pieces::to_piece_type(B_KA));
    CHECK_EQUAL(HI, Pieces::to_piece_type(B_HI));
    CHECK_EQUAL(KI, Pieces::to_piece_type(B_KI));
    CHECK_EQUAL(OU, Pieces::to_piece_type(B_OU));
    CHECK_EQUAL(TO, Pieces::to_piece_type(B_TO));
    CHECK_EQUAL(NG, Pieces::to_piece_type(B_NG));
    CHECK_EQUAL(UM, Pieces::to_piece_type(B_UM));
    CHECK_EQUAL(RY, Pieces::to_piece_type(B_RY));

    CHECK_EQUAL(FU, Pieces::to_piece_type(W_FU));
    CHECK_EQUAL(GI, Pieces::to_piece_type(W_GI));
    CHECK_EQUAL(KA, Pieces::to_piece_type(W_KA));
    CHECK_EQUAL(HI, Pieces::to_piece_type(W_HI));
    CHECK_EQUAL(KI, Pieces::to_piece_type(W_KI));
    CHECK_EQUAL(OU, Pieces::to_piece_type(W_OU));
    CHECK_EQUAL(TO, Pieces::to_piece_type(W_TO));
    CHECK_EQUAL(NG, Pieces::to_piece_type(W_NG));
    CHECK_EQUAL(UM, Pieces::to_piece_type(W_UM));
    CHECK_EQUAL(RY, Pieces::to_piece_type(W_RY));

    CHECK_EQUAL(FU, Pieces::to_piece_type('p'));
    CHECK_EQUAL(GI, Pieces::to_piece_type('s'));
    CHECK_EQUAL(KA, Pieces::to_piece_type('b'));
    CHECK_EQUAL(HI, Pieces::to_piece_type('r'));
    CHECK_EQUAL(KI, Pieces::to_piece_type('g'));
    CHECK_EQUAL(OU, Pieces::to_piece_type('k'));
    CHECK_EQUAL(NA, Pieces::to_piece_type('a'));

    CHECK_EQUAL(FU, Pieces::to_piece_type('P'));
    CHECK_EQUAL(GI, Pieces::to_piece_type('S'));
    CHECK_EQUAL(KA, Pieces::to_piece_type('B'));
    CHECK_EQUAL(HI, Pieces::to_piece_type('R'));
    CHECK_EQUAL(KI, Pieces::to_piece_type('G'));
    CHECK_EQUAL(OU, Pieces::to_piece_type('K'));
    CHECK_EQUAL(NA, Pieces::to_piece_type('A'));
}

TEST(minishogi_pieces, to_board_piece)
{
    CHECK_EQUAL(B_FU, Pieces::to_board_piece(vshogi::BLACK, FU));
    CHECK_EQUAL(B_GI, Pieces::to_board_piece(vshogi::BLACK, GI));
    CHECK_EQUAL(B_KA, Pieces::to_board_piece(vshogi::BLACK, KA));
    CHECK_EQUAL(B_HI, Pieces::to_board_piece(vshogi::BLACK, HI));
    CHECK_EQUAL(B_KI, Pieces::to_board_piece(vshogi::BLACK, KI));
    CHECK_EQUAL(B_OU, Pieces::to_board_piece(vshogi::BLACK, OU));
    CHECK_EQUAL(B_TO, Pieces::to_board_piece(vshogi::BLACK, TO));
    CHECK_EQUAL(B_NG, Pieces::to_board_piece(vshogi::BLACK, NG));
    CHECK_EQUAL(B_UM, Pieces::to_board_piece(vshogi::BLACK, UM));
    CHECK_EQUAL(B_RY, Pieces::to_board_piece(vshogi::BLACK, RY));

    CHECK_EQUAL(W_FU, Pieces::to_board_piece(vshogi::WHITE, FU));
    CHECK_EQUAL(W_GI, Pieces::to_board_piece(vshogi::WHITE, GI));
    CHECK_EQUAL(W_KA, Pieces::to_board_piece(vshogi::WHITE, KA));
    CHECK_EQUAL(W_HI, Pieces::to_board_piece(vshogi::WHITE, HI));
    CHECK_EQUAL(W_KI, Pieces::to_board_piece(vshogi::WHITE, KI));
    CHECK_EQUAL(W_OU, Pieces::to_board_piece(vshogi::WHITE, OU));
    CHECK_EQUAL(W_TO, Pieces::to_board_piece(vshogi::WHITE, TO));
    CHECK_EQUAL(W_NG, Pieces::to_board_piece(vshogi::WHITE, NG));
    CHECK_EQUAL(W_UM, Pieces::to_board_piece(vshogi::WHITE, UM));
    CHECK_EQUAL(W_RY, Pieces::to_board_piece(vshogi::WHITE, RY));

    CHECK_EQUAL(VOID, Pieces::to_board_piece(vshogi::BLACK, NA));
    CHECK_EQUAL(VOID, Pieces::to_board_piece(vshogi::WHITE, NA));
}

TEST(minishogi_pieces, to_char)
{
    CHECK_EQUAL('p', Pieces::to_char(FU));
    CHECK_EQUAL('s', Pieces::to_char(GI));
    CHECK_EQUAL('b', Pieces::to_char(KA));
    CHECK_EQUAL('r', Pieces::to_char(HI));
    CHECK_EQUAL('g', Pieces::to_char(KI));
    CHECK_EQUAL('k', Pieces::to_char(OU));
}

TEST(minishogi_pieces, append_sfen)
{
    // clang-format off
    {auto actual = std::string(); Pieces::append_sfen(B_FU, actual); STRCMP_EQUAL("P", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_GI, actual); STRCMP_EQUAL("S", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_KA, actual); STRCMP_EQUAL("B", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_HI, actual); STRCMP_EQUAL("R", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_KI, actual); STRCMP_EQUAL("G", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_OU, actual); STRCMP_EQUAL("K", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_TO, actual); STRCMP_EQUAL("+P", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_NG, actual); STRCMP_EQUAL("+S", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_UM, actual); STRCMP_EQUAL("+B", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(B_RY, actual); STRCMP_EQUAL("+R", actual.c_str());}

    {auto actual = std::string(); Pieces::append_sfen(W_FU, actual); STRCMP_EQUAL("p", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_GI, actual); STRCMP_EQUAL("s", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_KA, actual); STRCMP_EQUAL("b", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_HI, actual); STRCMP_EQUAL("r", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_KI, actual); STRCMP_EQUAL("g", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_OU, actual); STRCMP_EQUAL("k", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_TO, actual); STRCMP_EQUAL("+p", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_NG, actual); STRCMP_EQUAL("+s", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_UM, actual); STRCMP_EQUAL("+b", actual.c_str());}
    {auto actual = std::string(); Pieces::append_sfen(W_RY, actual); STRCMP_EQUAL("+r", actual.c_str());}
    // clang-format on
}

TEST(minishogi_pieces, get_attack_directions)
{
    {
        const auto actual = Pieces::get_attack_directions(B_FU);
        CHECK_EQUAL(vshogi::DIR_N, actual[0]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[1]);
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
        for (auto&& p : {B_KI, B_TO, B_NG}) {
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
        for (auto&& p : {W_KI, W_TO, W_NG}) {
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

} // namespace test_vshogi::test_minishogi
