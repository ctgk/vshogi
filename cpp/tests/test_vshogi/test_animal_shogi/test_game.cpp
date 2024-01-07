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

TEST_GROUP(animal_shogi_game){};

TEST(animal_shogi_game, num_dlshogi_policy)
{
    CHECK_EQUAL(3 * 4 * (8 + 3), Game::num_dlshogi_policy());
}

TEST(animal_shogi_game, to_sfen)
{
    {
        const auto actual = Game().to_sfen();
        STRCMP_EQUAL("gle/1c1/1C1/ELG b - 1", actual.c_str());
    }
}

TEST(animal_shogi_game, get_sfen_at)
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

TEST(animal_shogi_game, get_legal_moves)
{
    {
        auto g = Game();
        CHECK_EQUAL(4, g.get_legal_moves().size());
    }
    {
        auto g = Game();
        {
            const auto& actual = g.get_legal_moves();
            CHECK_EQUAL(4, actual.size());
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), Move(SQ_B2, SQ_B3))
                != actual.cend());
            CHECK_FALSE(
                std::find(actual.cbegin(), actual.cend(), Move(SQ_B3, SQ_B2))
                != actual.cend());
        }

        g.apply(Move(SQ_B2, SQ_B3));
        {
            const auto& actual = g.get_legal_moves();
            CHECK_EQUAL(5, actual.size());
        }
    }
}

TEST(animal_shogi_game, result)
{
    {
        auto game = Game();
        game.apply(Move(SQ_B2, SQ_B3)).apply(Move(SQ_A2, SQ_A1));
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
        game.apply(Move(SQ_C4, SQ_C3));
        CHECK_EQUAL(vshogi::ONGOING, game.get_result());
        game.apply(Move(SQ_A1, SQ_A2));
        CHECK_EQUAL(vshogi::DRAW, game.get_result()); // #repeat = 3
    }
}

} // namespace test_vshogi::test_animal_shogi
