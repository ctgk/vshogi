#include "vshogi/engine/dfpn.hpp"
#include "vshogi/minishogi/game.hpp"
#include <CppUTest/TestHarness.h>
#include <iostream>

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
    auto g = Game("2k2/5/2GB1/5/2K2 b -");
    auto root = Node();
    {
        auto g_tmp = Game(g);
        root.select_simulate_expand_backprop(g_tmp);
        CHECK_EQUAL(1, root.get_pn());
        CHECK_EQUAL(4, root.get_dn());
    }
    for (int ii = 10; ii--;) {
        if (root.found_mate())
            break;
        auto g_tmp = Game(g);
        root.select_simulate_expand_backprop(g_tmp);
    }

    const auto actual = root.get_mate_moves();
    CHECK_EQUAL(1, actual.size());
    CHECK_TRUE(Move(Squares::SQ_3B, Squares::SQ_3C) == actual[0]);
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
    auto g = Game("5/2p2/5/2K2/5 w 2g");
    auto searcher = Searcher();
    searcher.set_root(g);
    CHECK_TRUE(searcher.explore(100));
    CHECK_TRUE(searcher.found_mate());
    const auto actual = searcher.get_mate_moves();
    CHECK_EQUAL(3, actual.size());
    CHECK_TRUE(Move(Squares::SQ_3C, Pieces::KI) == actual[0]);
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
    auto g = Game("3bk/3s1/3BP/4+R/4K b -");
    auto root = Node();
    {
        CHECK_FALSE(root.found_mate());
        auto g_tmp = Game(g);
        root.select_simulate_expand_backprop(g_tmp);
        // root
        CHECK_EQUAL(1, root.get_pn());
        CHECK_EQUAL(1, root.get_dn());
    }
    {
        CHECK_FALSE(root.found_mate());
        auto g_tmp = Game(g);
        root.select_simulate_expand_backprop(g_tmp);
        // root -> Move(B1, C1)
        CHECK_EQUAL(1, root.get_pn());
        CHECK_EQUAL(1, root.get_dn());
    }
    {
        CHECK_FALSE(root.found_mate());
        auto g_tmp = Game(g);
        root.select_simulate_expand_backprop(g_tmp);
        // root -> Move(B1, C1) -> Move(B1, A2)
        CHECK_EQUAL(1, root.get_pn());
        CHECK_EQUAL(1, root.get_dn());
    }
    {
        CHECK_FALSE(root.found_mate());
        auto g_tmp = Game(g);
        root.select_simulate_expand_backprop(g_tmp);
        // root -> Move(B1, C1) -> Move(B1, A2) -> Move(B1, D1)
        const auto actual = root.get_mate_moves();
        CHECK_EQUAL(3, actual.size());
        CHECK_TRUE(Move(Squares::SQ_1B, Squares::SQ_1C) == actual[0]);
        CHECK_TRUE(Move(Squares::SQ_1B, Squares::SQ_2A) == actual[1]);
        CHECK_TRUE(Move(Squares::SQ_1B, Squares::SQ_1D) == actual[2]);
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
    auto g = Game("2sgk/5/3RG/5/4K b R");
    auto root = Node();
    for (int ii = 500; ii--;) {
        if (root.found_mate())
            break;
        auto g_tmp = Game(g);
        root.select_simulate_expand_backprop(g_tmp);
    }
    const auto actual = root.get_mate_moves();
    CHECK_EQUAL(3, actual.size());
    CHECK_TRUE(Move(Squares::SQ_1B, Squares::SQ_1C) == actual[0]);
    if (actual[1] == Move(Squares::SQ_1B, Squares::SQ_1A)) {
        CHECK_TRUE(Move(Squares::SQ_1C, Pieces::HI) == actual[2]);
    } else {
        CHECK_TRUE(Move(Squares::SQ_1B, Squares::SQ_2A) == actual[1]);
        CHECK_TRUE(Move(Squares::SQ_2A, Pieces::HI) == actual[2]);
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
    auto g = Game("2pkb/4R/2+bG1/5/5 b 2S");
    auto root = Node();
    for (int ii = 2000; ii--;) {
        if (root.found_mate())
            break;
        auto g_tmp = Game(g);
        root.select_simulate_expand_backprop(g_tmp);
    }
    const auto actual = root.get_mate_moves();
    CHECK_EQUAL(5, actual.size());
    CHECK_TRUE(Move(Squares::SQ_2B, Pieces::GI) == actual[0]);
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
    auto g = Game("2k2/5/5/5/2K2 b -");
    auto root = Node();
    root.select_simulate_expand_backprop(g);
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
    auto g = Game("2k2/5/1+P3/5/5 b 2S");
    auto searcher = Searcher();
    searcher.set_root(g);
    CHECK_FALSE(searcher.explore(10000));
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
}

} // namespace test_vshogi::test_engine
