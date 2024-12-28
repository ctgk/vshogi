#include "vshogi/engine/dfpn.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn
{

using namespace vshogi::engine::dfpn;

TEST_GROUP (dfpn_misc) {
};

TEST(dfpn_misc, had_two_consecutive_sacrifice_drops)
{
    using namespace vshogi::shogi;
    {
        auto g = Game("9/9/6k2/9/9/9/9/9/B8 w 8p");
        g.apply_dfpn(Move(SQ_8H, FU));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_8H, SQ_9I));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_7G, FU));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_7G, SQ_8H));
        CHECK_TRUE(had_two_consecutive_sacrifice_drops(g));
    }
    {
        auto g = Game("9/9/6k2/9/9/6N2/1p7/9/B5L2 w 8p");
        g.apply_dfpn(Move(SQ_8H, SQ_8G));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_8H, SQ_9I));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_4D, FU));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_4D, SQ_3F));
        // false because 1st sacrifice is not a drop move.
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
    }
    {
        auto g = Game("9/9/6k2/9/9/6N2/9/9/B5L2 w 8p");
        g.apply_dfpn(Move(SQ_8H, FU));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_4D, SQ_3F));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_3E, FU));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_3E, SQ_3F));
        // false because 1st sacrifice is not captured.
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
    }
    {
        auto g = Game("9/9/5pk2/9/9/9/9/9/B8 w 8p");
        g.apply_dfpn(Move(SQ_8H, FU));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_8H, SQ_9I));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_4D, SQ_4C));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_4D, SQ_8H));
        // false because 2nd sacrifice is not a drop move.
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
    }
    {
        auto g = Game("9/9/6k2/9/9/6N2/9/9/B5L2 w 8p");
        g.apply_dfpn(Move(SQ_8H, FU));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_8H, SQ_9I));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_4D, FU));
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
        g.apply_dfpn(Move(SQ_4D, SQ_3F));
        // false because different pieces carried out
        // the 1st ("9i8h") and the 2nd ("3f4d") captures.
        CHECK_FALSE(had_two_consecutive_sacrifice_drops(g));
    }
}

TEST_GROUP (dfpn_node) {
};

TEST(dfpn_node, init)
{
    using namespace vshogi::minishogi;
    using Node = Node<Parameters>;
    {
        auto n = Node();
        CHECK_TRUE(n.is_attacker());
        CHECK_FALSE(n.has_child());
        CHECK_EQUAL(unit, n.pn());
        CHECK_EQUAL(unit, n.dn());
    }
    {
        auto n = Node(false, Move(SQ_1A, SQ_1B));
        CHECK_FALSE(n.is_attacker());
        CHECK_FALSE(n.has_child());
        CHECK_EQUAL(unit, n.pn());
        CHECK_EQUAL(unit, n.dn());
        CHECK_EQUAL(Move(SQ_1A, SQ_1B).hash(), n.get_action().hash());
    }
}

TEST(dfpn_node, expand)
{
    using namespace vshogi::minishogi;
    using Node = Node<Parameters>;
    {
        auto n = Node();
        n.expand(Game("k4/5/5/5/5 b -"), nullptr, nullptr);
        CHECK_FALSE(n.has_child());
        CHECK_EQUAL(max_number, n.pn());
        CHECK_EQUAL(zero, n.dn());
    }
    {
        auto n = Node();
        n.expand(Game("2kp+R/5/5/5/5 b -"), nullptr, nullptr);
        CHECK_TRUE(n.has_child());
        CHECK_EQUAL(unit, n.pn());
        CHECK_EQUAL(unit * 2u, n.dn());
    }
}

