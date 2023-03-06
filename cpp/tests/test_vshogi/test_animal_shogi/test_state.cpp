#include "vshogi/animal_shogi/state.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_animal_shogi
{

using namespace vshogi::animal_shogi;

TEST_GROUP(animal_shogi_state){};

TEST(animal_shogi_state, get_turn)
{
    {
        const auto s = State();
        CHECK_EQUAL(BLACK, s.get_turn());
    }
    {
        const auto s = State("", WHITE);
        CHECK_EQUAL(WHITE, s.get_turn());
    }
}

} // namespace test_vshogi::test_animal_shogi
