#include "vshogi/engine/piece_value.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_piece_value_func
{

TEST_GROUP (test_minishogi_test_piece_value_func) {
};

TEST(test_minishogi_test_piece_value_func, default_position)
{
    using namespace vshogi::minishogi;
    const auto g = Game();
    const float actual = vshogi::engine::piece_value_func(g);
    DOUBLES_EQUAL(0.f, actual, 1e-2f);
}

TEST(test_minishogi_test_piece_value_func, has_stand_piece)
{
    using namespace vshogi::minishogi;
    auto g = Game();
    g.apply(Move(SQ_1B, SQ_1E));
    const float actual = vshogi::engine::piece_value_func(g);
    CHECK_TRUE(actual < 0.f);
}

} // namespace test_vshogi::test_piece_value_func
