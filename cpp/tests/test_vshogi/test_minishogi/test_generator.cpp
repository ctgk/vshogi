#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP(minishogi_generator){};

TEST(minishogi_generator, king_move_generator)
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
}

TEST(minishogi_generator, check_king_move_generator)
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

TEST(minishogi_generator, drop_move_generator)
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
        const auto s = State("4k/4G/5/5/5 w p");
        auto iter = DropMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
    }
    {
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
}

TEST(minishogi_generator, check_drop_move_generator)
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
}

TEST(minishogi_generator, non_king_board_move_generator)
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
}

} // namespace test_vshogi::test_minishogi
