#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP (animal_shogi_pieces) {
};

TEST(animal_shogi_pieces, is_promotable)
{
    CHECK_TRUE(Pieces::is_promotable(CH));
    CHECK_FALSE(Pieces::is_promotable(EL));
    CHECK_FALSE(Pieces::is_promotable(GI));
    CHECK_FALSE(Pieces::is_promotable(LI));
    CHECK_FALSE(Pieces::is_promotable(HE));
    CHECK_FALSE(Pieces::is_promotable(NA));
}

TEST(animal_shogi_pieces, to_piece_type)
{
    CHECK_EQUAL(CH, Pieces::to_piece_type('c'));
    CHECK_EQUAL(EL, Pieces::to_piece_type('e'));
    CHECK_EQUAL(GI, Pieces::to_piece_type('g'));
    CHECK_EQUAL(LI, Pieces::to_piece_type('l'));
    CHECK_EQUAL(HE, Pieces::to_piece_type('h'));
    CHECK_EQUAL(NA, Pieces::to_piece_type('a'));

    CHECK_EQUAL(CH, Pieces::to_piece_type('C'));
    CHECK_EQUAL(EL, Pieces::to_piece_type('E'));
    CHECK_EQUAL(GI, Pieces::to_piece_type('G'));
    CHECK_EQUAL(LI, Pieces::to_piece_type('L'));
    CHECK_EQUAL(HE, Pieces::to_piece_type('H'));
    CHECK_EQUAL(NA, Pieces::to_piece_type('A'));
}

TEST(animal_shogi_pieces, to_char)
{
    CHECK_EQUAL('c', Pieces::to_char(CH));
    CHECK_EQUAL('e', Pieces::to_char(EL));
    CHECK_EQUAL('g', Pieces::to_char(GI));
    CHECK_EQUAL('l', Pieces::to_char(LI));
    CHECK_EQUAL('h', Pieces::to_char(HE));
}

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
