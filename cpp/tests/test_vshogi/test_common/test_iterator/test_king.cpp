#include <string>
#include <vector>

#include "vshogi/common/iterator/king.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi
{

TEST_GROUP (king_move_iterator) {
    template <vshogi::IterEnum IterType, class P>
    void compare_moves(
        const std::string& sfen, const std::vector<std::string>& expect)
    {
        const auto s = vshogi::State<P>(sfen);
        auto iter = vshogi::KingMoveIterator<P, IterType>(s);
        for (unsigned int ii = 0u; ii < expect.size(); ++ii) {
            const auto a = *iter;
            const auto e = vshogi::Move<P>(expect[ii].c_str());
            CHECK_EQUAL(e.hash(), a.hash());
            ++iter;
        }
        CHECK_FALSE(iter != iter.end());
        CHECK_TRUE(iter.is_end());
    }
};

TEST(king_move_iterator, minishogi_legal)
{
    auto compare = [this](
                       const std::string& sfen,
                       const std::vector<std::string>& expect) {
        compare_moves<vshogi::IterEnum::LEGAL, vshogi::minishogi::Parameters>(
            sfen, expect);
    };

    // no piece to move
    compare("5/5/5/5/5 b -", {});

    // nominal case
    compare("5/2k2/5/2K2/5 b -", {"3d2d", "3d2e", "3d3e", "3d4d", "3d4e"});
    compare("R1+S2/4k/S2+bg/PK3/3B1 w r 40", {"1b1a", "1b2b"});
}

TEST(king_move_iterator, minishogi_check)
{
    auto compare = [this](
                       const std::string& sfen,
                       const std::vector<std::string>& expect) {
        compare_moves<vshogi::IterEnum::CHECK, vshogi::minishogi::Parameters>(
            sfen, expect);
    };

    // no piece to move
    compare("5/5/5/5/5 b -", {});

    // no piece to check
    compare("4k/5/2K2/5/5 b -", {});

    // nominal case
    compare("4k/5/4K/5/4R b -", {"1c2c", "1c2d"});
}

TEST(king_move_iterator, judkins_shogi_legal)
{
    auto compare
        = [this](
              const std::string& sfen, const std::vector<std::string>& expect) {
              compare_moves<
                  vshogi::IterEnum::LEGAL,
                  vshogi::judkins_shogi::Parameters>(sfen, expect);
          };

    compare("5k/6/6/5N/6/6 w -", {"1a1b", "1a2a"});
}

} // namespace test_vshogi