TEST(dfpn_node, expand_using_cousin)
{
    using namespace vshogi::minishogi;
    using Node = Node<Parameters>;
    {
        auto n = Node();
        auto cousin = Node();
        cousin.expand(Game("k2p+R/5/5/5/5 b -"), nullptr, nullptr);
        n.expand(Game("2kp+R/5/5/5/5 b -"), &cousin, nullptr);
        CHECK_TRUE(n.has_child());
        CHECK_EQUAL(unit, n.pn());
        CHECK_EQUAL(unit, n.dn());
        auto ch = n.get_child();
        CHECK_EQUAL(Move(SQ_2A, SQ_1A).hash(), ch->get_action().hash());

        // No move from SQ_1A to SQ_2B
        CHECK_EQUAL(nullptr, ch->get_sibling());
    }
    {
        auto cousin = Node();
        // one child
        cousin.expand(Game("3rk/3p1/3BP/5/4K b -"), nullptr, nullptr);
        CHECK_EQUAL(unit, cousin.pn());
        CHECK_EQUAL(unit, cousin.dn());
        cousin.get_child_1st()->simulate(
            Game("4K/5/5/5/+r+sg+bk w p")); // not mate
        CHECK_TRUE(cousin.get_child_1st()->found_no_mate());

        auto atk_node = Node();
        atk_node.expand(Game("3rk/3p1/3BP/5/4K b B"), &cousin, nullptr);
        CHECK_TRUE(atk_node.has_child());

        // stronger stand may lead to mate even if nibling is not mate.
        CHECK_FALSE(atk_node.found_conclusion());
        CHECK_EQUAL(kilo, atk_node.pn());
        CHECK_EQUAL(cent, atk_node.dn());
    }
    {
        auto cousin = Node(false, Move(SQ_1A, SQ_1B));
        // one child
        cousin.expand(Game("3rk/3pG/4R/2b2/1B2K w -"), nullptr, nullptr);
        {
            auto ch = cousin.get_child_1st();
            // one child
            ch->expand(Game("3rk/3pb/4R/5/1B2K b g"), nullptr, nullptr);
            ch->get_child_1st()->simulate(Game("3rk/3pR/5/5/1B2K w Bg"));
            CHECK_TRUE(ch->get_child_1st()->found_mate());
            ch->backprop_one(Game("3rk/3pb/4R/5/1B2K b g"));
        }
        CHECK_TRUE(cousin.get_child_1st()->found_mate());

        auto def_node = Node(false, Move(SQ_1A, SQ_1B));
        def_node.expand(Game("3rk/3pG/4R/2b2/1B2K w g"), &cousin, nullptr);
        CHECK_TRUE(def_node.has_child());

        // stronger stand may lead to no-mate even if nibling is mate.
        CHECK_FALSE(def_node.found_conclusion());
        CHECK_EQUAL(cent, def_node.pn());
        CHECK_EQUAL(kilo, def_node.dn());
    }
}

TEST(dfpn_node, expand_removes_no_promotion_moves_by_rook)
{
    using namespace vshogi::minishogi;
    using Node = Node<Parameters>;

    auto n = Node();
    n.expand(Game("k2pR/5/5/5/5 b -"), nullptr, nullptr);
    CHECK_TRUE(n.has_child());
    auto ch = n.get_child();
    CHECK_TRUE(ch != nullptr);
    CHECK_EQUAL(Move(SQ_2A, SQ_1A, true).hash(), ch->get_action().hash());
    ch = ch->get_sibling();
    CHECK_TRUE(ch == nullptr);
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(unit, n.dn());
}

TEST_GROUP (dfpn_transposition_table) {
};

TEST(dfpn_transposition_table, init)
{
    using namespace vshogi::minishogi;
    using Node = Node<Parameters>;
    using TranspositionTable = TranspositionTable<Parameters>;

    auto g = Game("4k/5/4G/5/5 b G");
    auto table = TranspositionTable();
    auto root = table.get_root();
    auto n = Node(!root->is_attacker(), Move(SQ_1B, SQ_1C));
    g.apply(Move(SQ_1B, SQ_1C));
    table.add(&n, g);
    CHECK_TRUE(table.look_up_fuzzy(g) == &n);

    table.clear();
    CHECK_TRUE(table.look_up_fuzzy(g) == nullptr);
}

TEST(dfpn_transposition_table, look_up_offence)
{
    using namespace vshogi::minishogi;
    using Node = Node<Parameters>;
    using TranspositionTable = TranspositionTable<Parameters>;

    auto g = Game("4k/5/4G/5/5 b G");
    auto table = TranspositionTable();
    auto n = Node();
    table.add(&n, g);

    CHECK_TRUE(table.look_up_fuzzy(Game("4k/5/4G/5/5 b -")) == nullptr);
    CHECK_TRUE(table.look_up_fuzzy(Game("4k/5/4G/5/5 b G")) == &n);
    CHECK_TRUE(table.look_up_fuzzy(Game("4k/5/4G/5/5 w G")) == nullptr);
    CHECK_TRUE(table.look_up_fuzzy(Game("4k/5/4G/5/5 b 2G")) == &n);
}

TEST(dfpn_transposition_table, look_up_defence)
{
    using namespace vshogi::minishogi;
    using Node = Node<Parameters>;
    using TranspositionTable = TranspositionTable<Parameters>;
    auto table = TranspositionTable();
    auto g = Game("4k/5/4P/5/5 b s");
    g.apply(Move(SQ_1B, SQ_1C));
    auto r = table.get_root();
    auto n = Node(!r->is_attacker(), Move(SQ_1B, SQ_1C)); // 4k/4P/5/5/5 w s
    table.add(&n, g);

    CHECK_TRUE(table.look_up_fuzzy(Game("4k/4P/5/5/5 w -")) == nullptr);
    CHECK_TRUE(table.look_up_fuzzy(Game("4k/4P/5/5/5 w s")) == &n);
    CHECK_TRUE(table.look_up_fuzzy(Game("4k/4P/5/5/5 w sg")) == &n);
}

