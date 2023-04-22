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
    game.apply(Move(SQ_34, SQ_25))
        .apply(Move(SQ_32, SQ_21))
        .apply(Move(SQ_12, SQ_15));
    CHECK_EQUAL(3, game.record_length());
}

TEST(game, apply)
{
    {
        auto game = Game();
        game.apply(Move(SQ_34, SQ_45));
        STRCMP_EQUAL("rbsgk/4p/5/P1G2/K1SBR w - 2", game.to_sfen().c_str());
    }
}

TEST(game, result)
{
    {
        auto game = Game();
        game.apply(Move(SQ_34, SQ_45))
            .apply(Move(SQ_32, SQ_21))
            .apply(Move(SQ_23, SQ_34))
            .apply(Move(SQ_52, SQ_51));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_12, SQ_23));
        CHECK_EQUAL(vshogi::BLACK_WIN, game.get_result());
    }
    {
        auto game = Game();
        game.apply(Move(SQ_14, SQ_15))
            .apply(Move(SQ_52, SQ_51))
            .apply(Move(SQ_15, SQ_14))
            .apply(Move(SQ_51, SQ_52));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result()); // #repeat = 2
        game.apply(Move(SQ_14, SQ_15))
            .apply(Move(SQ_52, SQ_51))
            .apply(Move(SQ_15, SQ_14))
            .apply(Move(SQ_51, SQ_52));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result()); // #repeat = 3
        game.apply(Move(SQ_14, SQ_15))
            .apply(Move(SQ_52, SQ_51))
            .apply(Move(SQ_15, SQ_14));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_51, SQ_52)); // #repeat = 4
        CHECK_EQUAL(vshogi::DRAW, game.get_result());
    }
    {
        auto game = Game();
        game.apply(Move(SQ_53, SQ_54)).apply(Move(SQ_32, SQ_21));
        game.apply(Move(SQ_12, SQ_15)); // #repeat = 1
        game.apply(Move(SQ_21, SQ_11))
            .apply(Move(SQ_22, SQ_12))
            .apply(Move(SQ_11, SQ_21))
            .apply(Move(SQ_12, SQ_22)); // #repeat = 2
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_21, SQ_11))
            .apply(Move(SQ_22, SQ_12))
            .apply(Move(SQ_11, SQ_21))
            .apply(Move(SQ_12, SQ_22)); // #repeat = 3
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_21, SQ_11))
            .apply(Move(SQ_22, SQ_12))
            .apply(Move(SQ_11, SQ_21));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_12, SQ_22)); // #repeat = 4
        CHECK_EQUAL(vshogi::WHITE_WIN, game.get_result());
    }
}

} // namespace test_vshogi::test_minishogi
