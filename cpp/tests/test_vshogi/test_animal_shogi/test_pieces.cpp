#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_pieces){};

TEST(animal_shogi_pieces, get_attack_directions)
{
    {
        const auto actual = Pieces::get_attack_directions(B_CH);
        CHECK_EQUAL(vshogi::DIR_N, actual[0]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[1]);
    }
    {
        const auto actual = Pieces::get_attack_directions(B_EL);
        CHECK_EQUAL(vshogi::DIR_NW, actual[0]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[1]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[2]);
        CHECK_EQUAL(vshogi::DIR_SE, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[4]);
    }
    {
        const auto actual = Pieces::get_attack_directions(B_GI);
        CHECK_EQUAL(vshogi::DIR_N, actual[0]);
        CHECK_EQUAL(vshogi::DIR_W, actual[1]);
        CHECK_EQUAL(vshogi::DIR_E, actual[2]);
        CHECK_EQUAL(vshogi::DIR_S, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[4]);
    }
    {
        const auto actual = Pieces::get_attack_directions(B_LI);
        CHECK_EQUAL(vshogi::DIR_NW, actual[0]);
        CHECK_EQUAL(vshogi::DIR_N, actual[1]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[2]);
        CHECK_EQUAL(vshogi::DIR_W, actual[3]);
        CHECK_EQUAL(vshogi::DIR_E, actual[4]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[5]);
        CHECK_EQUAL(vshogi::DIR_S, actual[6]);
        CHECK_EQUAL(vshogi::DIR_SE, actual[7]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[8]);
    }
    {
        const auto actual = Pieces::get_attack_directions(B_HE);
        CHECK_EQUAL(vshogi::DIR_NW, actual[0]);
        CHECK_EQUAL(vshogi::DIR_N, actual[1]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[2]);
        CHECK_EQUAL(vshogi::DIR_W, actual[3]);
        CHECK_EQUAL(vshogi::DIR_E, actual[4]);
        CHECK_EQUAL(vshogi::DIR_S, actual[5]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[6]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_CH);
        CHECK_EQUAL(vshogi::DIR_S, actual[0]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[1]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_EL);
        CHECK_EQUAL(vshogi::DIR_SE, actual[0]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[1]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[2]);
        CHECK_EQUAL(vshogi::DIR_NW, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[4]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_GI);
        CHECK_EQUAL(vshogi::DIR_S, actual[0]);
        CHECK_EQUAL(vshogi::DIR_E, actual[1]);
        CHECK_EQUAL(vshogi::DIR_W, actual[2]);
        CHECK_EQUAL(vshogi::DIR_N, actual[3]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[4]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_LI);
        CHECK_EQUAL(vshogi::DIR_SE, actual[0]);
        CHECK_EQUAL(vshogi::DIR_S, actual[1]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[2]);
        CHECK_EQUAL(vshogi::DIR_E, actual[3]);
        CHECK_EQUAL(vshogi::DIR_W, actual[4]);
        CHECK_EQUAL(vshogi::DIR_NE, actual[5]);
        CHECK_EQUAL(vshogi::DIR_N, actual[6]);
        CHECK_EQUAL(vshogi::DIR_NW, actual[7]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[8]);
    }
    {
        const auto actual = Pieces::get_attack_directions(W_HE);
        CHECK_EQUAL(vshogi::DIR_SE, actual[0]);
        CHECK_EQUAL(vshogi::DIR_S, actual[1]);
        CHECK_EQUAL(vshogi::DIR_SW, actual[2]);
        CHECK_EQUAL(vshogi::DIR_E, actual[3]);
        CHECK_EQUAL(vshogi::DIR_W, actual[4]);
        CHECK_EQUAL(vshogi::DIR_N, actual[5]);
        CHECK_EQUAL(vshogi::DIR_NA, actual[6]);
    }
}

} // namespace test_vshogi::test_animal_shogi
