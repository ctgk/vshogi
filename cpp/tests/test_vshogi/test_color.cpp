#include "vshogi/common/color.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi
{

using namespace vshogi;

TEST_GROUP(color){};

TEST(color, opposite)
{
    CHECK_EQUAL(BLACK, opposite(WHITE));
}

TEST(color, op)
{
    CHECK_EQUAL(WHITE, ~BLACK);
}

} // namespace test_vshogi
