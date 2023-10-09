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

TEST(game, get_legal_moves)
{
    {
        auto game = Game();
        game.apply(Move(SQ_4D, SQ_2F));
        CHECK_EQUAL(4, game.get_legal_moves().size());
    }
    {
        auto game = Game("4gk/5p/6/6/P5/KGSNBR b rbns");
        game.apply(Move(SQ_4D, SQ_2F));
        CHECK_EQUAL(9, game.get_legal_moves().size());
        game.apply(Move(SQ_2B, GI)).apply(Move(SQ_2B, SQ_4D, true));
        CHECK_EQUAL(2, game.get_legal_moves().size());
    }
    {
        auto game = Game("6/4gk/6/5B/6/K4R b p");
        game.apply(Move(SQ_2C, SQ_1D));
        CHECK_EQUAL(2, game.get_legal_moves().size());
    }
    {
        auto game = Game("6/4gk/6/5B/6/K4R b p");
        game.apply(Move(SQ_3B, SQ_1D, true));
        CHECK_EQUAL(4, game.get_legal_moves().size());
    }
}

} // namespace test_vshogi::test_judkins_shogi
