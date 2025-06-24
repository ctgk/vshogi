#include "vshogi/engine/dfpn/table.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn
{

TEST_GROUP (test_dfpn_table) {
};

TEST(test_dfpn_table, test_get_or_expand)
{
    namespace sg = vshogi::minishogi;
    namespace dp = vshogi::engine::dfpn;
    using Node = dp::Node<sg::Parameters>;
    using Table = dp::Table<sg::Parameters>;
    {
        Table t{vshogi::BLACK};
        auto g = sg::Game();
        Node& n = t.get_or_expand(g);
        CHECK_EQUAL(0u, n.num_edges());
    }
}

TEST_GROUP (test_dfpn_table_lookup_lgstand) {
    using Node = vshogi::engine::dfpn::Node<vshogi::minishogi::Parameters>;
    using Table = vshogi::engine::dfpn::Table<vshogi::minishogi::Parameters>;
};

TEST(test_dfpn_table_lookup_lgstand, test_lookup_none)
{
    using namespace vshogi::minishogi;
    Table t{vshogi::BLACK};
    t.get_or_expand(Game("4k/5/5/5/4K w -"));

    const Node *n_l_actual = nullptr, *n_g_actual = nullptr;
    t.lookup_lgstand(Game("4k/5/5/5/4K b -"), &n_l_actual, &n_g_actual);
    CHECK_TRUE(nullptr == n_l_actual);
    CHECK_TRUE(nullptr == n_g_actual);
}

TEST(test_dfpn_table_lookup_lgstand, test_lookup_both)
{
    using namespace vshogi::minishogi;
    Table t{vshogi::BLACK};
    Node& n_l_expect = t.get_or_expand(Game("4k/5/4P/5/5 b -"));
    Node& n_g_expect = t.get_or_expand(Game("4k/5/4P/5/5 b 2G"));

    const Node *n_l_actual = nullptr, *n_g_actual = nullptr;
    t.lookup_lgstand(Game("4k/5/4P/5/5 b G"), &n_l_actual, &n_g_actual);
    CHECK_TRUE(&n_g_expect == n_g_actual);
    CHECK_TRUE(&n_l_expect == n_l_actual);
}

TEST(test_dfpn_table_lookup_lgstand, test_offence_lstand_mate)
{
    using namespace vshogi::minishogi;
    Table t{vshogi::BLACK};
    t.get_or_expand(Game("4k/5/4P/5/5 b G"));
    Node& expect = t.get_or_expand(Game("4k/5/4P/5/5 b -"));
    t.get_or_expand(Game("4k/5/4P/5/5 b S"));

    const Node *n_l_actual = nullptr, *n_g_actual = nullptr;
    t.lookup_lgstand(Game("4k/5/4P/5/5 b GS"), &n_l_actual, &n_g_actual);
    CHECK_FALSE(&expect == n_l_actual);
    CHECK_TRUE(nullptr != n_l_actual);
    CHECK_TRUE(nullptr == n_g_actual);

    expect.simulate(Game("BBRRK/5/5/5/4k b 2P2S2G"));
    CHECK_TRUE(expect.found_mate(true));
    t.lookup_lgstand(Game("4k/5/4P/5/5 b GS"), &n_l_actual, &n_g_actual);
    CHECK_TRUE(&expect == n_l_actual);
    CHECK_TRUE(nullptr == n_g_actual);
}

TEST(test_dfpn_table_lookup_lgstand, test_offence_gstand_no_mate)
{
    using namespace vshogi::minishogi;
    Table t{vshogi::BLACK};
    t.get_or_expand(Game("4k/5/4P/5/5 b G"));
    Node& expect = t.get_or_expand(Game("4k/5/4P/5/5 b 2G"));
    t.get_or_expand(Game("4k/5/4P/5/5 b 2G2S"));

    const Node *n_l_actual = nullptr, *n_g_actual = nullptr;
    t.lookup_lgstand(Game("4k/5/4P/5/5 b -"), &n_l_actual, &n_g_actual);
    CHECK_FALSE(&expect == n_g_actual);
    CHECK_TRUE(nullptr != n_g_actual);
    CHECK_TRUE(nullptr == n_l_actual);

    expect.simulate(Game("4k/5/5/3gg/4K b -"));
    CHECK_TRUE(expect.found_no_mate(true));
    t.lookup_lgstand(Game("4k/5/4P/5/5 b -"), &n_l_actual, &n_g_actual);
    CHECK_TRUE(&expect == n_g_actual);
    CHECK_TRUE(nullptr == n_l_actual);
}

TEST(test_dfpn_table_lookup_lgstand, test_defence_lstand_no_mate)
{
    using namespace vshogi::minishogi;
    Table t{vshogi::BLACK};
    t.get_or_expand(Game("4k/4P/5/5/5 w -"));
    Node& expect = t.get_or_expand(Game("4k/4P/5/5/5 w g"));
    t.get_or_expand(Game("4k/4P/5/5/5 w 2g"));

    const Node *n_l_actual = nullptr, *n_g_actual = nullptr;
    t.lookup_lgstand(Game("4k/4P/5/5/5 w 2g2s"), &n_l_actual, &n_g_actual);
    CHECK_FALSE(&expect == n_l_actual);
    CHECK_TRUE(nullptr != n_l_actual);
    CHECK_TRUE(nullptr == n_g_actual);

    expect.simulate(Game("4K/5/5/5/bbrrk w 2p2s2g"));
    CHECK_TRUE(expect.found_no_mate(false));
    t.lookup_lgstand(Game("4k/4P/5/5/5 w 2g2s"), &n_l_actual, &n_g_actual);
    CHECK_TRUE(&expect == n_l_actual);
    CHECK_TRUE(nullptr == n_g_actual);
}
TEST(test_dfpn_table_lookup_lgstand, test_defence_gstand_mate)
{
    using namespace vshogi::minishogi;
    Table t{vshogi::BLACK};
    t.get_or_expand(Game("4k/4P/5/5/5 w g"));
    Node& expect = t.get_or_expand(Game("4k/4P/5/5/5 w 2g"));
    t.get_or_expand(Game("4k/4P/5/5/5 w 2g2s"));

    const Node *n_l_actual = nullptr, *n_g_actual = nullptr;
    t.lookup_lgstand(Game("4k/4P/5/5/5 w -"), &n_l_actual, &n_g_actual);
    CHECK_FALSE(&expect == n_g_actual);
    CHECK_TRUE(nullptr != n_g_actual);
    CHECK_TRUE(nullptr == n_l_actual);

    expect.simulate(Game("4k/4G/4P/5/5 w -"));
    CHECK_TRUE(expect.found_mate(false));
    t.lookup_lgstand(Game("4k/4P/5/5/5 w -"), &n_l_actual, &n_g_actual);
    CHECK_TRUE(&expect == n_g_actual);
    CHECK_TRUE(nullptr == n_l_actual);
}

} // namespace test_vshogi::test_engine::test_dfpn
