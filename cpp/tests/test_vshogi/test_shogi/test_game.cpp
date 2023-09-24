#include "vshogi/shogi/game.hpp"

#include <CppUTest/TestHarness.h>

#include "test_vshogi/test_shogi/test_shogi.hpp"

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(game){};

TEST(game, num_dlshogi_policy)
{
    CHECK_EQUAL(9 * 9 * (2 * 10 + 7), Game::num_dlshogi_policy());
}

TEST(game, record_length)
{
    auto game = Game();
    CHECK_EQUAL(0, game.record_length());
    game.apply(Move(SQ_2F, SQ_2G));
    CHECK_EQUAL(1, game.record_length());
}

TEST(game, apply)
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

TEST(game, result)
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

} // namespace test_vshogi::test_shogi