TEST(dfpn_transposition_table, look_up_offence_stronger_of_two_weakers)
{
    using namespace vshogi::minishogi;
    using Node = Node<Parameters>;
    using TranspositionTable = TranspositionTable<Parameters>;

    auto table = TranspositionTable();
    auto root = table.get_root();
    auto g1 = Game("4k/5/4G/5/5 b G");
    auto n1 = Node();
    table.add(&n1, g1);
    auto g2 = Game("4k/5/4G/5/5 b 2G");
    auto n2 = Node();
    table.add(&n2, g2);

    CHECK_TRUE(table.look_up_fuzzy(Game("4k/5/4G/5/5 b -")) == nullptr);
    CHECK_TRUE(table.look_up_fuzzy(Game("4k/5/4G/5/5 b G")) == &n1);
    CHECK_TRUE(table.look_up_fuzzy(Game("4k/5/4G/5/5 b 2G")) == &n2);
    CHECK_TRUE(table.look_up_fuzzy(Game("4k/5/4G/5/5 b 2GS")) == &n2);
}

TEST_GROUP (dfpn_searcher) {
};

TEST(dfpn_searcher, no_mate_no_check)
{
    using namespace vshogi::minishogi;

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
    auto searcher = Searcher<Parameters>();
    searcher.set_game(Game("2k2/5/5/5/2K2 b -"));
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
    CHECK_FALSE(searcher.search(1u));
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
}

TEST(dfpn_searcher, minishogi_no_mate_1)
{
    using namespace vshogi::minishogi;
    auto searcher = Searcher<Parameters>();
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
    // Black: GI
    searcher.set_game(Game("2k2/5/1+P3/5/5 b S"));
    searcher.search(100);
    const auto num_searched = searcher.get_search_count();
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
    CHECK_EQUAL(70, num_searched);
}

TEST(dfpn_searcher, minishogi_no_mate_2)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;
    auto searcher = Searcher();

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
    searcher.set_game(Game("2k2/5/1+P3/5/5 b 2S"));
    searcher.search(1100);
    const auto num_searched = searcher.get_search_count();
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());
    CHECK_COMPARE(700, <, num_searched);
    CHECK_COMPARE(num_searched, <, 800);
}

TEST(dfpn_searcher, no_mate_1)
{
    using namespace vshogi::shogi;
    using Searcher = Searcher<Parameters>;
    // Turn: WHITE
    // White: FUx3,KY,KE,GI
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |-KY|   |   |   |   |   |-KI|   |-KY|
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |   |   |   |   |-KI|-OU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // C |   |-FU|   |   |   |   |-KE|-FU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |   |+RY|   |-FU|-FU|   |-FU|
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |-FU|   |-FU|   |   |+FU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |-FU|   |   |   |   |+KE|+FU|   |+FU|
    //   +---+---+---+---+---+---+---+---+---+
    // G |   |+FU|   |+GI|   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |+FU|+KI|   |+GI|+KA|   |-TO|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |+KE|+OU|   |+KA|   |   |-RY|+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FU,GI,KI
    auto g = Game("l5g1l/6gk1/1p4np1/3+R1pp1p/2p1p2P1/p4NP1P/1P1S5/PG1SB1+p2/"
                  "1NK1B2+rL w GSPsnl3p 134");
    auto searcher = Searcher();
    searcher.set_game(g);
    CHECK_FALSE(searcher.search(5000));
    CHECK_TRUE(searcher.found_no_mate());
    CHECK_EQUAL(1248, searcher.get_search_count());
}

TEST(dfpn_searcher, mate_in_one_straight_forward)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;
    auto g = Game("3rk/3b1/4P/4R/4K b -");

    auto searcher = Searcher();
    CHECK_FALSE(searcher.is_ready());
    searcher.set_game(g);
    CHECK_TRUE(searcher.is_ready());

    CHECK_TRUE(searcher.search(1u));
    CHECK_EQUAL(Move(SQ_1B, SQ_1C).hash(), searcher.get_mate_move().hash());
    CHECK_EQUAL(1, searcher.get_search_count());
}

TEST(dfpn_searcher, mate_in_one)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;
    auto searcher = Searcher();

    {
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
        searcher.set_game(Game("2k2/5/2GB1/5/2K2 b -"));
        CHECK_TRUE(searcher.search(10u));
        CHECK_TRUE(searcher.found_mate());
        CHECK_EQUAL(Move(SQ_3B, SQ_3C).hash(), searcher.get_mate_move().hash());
        CHECK_TRUE(searcher.get_search_count() <= 10u);
    }
    {
        // DISCOVERED CHECK

        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |-KA|   |
        //   *---*---*---*---*---*
        // B |   |   |-HI|   |-OU|
        //   *---*---*---*---*---*
        // C |   |   |   |-FU|+KA|
        //   *---*---*---*---*---*
        // D |   |   |   |-KI|+HI|
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: KI
        searcher.set_game(Game("3b1/2r1k/3pB/3gR/5 b G"));
        CHECK_TRUE(searcher.search(50));
        CHECK_TRUE(searcher.get_search_count() < 50u);
    }
}

TEST(dfpn_searcher, mate_in_three_straight_forward)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;

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
    auto searcher = Searcher();
    searcher.set_game(g);
    CHECK_FALSE(searcher.search(1u));
    CHECK_FALSE(searcher.search(1u));
    CHECK_TRUE(searcher.search(1u));
    CHECK_EQUAL(Move(SQ_1B, SQ_1C).hash(), searcher.get_mate_move().hash());
}

