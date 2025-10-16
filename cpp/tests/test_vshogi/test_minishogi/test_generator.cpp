#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP (test_minishogi_generator) {
};

TEST(test_minishogi_generator, king_move_generator)
{
    {
        const auto s = State("5/5/5/5/5 b -");
        auto iter = vshogi::KingMoveGenerator<Parameters>(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        const auto s = State("5/2k2/5/2K2/5 b -");
        auto iter = vshogi::KingMoveGenerator<Parameters>(s);
        CHECK_TRUE(Move(SQ_3D, SQ_2D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3D, SQ_2E) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3D, SQ_3E) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3D, SQ_4D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3D, SQ_4E) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // Turn: WHITE
        // White: HI
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |+HI|   |+NG|   |   |
        //   *---*---*---*---*---*
        // B |   |   |   |   |-OU|
        //   *---*---*---*---*---*
        // C |+GI|   |   |-UM|-KI|
        //   *---*---*---*---*---*
        // D |+FU|+OU|   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |+KA|   |
        //   *---*---*---*---*---*
        // Black: -
        const auto s = State("R1+S2/4k/S2+bg/PK3/3B1 w r 40");
        auto iter = vshogi::KingMoveGenerator<Parameters>(s);
        CHECK_TRUE(Move(SQ_1B, SQ_1A) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_1B, SQ_2B) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        const auto s = State("5/5/5/5/5 b -");
        auto iter = vshogi::KingMoveGenerator<Parameters, true>(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        const auto s = State("4k/5/2K2/5/5 b -");
        auto iter = vshogi::KingMoveGenerator<Parameters, true>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/2K2/5/B4 b -");
        auto iter = vshogi::KingMoveGenerator<Parameters, true>(s);
        CHECK_TRUE(Move(SQ_3C, SQ_2C) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, SQ_2D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, SQ_3B) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, SQ_4B) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, SQ_4C) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
}

