#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP(test_shogi_generator){};

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
    auto iter = CheckDropMoveGenerator(s);
    CHECK_FALSE(iter != iter.end());
}

} // namespace test_vshogi::test_shogi
