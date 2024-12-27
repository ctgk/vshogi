#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (test_shogi_generator) {
};

TEST(test_shogi_generator, check_non_king_board_move_generator)
{
    const auto s = State("b8/8K/9/9/9/9/9/7l1/8K w -");
    auto iter = vshogi::CheckNonKingBoardMoveGenerator<Parameters>(s);
    CHECK_TRUE(Move(SQ_2I, SQ_2H, true) == *iter);
    ++iter;
    CHECK_TRUE(iter.is_end());
    CHECK_FALSE(iter != iter.end());
}

TEST(test_shogi_generator, test_check_drop_move_generator)
{
    // Turn: WHITE
    // White: FU
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |   |   |   |-OU|   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // C |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |   |   |   |   |   |   |   |-KI|   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |   |   |   |   |-HI|+FU|+OU|+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: -
    auto s = State("3k5/9/9/9/9/9/7g1/9/5rPKL w p");
    auto iter = vshogi::CheckDropMoveGenerator<Parameters>(s);
    CHECK_FALSE(iter != iter.end());
}

} // namespace test_vshogi::test_shogi