TEST(test_minishogi_generator, drop_move_generator)
{
    {
        const auto s = State("5/5/5/5/5 b -");
        auto iter = vshogi::DropMoveGenerator<Parameters>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("5/5/5/5/5 w pg");
        auto iter = vshogi::DropMoveGenerator<Parameters>(s);
        for (auto sq : vshogi::EnumIterator<SquareEnum, 25>()) {
            if (Squares::to_rank(sq) == RANK5)
                continue;
            CHECK_EQUAL(Move(FU, sq).hash(), (*iter).hash());
            ++iter;
        }
        for (auto sq : vshogi::EnumIterator<SquareEnum, 25>()) {
            CHECK_EQUAL(Move(KI, sq).hash(), (*iter).hash());
            ++iter;
        }
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        const auto s = State("3rk/R4/5/P4/KPPP1 b P");
        auto iter = vshogi::DropMoveGenerator<Parameters>(s);
        CHECK_TRUE(Move(FU, SQ_1C) == *iter);
        ++iter;
        CHECK_TRUE(Move(FU, SQ_1D) == *iter);
        ++iter;
        CHECK_TRUE(Move(FU, SQ_1E) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in check
        const auto s = State("4k/4G/5/5/5 w p");
        auto iter = vshogi::DropMoveGenerator<Parameters>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in check
        const auto s = State("4k/5/5/5/B4 w ps");
        auto iter = vshogi::DropMoveGenerator<Parameters>(s);
        CHECK_TRUE(Move(FU, SQ_2B) == *iter);
        ++iter;
        CHECK_TRUE(Move(FU, SQ_3C) == *iter);
        ++iter;
        CHECK_TRUE(Move(FU, SQ_4D) == *iter);
        ++iter;
        CHECK_TRUE(Move(GI, SQ_2B) == *iter);
        ++iter;
        CHECK_TRUE(Move(GI, SQ_3C) == *iter);
        ++iter;
        CHECK_TRUE(Move(GI, SQ_4D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in double check
        const auto s = State("r3k/5/5/5/K3r b PSG");
        auto iter = vshogi::DropMoveGenerator<Parameters>(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        const auto s = State("5/5/5/5/5 b -");
        auto iter = vshogi::DropMoveGenerator<Parameters, true>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/5/5/5 b P");
        auto iter = vshogi::DropMoveGenerator<Parameters, true>(s);
        CHECK_EQUAL(Move(FU, SQ_1B).hash(), (*iter).hash());
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/5/1P3/5 b BP");
        auto iter = vshogi::DropMoveGenerator<Parameters, true>(s);
        CHECK_EQUAL(Move(FU, SQ_1B).hash(), (*iter).hash());
        ++iter;
        CHECK_EQUAL(Move(KA, SQ_2B).hash(), (*iter).hash());
        ++iter;
        CHECK_EQUAL(Move(KA, SQ_3C).hash(), (*iter).hash());
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // Inhibit drop pawn check due to two pawns in a file.
        const auto s = State("4k/5/3g1/5/2PKP w p");
        auto iter = vshogi::DropMoveGenerator<Parameters, true>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in double check
        const auto s = State("3kb/5/5/5/K3r b R");
        auto iter = vshogi::DropMoveGenerator<Parameters, true>(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
}

TEST(test_minishogi_generator, soldier_move_generator)
{
    {
        // no piece to move
        const auto s = State("4k/5/5/5/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in double check
        const auto s = State("4k/5/2P2/s4/K3r b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in check
        const auto s = State("4k/5/5/3S1/K3r b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_TRUE(Move(SQ_2D, SQ_1E) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2D, SQ_3E) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in check
        const auto s = State("4k/P2bp/5/G4/K3P b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_TRUE(Move(SQ_5D, SQ_4D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // pinned
        const auto s = State("+b3k/5/5/3S1/4K b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_TRUE(Move(SQ_2D, SQ_3C) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        const auto s = State("4k/5/2P2/5/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_TRUE(Move(SQ_3C, SQ_3B) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("B3k/1p3/5/5/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_TRUE(Move(SQ_5A, SQ_4B, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5A, SQ_4B, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("B3k/1pP2/5/5/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, false>(s);
        CHECK_TRUE(Move(SQ_3B, SQ_3A, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5A, SQ_4B, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5A, SQ_4B, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // W_GI VOID VOID VOID VOID
        // W_OU W_GI VOID B_RY VOID
        // VOID VOID VOID B_KI B_FU
        // B_FU B_OU VOID VOID VOID
        // VOID VOID VOID W_KA B_HI
        const auto s = State("s4/ks1+R1/3GP/PK3/3bR w -");
        const auto& b = s.get_board();
        const auto src_mask = b.get_occupied<KA, HI, UM, RY>(vshogi::WHITE);
        CHECK_EQUAL(bb_2e.value(), src_mask.value());
        auto iter
            = vshogi::SoldierMoveGenerator<Parameters, false>(s, src_mask);
        CHECK_TRUE(Move(SQ_2E, SQ_1D, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2E, SQ_1D, true) == *iter);
        ++iter;
    }
    {
        const auto s = State("4k/5/5/5/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/2P2/5/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("2B1k/5/5/5/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
        CHECK_TRUE(Move(SQ_3A, SQ_2B, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3A, SQ_2B, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("B4/5/5/4k/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
        CHECK_TRUE(Move(SQ_5A, SQ_1E, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5A, SQ_2D, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("B1S1k/5/5/5/K4 b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
        CHECK_TRUE(Move(SQ_3A, SQ_2B, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3A, SQ_2B, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5A, SQ_3C, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_5A, SQ_3C, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        auto s = State("3b1/2r1k/3pB/3gR/5 b G");
        s.apply(Move(KI, SQ_2B)).apply(Move(SQ_3B, SQ_2B));
        auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
        // discovered checks
        CHECK_TRUE(Move(SQ_1C, SQ_2B) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_1C, SQ_2D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        auto s = State("5/4k/3P1/3R+B/K3R b -");
        auto iter = vshogi::SoldierMoveGenerator<Parameters, true>(s);
        CHECK_TRUE(Move(SQ_1D, SQ_1C) == *iter); // moved piece check
        ++iter;
        CHECK_TRUE(Move(SQ_1D, SQ_2E) == *iter); // discovered check
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
}

TEST(test_minishogi_generator, block_move_generator)
{
    {
        // no piece to move
        const auto s = State("4k/5/5/5/K4 b -");
        auto iter = vshogi::BlockMoveGenerator<Parameters>(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        // in double check
        const auto s = State("4k/5/2P2/s4/K3r b -");
        auto iter = vshogi::BlockMoveGenerator<Parameters>(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        // in check
        const auto s = State("4k/5/5/3S1/K3r b -");
        auto iter = vshogi::BlockMoveGenerator<Parameters>(s);
        CHECK_TRUE(Move(SQ_2D, SQ_1E) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2D, SQ_3E) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        // in check
        const auto s = State("4k/P2bp/5/G4/K3P b -");
        auto iter = vshogi::BlockMoveGenerator<Parameters>(s);
        CHECK_TRUE(Move(SQ_5D, SQ_4D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        // pinned
        const auto s = State("+b3k/4r/5/3S1/3GK b -");
        auto iter = vshogi::BlockMoveGenerator<Parameters>(s);
        CHECK_TRUE(Move(SQ_2E, SQ_1D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        // Turn: WHITE
        // White: GI
        //     5   4   3   2   1
        //   +---+---+---+---+---+
        // A |   |   |-FU|-OU|+RY|
        //   +---+---+---+---+---+
        // B |   |   |   |-KA|   |
        //   +---+---+---+---+---+
        // C |   |   |-UM|+KI|   |
        //   +---+---+---+---+---+
        // D |   |   |   |   |   |
        //   +---+---+---+---+---+
        // E |   |   |   |   |   |
        //   +---+---+---+---+---+
        // Black: GI
        const auto s = State("2pk+R/3b1/2+bG1/5/5 w Ss");
        auto iter = vshogi::BlockMoveGenerator<Parameters>(s);
        CHECK_EQUAL(Move(SQ_2B, SQ_1A).hash(), (*iter).hash());
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
}

} // namespace test_vshogi::test_minishogi
