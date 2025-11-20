#include <string>
#include <vector>

#include "vshogi/common/iterator/drop.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi
{

TEST_GROUP (drop_move_iterator) {
    template <vshogi::IterEnum IterType, class P>
    void compare_moves(
        const std::string& sfen, const std::vector<std::string>& expect)
    {
        const auto s = vshogi::State<P>(sfen);
        auto iter = vshogi::DropMoveIterator<P, IterType>(s);
        for (unsigned int ii = 0u; ii < expect.size(); ++ii) {
            const auto a = *iter;
            const auto e = vshogi::Move<P>(expect[ii].c_str());
            CHECK_EQUAL(e.hash(), a.hash());
            ++iter;
        }
        CHECK_FALSE(iter);
    }
};

TEST(drop_move_iterator, minishogi_legal)
{
    auto compare = [this](
                       const std::string& sfen,
                       const std::vector<std::string>& expect) {
        compare_moves<vshogi::IterEnum::LEGAL, vshogi::minishogi::Parameters>(
            sfen, expect);
    };

    // empty stand
    compare("5/5/5/5/5 b -", {});

    // in check by adjacent piece
    compare("4k/4G/5/5/5 w p", {});

    // in check by sliding piece
    compare("4k/5/5/1B3/5 w ps", {"P*2b", "P*3c", "S*2b", "S*3c"});

    // in double check
    compare("3kb/5/5/5/K3r b R", {});
    compare("r3k/5/5/5/K3r b PSG", {});

    // two pawns in a file
    compare("3rk/R4/5/P4/KPPP1 b P", {"P*1c", "P*1d", "P*1e"});

    // pawn final rank
    compare("sssss/sssss/sssss/ssss1/ssss1 w pg", {"P*1d", "G*1d", "G*1e"});
}

TEST(drop_move_iterator, minishogi_check)
{
    auto compare = [this](
                       const std::string& sfen,
                       const std::vector<std::string>& expect) {
        compare_moves<vshogi::IterEnum::CHECK, vshogi::minishogi::Parameters>(
            sfen, expect);
    };

    // empty stand
    compare("5/5/5/5/5 b -", {});

    // in check by adjacent piece
    compare("4k/4G/5/5/4K w p", {});

    // in double check
    compare("3kb/5/5/5/K3r b R", {});

    // nominal case
    compare("4k/5/5/5/5 b P", {"P*1b"});
    compare("4k/5/5/1P3/5 b BP", {"P*1b", "B*2b", "B*3c"});

    // two pawns in a file
    compare("4k/5/5/5/4P b P", {});

    // drop pawn mate
    compare("3pk/5/4S/5/5 b RP", {"R*1b"});
}

TEST(drop_move_iterator, minishogi_evade)
{
    auto compare = [this](
                       const std::string& sfen,
                       const std::vector<std::string>& expect) {
        compare_moves<vshogi::IterEnum::EVADE, vshogi::minishogi::Parameters>(
            sfen, expect);
    };

    // empty stand
    compare("4k/5/5/1B3/5 w -", {});

    // in check by adjacent piece
    compare("r4/2k2/4P/P+bB2/K1S2 b R2gs", {});
    compare("4k/4G/5/5/5 w p", {});

    // in double check
    compare("r3k/5/5/5/K3r b PSG", {});

    // nominal case
    compare("4k/5/5/1B3/5 w ps", {"P*2b", "S*2b", "P*3c", "S*3c"});

    // two pawns in a file
    compare("3pk/5/5/1B3/5 w p", {"P*3c"}); // note skipping "P*2b"

    // immobile pawn
    compare("r3K/5/5/5/5 b P", {});

    // drop pawn mate
    compare("G1kb+R/5/2G2/K4/5 b P", {"P*4c"}); // note skipping "P*3b"
}

TEST(drop_move_iterator, judkins_shogi_evade)
{
    auto compare
        = [this](
              const std::string& sfen, const std::vector<std::string>& expect) {
              compare_moves<
                  vshogi::IterEnum::EVADE,
                  vshogi::judkins_shogi::Parameters>(sfen, expect);
          };
    // empty stand
    // in check by adjacent piece
    // in double check
    // nominal case
    // two pawns in a file
    // immobile pawn
    // immobile knight
    compare("5K/6/6/2b3/6/6 b N", {"N*3c"}); // skipping N*2b
    // drop pawn mate
}

TEST(drop_move_iterator, standard_shogi_check)
{
    auto compare
        = [this](
              const std::string& sfen, const std::vector<std::string>& expect) {
              compare_moves<vshogi::IterEnum::CHECK, vshogi::shogi::Parameters>(
                  sfen, expect);
          };

    // empty stand
    // in check by adjacent piece
    // in double check
    // nominal case
    // two pawns in a file
    compare("3k5/9/9/9/9/9/7g1/9/5rPKL w p", {}); // drop pawn mate
}

TEST(drop_move_iterator, standard_shogi_evade)
{
    auto compare
        = [this](
              const std::string& sfen, const std::vector<std::string>& expect) {
              compare_moves<vshogi::IterEnum::EVADE, vshogi::shogi::Parameters>(
                  sfen, expect);
          };
    // empty stand
    // in check by adjacent piece
    // in double check
    // nominal case
    // two pawns in a file
    compare("9/9/9/9/9/9/1PpPpPpP1/9/+r7K b P", {"P*3i", "P*5i", "P*7i"});
    // immobile pawn

    {
        compare("r1K6/9/9/9/9/9/9/9/9 b L", {}); // immobile lance
        compare("9/r1K6/9/9/9/9/9/9/9 b L", {"L*8b"});
    }

    // immobile knight
    // drop pawn mate
}

} // namespace test_vshogi