TEST(dfpn_searcher, mate_in_three_1)
{
    using namespace vshogi::minishogi;
    auto searcher = Searcher<Parameters>();
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
    searcher.set_game(Game("5/2p2/5/2K2/5 w 2g"));
    CHECK_TRUE(searcher.search(100));
    CHECK_TRUE(searcher.found_mate());
    CHECK_COMPARE(20, <, searcher.get_search_count());
    CHECK_COMPARE(searcher.get_search_count(), <, 30);
    CHECK_EQUAL(Move(SQ_3C, KI).hash(), searcher.get_mate_move().hash());
}

TEST(dfpn_searcher, mate_in_three_2)
{
    using namespace vshogi::minishogi;
    auto searcher = Searcher<Parameters>();
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
    searcher.set_game(Game("2sgk/5/3RG/5/4K b R"));
    CHECK_TRUE(searcher.search(1000));
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_TRUE(searcher.found_mate());
    CHECK_EQUAL(Move(SQ_1B, SQ_1C).hash(), searcher.get_mate_move().hash());
    const auto num_searched = searcher.get_search_count();
    CHECK_EQUAL(231, num_searched);
}

TEST(dfpn_searcher, mate_in_three_3)
{
    using namespace vshogi::judkins_shogi;
    auto searcher = Searcher<Parameters>();
    // Turn: BLACK
    // White: -
    //     6   5   4   3   2   1
    //   +---+---+---+---+---+---+
    // A |   |   |   |-GI|+GI|-HI|
    //   +---+---+---+---+---+---+
    // B |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // C |   |   |   |-FU|-OU|-KE|
    //   +---+---+---+---+---+---+
    // D |   |   |   |   |+KE|-FU|
    //   +---+---+---+---+---+---+
    // E |   |   |   |   |+RY|   |
    //   +---+---+---+---+---+---+
    // F |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // Black: KI
    auto g = Game("3sSr/6/3pkn/4Np/4+R1/6 b G");
    searcher.set_game(g);
    CHECK_TRUE(searcher.search(100u));
    CHECK_TRUE(searcher.found_mate());
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
}

TEST(dfpn_searcher, mate_in_three_4)
{
    using namespace vshogi::judkins_shogi;
    auto searcher = Searcher<Parameters>();
    // Turn: BLACK
    // White: -
    //     6   5   4   3   2   1
    //   +---+---+---+---+---+---+
    // A |   |   |   |-GI|+GI|-HI|
    //   +---+---+---+---+---+---+
    // B |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // C |   |   |   |-FU|-OU|-KE|
    //   +---+---+---+---+---+---+
    // D |   |   |   |   |+KE|-FU|
    //   +---+---+---+---+---+---+
    // E |   |   |   |   |+RY|   |
    //   +---+---+---+---+---+---+
    // F |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // Black: KI
    auto g = Game("3sSr/6/3pkn/4Np/4+R1/6 b Gpgbr");
    searcher.set_game(g);
    CHECK_TRUE(searcher.search(100u));
    CHECK_TRUE(searcher.found_mate());
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
}

TEST(dfpn_searcher, mate_in_three_by_king_move)
{
    using namespace vshogi::judkins_shogi;
    using Searcher = Searcher<Parameters>;
    auto searcher = Searcher();
    {
        // Turn: BLACK
        // White: FU
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |+HI|-KE|-HI|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |-OU|
        //   +---+---+---+---+---+---+
        // C |   |-UM|   |   |   |-FU|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+OU|   |   |
        //   +---+---+---+---+---+---+
        // E |   |   |+KA|   |   |-KI|
        //   +---+---+---+---+---+---+
        // F |   |   |   |-KI|   |   |
        //   +---+---+---+---+---+---+
        // Black: -
        searcher.set_game(Game("3Rnr/5k/1+b3p/3K2/2B2g/3g2 b p"));
        searcher.search(100);
        CHECK_TRUE(searcher.found_conclusion());
        CHECK_TRUE(searcher.found_mate());
        CHECK_EQUAL(
            Move(SQ_3B, SQ_3A, true).hash(), searcher.get_mate_move().hash());
    }
    {
        // COUNTER CHECK

        // Turn: BLACK
        // White: KE
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |+HI|-KE|-HI|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |-OU|
        //   +---+---+---+---+---+---+
        // C |   |-UM|   |   |   |-FU|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+OU|   |   |
        //   +---+---+---+---+---+---+
        // E |   |   |+KA|   |   |   |
        //   +---+---+---+---+---+---+
        // F |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // Black: -
        searcher.set_game(Game("3Rnr/5k/1+b3p/3K2/2B3/6 b n"));
        CHECK_TRUE(searcher.search(100));
    }
}

