#include "vshogi/common/direction.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi
{

using namespace vshogi;

TEST_GROUP (test_direction) {
};

TEST(test_direction, rotate)
{
    CHECK_EQUAL(DIR_SE, rotate(DIR_NW));
    CHECK_EQUAL(DIR_S, rotate(DIR_N));
    CHECK_EQUAL(DIR_SW, rotate(DIR_NE));
    CHECK_EQUAL(DIR_E, rotate(DIR_W));
    CHECK_EQUAL(DIR_W, rotate(DIR_E));
    CHECK_EQUAL(DIR_NE, rotate(DIR_SW));
    CHECK_EQUAL(DIR_N, rotate(DIR_S));
    CHECK_EQUAL(DIR_NW, rotate(DIR_SE));
    CHECK_EQUAL(DIR_NA, rotate(DIR_NA));
}

} // namespace test_vshogi
