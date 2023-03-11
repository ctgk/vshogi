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
}

} // namespace test_vshogi::test_animal_shogi
