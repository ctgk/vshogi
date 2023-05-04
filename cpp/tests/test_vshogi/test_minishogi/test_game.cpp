#include "vshogi/minishogi/game.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(game){};

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
        game.apply(Move(SQ_3D, SQ_4E))
            .apply(Move(SQ_3B, SQ_2A))
            .apply(Move(SQ_2C, SQ_3D))
            .apply(Move(SQ_5B, SQ_5A));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_1B, SQ_2C));
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

} // namespace test_vshogi::test_minishogi
