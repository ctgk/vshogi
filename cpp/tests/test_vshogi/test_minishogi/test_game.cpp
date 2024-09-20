#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP (minishogi_game) {
};

TEST(minishogi_game, num_dlshogi_policy)
{
    CHECK_EQUAL(5 * 5 * (2 * 8 + 5), Game::num_dlshogi_policy());
}

TEST(minishogi_game, record_length)
{
    auto game = Game();
    CHECK_EQUAL(0, game.record_length());
    game.apply(Move(SQ_3D, SQ_2E))
        .apply(Move(SQ_3B, SQ_2A))
        .apply(Move(SQ_1B, SQ_1E));
    CHECK_EQUAL(3, game.record_length());
}

TEST(minishogi_game, apply)
{
    {
        auto game = Game();
        game.apply(Move(SQ_3D, SQ_4E));
        STRCMP_EQUAL("rbsgk/4p/5/P1G2/K1SBR w - 2", game.to_sfen().c_str());
    }
}

TEST(minishogi_game, is_legal)
{
    {
        // Turn: WHITE
        // White: FU
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |   |   |
        //   *---*---*---*---*---*
        // B |   |-OU|   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |-HI|   |   |
        //   *---*---*---*---*---*
        // D |   |+OU|   |   |   |
        //   *---*---*---*---*---*
        // E |-KI|   |   |   |   |
        //   *---*---*---*---*---*
        // Black: -
        auto g = Game("5/1k3/2r2/1K3/g4 w p");
        CHECK_TRUE(g.is_legal(Move(SQ_4C, FU)));
    }
    {
        // Turn: BLACK
        // White: FU
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |-HI|-KA|-GI|-KI|   |
        //   *---*---*---*---*---*
        // B |   |   |   |-OU|-FU|
        //   *---*---*---*---*---*
        // C |   |   |   |   |   |
        //   *---*---*---*---*---*
        // D |+FU|+OU|   |   |   |
        //   *---*---*---*---*---*
        // E |   |+KI|+GI|+KA|+HI|
        //   *---*---*---*---*---*
        // Black: -
        auto g = Game("rbsg1/3kp/5/PK3/1GSBR b -");
        CHECK_FALSE(g.is_legal(Move(SQ_2B, SQ_1A)));
    }
    {
        // Turn: WHITE
        // White: FU
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |-HI|-KA|-GI|-KI|   |
        //   *---*---*---*---*---*
        // B |   |   |   |-OU|-FU|
        //   *---*---*---*---*---*
        // C |   |   |   |   |   |
        //   *---*---*---*---*---*
        // D |+FU|+OU|   |   |   |
        //   *---*---*---*---*---*
        // E |   |+KI|+GI|+KA|+HI|
        //   *---*---*---*---*---*
        // Black: -
        auto g = Game("rbsg1/3kp/5/PK3/1GSBR w -");
        CHECK_FALSE(g.is_legal(Move(SQ_2B, SQ_1A)));
    }
}

