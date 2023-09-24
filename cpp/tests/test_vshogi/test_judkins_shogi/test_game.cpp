#include "vshogi/judkins_shogi/game.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_judkins_shogi/test_judkins_shogi.hpp"

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP(game){};

TEST(game, num_dlshogi_policy)
{
    CHECK_EQUAL(6 * 6 * (2 * 10 + 6), Game::num_dlshogi_policy());
}

TEST(game, record_length)
{
    auto game = Game();
    CHECK_EQUAL(0, game.record_length());
    game.apply(Move(SQ_1B, SQ_1F));
    CHECK_EQUAL(1, game.record_length());
}

TEST(game, result)
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

} // namespace test_vshogi::test_judkins_shogi
