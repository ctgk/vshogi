#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (shogi_game) {
};

TEST(shogi_game, feature_channels)
{
    CHECK_EQUAL(2 * (14 + 7), Game::feature_channels());
}

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

TEST(shogi_game, is_legal)
{
    // Turn: WHITE
    // White: FU
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |   |   |   |-OU|   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // C |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |   |   |   |   |   |   |   |-KI|   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |   |   |   |   |-HI|+FU|+OU|+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: -
    auto g = Game("3k5/9/9/9/9/9/7g1/9/5rPKL w p");
    CHECK_FALSE(g.is_legal(Move(SQ_2H, FU)));
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
    {
        // Turn: BLACK
        // White: FUx2
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |-GI|-KY|-KI|   |   |   |   |-KY|-KA|
        //   +---+---+---+---+---+---+---+---+---+
        // B |-KI|-HI|   |-OU|   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |-FU|-FU|-FU|   |-GI|   |   |-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |   |   |-FU|   |-FU|-KE|
        //   +---+---+---+---+---+---+---+---+---+
        // E |-FU|   |   |   |-FU|-KE|   |+FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |+KI|+FU|+FU|   |   |   |-FU|   |+FU|
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |   |   |+FU|   |+FU|+FU|+GI|   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |   |   |   |+OU|   |+KY|+KI|
        //   +---+---+---+---+---+---+---+---+---+
        // I |+KE|+KY|+HI|+KE|   |+GI|   |   |+KA|
        //   +---+---+---+---+---+---+---+---+---+
        // Black: -
        constexpr char sfen[] = "slg4lb/gr1k5/1ppp1s2p/5p1pn/p3pn1P1/GPP3p1P/"
                                "3P1PPS1/5K1LG/NLRN1S2B b 2p";
        auto g = Game(sfen);
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5G, SQ_4H))
            != actual.cend());
    }
}

} // namespace test_vshogi::test_shogi