TEST(minishogi_game, get_legal_moves)
{
    {
        auto g = Game();
        const auto& actual = g.get_legal_moves();
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5C, SQ_5D))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5C, SQ_5D, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5B, SQ_5D))
            != actual.cend());
    }
    {
        // W: -KAx2
        // +---+---+---+---+---+
        // |   |-HI|+NG|   |-OU|
        // +---+---+---+---+---+
        // |   |+HI|   |   |   |
        // +---+---+---+---+---+
        // |   |+OU|   |   |   |
        // +---+---+---+---+---+
        // |   |   |   |   |   |
        // +---+---+---+---+---+
        // |   |   |   |   |   |
        // +---+---+---+---+---+
        // B: +FUx2 +KIx2 +GI
        auto g = Game("1r+S1k/1R3/1K3/5/5 b 2b2P2GS 1");
        const auto& actual = g.get_legal_moves();

        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1C, FU))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5A, FU))
            != actual.cend()); // Unmovable
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1B, FU))
            != actual.cend()); // drop pawn mate

        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4A, SQ_4B))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4A, SQ_4B, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5A, SQ_4B))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_5B, SQ_4B))
            != actual.cend()); // discovered check
    }
    {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |-HI|-KA|-GI|+KA|-OU|
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |   |   |   |
        //   *---*---*---*---*---*
        // D |+FU|   |   |   |-FU|
        //   *---*---*---*---*---*
        // E |+OU|+KI|+GI|   |+HI|
        //   *---*---*---*---*---*
        // Black: KI
        auto g = Game("rbsBk/5/5/P3p/KGS1R b G 5");
        const auto& actual = g.get_legal_moves();
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1B, SQ_2A, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1D, SQ_1E, true))
            != actual.cend());
    }
    {
        // Turn: WHITE
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |+UM|   |-GI|   |   |
        //   *---*---*---*---*---*
        // B |+KA|   |-KI|   |-FU|
        //   *---*---*---*---*---*
        // C |   |   |   |-OU|   |
        //   *---*---*---*---*---*
        // D |+FU|+GI|   |+HI|   |
        //   *---*---*---*---*---*
        // E |+OU|+KI|   |   |+HI|
        //   *---*---*---*---*---*
        // Black: -
        auto g = Game("+B1s2/B1g1p/3k1/PS1R1/KG2R w - 10");
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_2D, SQ_3A))
            != actual.cend());
    }
    {
        // Turn: BLACK
        // White: GI
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |+TO|   |   |-KI|-OU|
        //   *---*---*---*---*---*
        // B |   |+FU|-KA|-GI|   |
        //   *---*---*---*---*---*
        // C |   |   |-KA|   |   |
        //   *---*---*---*---*---*
        // D |   |+HI|   |   |   |
        //   *---*---*---*---*---*
        // E |+OU|+KI|   |   |-RY|
        //   *---*---*---*---*---*
        // Black: -
        auto g = Game("+P2gk/1Pbs1/2b2/1R3/KG2+r b s 2");
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4A, SQ_4B))
            != actual.cend());
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4A, SQ_4B, true))
            != actual.cend());
    }
    {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |+HI|   |   |-KI|-OU|
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |   |   |+KI|
        //   *---*---*---*---*---*
        // D |   |+FU|   |   |   |
        //   *---*---*---*---*---*
        // E |+OU|   |   |   |   |
        //   *---*---*---*---*---*
        // Black: FU
        auto g = Game("R2gk/5/4G/1P3/K4 b P");
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_1B, FU))
            != actual.cend()); // drop pawn mate
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_4B, FU))
            != actual.cend()); // two pawns on the same file
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_3A, FU))
            != actual.cend()); // unmovable after drop
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), Move(SQ_3B, FU))
            != actual.cend());
    }
    {
        // W:
        // +---+---+---+---+---+
        // |+KA|   |   |   |-OU|
        // +---+---+---+---+---+
        // |   |+KI|   |-FU|-HI|
        // +---+---+---+---+---+
        // |+GI|   |   |   |-GI|
        // +---+---+---+---+---+
        // |+HI|+FU|   |-KI|   |
        // +---+---+---+---+---+
        // |+OU|   |   |   |-KA|
        // +---+---+---+---+---+
        // B:
        auto g = Game("B3k/1G1pr/S3s/RP1g1/K3b b - 1");

        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(8, actual.size());
    }
    {
        // W: -FU
        // +---+---+---+---+---+
        // |+KA|   |   |   |-KA|
        // +---+---+---+---+---+
        // |+HI|+KI|   |-OU|   |
        // +---+---+---+---+---+
        // |+GI|+FU|   |   |-GI|
        // +---+---+---+---+---+
        // |   |   |   |   |-HI|
        // +---+---+---+---+---+
        // |+OU|   |-KI|   |   |
        // +---+---+---+---+---+
        // B:
        auto g = Game("B3b/RG1k1/SP2s/4r/K1g2 w p 1");

        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(
            // clang-format off
            4 // -OU (SQ_2A, SQ_1B, SQ_3C, SQ_2C)
            + 1 // -GI (SQ_2D)
            + 6 // -HI (SQ_5D, SQ_4D, SQ_3D, SQ_2D, SQ_1E, SQ_1E+)
            + 3 // -KI (SQ_3D, SQ_4E, SQ_2E)
            + 10, // -FU (41, 31, 21, 32, 12, 33, 23, 44, 34, 24)
            // clang-format on
            actual.size());
    }
    {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |   |-OU|
        //   *---*---*---*---*---*
        // B |+OU|   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |+KA|   |   |   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: -
        auto g = Game("4k/K4/1B3/5/5 b - 1");

        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(
            // clang-format off
            6 // +KA (SQ_2A, SQ_2A+, SQ_3B, SQ_5D, SQ_3D, SQ_2E)
            + 4, // +OU (SQ_5A, 4A, 4B, 5C)
            // clang-format on
            actual.size());
    }
    {
        // Turn: WHITE
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |+UM|   |-GI|   |   |
        //   *---*---*---*---*---*
        // B |+KA|   |-KI|   |-FU|
        //   *---*---*---*---*---*
        // C |   |   |   |-OU|   |
        //   *---*---*---*---*---*
        // D |+FU|+GI|   |+HI|   |
        //   *---*---*---*---*---*
        // E |+OU|+KI|   |   |+HI|
        //   *---*---*---*---*---*
        // Black: -
        auto g = Game("+B1s2/B1g1p/3k1/PS1R1/KG2R w - 10");
        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(0, actual.size());
    }
    {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |+GI|   |   |   |   |
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |   |   |   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |+OU|   |   |   |-OU|
        //   *---*---*---*---*---*
        // Black: -
        auto g = Game("S4/5/5/5/K3k b -");
        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(2 + 3, actual.size());
    }
}

