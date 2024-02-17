#include <algorithm>

#include "vshogi/common/legals.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_legals
{

TEST_GROUP(minishogi_legals){};

TEST(minishogi_legals, append_legal_moves_by_king)
{
    using namespace vshogi::minishogi;

    {
        auto game = Game();
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_king(actual, game);
        CHECK_EQUAL(1, actual.size());
        CHECK_TRUE(actual[0] == Move(SQ_4D, SQ_5E));
    }
    {
        auto game = Game("5/5/2K2/5/5 b -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_king(actual, game);
        CHECK_EQUAL(8, actual.size());
        for (auto&& m :
             {Move(SQ_4B, SQ_3C),
              Move(SQ_3B, SQ_3C),
              Move(SQ_2B, SQ_3C),
              Move(SQ_4C, SQ_3C),
              Move(SQ_2C, SQ_3C),
              Move(SQ_4D, SQ_3C),
              Move(SQ_3D, SQ_3C),
              Move(SQ_2D, SQ_3C)}) {
            CHECK_TRUE(
                std::find(actual.cbegin(), actual.cend(), m) != actual.cend());
        }
    }
    {
        auto game = Game("5/5/2K2/5/5 w -");
        std::vector<Move> actual;
        vshogi::append_legal_moves_by_king(actual, game);
        CHECK_EQUAL(0, actual.size());
    }
}

} // namespace test_vshogi::test_legals
