#include "vshogi/minishogi/game.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_minishogi/test_minishogi.hpp"

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(game){};

TEST(game, num_dlshogi_policy)
{
    CHECK_EQUAL(5 * 5 * (2 * 8 + 5), Game::num_dlshogi_policy());
}

TEST(game, record_length)
{
    auto game = Game();
    CHECK_EQUAL(0, game.record_length());
    game.apply(Move(SQ_3D, SQ_2E))
        .apply(Move(SQ_3B, SQ_2A))
        .apply(Move(SQ_1B, SQ_1E));
    CHECK_EQUAL(3, game.record_length());
}

TEST(game, apply)
{
    {
        auto game = Game();
        game.apply(Move(SQ_3D, SQ_4E));
        STRCMP_EQUAL("rbsgk/4p/5/P1G2/K1SBR w - 2", game.to_sfen().c_str());
    }
}

TEST(game, result)
{
    {
        auto game = Game();
        game.apply(Move(SQ_4C, SQ_2E))
            .apply(Move(SQ_1C, SQ_1B))
            .apply(Move(SQ_2E, SQ_1E))
            .apply(Move(SQ_1D, SQ_1C));
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

TEST(game, get_sfen_move_at)
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
