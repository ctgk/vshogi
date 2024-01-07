#include "vshogi/engine/dfpn.hpp"
#include "vshogi/judkins_shogi/game.hpp"
#include "vshogi/minishogi/game.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

TEST_GROUP(dfpn){};

TEST(dfpn, mate_in_one_black)
{
    using namespace vshogi::minishogi;
    using Node = vshogi::engine::dfpn::Node<Game, Move>;

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-OU|   |   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |+KI|+KA|   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |+OU|   |   |
    //   *---*---*---*---*---*
    // Black: -
    auto root = Node(Game("2k2/5/2GB1/5/2K2 b -"));
    root.select_simulate_expand_backprop();
    CHECK_EQUAL(1, root.get_pn());
    CHECK_EQUAL(4, root.get_dn());
    for (int ii = 10; ii--;) {
        if (root.found_mate())
            break;
        root.select_simulate_expand_backprop();
    }

    const auto actual = root.get_mate_moves();
    CHECK_EQUAL(1, actual.size());
    CHECK_TRUE(Move(SQ_3B, SQ_3C) == actual[0]);
}

TEST(dfpn, mate_in_three_white)
{
    using namespace vshogi::minishogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Game, Move>;

    // Turn: White
    // White: KIx2
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |   |   |   |
    //   *---*---*---*---*---*
    // B |   |   |-FU|   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |+OU|   |   |
    //   *---*---*---*---*---*
    // E |   |   |   |   |   |
    //   *---*---*---*---*---*
    // Black: -
    auto searcher = Searcher(Game("5/2p2/5/2K2/5 w 2g"));
    CHECK_TRUE(searcher.explore(100));
    CHECK_TRUE(searcher.found_mate());
    const auto actual = searcher.get_mate_moves();
    CHECK_EQUAL(3, actual.size());
    CHECK_TRUE(Move(SQ_3C, KI) == actual[0]);
}

TEST(dfpn, mate_in_three_straight_forward)
{
    using namespace vshogi::minishogi;
    using Node = vshogi::engine::dfpn::Node<Game, Move>;

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |   |-KA|-OU|
    //   *---*---*---*---*---*
    // B |   |   |   |-GI|   |
    //   *---*---*---*---*---*
    // C |   |   |   |+KA|+FU|
    //   *---*---*---*---*---*
    // D |   |   |   |   |+RY|
    //   *---*---*---*---*---*
    // E |   |   |   |   |+OU|
    //   *---*---*---*---*---*
    // Black: HI
    auto root = Node(Game("3bk/3s1/3BP/4+R/4K b -"));
    {
        CHECK_FALSE(root.found_mate());
        root.select_simulate_expand_backprop();
        // root
        CHECK_EQUAL(1, root.get_pn());
        CHECK_EQUAL(1, root.get_dn());
    }
    {
        CHECK_FALSE(root.found_mate());
        root.select_simulate_expand_backprop();
        // root -> Move(B1, C1)
        CHECK_EQUAL(1, root.get_pn());
        CHECK_EQUAL(1, root.get_dn());
    }
    {
        CHECK_FALSE(root.found_mate());
        root.select_simulate_expand_backprop();
        // root -> Move(B1, C1) -> Move(B1, A2)
        CHECK_EQUAL(1, root.get_pn());
        CHECK_EQUAL(1, root.get_dn());
    }
    {
        CHECK_FALSE(root.found_mate());
        root.select_simulate_expand_backprop();
        // root -> Move(B1, C1) -> Move(B1, A2) -> Move(B1, D1)
        const auto actual = root.get_mate_moves();
        CHECK_EQUAL(3, actual.size());
        CHECK_TRUE(Move(SQ_1B, SQ_1C) == actual[0]);
        CHECK_TRUE(Move(SQ_1B, SQ_2A) == actual[1]);
        CHECK_TRUE(Move(SQ_1B, SQ_1D) == actual[2]);
    }
}

