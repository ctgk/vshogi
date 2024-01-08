#include "vshogi/animal_shogi/bitboard.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_bitboard){};

TEST(animal_shogi_bitboard, shift)
{
    {
        CHECK_TRUE(bb_fileb == bb_filea.shift<vshogi::DIR_E>());
        CHECK_TRUE(bb_filec == bb_fileb.shift<vshogi::DIR_E>());
        CHECK_TRUE(BitBoard(0) == bb_filec.shift<vshogi::DIR_E>());
    }
    {
        CHECK_TRUE(BitBoard(0) == bb_filea.shift<vshogi::DIR_W>());
        CHECK_TRUE(bb_filea == bb_fileb.shift<vshogi::DIR_W>());
        CHECK_TRUE(bb_fileb == bb_filec.shift<vshogi::DIR_W>());
    }
}

TEST(animal_shogi_bitboard, chick_attacks)
{
    CHECK_TRUE(bb_a1 == BitBoard::get_attacks_by(B_CH, SQ_A2));
    CHECK_TRUE(bb_a3 == BitBoard::get_attacks_by(W_CH, SQ_A2));
}

TEST(animal_shogi_bitboard, elephant_attacks)
{
    CHECK_TRUE(bb_b2 == BitBoard::get_attacks_by(B_EL, SQ_A1));
    CHECK_TRUE((bb_b2 | bb_b4) == BitBoard::get_attacks_by(W_EL, SQ_C3));
    CHECK_TRUE((bb_a3 | bb_c3) == BitBoard::get_attacks_by(B_EL, SQ_B4));
}

TEST(animal_shogi_bitboard, giraffe_attacks)
{
    CHECK_TRUE((bb_c3 | bb_b4) == BitBoard::get_attacks_by(W_GI, SQ_C4));
}

TEST(animal_shogi_bitboard, lion_attacks)
{
    CHECK_TRUE(
        (bb_a3 | bb_b3 | bb_b4) == BitBoard::get_attacks_by(B_LI, SQ_A4));
}

TEST(animal_shogi_bitboard, hen_attacks)
{
    CHECK_TRUE((bb_b1 | bb_c2) == BitBoard::get_attacks_by(B_HE, SQ_C1));
    CHECK_TRUE((bb_b4 | bb_c3) == BitBoard::get_attacks_by(W_HE, SQ_C4));
}

} // namespace test_vshogi::test_animal_shogi
