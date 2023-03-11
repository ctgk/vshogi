#include "vshogi/animal_shogi/move.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(Move){};

TEST(Move, source)
{
    CHECK_EQUAL(MS_GI, Move(SQ_A1, GI).source());
    CHECK_EQUAL(MS_A1, Move(SQ_A1, LI).source());
}

} // namespace test_vshogi::test_animal_shogi
