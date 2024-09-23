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
        auto iter = KingMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("5/2k2/5/2K2/5 b -");
        auto iter = KingMoveGenerator(s);
        CHECK_TRUE(Move(SQ_4D, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2D, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_4E, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3E, SQ_3D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2E, SQ_3D) == *iter);
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
        auto iter = KingMoveGenerator(s);
        CHECK_TRUE(Move(SQ_1A, SQ_1B) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2B, SQ_1B) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
}

TEST(test_minishogi_generator, check_king_move_generator)
{
    {
        const auto s = State("4k/5/2K2/5/5 b -");
        auto iter = CheckKingMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/2K2/5/B4 b -");
        auto iter = CheckKingMoveGenerator(s);
        CHECK_TRUE(Move(SQ_4B, SQ_3C) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3B, SQ_3C) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_4C, SQ_3C) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2C, SQ_3C) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3D, SQ_3C) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2D, SQ_3C) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
}

TEST(test_minishogi_generator, drop_move_generator)
{
    {
        const auto s = State("5/5/5/5/5 b -");
        auto iter = DropMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("5/5/5/5/5 w pg");
        auto iter = DropMoveGenerator(s);
        for (int i = 0; i < 20; ++i) {
            CHECK_TRUE(Move(static_cast<SquareEnum>(i), FU) == *iter);
            ++iter;
        }
        for (int i = 0; i < 25; ++i) {
            CHECK_TRUE(Move(static_cast<SquareEnum>(i), KI) == *iter);
            ++iter;
        }
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("3rk/R4/5/P4/KPPP1 b P");
        auto iter = DropMoveGenerator(s);
        CHECK_TRUE(Move(SQ_1C, FU) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_1D, FU) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_1E, FU) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in check
        const auto s = State("4k/4G/5/5/5 w p");
        auto iter = DropMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in check
        const auto s = State("4k/5/5/5/B4 w ps");
        auto iter = DropMoveGenerator(s);
        CHECK_TRUE(Move(SQ_2B, FU) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, FU) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_4D, FU) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2B, GI) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, GI) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_4D, GI) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in double check
        const auto s = State("r3k/5/5/5/K3r b PSG");
        auto iter = DropMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
}

TEST(test_minishogi_generator, test_check_drop_move_generator)
{
    {
        const auto s = State("5/5/5/5/5 b -");
        auto iter = CheckDropMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/5/5/5 b P");
        auto iter = CheckDropMoveGenerator(s);
        CHECK_TRUE(Move(SQ_1B, FU) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/5/1P3/5 b BP");
        auto iter = CheckDropMoveGenerator(s);
        CHECK_TRUE(Move(SQ_1B, FU) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2B, KA) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, KA) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/3g1/5/2PKP w p");
        auto iter = CheckDropMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in double check
        const auto s = State("3kb/5/5/5/K3r b R");
        auto iter = CheckDropMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
}

TEST(test_minishogi_generator, no_promo_move_generator)
{
    const auto s = State("4k/5/5/K4/G4 b -");
    const auto src_mask = s.get_board().get_occupied<KI>(s.get_turn());
    CHECK_EQUAL(bb_5e.value(), src_mask.value());
    auto iter = vshogi::NoPromoMoveGenerator<Config>(s, src_mask, BitBoard());
    CHECK_TRUE(Move(SQ_4D, SQ_5E) == *iter);
    ++iter;
    CHECK_TRUE(Move(SQ_4E, SQ_5E) == *iter);
    ++iter;
    CHECK_FALSE(iter != iter.end());
    CHECK_TRUE(iter.is_end());
}

TEST(test_minishogi_generator, non_king_board_move_generator)
{
    {
        // no piece to move
        const auto s = State("4k/5/5/5/K4 b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in double check
        const auto s = State("4k/5/2P2/s4/K3r b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in check
        const auto s = State("4k/5/5/3S1/K3r b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_3E, SQ_2D) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_1E, SQ_2D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // in check
        const auto s = State("4k/P2bp/5/G4/K3P b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_4D, SQ_5D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        // pinned
        const auto s = State("+b3k/5/5/3S1/4K b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_3C, SQ_2D) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        const auto s = State("4k/5/2P2/5/K4 b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_3B, SQ_3C) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("B3k/1p3/5/5/K4 b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_4B, SQ_5A, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_4B, SQ_5A, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("B3k/1pP2/5/5/K4 b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_4B, SQ_5A, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_4B, SQ_5A, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3A, SQ_3B, true) == *iter);
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
        auto iter = NonKingBoardMoveGenerator(s, src_mask);
        CHECK_TRUE(Move(SQ_4C, SQ_2E, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_4C, SQ_2E, true) == *iter);
        ++iter;
    }
}

TEST(test_minishogi_generator, check_non_king_board_move_generator)
{
    {
        const auto s = State("4k/5/5/5/K4 b -");
        auto iter = CheckNonKingBoardMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("4k/5/2P2/5/K4 b -");
        auto iter = CheckNonKingBoardMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("2B1k/5/5/5/K4 b -");
        auto iter = CheckNonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_2B, SQ_3A, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2B, SQ_3A, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("B4/5/5/4k/K4 b -");
        auto iter = CheckNonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_2D, SQ_5A, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_1E, SQ_5A, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        const auto s = State("B1S1k/5/5/5/K4 b -");
        auto iter = CheckNonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_3C, SQ_5A, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3C, SQ_5A, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2B, SQ_3A, false) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2B, SQ_3A, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        auto s = State("3b1/2r1k/3pB/3gR/5 b G");
        s.apply(Move(SQ_2B, KI)).apply(Move(SQ_2B, SQ_3B));
        auto iter = CheckNonKingBoardMoveGenerator(s);
        // discovered checks
        CHECK_TRUE(Move(SQ_2B, SQ_1C) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_2D, SQ_1C) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
    {
        auto s = State("5/4k/3P1/3R+B/K3R b -");
        auto iter = CheckNonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_1C, SQ_1D) == *iter); // moved piece check
        ++iter;
        CHECK_TRUE(Move(SQ_2E, SQ_1D) == *iter); // discovered check
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
}

} // namespace test_vshogi::test_minishogi