TEST(dfpn_searcher, cache_for_mate)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;
    auto searcher = Searcher();
    searcher.set_game(Game("3r1/+BP1r1/1+BP2/4p/2G1k b G"));
    searcher.search(100);
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_TRUE(searcher.found_mate());
    CHECK_EQUAL(5, searcher.get_mate_moves().size());

    // It is 27 without cache table
    CHECK_COMPARE(22, ==, searcher.get_search_count());
    // searches=1, SiEx: 3r1/+BP1r1/1+BP2/4p/2GGk w - 2, #P=200, #D=100
    // searches=2, SiEx: 3r1/+BP1r1/1+BP2/4p/3Gk w G 2, #P=200, #D=100
    // searches=3, SiEx: 3r1/+BP1r1/2P2/4p/2G+Bk w G 2, #P=200, #D=100
    // searches=4, SiEx: 3r1/+BP3/2P2/4p/2Grk b Gb 3, #P=100, #D=200
    // searches=5, SiEx: 3r1/+BP3/2P2/4p/2G+rk b Gb 3, #P=100, #D=200
    // searches=6, SiEx: 3r1/+BP3/2P2/4p/3Gk w RGb 4, #P=200, #D=100
    // searches=7, SiEx: 3r1/1P3/2P2/4p/2G+Bk w RGb 4, #P=200, #D=100
    // searches=8, SiEx: 5/1P3/2P2/4p/2Grk b RG2b 5, #P=100, #D=100
    // searches=9, SiEx: 5/1P3/2P2/4p/3Gk w 2RG2b 6, #P=100, #D=100
    // searches=10, SiEx: 5/1P3/2P2/4p/3k1 b 2RG2bg 7, #P=100, #D=1100
    // searches=11, SiEx: 5/1P3/2P2/4p/2G+rk b RG2b 5, #P=100, #D=100
    // searches=12, SiEx: 5/1P3/2P2/4p/3Gk w 2RG2b 6, #P=100, #D=100
    // searches=13, SiEx: 5/1P3/2P2/4p/3k1 b 2RG2bg 7, #P=100, #D=1100
    // searches=14, SiEx: 3r1/+BP3/1+BP2/4p/2Grk b g 3, #P=100, #D=200
    // searches=15, SiEx: 3r1/+BP3/1+BP2/4p/2G+rk b g 3, #P=100, #D=200
    // searches=16, SiEx: 3r1/+BP3/1+BP2/4p/3Gk w Rg 4, #P=200, #D=100
    // searches=17, SiEx: 3r1/+BP3/2P2/4p/2G+Bk w Rg 4, #P=200, #D=100
    // searches=18, SiEx: 5/+BP3/2P2/4p/2Grk b Rbg 5, #P=100, #D=200
    // searches=19, SiEx: 5/+BP3/2P2/4p/2G+rk b Rbg 5, #P=100, #D=200
    // searches=20, SiEx: 5/+BP3/2P2/4p/3Gk w 2Rbg 6, #P=0, #D=4294967295
    // searches=21, SiEx: 5/+BP3/2P2/4p/3Gk w 2Rbg 6, #P=0, #D=4294967295
    // searches=22, SiEx: 3r1/+BP3/1+BP2/4p/3Gk w Rg 4, #P=200, #D=100
    // searches=23, SiEx: 3r1/+BP3/2P2/4p/2G+Bk w Rg 4, #P=200, #D=100
    // searches=24, SiEx: 5/+BP3/2P2/4p/2Grk b Rbg 5, #P=100, #D=200
    // searches=25, SiEx: 5/+BP3/2P2/4p/2G+rk b Rbg 5, #P=100, #D=200
    // searches=26, SiEx: 5/+BP3/2P2/4p/3Gk w 2Rbg 6, #P=0, #D=4294967295
    // searches=27, SiEx: 5/+BP3/2P2/4p/3Gk w 2Rbg 6, #P=0, #D=4294967295
}

TEST(dfpn_searcher, cache_for_no_mate)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;
    auto searcher = Searcher();

    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |-OU|+GI|   |+UM|+HI|
    //   *---*---*---*---*---*
    // B |   |-KA|   |   |-FU|
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |   |   |   |
    //   *---*---*---*---*---*
    // Black: -
    searcher.set_game(Game("kS1+BR/1b2p/5/5/5 b -"));
    searcher.search(100);
    const auto num_searched = searcher.get_search_count();
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_FALSE(searcher.found_mate());
    CHECK_TRUE(searcher.found_no_mate());

    // it is 8 without cache
    CHECK_EQUAL(num_searched, 6);
    // 99, k2+BR/Sb2p/5/5/5 w - 2, #P=100, #D=100
    // 98, 3+BR/kb2p/5/5/5 b s 3, #P=100, #D=100
    // 97, 4R/kb2p/1+B3/5/5 w s 4, #P=100, #D=100
    // 96, 4R/1b2p/1k3/5/5 b bs 5, #P=4294967295, #D=0
    // 95, k2+BR/+Sb2p/5/5/5 w - 2, #P=100, #D=100
    // 94, 3+BR/kb2p/5/5/5 b s 3, #P=100, #D=100 (equiv. 98)
    // 93, 4R/kb2p/1+B3/5/5 w s 4, #P=100, #D=100 (equiv. 97)
    // 92, 4R/1b2p/1k3/5/5 b bs 5, #P=4294967295, #D=0
}

