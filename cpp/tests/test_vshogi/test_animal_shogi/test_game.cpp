#include "vshogi/animal_shogi/game.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;
static constexpr auto CH = Pieces::CH; // NOLINT
static constexpr auto EL = Pieces::EL; // NOLINT
static constexpr auto GI = Pieces::GI; // NOLINT
static constexpr auto LI = Pieces::LI; // NOLINT
static constexpr auto HE = Pieces::HE; // NOLINT
static constexpr auto SQ_A4 = Squares::SQ_A4; // NOLINT
static constexpr auto SQ_A3 = Squares::SQ_A3; // NOLINT
static constexpr auto SQ_A2 = Squares::SQ_A2; // NOLINT
static constexpr auto SQ_A1 = Squares::SQ_A1; // NOLINT
static constexpr auto SQ_B4 = Squares::SQ_B4; // NOLINT
static constexpr auto SQ_B3 = Squares::SQ_B3; // NOLINT
static constexpr auto SQ_B2 = Squares::SQ_B2; // NOLINT
static constexpr auto SQ_B1 = Squares::SQ_B1; // NOLINT
static constexpr auto SQ_C4 = Squares::SQ_C4; // NOLINT
static constexpr auto SQ_C3 = Squares::SQ_C3; // NOLINT
static constexpr auto SQ_C2 = Squares::SQ_C2; // NOLINT
static constexpr auto SQ_C1 = Squares::SQ_C1; // NOLINT

TEST_GROUP(Game){};

TEST(Game, num_dlshogi_policy)
{
    CHECK_EQUAL(3 * 4 * (8 + 3), Game::num_dlshogi_policy());
}

TEST(Game, to_sfen)
{
    {
        const auto actual = Game().to_sfen();
        STRCMP_EQUAL("gle/1c1/1C1/ELG b - 1", actual.c_str());
    }
}

TEST(Game, get_sfen_at)
{
    {
        const auto actual = Game().apply(Move(SQ_B2, SQ_B3)).get_sfen_at(0);
        STRCMP_EQUAL("gle/1c1/1C1/ELG b - 1", actual.c_str());
    }
    {
        const auto actual
            = Game().apply(Move(SQ_B2, SQ_B3)).get_sfen_at(0, false);
        STRCMP_EQUAL("gle/1c1/1C1/ELG b -", actual.c_str());
    }
    {
        const auto actual = Game()
                                .apply(Move(SQ_B2, SQ_B3))
                                .apply(Move(SQ_B2, SQ_B1))
                                .get_sfen_at(1);
        STRCMP_EQUAL("gle/1C1/3/ELG w C 2", actual.c_str());
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
}

TEST(Game, repetition)
{
    auto game = Game();
    game.apply(Move(SQ_B2, SQ_B3));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, SQ_C1));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, CH));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A2, SQ_A1));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B3, SQ_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A3, SQ_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, SQ_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, SQ_A2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A3, SQ_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, SQ_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B4, SQ_A3));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, SQ_B1));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, SQ_B3));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, SQ_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C3, SQ_C4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B1, SQ_C2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, SQ_C3));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_C2, SQ_B1));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, SQ_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, SQ_C2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B4, SQ_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A2, SQ_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, SQ_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, SQ_A2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B4, SQ_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A2, SQ_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, SQ_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, SQ_A2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B4, SQ_A4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A2, SQ_B2));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_A4, SQ_B4));
    CHECK_EQUAL(vshogi::ONGOING, game.get_result());
    game.apply(Move(SQ_B2, SQ_A2));
    CHECK_EQUAL(vshogi::DRAW, game.get_result());
}

} // namespace test_vshogi::test_animal_shogi
