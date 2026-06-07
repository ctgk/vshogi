#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;
using MT = vshogi::MoveTraits<Parameters>;

TEST_GROUP (judkins_shogi_game) {
};

TEST(judkins_shogi_game, dlshogi_policy_size)
{
    CHECK_EQUAL(6 * 6 * (2 * 10 + 6), Config::dlshogi_policy_size);
}

TEST(judkins_shogi_game, ply)
{
    auto game = Game();
    CHECK_EQUAL(0, game.ply());
    game.apply(MT::make_move(SQ_1F, SQ_1B));
    CHECK_EQUAL(1, game.ply());
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
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_6E, SQ_6D))
            != actual.cend());
        CHECK_TRUE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_3F, SQ_4D))
            != actual.cend());
        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_1F, SQ_1A))
            != actual.cend());
        CHECK_FALSE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_3F, SQ_4D, true))
            != actual.cend());
    }
    {
        auto game = Game();
        game.apply(MT::make_move(SQ_2F, SQ_4D));
        CHECK_EQUAL(4, game.get_legal_moves().size());
    }
    {
        auto game = Game("4gk/5p/6/6/P5/KGSNBR b rbns");
        game.apply(MT::make_move(SQ_2F, SQ_4D));
        CHECK_EQUAL(9, game.get_legal_moves().size());
        game.apply(MT::make_move(GI, SQ_2B));
        game.apply(MT::make_move(SQ_4D, SQ_2B, true));
        CHECK_EQUAL(2, game.get_legal_moves().size());
    }
    {
        auto game = Game("6/4gk/6/5B/6/K4R b p");
        game.apply(MT::make_move(SQ_1D, SQ_2C));
        CHECK_EQUAL(2, game.get_legal_moves().size());
    }
    {
        auto game = Game("6/4gk/6/5B/6/K4R b p");
        game.apply(MT::make_move(SQ_1D, SQ_3B, true));
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
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_1C))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_1B))
            != actual.cend()); // drop pawn mate
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_6A))
            != actual.cend()); // unmovable

        CHECK_TRUE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_4B, SQ_4A))
            != actual.cend());
        CHECK_TRUE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_4B, SQ_4A, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_4B, SQ_1B))
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
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_1E, SQ_5A))
            != actual.cend());
        CHECK_TRUE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_1E, SQ_5A, true))
            != actual.cend());
        CHECK_TRUE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_1E, SQ_2F))
            != actual.cend());
        CHECK_TRUE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_1E, SQ_2F, true))
            != actual.cend());
        CHECK_TRUE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_2E, SQ_2A))
            != actual.cend());
        // promotion of promoted piece
        CHECK_FALSE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_2E, SQ_2A, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_2E, SQ_1E))
            != actual.cend());

        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_4C, SQ_3E))
            != actual.cend()); // unmovable
        CHECK_TRUE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_4C, SQ_3E, true))
            != actual.cend());

        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_4E, SQ_4F))
            != actual.cend()); // unmovable
        CHECK_TRUE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_4E, SQ_4F, true))
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
        CHECK_TRUE(MT::make_move(SQ_1B, SQ_1A, true) == actual[0]);
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
            CHECK_TRUE(KE == MT::get_src_pt(a));
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
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_2F, SQ_1D))
            != actual.cend());
    }
}

} // namespace test_vshogi::test_judkins_shogi
