#include "vshogi/animal_shogi/color.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_color){};

TEST(animal_shogi_color, opposite)
{
    CHECK_EQUAL(BLACK, opposite(WHITE));
}

TEST(animal_shogi_color, op)
{
    CHECK_EQUAL(WHITE, ~BLACK);
}

} // namespace test_vshogi::test_animal_shogi
