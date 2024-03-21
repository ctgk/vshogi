#include <algorithm>

#include "vshogi/common/legals.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_legals
{

TEST_GROUP(minishogi_legals){};
TEST_GROUP(judkins_shogi_legals){};
TEST_GROUP(shogi_legals){};

TEST(minishogi_legals, append_legal_moves_by_king)
{
    using namespace vshogi::minishogi;

    {
        auto game = Game();
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_king(actual, game);
        CHECK_EQUAL(1, actual.size());
        CHECK_TRUE(actual[0] == Move(SQ_4D, SQ_5E));
    }
    {
        auto game = Game("5/5/2K2/5/5 b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_king(actual, game);
        CHECK_EQUAL(8, actual.size());
        for (auto&& m :
             {Move(SQ_4B, SQ_3C),
              Move(SQ_3B, SQ_3C),
              Move(SQ_2B, SQ_3C),
              Move(SQ_4C, SQ_3C),
              Move(SQ_2C, SQ_3C),
              Move(SQ_4D, SQ_3C),
              Move(SQ_3D, SQ_3C),
              Move(SQ_2D, SQ_3C)}) {
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
    }
    {
        auto game = Game("5/5/2K2/5/5 w -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_king(actual, game);
        CHECK_EQUAL(0, actual.size());
    }
}

TEST(judkins_shogi_legals, append_legal_moves_by_drop)
{
    using namespace vshogi::judkins_shogi;
    {
        auto game = Game();
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_drop(actual, game);
        CHECK_EQUAL(0, actual.size());
    }
    {
        // Turn: BLACK
        // White: -
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |   |-KE|-OU|
        //   +---+---+---+---+---+---+
        // B |   |+HI|   |   |   |   |
        //   +---+---+---+---+---+---+
        // C |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // D |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // E |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // F |+FU|   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // Black: FU,KE,KI
        auto game = Game("4nk/1R4/6/6/6/P5 b PNG");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_drop(actual, game);
        for (auto&& m : {Move(SQ_4B, FU), Move(SQ_3C, KE), Move(SQ_6A, KI)}) {
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
        for (auto&& m :
             {Move(SQ_3A, FU),
              Move(SQ_6D, FU),
              Move(SQ_1B, FU),
              Move(SQ_4B, KE),
              Move(SQ_2A, KI)}) {
            CHECK_FALSE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
        CHECK_EQUAL((5 * 5 - 2) + (4 * 6 - 1) + (6 * 6 - 4), actual.size());
    }
}

TEST(minishogi_legals, append_legal_moves_by_non_king_forced_promotion)
{
    using namespace vshogi::minishogi;
    auto game = Game("5/4P/5/5/5 b -");
    std::vector<Move> actual;
    vshogi::append_legal_moves_by_non_king(actual, game);
    CHECK_EQUAL(1, actual.size());
    CHECK_TRUE(actual[0] == Move(SQ_1A, SQ_1B, true));
}

TEST(minishogi_legals, append_legal_moves_by_non_king_ally_occupied)
{
    using namespace vshogi::minishogi;
    auto game = Game("4G/4P/5/5/5 b -");
    std::vector<Move> actual;
    vshogi::append_legal_moves_by_non_king(actual, game);
    CHECK_EQUAL(1, actual.size());
    CHECK_TRUE(actual[0] == Move(SQ_2A, SQ_1A, false));
}

TEST(minishogi_legals, append_legal_moves_by_non_king_hidden_attacker)
{
    using namespace vshogi::minishogi;
    {
        auto game = Game("b4/5/2S2/5/4K b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_non_king(actual, game);
        for (auto&& m : {
                 Move(SQ_4B, SQ_3C),
                 Move(SQ_2D, SQ_3C),
             }) {
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
        CHECK_EQUAL(2, actual.size());
    }
    {
        auto game = Game("5/5/5/5/1rR1K b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_non_king(actual, game);
        for (auto&& m : {
                 Move(SQ_4E, SQ_3E),
                 Move(SQ_2E, SQ_3E),
             }) {
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
        CHECK_EQUAL(2, actual.size());
    }
}

TEST(minishogi_legals, append_legal_moves_by_non_king)
{
    using namespace vshogi::minishogi;
    {
        auto game = Game("5/5/1pRK1/5/2+p2 b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_non_king(actual, game);
        CHECK_EQUAL(6, actual.size());
        for (auto&& m : {
                 Move(SQ_3A, SQ_3C, false),
                 Move(SQ_3A, SQ_3C, true),
                 Move(SQ_3B, SQ_3C),
                 Move(SQ_4C, SQ_3C),
                 Move(SQ_3D, SQ_3C),
                 Move(SQ_3E, SQ_3C),
             }) {
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
    }
    {
        auto game = Game("2B2/3p1/K4/5/5 b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_non_king(actual, game);
        CHECK_EQUAL(4, actual.size());
        for (auto&& m : {
                 Move(SQ_2B, SQ_3A, false),
                 Move(SQ_2B, SQ_3A, true),
                 Move(SQ_4B, SQ_3A, false),
                 Move(SQ_4B, SQ_3A, true),
             }) {
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
    }
    {
        auto game = Game();
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_non_king(actual, game);
        CHECK_EQUAL(1 + 2 + 3 + 4 + 3, actual.size());
    }
}

TEST(minishogi_legals, append_legal_moves_by_non_king_moving_to)
{
    using namespace vshogi::minishogi;
    {
        auto game = Game("5/5/5/5/5 b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_non_king_moving_to(actual, game, SQ_1A);
        CHECK_EQUAL(0, actual.size());
    }
    {
        auto game = Game("5/5/4R/5/3K1 b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_non_king_moving_to(actual, game, SQ_1E);
        CHECK_EQUAL(1, actual.size());
    }
}

TEST(minishogi_legals, append_legal_moves_defending_king_dropping_to)
{
    using namespace vshogi::minishogi;
    {
        auto game = Game("5/5/5/5/5 b RBGSP");
        std::vector<Move> actual;
        vshogi::append_legal_moves_defending_king_dropping_to(
            actual, game, SQ_1A);
        CHECK_EQUAL(4, actual.size());
    }
    {
        auto game = Game("5/5/5/5/5 b RBGSP");
        std::vector<Move> actual;
        vshogi::append_legal_moves_defending_king_dropping_to(
            actual, game, SQ_1B);
        CHECK_EQUAL(5, actual.size());
    }
}

TEST(minishogi_legals, append_legal_moves_to_defend_king)
{
    using namespace vshogi::minishogi;
    {
        auto game = Game("5/5/5/R3p/4K b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_to_defend_king(actual, game);
        CHECK_EQUAL(1, actual.size());
    }
    {
        auto game = Game("b4/5/5/5/1P2K b P");
        std::vector<Move> actual;
        vshogi::append_legal_moves_to_defend_king(actual, game);
        for (auto&& m : {
                 Move(SQ_3C, FU),
                 Move(SQ_2D, FU),
             }) {
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
        CHECK_EQUAL(2, actual.size());
    }
}

TEST(minishogi_legals, append_check_moves_moving_to)
{
    using namespace vshogi::minishogi;
    auto game = Game("kB2r/4R/P4/5/4K b -");
    std::vector<Move> actual;
    vshogi::append_check_moves_moving_to(actual, game, SQ_5B);
    for (auto&& m : {
             Move(SQ_5B, SQ_4A, true), // forced promotion
             Move(SQ_5B, SQ_5C),
         }) {
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
    }
    CHECK_EQUAL(2, actual.size());

    // pinned
    CHECK_FALSE(
        std::find(actual.cbegin(), actual.cend(), Move(SQ_5B, SQ_1B))
        != actual.cend());
}

TEST(shogi_legals, append_check_moves_moving_to_force_no_promotion)
{
    using namespace vshogi::shogi;
    auto game = Game("4k4/9/9/9/4N4/9/9/9/9 b -");
    std::vector<Move> actual;
    vshogi::append_check_moves_moving_to(actual, game, SQ_4C);
    CHECK_EQUAL(1, actual.size());
    CHECK_TRUE(Move(SQ_4C, SQ_5E, false) == actual[0]);
}

TEST(minishogi_legals, append_counter_check_moves_dropping_to)
{
    using namespace vshogi::minishogi;
    auto game = Game("b4/5/k4/5/4K b RBGSP");
    std::vector<Move> actual;
    vshogi::append_counter_check_moves_dropping_to(actual, game, SQ_4B);
    CHECK_EQUAL(2, actual.size());
    for (auto&& m : {
             Move(SQ_4B, KA),
             Move(SQ_4B, GI),
         }) {
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
    }
}

TEST(minishogi_legals, get_counter_check_moves_non_ranging)
{
    using namespace vshogi::minishogi;
    auto game = Game("2k2/5/4p/4K/2B2 b -");
    const auto actual = vshogi::get_counter_check_moves(game);
    CHECK_EQUAL(1, actual.size());
    CHECK_TRUE(Move(SQ_1C, SQ_3E) == actual[0]);
}

TEST(minishogi_legals, get_counter_check_moves_ranging_check)
{
    using namespace vshogi::minishogi;
    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |-KA|   |-OU|   |   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |+HI|   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |   |   |+OU|
    //   *---*---*---*---*---*
    // Black: KA
    auto game = Game("b1k2/5/1R3/5/4K b B");
    const auto actual = vshogi::get_counter_check_moves(game);
    CHECK_EQUAL(2, actual.size());
    for (auto&& m : {
             Move(SQ_4B, KA), // drop move
             Move(SQ_3C, SQ_4C), // board move
         }) {
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
    }
}

TEST(minishogi_legals, append_check_moves_by_drop_drop_pawn_mate)
{
    using namespace vshogi::minishogi;
    auto game = Game("3rk/R4/5/5/5 b P");
    std::vector<Move> actual;
    vshogi::append_check_moves_by_drop(actual, game);
    CHECK_EQUAL(0, actual.size());
}

TEST(minishogi_legals, append_check_moves_by_drop_two_pawn_in_a_file)
{
    using namespace vshogi::minishogi;
    auto game = Game("4k/5/5/4P/5 b P");
    std::vector<Move> actual;
    vshogi::append_check_moves_by_drop(actual, game);
    CHECK_EQUAL(0, actual.size());
}

TEST(minishogi_legals, append_check_moves_by_drop_ranging_piece)
{
    using namespace vshogi::minishogi;
    auto game = Game("4k/5/5/4p/5 b R");
    std::vector<Move> actual;
    vshogi::append_check_moves_by_drop(actual, game);
    CHECK_EQUAL(4 + 2, actual.size());
    for (auto&& m : {
             Move(SQ_5A, HI),
             Move(SQ_1C, HI),
         }) {
        CHECK_TRUE(
            std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
    }
    for (auto&& m : {
             Move(SQ_1D, HI),
             Move(SQ_1E, HI),
         }) {
        CHECK_FALSE(
            std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
    }
}

TEST(minishogi_legals, append_check_moves_by_drop)
{
    using namespace vshogi::minishogi;
    auto game = Game("5/5/2k2/5/5 b RBGSP");
    std::vector<Move> actual;
    vshogi::append_check_moves_by_drop(actual, game);
    CHECK_EQUAL(8 + 8 + 6 + 5 + 1, actual.size());
}

} // namespace test_vshogi::test_legals
