#include <algorithm>

#include "vshogi/judkins_shogi/state.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_judkins_shogi/test_judkins_shogi.hpp"

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP(state){};

TEST(state, set_sfen)
{
    {
        auto s = State();
        // W: -KAx2
        // +---+---+---+---+---+---+
        // |   |   |+NG|   |-OU|   |
        // +---+---+---+---+---+---+
        // |   |-HI|   |   |+TO|   |
        // +---+---+---+---+---+---+
        // |   |   |+OU|   |   |+NK|
        // +---+---+---+---+---+---+
        // |   |   |   |   |-KE|   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // B: +FU +GI +KIx2 +HI
        s.set_sfen("2+S1k1/1r2+P1/2K2+N/4n1/6/6 w R2GSP2b 1");
        CHECK_EQUAL(W_OU, s.get_board()[SQ_2A]);
        CHECK_EQUAL(B_TO, s.get_board()[SQ_2B]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_2C]);
        CHECK_EQUAL(1, s.get_stand(vshogi::BLACK).count(FU));
        CHECK_EQUAL(2, s.get_stand(vshogi::BLACK).count(KI));
        CHECK_EQUAL(0, s.get_stand(vshogi::BLACK).count(KA));
        CHECK_EQUAL(0, s.get_stand(vshogi::WHITE).count(FU));
        CHECK_EQUAL(2, s.get_stand(vshogi::WHITE).count(KA));
        CHECK_EQUAL(vshogi::WHITE, s.get_turn());
    }
}

TEST(state, to_sfen)
{
    auto s = State();
    s.set_sfen("2+S1k1/1r2+P1/2K2+N/4n1/6/6 w R2GSP2b 1");
    const auto actual = s.to_sfen();
    STRCMP_EQUAL("2+S1k1/1r2+P1/2K2+N/4n1/6/6 w R2GSP2b", actual.c_str());
}

TEST(state, apply)
{
    {
        auto s = State();
        CHECK_EQUAL(VOID, s.get_board()[SQ_6D]);
        CHECK_EQUAL(B_FU, s.get_board()[SQ_6E]);
        s.apply(Move(SQ_6D, SQ_6E));
        CHECK_EQUAL(B_FU, s.get_board()[SQ_6D]);
        CHECK_EQUAL(VOID, s.get_board()[SQ_6E]);
    }
    {
        auto s = State()
                     .apply(Move(SQ_4D, SQ_3F))
                     .apply(Move(SQ_1C, SQ_1B))
                     .apply(Move(SQ_5B, SQ_4D, true));
        CHECK_EQUAL(B_NK, s.get_board()[SQ_5B]);
    }
}

TEST(state, get_legal_moves)
{
    {
        auto s = State();
        auto actual = std::vector<Move>();
        s.get_legal_moves(actual);
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_6D, SQ_6E))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4D, SQ_3F))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1A, SQ_1F))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4D, SQ_3F, true))
            != actual.cend());
    }
    {
        auto s = State();
        // W:
        // +---+---+---+---+---+---+
        // |   |   |-HI|+KI|   |-OU|
        // +---+---+---+---+---+---+
        // |   |   |+HI|   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |+OU|   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // B: +FUx1
        s.set_sfen("2rG1k/2R3/2K3/6/6/6 b P");
        auto actual = std::vector<Move>();
        s.get_legal_moves(actual);
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1C, FU))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1B, FU))
            != actual.cend()); // drop pawn mate
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_6A, FU))
            != actual.cend()); // unmovable

        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4A, SQ_4B))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4A, SQ_4B, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1B, SQ_4B))
            != actual.cend()); // pinned
    }
    {
        auto s = State();
        // W:
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |-OU|
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |-KE|   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |-FU|   |-RY|-KA|
        // +---+---+---+---+---+---+
        // |+OU|   |   |   |   |   |
        // +---+---+---+---+---+---+
        // B:
        s.set_sfen("5k/6/2n3/6/2p1+rb/K5 w -");
        auto actual = std::vector<Move>();
        s.get_legal_moves(actual);
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5A, SQ_1E))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5A, SQ_1E, true))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_2F, SQ_1E))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_2F, SQ_1E, true))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_2A, SQ_2E))
            != actual.cend());
        // promotion of promoted piece
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_2A, SQ_2E, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1E, SQ_2E))
            != actual.cend());

        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_3E, SQ_4C))
            != actual.cend()); // unmovable
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_3E, SQ_4C, true))
            != actual.cend());

        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4F, SQ_4E))
            != actual.cend()); // unmovable
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4F, SQ_4E, true))
            != actual.cend());
    }

    {
        auto s = State();
        // W:
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |+FU|
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // B:
        s.set_sfen("6/5P/6/6/6/6 b -");
        auto actual = std::vector<Move>();
        s.get_legal_moves(actual);
        CHECK_EQUAL(1, actual.size());
        CHECK_TRUE(Move(SQ_1A, SQ_1B, true) == actual[0]);
    }
    {
        auto s = State();
        // W: KE
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // |   |   |   |   |   |   |
        // +---+---+---+---+---+---+
        // B:
        s.set_sfen("6/6/6/6/6/6 w n");
        auto actual = std::vector<Move>();
        s.get_legal_moves(actual);
        CHECK_EQUAL(6 * 4, actual.size());
        for (auto a : actual)
            CHECK_TRUE(KE == a.source_piece());
    }
}

} // namespace test_vshogi::test_judkins_shogi
