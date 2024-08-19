#include "vshogi/common/utils.hpp"
#include "vshogi/variants/animal_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_bitboard){};

TEST(animal_shogi_bitboard, xor_operator)
{
    {
        const auto actual = (bb_a1 | bb_a2) ^ (bb_a2 | bb_a3);
        const auto expect = (bb_a1 | bb_a3);
        CHECK_TRUE(expect == actual);
    }
    {
        auto actual = (bb_a1 | bb_a2);
        actual ^= (bb_a2 | bb_a3);
        const auto expect = (bb_a1 | bb_a3);
        CHECK_TRUE(expect == actual);
    }
}

TEST(animal_shogi_bitboard, shift)
{
    {
        CHECK_FALSE(bb_b1.shift(vshogi::DIR_N).any());
        CHECK_TRUE(bb_b1 == bb_b2.shift(vshogi::DIR_N));
    }
    {
        CHECK_TRUE(bb_fileb == bb_filea.shift(vshogi::DIR_E));
        CHECK_TRUE(bb_filec == bb_fileb.shift(vshogi::DIR_E));
        CHECK_TRUE(BitBoard(0) == bb_filec.shift(vshogi::DIR_E));
    }
    {
        CHECK_TRUE(BitBoard(0) == bb_filea.shift(vshogi::DIR_W));
        CHECK_TRUE(bb_filea == bb_fileb.shift(vshogi::DIR_W));
        CHECK_TRUE(bb_fileb == bb_filec.shift(vshogi::DIR_W));
    }

    for (auto dir :
         vshogi::EnumIterator<vshogi::DirectionEnum, Config::num_dir>()) {
        for (auto sq :
             vshogi::EnumIterator<SquareEnum, Config::num_squares>()) {
            CHECK_TRUE(
                BitBoard::from_square(Squares::shift(sq, dir))
                == BitBoard::from_square(sq).shift(dir));
        }
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

TEST(animal_shogi_bitboard, get_promotion_zone)
{
    {
        const auto actual = BitBoard::get_promotion_zone(vshogi::BLACK);
        CHECK_TRUE(bb_rank1 == actual);
    }
    {
        const auto actual = BitBoard::get_promotion_zone(vshogi::WHITE);
        CHECK_TRUE(bb_rank4 == actual);
    }
}

} // namespace test_vshogi::test_animal_shogi
