#include "vshogi/animal_shogi/game.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(Game){};

TEST(Game, result)
{
    {
        auto game = Game();
        game.apply_move(Move(SQ_B2, SQ_B3)).apply_move(Move(SQ_C2, SQ_C1));
        CHECK_EQUAL(UNKNOWN, game.get_result());
        game.apply_move(Move(SQ_B1, SQ_B2));
        CHECK_EQUAL(BLACK_WIN, game.get_result());
    }
    {
        auto game = Game();
        game.apply_move(Move(SQ_C3, SQ_C4)).apply_move(Move(SQ_A2, SQ_A1));
        game.apply_move(Move(SQ_C4, SQ_C3)).apply_move(Move(SQ_A1, SQ_A2));
        CHECK_EQUAL(UNKNOWN, game.get_result()); // #repeat = 1

        game.apply_move(Move(SQ_C3, SQ_C4)).apply_move(Move(SQ_A2, SQ_A1));
        game.apply_move(Move(SQ_C4, SQ_C3)).apply_move(Move(SQ_A1, SQ_A2));
        CHECK_EQUAL(UNKNOWN, game.get_result()); // #repeat = 2

        game.apply_move(Move(SQ_C3, SQ_C4)).apply_move(Move(SQ_A2, SQ_A1));
        game.apply_move(Move(SQ_C4, SQ_C3)).apply_move(Move(SQ_A1, SQ_A2));
        CHECK_EQUAL(UNKNOWN, game.get_result()); // #repeat = 3

        game.apply_move(Move(SQ_C3, SQ_C4)).apply_move(Move(SQ_A2, SQ_A1));
        game.apply_move(Move(SQ_C4, SQ_C3));
        CHECK_EQUAL(UNKNOWN, game.get_result());
        game.apply_move(Move(SQ_A1, SQ_A2));
        CHECK_EQUAL(DRAW, game.get_result()); // #repeat = 4
    }
}

} // namespace test_vshogi::test_animal_shogi
