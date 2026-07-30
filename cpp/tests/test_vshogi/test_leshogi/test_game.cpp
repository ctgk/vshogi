#include <set>

#include "vshogi/common/notation.hpp"
#include "vshogi/variants/leshogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_leshogi
{

using namespace vshogi::leshogi;
using MT = vshogi::MoveTraits<Parameters>;
using NT = vshogi::Notation<Parameters>;

TEST_GROUP (test_leshogi_game) {
};

TEST(test_leshogi_game, dlshogi_policy_size)
{
    CHECK_EQUAL(4 * 6 * (2 * 10 + 7), Config::dlshogi_policy_size);
}

TEST(test_leshogi_game, ply)
{
    auto game = Game();
    CHECK_EQUAL(0, game.ply());
    game.apply(MT::make_move(SQ_1E, SQ_1D))
        .apply(MT::make_move(SQ_4B, SQ_4C))
        .apply(MT::make_move(SQ_1D, SQ_1C));
    CHECK_EQUAL(3, game.ply());
}

TEST(test_leshogi_game, get_board_turn_hash)
{
    auto g1 = Game("3k/4/3G/4/4/4 b G");
    auto g2 = Game("3k/4/3G/4/4/4 b GS");
    CHECK_TRUE(g1.get_zobrist_hash() != g2.get_zobrist_hash());
    CHECK_EQUAL(g1.get_board_turn_hash(), g2.get_board_turn_hash());
}

TEST(test_leshogi_game, hash_with_history)
{
    {
        std::set<vshogi::ZobristHashType> hashes{};
        auto g = Game("3k/4/4/4/3K/4 b -");
        hashes.emplace(g.hash_with_history());
        for (auto m : {"1e1d", "1a1b", "1d1e", "1b1a"}) {
            g.apply(MT::make_move(m));
            hashes.emplace(g.hash_with_history());
        }
        CHECK_EQUAL(5u, hashes.size());
    }
    {
        std::set<vshogi::ZobristHashType> hashes{};
        auto g = Game("4/2K1/4/2k1/4/4 b -");
        hashes.emplace(g.hash_with_history());
        for (auto m : {"2b1b", "2d1d", "1b2b", "1d2d"}) {
            g.apply(MT::make_move(m));
            hashes.emplace(g.hash_with_history());
        }
        CHECK_EQUAL(5u, hashes.size());
    }
    {
        auto g1 = Game("2pS/3P/4/4/4/4 b -");
        g1.apply(MT::make_move("1a2b+")).apply(MT::make_move("2a2b"));
        const auto h1 = g1.hash_with_history();
        auto g2 = Game("2pS/3P/4/4/4/4 b -");
        g2.apply(MT::make_move("1a2b")).apply(MT::make_move("2a2b"));
        const auto h2 = g2.hash_with_history();
        CHECK_EQUAL(h1, h2);
    }
    {
        auto g1 = Game("2gB/4/4/4/3K/4 b -");
        auto g2 = Game("2gB/4/4/4/3K/4 b -");
        CHECK_EQUAL(g1.get_zobrist_hash(), g2.get_zobrist_hash());
        g1.apply(MT::make_move("1a2b")).apply(MT::make_move("2a2b"));
        g2.apply(MT::make_move("1a3c"))
            .apply(MT::make_move("2a3a"))
            .apply(MT::make_move("3c2b"))
            .apply(MT::make_move("3a2b"));
        CHECK_EQUAL(g1.get_zobrist_hash(), g2.get_zobrist_hash());
        CHECK_TRUE(g1.hash_with_history() != g2.hash_with_history());
    }
}

TEST(test_leshogi_game, apply)
{
    {
        auto game = Game();
        game.apply(MT::make_move(SQ_3E, SQ_2D));
        STRCMP_EQUAL(
            "sknl/prbg/4/2B1/G1RP/LNKS w - 2", NT::to_sfen(game).c_str());
    }
}

TEST(test_leshogi_game, undo)
{
    {
        // no promotion no capturing
        auto game = Game();
        game.apply(MT::make_move(SQ_3E, SQ_2D));
        CHECK_EQUAL(1, game.ply());
        game.undo();
        CHECK_EQUAL(0, game.ply());
        STRCMP_EQUAL(NT::to_sfen(Game()).c_str(), NT::to_sfen(game).c_str());
    }
    {
        // undo capturing move
        auto game = Game();
        game.apply(MT::make_move(SQ_2E, SQ_2B));
        CHECK_EQUAL(1, game.get_stand(vshogi::BLACK).count(KA));
        game.undo();
        CHECK_EQUAL(0, game.get_stand(vshogi::BLACK).count(KA));
        STRCMP_EQUAL(NT::to_sfen(Game()).c_str(), NT::to_sfen(game).c_str());
    }
    {
        // undo promotion
        auto game = Game();
        game.apply(MT::make_move(SQ_2E, SQ_2B, true));
        CHECK_EQUAL(B_RY, game.get_board()[SQ_2B]);
        game.undo();
        CHECK_EQUAL(W_KA, game.get_board()[SQ_2B]);
        STRCMP_EQUAL(NT::to_sfen(Game()).c_str(), NT::to_sfen(game).c_str());
    }
    {
        auto game = Game("3k/3r/4/4/3K/4 b P");
        game.apply(MT::make_move(FU, SQ_1C));
        game.undo();
        CHECK_TRUE(game.in_check());
        CHECK_EQUAL(SQ_1B, game.get_state().find_checker_square());
    }
    {
        // undo ignoring check & undo drop move
        auto game = Game("3k/3p/3K/4/4/4 b P");
        game.apply(MT::make_move(FU, SQ_4F));
        CHECK_EQUAL(0, game.get_stand(vshogi::BLACK).count(FU));
        CHECK_EQUAL(vshogi::WHITE_WIN, game.get_result());
        game.undo();
        CHECK_EQUAL(1, game.get_stand(vshogi::BLACK).count(FU));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        CHECK_TRUE(game.in_check());
    }
}

TEST(test_leshogi_game, is_legal)
{
    {
        // Turn: WHITE
        // White: FU
        //     4   3   2   1
        //   *---*---*---*---*
        // A |   |   |   |   |
        //   *---*---*---*---*
        // B |   |-OU|   |   |
        //   *---*---*---*---*
        // C |   |   |-HI|   |
        //   *---*---*---*---*
        // D |   |+OU|   |   |
        //   *---*---*---*---*
        // E |-KI|   |   |   |
        //   *---*---*---*---*
        // Black: -
        auto g = Game("4/1k2/2r1/1K2/g3/4 w p");
        CHECK_TRUE(g.is_legal(MT::make_move(FU, SQ_3C)));
    }
    {
        // Turn: BLACK
        // White: FU
        //     4   3   2   1
        //   *---*---*---*---*
        // A |-KA|-GI|-KI|   |
        //   *---*---*---*---*
        // B |   |   |-OU|-FU|
        //   *---*---*---*---*
        // C |   |   |   |   |
        //   *---*---*---*---*
        // D |+OU|   |   |   |
        //   *---*---*---*---*
        // E |+KI|+GI|+KA|+HI|
        //   *---*---*---*---*
        // Black: -
        auto g = Game("bsg1/2kp/4/4/K3/GSBR b -");
        CHECK_FALSE(g.is_legal(MT::make_move(SQ_1A, SQ_2B)));
    }
    {
        // Turn: BLACK
        // White: -
        //     4   3   2   1
        //   *---*---*---*---*
        // A |   |   |   |   |
        //   *---*---*---*---*
        // B |+OU|-KA|   |-RY|
        //   *---*---*---*---*
        // C |   |   |+KA|   |
        //   *---*---*---*---*
        // D |   |   |   |-KI|
        //   *---*---*---*---*
        // E |   |   |   |   |
        //   *---*---*---*---*
        // Black: -
        auto g = Game("4/Kb1+r/2B1/1g2/4/4 b -");
        CHECK_FALSE(g.is_legal(MT::make_move(SQ_2C, SQ_4E)));
    }
}

TEST(test_leshogi_game, test_is_check)
{
    {
        // standard check
        auto g = Game("3k/3p/4/4/4/K2R b -");
        CHECK_TRUE(g.is_check(MT::make_move("1f1b")));
        CHECK_FALSE(g.is_check(MT::make_move("1f1c")));
    }
    {
        // discovered check
        auto g = Game("3k/4/4/3S/K2R/4 b -");
        CHECK_TRUE(g.is_check(MT::make_move("1d2e")));
        CHECK_TRUE(g.is_check(MT::make_move("1d2c")));
        CHECK_FALSE(g.is_check(MT::make_move("1d1c")));
    }
}

TEST(test_leshogi_game, get_legal_moves)
{
    {
        auto g = Game();
        const auto& actual = g.get_legal_moves();
        CHECK_TRUE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_2E, SQ_2C))
            != actual.cend());
        CHECK_FALSE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_2E, SQ_2C, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_1E, SQ_1C))
            != actual.cend());
    }
    {
        // W: -KAx2
        // +---+---+---+---+
        // |-HI|+NG|   |-OU|
        // +---+---+---+---+
        // |+HI|   |   |   |
        // +---+---+---+---+
        // |+OU|   |   |   |
        // +---+---+---+---+
        // |   |   |   |   |
        // +---+---+---+---+
        // |   |   |   |   |
        // +---+---+---+---+
        // B: +FUx2 +KIx2 +GI
        auto g = Game("r+S1k/R3/K3/4/4/4 b 2b2P2GS 1");
        const auto& actual = g.get_legal_moves();

        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_1C))
            != actual.cend());
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_2A))
            != actual.cend()); // Unmovable
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_1B))
            != actual.cend()); // drop pawn mate

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
                actual.cbegin(), actual.cend(), MT::make_move(SQ_4B, SQ_3A))
            != actual.cend());
        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_4B, SQ_3B))
            != actual.cend()); // discovered check
    }
    {
        // Turn: BLACK
        // White: -
        //     4   3   2   1
        //   *---*---*---*---*
        // A |-KA|-GI|+KA|-OU|
        //   *---*---*---*---*
        // B |   |   |   |   |
        //   *---*---*---*---*
        // C |   |   |   |   |
        //   *---*---*---*---*
        // D |   |   |   |-FU|
        //   *---*---*---*---*
        // E |+KI|+GI|   |+HI|
        //   *---*---*---*---*
        // Black: KI
        auto g = Game("bsBk/4/4/3p/GS1R/4 b G 5");
        const auto& actual = g.get_legal_moves();
        CHECK_TRUE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_2A, SQ_1B, true))
            != actual.cend());
        CHECK_FALSE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_1E, SQ_1D, true))
            != actual.cend());
    }
    {
        // Turn: WHITE
        // White: -
        //     4   3   2   1
        //   *---*---*---*---*
        // A |   |-GI|   |   |
        //   *---*---*---*---*
        // B |   |-KI|   |-FU|
        //   *---*---*---*---*
        // C |   |   |-OU|   |
        //   *---*---*---*---*
        // D |+GI|   |+HI|   |
        //   *---*---*---*---*
        // E |+KI|   |   |+HI|
        //   *---*---*---*---*
        // Black: -
        auto g = Game("1s2/1g1p/2k1/S1R1/G2R/4 w - 10");
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_3A, SQ_2D))
            != actual.cend());
    }
    {
        // Turn: BLACK
        // White: GI
        //     4   3   2   1
        //   *---*---*---*---*
        // A |   |   |-KI|-OU|
        //   *---*---*---*---*
        // B |+FU|-KA|-GI|   |
        //   *---*---*---*---*
        // C |   |-KA|   |   |
        //   *---*---*---*---*
        // D |+HI|   |   |   |
        //   *---*---*---*---*
        // E |+KI|   |   |-RY|
        //   *---*---*---*---*
        // Black: -
        auto g = Game("2gk/Pbs1/1b2/R3/G2+r/4 b s 2");
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(
                actual.cbegin(), actual.cend(), MT::make_move(SQ_4B, SQ_4A))
            != actual.cend());
        CHECK_TRUE(
            std::find(
                actual.cbegin(),
                actual.cend(),
                MT::make_move(SQ_4B, SQ_4A, true))
            != actual.cend());
    }
    {
        // Turn: BLACK
        // White: -
        //     4   3   2   1
        //   *---*---*---*---*
        // A |+HI|   |-KI|-OU|
        //   *---*---*---*---*
        // B |   |   |   |   |
        //   *---*---*---*---*
        // C |   |   |   |+KI|
        //   *---*---*---*---*
        // D |+FU|   |   |   |
        //   *---*---*---*---*
        // E |   |   |   |   |
        //   *---*---*---*---*
        // Black: FU
        auto g = Game("R1gk/4/3G/P3/4/4 b P");
        const auto& actual = g.get_legal_moves();
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_1B))
            != actual.cend()); // drop pawn mate
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_4B))
            != actual.cend()); // two pawns on the same file
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_3A))
            != actual.cend()); // unmovable after drop
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), MT::make_move(FU, SQ_3B))
            != actual.cend());
    }
    {
        // Turn: BLACK
        // White: -
        //     4   3   2   1
        //   *---*---*---*---*
        // A |   |   |   |   |
        //   *---*---*---*---*
        // B |+OU|   |   |   |
        //   *---*---*---*---*
        // C |   |+KA|   |   |
        //   *---*---*---*---*
        // D |   |   |   |   |
        //   *---*---*---*---*
        // E |   |   |   |   |
        //   *---*---*---*---*
        // Black: -
        auto g = Game("4/K3/1B2/4/4/4 b - 1");

        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(
            // clang-format off
            7 // +KA (SQ_1A, SQ_1A+, SQ_2B, SQ_2B+, SQ_4D, SQ_2D, SQ_1E)
            + 4, // +OU (SQ_5A, 4A, 4B, 5C)
            // clang-format on
            actual.size());
    }
    {
        // Turn: WHITE
        // White: -
        //     4   3   2   1
        //   *---*---*---*---*
        // A |   |-GI|   |   |
        //   *---*---*---*---*
        // B |   |-KI|   |-FU|
        //   *---*---*---*---*
        // C |   |   |-OU|   |
        //   *---*---*---*---*
        // D |+GI|   |+HI|   |
        //   *---*---*---*---*
        // E |   |+KI|   |+HI|
        //   *---*---*---*---*
        // Black: -
        auto g = Game("1s2/1g1p/2k1/S1R1/1G1R/4 w - 10");
        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(0, actual.size());
    }
    {
        // Turn: BLACK
        // White: -
        //     4   3   2   1
        //   *---*---*---*---*
        // A |+GI|   |   |   |
        //   *---*---*---*---*
        // B |   |   |   |   |
        //   *---*---*---*---*
        // C |   |   |   |   |
        //   *---*---*---*---*
        // D |   |   |   |   |
        //   *---*---*---*---*
        // E |+OU|   |   |-OU|
        //   *---*---*---*---*
        // Black: -
        auto g = Game("S3/4/4/4/4/K2k b -");
        const auto& actual = g.get_legal_moves();
        CHECK_EQUAL(2 + 3, actual.size());
    }
}

