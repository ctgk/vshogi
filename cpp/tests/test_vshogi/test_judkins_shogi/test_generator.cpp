#include "vshogi/variants/judkins_shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP(judkins_shogi_generator){};

TEST(judkins_shogi_generator, non_king_board_move_generator)
{
    {
        const auto s = State("6/6/6/2N3/6/k4K b -");
        auto iter = NonKingBoardMoveGenerator(s);
        CHECK_TRUE(Move(SQ_5B, SQ_4D, true) == *iter);
        ++iter;
        CHECK_TRUE(Move(SQ_3B, SQ_4D, true) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
    }
}

TEST(judkins_shogi_generator, check_non_king_board_move_generator)
{
    {
        // Turn: BLACK
        // White: -
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |   |-FU|-FU|
        //   +---+---+---+---+---+---+
        // B |   |   |   |-FU|-KA|-OU|
        //   +---+---+---+---+---+---+
        // C |   |   |   |-KE|   |+KE|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+FU|   |+FU|
        //   +---+---+---+---+---+---+
        // E |   |   |+GI|   |+GI|+HI|
        //   +---+---+---+---+---+---+
        // F |   |   |   |+KI|+HI|+KI|
        //   +---+---+---+---+---+---+
        // Black: -
        const auto s = State("4pp/3pbk/3n1N/3P1P/2S1SR/3GRG b -");
        auto iter = CheckNonKingBoardMoveGenerator(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
}

} // namespace test_vshogi::test_judkins_shogi
