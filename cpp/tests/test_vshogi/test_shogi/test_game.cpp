#include "vshogi/shogi/game.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_shogi/test_shogi.hpp"

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(shogi_game){};

TEST(shogi_game, num_dlshogi_policy)
{
    CHECK_EQUAL(9 * 9 * (2 * 10 + 7), Game::num_dlshogi_policy());
}

TEST(shogi_game, record_length)
{
    auto game = Game();
    CHECK_EQUAL(0, game.record_length());
    game.apply(Move(SQ_2F, SQ_2G));
    CHECK_EQUAL(1, game.record_length());
}

TEST(shogi_game, apply)
{
    {
        auto game = Game();
        game.apply(Move(SQ_2F, SQ_2G)).apply(Move(SQ_8D, SQ_8C));
        STRCMP_EQUAL(
            "lnsgkgsnl/1r5b1/p1ppppppp/1p7/9/7P1/PPPPPPP1P/1B5R1/LNSGKGSNL b - "
            "3",
            game.to_sfen().c_str());
    }
}

TEST(shogi_game, result)
{
    {
        auto game = Game();
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    }
    {
        auto game = Game("8k/8G/8G/9/9/9/9/9/8K w -");
        CHECK_EQUAL(vshogi::BLACK_WIN, game.get_result());
    }
    {
        auto game = Game();
        game.apply(Move(SQ_5H, SQ_5I))
            .apply(Move(SQ_5B, SQ_5A))
            .apply(Move(SQ_5I, SQ_5H))
            .apply(Move(SQ_5A, SQ_5B)); // #repeat=2
        game.apply(Move(SQ_5H, SQ_5I))
            .apply(Move(SQ_5B, SQ_5A))
            .apply(Move(SQ_5I, SQ_5H))
            .apply(Move(SQ_5A, SQ_5B)); // #repeat=3
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_5H, SQ_5I))
            .apply(Move(SQ_5B, SQ_5A))
            .apply(Move(SQ_5I, SQ_5H))
            .apply(Move(SQ_5A, SQ_5B)); // #repeat=4
        CHECK_EQUAL(vshogi::DRAW, game.get_result());
    }
}

TEST(shogi_game, get_legal_moves)
{
    {
        auto g = Game("8+L/8g/9/9/4k4/9/9/2K6/9 w 2br10PR");
        const auto& actual = g.get_legal_moves();
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1A, SQ_1B))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_2A, KA))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1A, KA))
            != actual.cend());
    }
    {
        // Turn: WHITE
        // White: FUx10
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |   |   |   |   |   |   |   |   |-OU|
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |   |   |   |   |   |   |-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |   |   |-TO|   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |+OU|   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // Black: -
        auto g = Game("8k/8p/9/9/9/9/9/4+p4/K8 w 10p");
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1F, FU))
            != actual.cend()); // two pawns on the same file
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_2F, FU))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_2I, FU))
            != actual.cend()); // unmovable after drop
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5G, FU))
            != actual.cend());
    }
    {
        auto g = Game("9/9/9/9/4k4/9/9/2K6/9 b -");
        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(8, actual.size());
    }
}

} // namespace test_vshogi::test_shogi