TEST(test_leshogi_game, result)
{
    {
        auto game = Game("3k/4/3K/4/4/4 b G");
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(MT::make_move(KI, SQ_1B));
        CHECK_EQUAL(vshogi::BLACK_WIN, game.get_result());
    }
    {
        auto game = Game();
        game.apply(MT::make_move(SQ_2E, SQ_2D))
            .apply(MT::make_move(SQ_3B, SQ_3C))
            .apply(MT::make_move(SQ_2D, SQ_2E))
            .apply(MT::make_move(SQ_3C, SQ_3B));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result()); // #repeat = 2
        game.apply(MT::make_move(SQ_2E, SQ_2D))
            .apply(MT::make_move(SQ_3B, SQ_3C))
            .apply(MT::make_move(SQ_2D, SQ_2E));
        game.apply(MT::make_move(SQ_3C, SQ_3B));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result()); // #repeat = 3
        game.apply(MT::make_move(SQ_2E, SQ_2D))
            .apply(MT::make_move(SQ_3B, SQ_3C))
            .apply(MT::make_move(SQ_2D, SQ_2E));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(MT::make_move(SQ_3C, SQ_3B)); // #repeat = 4
        CHECK_EQUAL(4, game.count_repetitions());
        CHECK_EQUAL(vshogi::DRAW, game.get_result());
    }
    {
        auto game = Game("1kp1/1R2/K3/4/4/4 w -"); // #repeat = 1
        game.apply(MT::make_move(SQ_3A, SQ_4A));
        game.apply(MT::make_move(SQ_3B, SQ_4B));
        game.apply(MT::make_move(SQ_4A, SQ_3A));
        game.apply(MT::make_move(SQ_4B, SQ_3B)); // #repeat = 2
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(MT::make_move(SQ_3A, SQ_4A));
        game.apply(MT::make_move(SQ_3B, SQ_4B));
        game.apply(MT::make_move(SQ_4A, SQ_3A));
        game.apply(MT::make_move(SQ_4B, SQ_3B)); // #repeat = 3
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(MT::make_move(SQ_3A, SQ_4A));
        game.apply(MT::make_move(SQ_3B, SQ_4B));
        game.apply(MT::make_move(SQ_4A, SQ_3A));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(MT::make_move(SQ_4B, SQ_3B)); // #repeat = 4
        CHECK_EQUAL(vshogi::WHITE_WIN, game.get_result());
    }
    {
        auto game = Game("3k/4/4/4/4/3R b -");
        CHECK_EQUAL(vshogi::BLACK_WIN, game.get_result());
    }
}

} // namespace test_vshogi::test_leshogi
