#include "color.hpp"

#include <CppUTest/TestHarness.h>

TEST_GROUP(color){};

TEST(color, opposite)
{
    CHECK_EQUAL(color::WHITE, color::opposite(color::BLACK));
}

TEST(color, operator_tilde)
{
    CHECK_EQUAL(color::BLACK, ~color::WHITE);
}