TEST(dfpn_searcher, mate_in_five_straight_forward)
{
    using namespace vshogi::judkins_shogi;

    // Turn: BLACK
    // White: -
    //     6   5   4   3   2   1
    //   +---+---+---+---+---+---+
    // A |   |   |   |   |-FU|-FU|
    //   +---+---+---+---+---+---+
    // B |   |   |   |-FU|-KA|-OU|
    //   +---+---+---+---+---+---+
    // C |   |   |   |-KE|   |+KE|
    //   +---+---+---+---+---+---+
    // D |   |   |   |+FU|   |+FU|
    //   +---+---+---+---+---+---+
    // E |   |   |+GI|   |+GI|+HI|
    //   +---+---+---+---+---+---+
    // F |   |   |   |+KI|+HI|+KI|
    //   +---+---+---+---+---+---+
    // Black: KA
    auto searcher = Searcher<Parameters>();
    searcher.set_game(Game("4pp/3pbk/3n1N/3P1P/2S1SR/3GRG b B"));
    CHECK_FALSE(searcher.search(4u));
    CHECK_TRUE(searcher.search(1u));
    CHECK_EQUAL(Move(SQ_2C, KA).hash(), searcher.get_mate_move().hash());
}

TEST(dfpn_searcher, mate_in_five)
{
    using namespace vshogi::minishogi;

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
    auto searcher = Searcher<Parameters>();
    searcher.set_game(Game("2pkb/4R/2+bG1/5/5 b 2S"));
    CHECK_TRUE(searcher.search(5000));
    CHECK_EQUAL(Move(SQ_2B, GI).hash(), searcher.get_mate_move().hash());
    const auto num_searched = searcher.get_search_count();
    CHECK_EQUAL(124, num_searched);
}

TEST(dfpn_searcher, king_entering_before_mate)
{
    using namespace vshogi::judkins_shogi;
    using Searcher = Searcher<Parameters>;
    // Turn: WHITE
    // White: GI
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
    // E |+FU|   |   |   |-KI|-RY|
    //   +---+---+---+---+---+---+
    // F |   |+OU|   |-OU|-TO|-UM|
    //   +---+---+---+---+---+---+
    // Black: -
    auto searcher = Searcher();
    searcher.set_game(Game("6/6/3n2/6/P3g+r/1K1k+p+b w s"));
    CHECK_TRUE(searcher.search(100));
    CHECK_TRUE(searcher.found_mate());
    const auto actual = searcher.get_mate_moves();
    CHECK_EQUAL(2, actual.size());
    CHECK_TRUE(actual[0] == Move(SQ_4E, SQ_3C, true));
    CHECK_TRUE(actual[1] == Move(SQ_6F, SQ_5F));
}

TEST(dfpn_searcher, avoid_consecutive_checks)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;
    // Turn: WHITE
    // White: HI,KI
    //     5   4   3   2   1
    //   +---+---+---+---+---+
    // A |   |   |-GI|   |-OU|
    //   +---+---+---+---+---+
    // B |   |   |   |   |-FU|
    //   +---+---+---+---+---+
    // C |+GI|+OU|   |   |   |
    //   +---+---+---+---+---+
    // D |   |   |   |   |   |
    //   +---+---+---+---+---+
    // E |   |-UM|-KI|   |   |
    //   +---+---+---+---+---+
    // Black: FU,KA
    auto g = Game("2s1k/4p/SK3/5/1+bg2 w BPrg 32");
    g.apply(Move(SQ_3D, SQ_4E));
    g.apply(Move(SQ_5D, SQ_4C));
    g.apply(Move(SQ_4E, SQ_3D));
    g.apply(Move(SQ_4C, SQ_5D));
    auto searcher = Searcher();
    searcher.set_game(g);
    CHECK_TRUE(searcher.search(1000u));
    const auto actual = searcher.get_mate_moves();
    for (auto&& m : actual) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());

    auto root = searcher.get_root();
    auto ch = root->get_child();
    for (; ch; ch = ch->get_sibling()) {
        if (ch->get_action() == Move(SQ_3D, SQ_4E)) {
            CHECK_TRUE(ch->found_no_mate());
            break;
        }
    }
    CHECK_TRUE(ch != nullptr);
}