TEST(dfpn, mate_in_three)
{
    using namespace vshogi::minishogi;
    using Node = vshogi::engine::dfpn::Node<Game, Move>;

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-GI|-KI|-OU|
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |+HI|+KI|
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |   |   |+OU|
    //   *---*---*---*---*---*
    // Black: HI
    auto root = Node(Game("2sgk/5/3RG/5/4K b R"));
    for (int ii = 500; ii--;) {
        if (root.found_mate())
            break;
        root.select_simulate_expand_backprop();
    }
    const auto actual = root.get_mate_moves();
    CHECK_EQUAL(3, actual.size());
    CHECK_TRUE(Move(SQ_1B, SQ_1C) == actual[0]);
    if (actual[1] == Move(SQ_1B, SQ_1A)) {
        CHECK_TRUE(Move(SQ_1C, HI) == actual[2]);
    } else {
        CHECK_TRUE(Move(SQ_1B, SQ_2A) == actual[1]);
        CHECK_TRUE(Move(SQ_2A, HI) == actual[2]);
    }
}

TEST(dfpn, mate_in_five)
{
    using namespace vshogi::minishogi;
    using Node = vshogi::engine::dfpn::Node<Game, Move>;

    // Turn: BLACK
    // White: FU
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-FU|-OU|-KA|
    //   *---*---*---*---*---*
    // B |   |   |   |   |+HI|
    //   *---*---*---*---*---*
    // C |   |   |-UM|+KI|   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |   |   |   |
    //   *---*---*---*---*---*
    // Black: GIx2
    auto root = Node(Game("2pkb/4R/2+bG1/5/5 b 2S"));
    for (int ii = 2000; ii--;) {
        if (root.found_mate())
            break;
        root.select_simulate_expand_backprop();
    }
    const auto actual = root.get_mate_moves();
    CHECK_EQUAL(5, actual.size());
    CHECK_TRUE(Move(SQ_2B, GI) == actual[0]);
}

TEST(dfpn, no_mate_no_check)
{
    using namespace vshogi::minishogi;
    using Node = vshogi::engine::dfpn::Node<Game, Move>;

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-OU|   |   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |+OU|   |   |
    //   *---*---*---*---*---*
    // Black: -
    auto root = Node(Game("2k2/5/5/5/2K2 b -"));
    root.select_simulate_expand_backprop();
    CHECK_FALSE(root.found_mate());
    CHECK_TRUE(root.found_no_mate());
}

TEST(dfpn, no_mate_1)
{
    using namespace vshogi::minishogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Game, Move>;

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-OU|   |   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |+TO|   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |   |   |   |
    //   *---*---*---*---*---*
    // Black: GIx2
    auto searcher = Searcher(Game("2k2/5/1+P3/5/5 b 2S"));
    CHECK_FALSE(searcher.explore(10000));
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
}

TEST(dfpn, king_entering_before_mate)
{
    using namespace vshogi::judkins_shogi;
    using Searcher = vshogi::engine::dfpn::Searcher<Game, Move>;
    // Turn: WHITE
    // White: -
    //     6   5   4   3   2   1
    //   +---+---+---+---+---+---+
    // A |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // B |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // C |   |   |   |-KE|   |   |
    //   +---+---+---+---+---+---+
    // D |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // E |+FU|   |   |   |   |-RY|
    //   +---+---+---+---+---+---+
    // F |   |+OU|   |   |   |-OU|
    //   +---+---+---+---+---+---+
    // Black: -
    auto searcher = Searcher(Game("6/6/3n2/6/P4+r/1K3k w -"));
    CHECK_TRUE(searcher.explore(100));
    CHECK_TRUE(searcher.found_mate());
    const auto actual = searcher.get_mate_moves();
    CHECK_EQUAL(2, actual.size());
    CHECK_TRUE(actual[0] == Move(SQ_4E, SQ_3C, true));
    CHECK_TRUE(actual[1] == Move(SQ_6F, SQ_5F));
}

} // namespace test_vshogi::test_engine
