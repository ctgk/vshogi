#include "vshogi/common/generators/soldier.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi
{

TEST_GROUP (soldier_move_iterator) {
    template <vshogi::GenEnum GenType, class P>
    void compare_moves(
        const std::string& sfen, const std::vector<std::string>& expect)
    {
        const auto s = vshogi::State<P>(sfen);
        auto iter = vshogi::SoldierMoveGenerator<P, GenType>(s);
        for (unsigned int ii = 0u; ii < expect.size(); ++ii) {
            const auto a = *iter;
            const auto e = vshogi::Move<P>(expect[ii].c_str());
            CHECK_EQUAL(e.hash(), a.hash());
            ++iter;
        }
        CHECK_FALSE(iter);
    }
};

TEST(soldier_move_iterator, minishogi_legal)
{
    auto compare = [this](
                       const std::string& sfen,
                       const std::vector<std::string>& expect) {
        compare_moves<vshogi::GenEnum::LEGAL, vshogi::minishogi::Parameters>(
            sfen, expect);
    };

    compare("4k/5/5/5/K4 b -", {}); // no soldiers on the board

    compare("4k/5/2P2/s4/K3r b -", {}); // in double check

    compare("4k/G4/5/4g/3SK b -", {"2e1d"}); // in check by adjacent piece

    // in check by sliding piece
    compare("4k/5/5/3S1/K3r b -", {"2d1e", "2d3e"});
    compare("4k/P2bp/5/G4/K3P b -", {"5d4d"});

    // forced promotion
    compare("4k/P4/5/K4/4P b -", {"1e1d", "5b5a+"});

    // pinned
    compare("+b3k/5/5/3S1/4K b -", {"2d3c"}); // skip 2d2c, 2d1c, 2d3e

    // unpromotable

    compare("B3k/1p3/5/5/K4 b -", {"5a4b", "5a4b+"});
    compare("B3k/1pP2/5/5/K4 b -", {"3b3a+", "5a4b", "5a4b+"});
}

TEST(soldier_move_iterator, minishogi_check)
{
    auto compare = [this](
                       const std::string& sfen,
                       const std::vector<std::string>& expect) {
        compare_moves<vshogi::GenEnum::CHECK, vshogi::minishogi::Parameters>(
            sfen, expect);
    };

    // no soldiers on the board
    compare("4k/5/5/5/K4 b -", {});

    // cannot give a check to the opponent king
    compare("4k/5/2P2/5/K4 b -", {});

    // check by sliding piece
    compare("2B1k/5/5/5/K4 b -", {"3a2b", "3a2b+"});

    // check that requires promotion
    compare("B4/5/5/4k/K4 b -", {"5a1e+", "5a2d+"});

    // in double check
    compare("r1k1b/5/5/5/K3R b -", {});

    // in check by adjacent piece
    compare("5/5/k1S2/1s3/KP3 b -", {"3c4d" /*3c4b*/});

    // in check by sliding piece
    compare("2k2/3b1/3+R1/5/K4 b -", {"2c2b", "2c3c"});

    // pinned
    compare("1k3/5/5/5/K2R+r b -", {"2e4e" /*"2e2a"*/});

    // discovered check
    compare("5/4k/3P1/3R+B/K3R b -", {"1d1c", "1d2e"});

    // unpromotable

    compare("B1S1k/5/5/5/K4 b -", {"3a2b", "3a2b+", "5a3c", "5a3c+"});
    compare("2B1k/5/3P1/3GK/4R b P", {"3a2b", "3a2b+" /*1d2e*/});
}

TEST(soldier_move_iterator, minishogi_evade)
{
    auto compare = [this](
                       const std::string& sfen,
                       const std::vector<std::string>& expect) {
        compare_moves<vshogi::GenEnum::EVADE, vshogi::minishogi::Parameters>(
            sfen, expect);
    };

    // no soldiers on the board
    compare("r3k/5/5/5/K4 b PSG", {});

    // in double check
    compare("4k/5/2P2/s4/K3r b -", {});

    // in check by adjacent piece
    compare("2pk+R/3b1/2+bG1/5/5 w Ss", {"2b1a"});

    // in check by sliding piece
    compare("4k/5/5/3S1/K3r b -", {"2d1e", "2d3e"});
    compare("4k/P2bp/5/G4/K3P b -", {"5d4d"});

    // pinned
    compare("+b3k/4r/5/3S1/3GK b -", {"2e1d" /*2d1c*/});

    // unpromotable
    compare("K3r/G4/5/5/4k b -", {"5b4a" /*5b4a+*/});
}

TEST(soldier_move_iterator, judkins_shogi_legal)
{
    auto compare
        = [this](
              const std::string& sfen, const std::vector<std::string>& expect) {
              compare_moves<
                  vshogi::GenEnum::LEGAL,
                  vshogi::judkins_shogi::Parameters>(sfen, expect);
          };

    // no soldiers on the board
    // in double check
    // in check by adjacent piece
    // in check by sliding piece

    // forced promotion
    compare("5k/6/6/2N3/6/6 b -", {"4d3b+", "4d5b+"});

    // pinned
}

TEST(soldier_move_iterator, judkins_shogi_check)
{
    auto compare
        = [this](
              const std::string& sfen, const std::vector<std::string>& expect) {
              compare_moves<
                  vshogi::GenEnum::CHECK,
                  vshogi::judkins_shogi::Parameters>(sfen, expect);
          };

    // no soldiers on the board

    // cannot give a check to the opponent king

    // check by sliding piece

    // check that requires promotion

    // in double check

    // in check by adjacent piece

    // in check by sliding piece

    // pinned

    // discovered check
    compare("5k/6/6/2N3/6/B4K b -", {"4d3b+", "4d5b+"});
}

TEST(soldier_move_iterator, standard_shogi_check)
{
    auto compare
        = [this](
              const std::string& sfen, const std::vector<std::string>& expect) {
              compare_moves<vshogi::GenEnum::CHECK, vshogi::shogi::Parameters>(
                  sfen, expect);
          };

    // no soldiers on the board

    // cannot give a check to the opponent king

    // check by sliding piece

    // check that requires promotion

    // in double check

    // in check by adjacent piece

    // in check by sliding piece

    // pinned

    // discovered check
    compare("b8/8k/9/9/9/9/6l2/9/8K w -", {"3g3h", "3g3h+", "3g3i+"});
}

} // namespace test_vshogi