TEST(minishogi_game, result)
{
    {
        auto game = Game();
        game.apply(Move(SQ_4C, SQ_2E));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_1C, SQ_1B));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_2E, SQ_1E));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_1D, SQ_1C));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_2A, SQ_2E, true));
        CHECK_EQUAL(vshogi::BLACK_WIN, game.get_result());
    }
    {
        auto game = Game();
        game.apply(Move(SQ_1D, SQ_1E))
            .apply(Move(SQ_5B, SQ_5A))
            .apply(Move(SQ_1E, SQ_1D))
            .apply(Move(SQ_5A, SQ_5B));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result()); // #repeat = 2
        game.apply(Move(SQ_1D, SQ_1E))
            .apply(Move(SQ_5B, SQ_5A))
            .apply(Move(SQ_1E, SQ_1D))
            .apply(Move(SQ_5A, SQ_5B));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result()); // #repeat = 3
        game.apply(Move(SQ_1D, SQ_1E))
            .apply(Move(SQ_5B, SQ_5A))
            .apply(Move(SQ_1E, SQ_1D));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_5A, SQ_5B)); // #repeat = 4
        CHECK_EQUAL(vshogi::DRAW, game.get_result());
    }
    {
        auto game = Game();
        game.apply(Move(SQ_5C, SQ_5D)).apply(Move(SQ_3B, SQ_2A));
        game.apply(Move(SQ_1B, SQ_1E)); // #repeat = 1
        game.apply(Move(SQ_2A, SQ_1A))
            .apply(Move(SQ_2B, SQ_1B))
            .apply(Move(SQ_1A, SQ_2A))
            .apply(Move(SQ_1B, SQ_2B)); // #repeat = 2
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_2A, SQ_1A))
            .apply(Move(SQ_2B, SQ_1B))
            .apply(Move(SQ_1A, SQ_2A))
            .apply(Move(SQ_1B, SQ_2B)); // #repeat = 3
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_2A, SQ_1A))
            .apply(Move(SQ_2B, SQ_1B))
            .apply(Move(SQ_1A, SQ_2A));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_1B, SQ_2B)); // #repeat = 4
        CHECK_EQUAL(vshogi::WHITE_WIN, game.get_result());
    }
}

TEST(minishogi_game, get_sfen_move_at)
{
    {
        const auto game = Game().apply(Move(SQ_3D, SQ_4E));
        CHECK_TRUE(Move(SQ_3D, SQ_4E) == game.get_move_at(0));
        STRCMP_EQUAL("rbsgk/4p/5/P4/KGSBR b - 1", game.get_sfen_at(0).c_str());
        STRCMP_EQUAL(
            "rbsgk/4p/5/P4/KGSBR b -", game.get_sfen_at(0, false).c_str());
    }
}

} // namespace test_vshogi::test_minishogi