TEST(dfpn_searcher, debug)
{
    using namespace vshogi::shogi;
    using Searcher = Searcher<Parameters>;
    // Turn: BLACK
    // White: FUx3,KI
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |   |-KA|   |-KE|   |   |   |   |-OU|
    //   +---+---+---+---+---+---+---+---+---+
    // B |-KY|   |-GI|   |   |   |-FU|   |-GI|
    //   +---+---+---+---+---+---+---+---+---+
    // C |-FU|-HI|-FU|   |   |-FU|-KE|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |-FU|   |+UM|-FU|   |   |   |+HI|
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |   |+FU|   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |   |   |   |+KI|-FU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |+FU|+FU|+FU|+FU|   |+FU|+KE|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |   |   |   |   |+OU|+KI|+GI|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |+KY|+KE|+GI|   |   |+KI|   |   |+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FUx2,KY
    auto g = Game("1b1n4k/l1s3p1s/prp2pn2/1p1+Bp3R/4P4/6Gp1/PPPP1PN2/4KGS2/"
                  "LNS2G2L b L2Pg3p");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(100);
    const auto actual = searcher.get_mate_moves();
    for (auto&& m : actual) {
        g.apply(m);
    }
    CHECK_TRUE(g.get_result() == vshogi::BLACK_WIN);
}

TEST(dfpn_searcher, debug2)
{
    using namespace vshogi::shogi;
    using Searcher = Searcher<Parameters>;
    auto g = Game("ln5nl/3rgkgs1/1pp1pp1pp/p3s1p2/3P3Pb/P1P1P4/1PBSSPP1P/"
                  "2G1G2R1/LN2K2NL b P 29");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(100);
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_TRUE(searcher.found_no_mate());
}

TEST(dfpn_searcher, debug_tmp)
{
    using namespace vshogi::shogi;
    auto g = Game("7bk/8p/9/9/9/9/B8/9/9 b Nplnsgbr 1");
    g.apply(Move("9g8h"))
        .apply(Move("L*5e"))
        .apply(Move("8h5e"))
        .apply(Move("P*2b"));
    auto searcher = Searcher<Parameters>();
    searcher.set_game(g);
    searcher.search(100);
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_TRUE(searcher.found_mate());
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
}

TEST(dfpn_searcher, debug_tmp2)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;

    // Turn: BLACK
    // White: KI
    //     5   4   3   2   1
    //   +---+---+---+---+---+
    // A |   |   |   |   |-OU|
    //   +---+---+---+---+---+
    // B |-HI|   |   |   |-FU|
    //   +---+---+---+---+---+
    // C |   |   |-GI|   |   |
    //   +---+---+---+---+---+
    // D |+FU|+UM|   |+GI|   |
    //   +---+---+---+---+---+
    // E |+OU|   |   |   |+HI|
    //   +---+---+---+---+---+
    // Black: KA,KI
    auto g = Game("4k/r3p/2s2/P+B1S1/K3R b BGg 17");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(10000);
    CHECK_TRUE(searcher.found_conclusion());
    CHECK_TRUE(searcher.found_mate());
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
}

TEST(dfpn_searcher, minishogi_debug)
{
    using namespace vshogi::minishogi;
    using Searcher = Searcher<Parameters>;

    // Turn: WHITE
    // White: FU,GI,KA
    //     5   4   3   2   1
    //   +---+---+---+---+---+
    // A |   |   |+UM|   |-OU|
    //   +---+---+---+---+---+
    // B |   |   |   |-HI|   |
    //   +---+---+---+---+---+
    // C |   |   |   |+GI|-FU|
    //   +---+---+---+---+---+
    // D |+KI|   |+OU|   |   |
    //   +---+---+---+---+---+
    // E |   |   |   |-HI|   |
    //   +---+---+---+---+---+
    // Black: KI
    auto g = Game("2+B1k/3r1/3Sp/G1K2/3r1 w Gbsp 30");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(10000u);

    CHECK_TRUE(searcher.found_conclusion());
    CHECK_TRUE(searcher.found_mate());
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());
}

TEST(dfpn_searcher, test_shogi_debug_1)
{
    using namespace vshogi::shogi;
    using Searcher = Searcher<Parameters>;

    // Turn: WHITE
    // White: FUx4,KE,KIx2
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |-KY|-KE|-GI|   |   |   |-KI|-KE|   |
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |-KI|   |-OU|   |   |-KA|-KY|
    //   +---+---+---+---+---+---+---+---+---+
    // C |   |   |-FU|-FU|-FU|-FU|-FU|-GI|   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |+FU|   |   |   |   |   |   |   |-FU|
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |+KE|   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |+OU|+FU|   |   |+KA|+HI|   |+FU|
    //   +---+---+---+---+---+---+---+---+---+
    // G |   |+FU|   |+FU|+FU|+FU|+FU|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |+KY|   |+GI|   |-RY|   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |   |   |   |-GI|   |   |   |+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: -
    auto g = Game("lns3gn1/2g1k2bl/2ppppps1/P7p/3N5/1KP2BR1P/1P1PPPP2/L1S1+r4/"
                  "4s3L w 2gn4p 74");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(10000u);
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
}

