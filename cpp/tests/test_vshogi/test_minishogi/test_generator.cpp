#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP (test_minishogi_generator) {
};

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
