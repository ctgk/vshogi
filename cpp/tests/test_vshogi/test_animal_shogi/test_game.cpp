#include "vshogi/animal_shogi/game.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(Game){};

TEST(Game, to_sfen)
{
    {
        const auto actual = Game().to_sfen();
        STRCMP_EQUAL("gle/1c1/1C1/ELG b - 1", actual.c_str());
    }
}

TEST(Game, result)
{
    {
        auto game = Game();
        game.apply(Move(SQ_B2, SQ_B3)).apply(Move(SQ_C2, SQ_C1));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_B1, SQ_B2));
        CHECK_EQUAL(vshogi::BLACK_WIN, game.get_result());
    }
    {
        auto game = Game();
        game.apply(Move(SQ_C3, SQ_C4)).apply(Move(SQ_A2, SQ_A1));
        game.apply(Move(SQ_C4, SQ_C3)).apply(Move(SQ_A1, SQ_A2));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result()); // #repeat = 2

        game.apply(Move(SQ_C3, SQ_C4)).apply(Move(SQ_A2, SQ_A1));
        game.apply(Move(SQ_C4, SQ_C3)).apply(Move(SQ_A1, SQ_A2));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result()); // #repeat = 3

        game.apply(Move(SQ_C3, SQ_C4)).apply(Move(SQ_A2, SQ_A1));
        game.apply(Move(SQ_C4, SQ_C3));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_A1, SQ_A2));
        CHECK_EQUAL(vshogi::DRAW, game.get_result()); // #repeat = 4
    }
    {
        auto game = Game(1452042304300031UL);
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());

        game.apply(Move(SQ_C4, CH));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    }
}

TEST(Game, repetition)
{
    auto game = Game();
    game.apply(Move(SQ_B2, MS_B3));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, MS_C1));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, MS_CH));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A2, MS_A1));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B3, MS_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A3, MS_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, MS_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, MS_A2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A3, MS_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, MS_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B4, MS_A3));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, MS_B1));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, MS_B3));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, MS_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C3, MS_C4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B1, MS_C2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, MS_C3));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, MS_B1));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, MS_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, MS_C2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B4, MS_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A2, MS_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, MS_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, MS_A2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B4, MS_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A2, MS_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, MS_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, MS_A2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B4, MS_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A2, MS_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, MS_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, MS_A2));
    CHECK_EQUAL(vshogi::DRAW, game.get_result());
}

} // namespace test_vshogi::test_animal_shogi
