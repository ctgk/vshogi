#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP (judkins_shogi_game) {
};

TEST(judkins_shogi_game, num_dlshogi_policy)
{
    CHECK_EQUAL(6 * 6 * (2 * 10 + 6), Game::num_dlshogi_policy());
}

TEST(judkins_shogi_game, record_length)
{
    auto game = Game();
    CHECK_EQUAL(0, game.record_length());
    game.apply(Move(SQ_1B, SQ_1F));
    CHECK_EQUAL(1, game.record_length());
}

TEST(judkins_shogi_game, result)
{
    {
        auto game = Game();
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    }
    {
        auto game = Game("5k/5G/5P/6/6/K5 w -");
        CHECK_EQUAL(vshogi::BLACK_WIN, game.get_result());
    }
    {
        auto game = Game("5k/6/6/p5/g5/K5 b -");
        CHECK_EQUAL(vshogi::WHITE_WIN, game.get_result());
    }
}

TEST(judkins_shogi_game, get_legal_moves)
{
    {
        auto g = Game();
        const auto& actual = g.get_legal_moves();
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
        auto game = Game();
        game.apply(Move(SQ_4D, SQ_2F));
        CHECK_EQUAL(4, game.get_legal_moves().size());
    }
    {
        auto game = Game("4gk/5p/6/6/P5/KGSNBR b rbns");
        game.apply(Move(SQ_4D, SQ_2F));
        CHECK_EQUAL(9, game.get_legal_moves().size());
        game.apply(Move(SQ_2B, GI));
        game.apply(Move(SQ_2B, SQ_4D, true));
        CHECK_EQUAL(2, game.get_legal_moves().size());
    }
    {
        auto game = Game("6/4gk/6/5B/6/K4R b p");
        game.apply(Move(SQ_2C, SQ_1D));
        CHECK_EQUAL(2, game.get_legal_moves().size());
    }
    {
        auto game = Game("6/4gk/6/5B/6/K4R b p");
        game.apply(Move(SQ_3B, SQ_1D, true));
        CHECK_EQUAL(4, game.get_legal_moves().size());
    }
    {
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
        auto g = Game("2rG1k/2R3/2K3/6/6/6 b P");
        const auto& actual = g.get_legal_moves();
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
        auto g = Game("5k/6/2n3/6/2p1+rb/K5 w -");
        const auto& actual = g.get_legal_moves();
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
        auto g = Game("6/5P/6/6/6/6 b -");
        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(1, actual.size());
        CHECK_TRUE(Move(SQ_1A, SQ_1B, true) == actual[0]);
    }
    {
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
        auto g = Game("6/6/6/6/6/6 w n");
        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(6 * 4, actual.size());
        for (auto a : actual)
            CHECK_TRUE(KE == a.source_piece());
    }
    {
        // Turn: BLACK
        // White: -
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |-HI|-KA|-KE|-GI|   |-RY|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |-KI|   |
        //   +---+---+---+---+---+---+
        // C |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // D |+FU|   |   |   |+KE|-OU|
        //   +---+---+---+---+---+---+
        // E |   |   |+OU|   |+FU|   |
        //   +---+---+---+---+---+---+
        // F |   |+KI|+GI|   |+UM|   |
        //   +---+---+---+---+---+---+
        // Black: -
        const auto g = Game("rbns1+r/4g1/6/P3Nk/2K1P1/1GS1+B1 b -");
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1D, SQ_2F))
            != actual.cend());
    }
}

} // namespace test_vshogi::test_judkins_shogi