TEST(dfpn_searcher, test_shogi_debug_2)
{
    using namespace vshogi::shogi;
    using Searcher = Searcher<Parameters>;
    // Turn: WHITE
    // White: FUx5,GI
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |-HI|   |   |   |   |   |-FU|+KI|   |
    //   +---+---+---+---+---+---+---+---+---+
    // B |-KY|   |   |-FU|-GI|   |-KI|-FU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // C |-KE|   |-OU|   |-FU|   |-GI|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |-FU|   |-KA|+FU|   |   |-KY|
    //   +---+---+---+---+---+---+---+---+---+
    // E |-FU|   |   |   |+HI|   |   |+KE|+FU|
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |+KI|   |-KE|+FU|-KY|   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |+FU|-FU|   |+KI|   |   |+KE|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |+KY|   |   |-KA|   |+GI|   |   |+OU|
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FUx2
    auto g = Game("r5pG1/l2ps1gp1/n1k1p1s2/2p1bP2l/p3R2NP/3G1nPl1/Pp1G2N2/"
                  "L2b1S2K/9 w 2Ps5p 162");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(200u);
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());
}

TEST(dfpn_searcher, test_shogi_debug_3)
{
    using namespace vshogi::shogi;
    using Searcher = Searcher<Parameters>;
    // Turn: WHITE
    // White: FUx2,KEx2,GI
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |-KY|   |   |   |   |-KI|   |   |-KY|
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |-GI|-OU|   |   |   |-GI|-KA|   |
    //   +---+---+---+---+---+---+---+---+---+
    // C |-FU|-FU|-FU|-FU|   |-FU|-FU|   |-KE|
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |   |   |   |   |   |-KY|-FU|
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |+FU|   |   |   |+FU|   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |   |   |+FU|+FU|+KI|+FU|+FU|+FU|+FU|
    //   +---+---+---+---+---+---+---+---+---+
    // H |   |-RY|   |+KI|   |+OU|+KI|+GI|   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |-RY|   |+KA|   |   |   |+KE|+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FU
    auto g = Game("l4g2l/1sk3sb1/pppp1pp1n/7lp/9/P3P4/2PPGPPPP/1+r1G1KGS1/"
                  "1+r1B3NL w Ps2n2p 72");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(500u);
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());
}

TEST(dfpn_searcher, test_shogi_debug_4)
{
    using namespace vshogi::shogi;
    using Searcher = Searcher<Parameters>;
    // Turn: WHITE
    // White: FU,KY,KIx2
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |-KY|-KE|-GI|-KI|-OU|-KI|-GI|-KE|-KY|
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |   |   |   |   |   |-KA|   |
    //   +---+---+---+---+---+---+---+---+---+
    // C |-FU|+TO|-FU|-FU|-FU|-FU|-FU|-FU|-FU|
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |   |   |+KA|   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |   |   |+FU|   |+FU|   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |+FU|   |+FU|+FU|+FU|+OU|+FU|   |+FU|
    //   +---+---+---+---+---+---+---+---+---+
    // H |   |   |   |-RY|+GI|   |   |   |+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // I |-RY|+KE|   |   |   |   |-GI|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // Black: KE
    auto g = Game("lnsgkgsnl/7b1/p+Pppppppp/9/5B3/5P1P1/P1PPPKP1P/3+rS3L/"
                  "+rN4s2 w N2glp 44");
    auto searcher = Searcher();
    searcher.set_game(g);
    searcher.search(10000u);
    CHECK_TRUE(searcher.found_mate());
    const auto mate_moves = searcher.get_mate_moves();
    for (auto&& m : mate_moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());
}

// TEST(dfpn_searcher, mate_moves_without_waste_moves)
// {
//     using namespace vshogi::shogi;
//     using Searcher = Searcher<Parameters>;
//     // Turn: BLACK
//     // White: -
//     //     9   8   7   6   5   4   3   2   1
//     //   +---+---+---+---+---+---+---+---+---+
//     // A |   |   |   |   |   |   |   |   |   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // B |   |   |   |   |   |   |   |   |   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // C |   |   |   |   |   |   |-KE|-FU|   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // D |   |   |   |   |   |   |-FU|   |-FU|
//     //   +---+---+---+---+---+---+---+---+---+
//     // E |   |   |   |   |   |   |   |-OU|   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // F |   |   |   |   |   |   |+FU|   |   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // G |   |   |   |   |   |   |   |+FU|+FU|
//     //   +---+---+---+---+---+---+---+---+---+
//     // H |   |   |   |   |   |   |+HI|   |   |
//     //   +---+---+---+---+---+---+---+---+---+
//     // I |   |   |   |   |   |+OU|   |   |+KY|
//     //   +---+---+---+---+---+---+---+---+---+
//     // Black: KY,KA
//     auto g = Game("9/9/6np1/6p1p/7k1/6P2/7PP/6R2/5K2L b BL");
//     auto searcher = Searcher();
//     searcher.set_game(g);
//     searcher.search(10);
//     // Mate moves with waste moves: ['L*2f', '2e1e', 'B*2d', '2c2d', '1g1f']
//     // Mate moves w/o waste moves: ['L*2f', '2e1e', '1g1f']
//     const auto actual = searcher.get_mate_moves();
//     CHECK_EQUAL(3, actual.size());
// }

} // namespace test_vshogi::test_engine::test_dfpn
