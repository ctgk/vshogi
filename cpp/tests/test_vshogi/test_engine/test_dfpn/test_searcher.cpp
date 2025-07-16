#include "vshogi/engine/dfpn/searcher.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn::test_searcher
{

using namespace vshogi::minishogi;
namespace dfpn = vshogi::engine::dfpn2;
using Table = dfpn::Table<Parameters>;
using Node = dfpn::Node<Parameters>;

TEST_GROUP (dfpn_table) {
};

TEST(dfpn_table, look_up_e)
{
    auto t = Table();
    auto n = Node();
    auto g = Game();
    t.add(&n, g);

    const Node *node_l, *node_e, *node_g;
    t.look_up_leg_stand_nodes(g, &node_l, &node_e, &node_g);
    CHECK_TRUE(nullptr == node_l);
    CHECK_TRUE(&n == node_e);
    CHECK_TRUE(nullptr == node_g);
}

TEST(dfpn_table, look_up_l)
{
    auto t = Table();
    auto n = Node();
    auto g = Game("4k/5/4P/5/5 b G");
    t.add(&n, g);

    const Node *node_l, *node_e, *node_g;
    t.look_up_leg_stand_nodes(
        Game("4k/5/4P/5/5 b SG"), &node_l, &node_e, &node_g);
    CHECK_TRUE(&n == node_l);
    CHECK_TRUE(nullptr == node_e);
    CHECK_TRUE(nullptr == node_g);
}

TEST(dfpn_table, look_up_l_prefer_mate_at_offence)
{

    auto n1 = Node();
    auto n2 = Node();
    auto g1 = Game("3rk/3p1/4P/5/5 b G");
    n1.expand_children(g1);
    n1.backprop(g1);
    Node* const c1 = n1.select();
    g1.apply(c1->get_action());
    CHECK_EQUAL(vshogi::BLACK_WIN, g1.get_result());
    CHECK_TRUE(c1->simulate(g1));
    g1.undo();
    n1.backprop(g1);
    CHECK_TRUE(n1.proved_mate());

    {
        Table t{};
        t.add(&n1, g1);
        t.add(&n2, Game("3rk/3p1/4P/5/5 b -"));

        const Node *node_l, *node_e, *node_g;
        t.look_up_leg_stand_nodes(
            Game("3rk/3p1/4P/5/5 b GS"), &node_l, &node_e, &node_g);
        CHECK_TRUE(&n1 == node_l);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(nullptr == node_g);
    }
    {
        Table t{};
        t.add(&n2, Game("3rk/3p1/4P/5/5 b -"));
        t.add(&n1, g1);

        const Node *node_l, *node_e, *node_g;
        t.look_up_leg_stand_nodes(
            Game("3rk/3p1/4P/5/5 b GS"), &node_l, &node_e, &node_g);
        CHECK_TRUE(&n1 == node_l);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(nullptr == node_g);
    }
}

TEST(dfpn_table, look_up_l_prefer_no_mate_at_defence)
{
    auto n1 = Node(false, Move());
    auto n2 = Node(false, Move());
    auto g1 = Game("4k/4P/5/5/5 w -");
    n1.expand_children(g1);
    n1.backprop(g1);
    Node* const c1 = n1.select();
    g1.apply_dfpn(c1->get_action());
    c1->expand_children(g1);
    c1->backprop(g1);
    CHECK_TRUE(c1->proved_no_mate());
    g1.undo();
    n1.backprop(g1);
    CHECK_TRUE(n1.proved_no_mate());

    {
        Table t{};
        t.add(&n1, g1);
        t.add(&n2, Game("4k/4P/5/5/5 w ps"));
        const Node *node_l, *node_e, *node_g;
        t.look_up_leg_stand_nodes(
            Game("4k/4P/5/5/5 w psg"), &node_l, &node_e, &node_g);
        CHECK_TRUE(&n1 == node_l);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(nullptr == node_g);
    }
    {
        Table t{};
        t.add(&n2, Game("4k/4P/5/5/5 w ps"));
        t.add(&n1, g1);
        const Node *node_l, *node_e, *node_g;
        t.look_up_leg_stand_nodes(
            Game("4k/4P/5/5/5 w psg"), &node_l, &node_e, &node_g);
        CHECK_TRUE(&n1 == node_l);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(nullptr == node_g);
    }
}

TEST(dfpn_table, look_up_g)
{
    auto t = Table();
    auto n = Node();
    auto g = Game("4k/5/4P/5/5 b G");
    t.add(&n, g);

    const Node *node_l, *node_e, *node_g;
    t.look_up_leg_stand_nodes(
        Game("4k/5/4P/5/5 b -"), &node_l, &node_e, &node_g);
    CHECK_TRUE(nullptr == node_l);
    CHECK_TRUE(nullptr == node_e);
    CHECK_TRUE(&n == node_g);
}

TEST(dfpn_table, look_up_g_prefer_no_mate_at_offence)
{
    auto n1 = Node();
    auto n2 = Node();
    auto g1 = Game("3rk/3gs/5/5/5 b PSG");
    n1.expand_children(g1);
    n1.backprop(g1);
    CHECK_TRUE(n1.proved_no_mate());
    {
        Table t{};
        t.add(&n1, g1);
        t.add(&n2, Game("3rk/3gs/5/5/5 b PS"));
        const Node *node_l, *node_e, *node_g;
        t.look_up_leg_stand_nodes(
            Game("3rk/3gs/5/5/5 b P"), &node_l, &node_e, &node_g);
        CHECK_TRUE(nullptr == node_l);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(&n1 == node_g);
    }
    {
        Table t{};
        t.add(&n2, Game("3rk/3gs/5/5/5 b PS"));
        t.add(&n1, g1);
        const Node *node_l, *node_e, *node_g;
        t.look_up_leg_stand_nodes(
            Game("3rk/3gs/5/5/5 b P"), &node_l, &node_e, &node_g);
        CHECK_TRUE(nullptr == node_l);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(&n1 == node_g);
    }
}

TEST(dfpn_table, look_up_g_prefer_mate_at_defence)
{
    auto n1 = Node(false, Move());
    auto n2 = Node(false, Move());
    auto g1 = Game("4k/4G/4P/5/5 w psg");
    n1.simulate(g1);
    CHECK_TRUE(n1.proved_mate());
    {
        Table t{};
        t.add(&n1, g1);
        t.add(&n2, Game("4k/4G/4P/5/5 w ps"));
        const Node *node_l, *node_e, *node_g;
        t.look_up_leg_stand_nodes(
            Game("4k/4G/4P/5/5 w p"), &node_l, &node_e, &node_g);
        CHECK_TRUE(nullptr == node_l);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(&n1 == node_g);
    }
    {
        Table t{};
        t.add(&n2, Game("4k/4G/4P/5/5 w ps"));
        t.add(&n1, g1);
        const Node *node_l, *node_e, *node_g;
        t.look_up_leg_stand_nodes(
            Game("4k/4G/4P/5/5 w p"), &node_l, &node_e, &node_g);
        CHECK_TRUE(nullptr == node_l);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(&n1 == node_g);
    }
}

} // namespace test_vshogi::test_engine::test_dfpn::test_searcher
