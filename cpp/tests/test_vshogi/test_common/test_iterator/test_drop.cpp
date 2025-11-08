#include "vshogi/common/iterator/drop.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_minishogi
{

using namespace vshogi::minishogi;

TEST_GROUP (test_drop_evasion_iterator_minishogi) {
};

TEST(test_drop_evasion_iterator_minishogi, neighboring_ranging_checker)
{
    const auto s = State("r4/2k2/4P/P+bB2/K1S2 b R2gs");
    auto iter = vshogi::DropEvasionIterator<Parameters>(s);
    CHECK_FALSE(iter != iter.end());
    CHECK_TRUE(iter.is_end());
}

TEST(test_drop_evasion_iterator_minishogi, immobile_pawn)
{
    const auto s = State("r3K/5/5/5/5 b P");
    auto iter = vshogi::DropEvasionIterator<Parameters>(s);
    CHECK_FALSE(iter != iter.end());
    CHECK_TRUE(iter.is_end());
}

TEST(test_drop_evasion_iterator_minishogi, drop_pawn_mate)
{
    const auto s = State("G1kb+R/5/2G2/K4/5 b P");
    auto iter = vshogi::DropEvasionIterator<Parameters>(s);
    CHECK_TRUE(Move(FU, SQ_4C) == *iter);
    ++iter;
    // CHECK_TRUE(Move(FU, SQ_3B) == *iter);  <- Drop pawn mate
    CHECK_FALSE(iter != iter.end());
    CHECK_TRUE(iter.is_end());
}

TEST(test_drop_evasion_iterator_minishogi, nominal_case)
{
    const auto s = State("4k/5/5/5/B4 w ps");
    auto iter = vshogi::DropEvasionIterator<Parameters>(s);
    CHECK_TRUE(Move(FU, SQ_2B) == *iter);
    ++iter;
    CHECK_TRUE(Move(GI, SQ_2B) == *iter);
    ++iter;
    CHECK_TRUE(Move(FU, SQ_3C) == *iter);
    ++iter;
    CHECK_TRUE(Move(GI, SQ_3C) == *iter);
    ++iter;
    CHECK_TRUE(Move(FU, SQ_4D) == *iter);
    ++iter;
    CHECK_TRUE(Move(GI, SQ_4D) == *iter);
    ++iter;
    CHECK_FALSE(iter != iter.end());
}

TEST(test_drop_evasion_iterator_minishogi, non_ranging_checker)
{
    const auto s = State("4k/4G/5/5/5 w p");
    auto iter = vshogi::DropEvasionIterator<Parameters>(s);
    CHECK_FALSE(iter != iter.end());
}

TEST(test_drop_evasion_iterator_minishogi, in_double_check)
{
    const auto s = State("r3k/5/5/5/K3r b PSG");
    auto iter = vshogi::DropEvasionIterator<Parameters>(s);
    CHECK_FALSE(iter != iter.end());
    CHECK_TRUE(iter.is_end());
}

} // namespace test_vshogi::test_minishogi

namespace test_vshogi::test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST_GROUP (test_drop_evasion_iterator_judkins_shogi) {
};

TEST(test_drop_evasion_iterator_judkins_shogi, immobile_knight)
{
    const auto s = State("5K/6/6/2b3/6/6 b N");
    auto iter = vshogi::DropEvasionIterator<Parameters>(s);
    // CHECK_TRUE(Move(KE, SQ_2B) == *iter); immobile knight
    CHECK_TRUE(Move(KE, SQ_3C) == *iter);
    ++iter;
    CHECK_FALSE(iter != iter.end());
    CHECK_TRUE(iter.is_end());
}

} // namespace test_vshogi::test_judkins_shogi

namespace test_vshogi::test_shogi
{

using namespace vshogi::shogi;

TEST_GROUP (test_drop_evasion_iterator_shogi) {
};

TEST(test_drop_evasion_iterator_shogi, two_pawns_in_a_file)
{
    // two pawns in a file
    const auto s = State("9/9/9/9/9/9/3PpP3/9/+r7K b P");
    auto iter = vshogi::DropEvasionIterator<Parameters>(s);
    CHECK_TRUE(Move(FU, SQ_2I) == *iter);
    ++iter;
    CHECK_TRUE(Move(FU, SQ_3I) == *iter);
    ++iter;
    // CHECK_TRUE(Move(FU, SQ_4I) == *iter); two pawns in a file
    CHECK_TRUE(Move(FU, SQ_5I) == *iter);
    ++iter;
    // CHECK_TRUE(Move(FU, SQ_6I) == *iter); two pawns in a file
    CHECK_TRUE(Move(FU, SQ_7I) == *iter);
    ++iter;
    CHECK_TRUE(Move(FU, SQ_8I) == *iter);
    ++iter;
    CHECK_FALSE(iter != iter.end());
    CHECK_TRUE(iter.is_end());
}

TEST(test_drop_evasion_iterator_shogi, immobile_lance)
{
    {
        const auto s = State("r1K6/9/9/9/9/9/9/9/9 b L");
        auto iter = vshogi::DropEvasionIterator<Parameters>(s);
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
    {
        const auto s = State("9/r1K6/9/9/9/9/9/9/9 b L");
        auto iter = vshogi::DropEvasionIterator<Parameters>(s);
        CHECK_TRUE(Move(KY, SQ_8B) == *iter);
        ++iter;
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
}

} // namespace test_vshogi::test_shogi
