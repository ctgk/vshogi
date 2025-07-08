#include "vshogi/engine/dfpn/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn::test_node
{

using namespace vshogi::minishogi;
namespace dfpn = vshogi::engine::dfpn2;
constexpr uint zero = dfpn::zero;
// constexpr uint unit = dfpn::unit;
// constexpr uint cent = dfpn::cent;
// constexpr uint kilo = dfpn::kilo;
constexpr uint inf = dfpn::inf;

TEST_GROUP (dfpn_node_simulate) {
    using Node = dfpn::Node<Parameters>;
};

TEST(dfpn_node_simulate, using_game_ongoing)
{
    auto g = Game();
    auto n = Node();
    CHECK_FALSE(n.proved());

    CHECK_FALSE(n.simulate(g));
    CHECK_FALSE(n.proved());
}

TEST(dfpn_node_simulate, using_game_draw)
{
    auto g = Game("4k/5/5/5/4K b -");
    g.apply(Move(SQ_1D, SQ_1E))
        .apply(Move(SQ_1B, SQ_1A))
        .apply(Move(SQ_1E, SQ_1D))
        .apply(Move(SQ_1A, SQ_1B));
    g.update_result_dfpn(1u);
    CHECK_EQUAL(vshogi::DRAW, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_game_won_by_offence_at_offence_turn)
{
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u);
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_game_won_by_defence_at_offence_turn)
{
    auto g = Game("5/5/3gk/5/4K w -").apply(Move(SQ_1D, SQ_2C));
    CHECK_EQUAL(vshogi::BLACK, g.get_turn());
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_game_won_by_defence_at_defence_turn)
{
    using namespace vshogi::minishogi;
    auto g = Game("4k/4S/5/5/4K w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));
    g.update_result_dfpn(1u);
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());

    auto n = Node(false, Move());
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_offence_cousin_l_mate)
{
    auto node_l = Node();
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u);
    node_l.simulate(g);

    auto n = Node();
    CHECK_TRUE(n.simulate(Game(), &node_l));
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_offence_cousin_l_no_mate)
{
    auto node_l = Node();
    auto g = Game("5/5/3gk/5/4K w -").apply(Move(SQ_1D, SQ_2C));
    node_l.simulate(g);
    CHECK_TRUE(node_l.proved_no_mate());

    auto n = Node();
    CHECK_FALSE(n.simulate(Game(), &node_l));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_defence_cousin_l_no_mate)
{
    auto g = Game("4k/4S/5/5/4K w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));
    g.update_result_dfpn(1u);
    auto node_l = Node(false, Move());
    node_l.simulate(g);
    CHECK_TRUE(node_l.proved_no_mate());

    auto n = Node(false, Move());
    CHECK_TRUE(n.simulate(Game(), &node_l));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_defence_cousin_l_ongoing)
{
    auto g = Game("4k/4S/5/5/4K w -");
    auto node_l = Node(false, Move());
    node_l.simulate(g);
    CHECK_FALSE(node_l.proved());

    auto n = Node(false, Move());
    CHECK_FALSE(n.simulate(Game(), &node_l));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_cousin_e_mate)
{
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u);
    auto node_e = Node();
    node_e.simulate(g);
    CHECK_TRUE(node_e.proved_mate());

    auto n = Node();
    CHECK_TRUE(n.simulate(Game(), nullptr, &node_e));
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_cousin_e_no_mate)
{
    auto g = Game("4k/5/5/5/4K b -");
    g.apply(Move(SQ_1D, SQ_1E))
        .apply(Move(SQ_1B, SQ_1A))
        .apply(Move(SQ_1E, SQ_1D))
        .apply(Move(SQ_1A, SQ_1B));
    g.update_result_dfpn(1u);
    auto node_e = Node();
    node_e.simulate(g);
    CHECK_TRUE(node_e.proved_no_mate());

    auto n = Node();
    CHECK_TRUE(n.simulate(Game(), nullptr, &node_e));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_cousin_e_ongoing)
{
    auto g = Game();
    auto node_e = Node();
    node_e.simulate(g);
    CHECK_FALSE(node_e.proved());

    auto n = Node();
    CHECK_FALSE(n.simulate(Game(), nullptr, &node_e));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_offence_cousin_g_mate)
{
    auto node_g = Node();
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u);
    node_g.simulate(g);

    auto n = Node();
    CHECK_FALSE(n.simulate(Game(), nullptr, nullptr, &node_g));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_offence_cousin_g_no_mate)
{
    auto node_g = Node();
    auto g = Game("5/5/3gk/5/4K w -").apply(Move(SQ_1D, SQ_2C));
    node_g.simulate(g);
    CHECK_TRUE(node_g.proved_no_mate());

    auto n = Node();
    CHECK_TRUE(n.simulate(Game(), nullptr, nullptr, &node_g));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_defence_cousin_g_no_mate)
{
    auto g = Game("4k/4S/5/5/4K w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));
    g.update_result_dfpn(1u);
    auto node_g = Node(false, Move());
    node_g.simulate(g);
    CHECK_TRUE(node_g.proved_no_mate());

    auto n = Node(false, Move());
    CHECK_FALSE(n.simulate(Game(), nullptr, nullptr, &node_g));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_defence_cousin_g_ongoing)
{
    auto g = Game("4k/4S/5/5/4K w -");
    auto node_g = Node(false, Move());
    node_g.simulate(g);
    CHECK_FALSE(node_g.proved());

    auto n = Node(false, Move());
    CHECK_FALSE(n.simulate(Game(), nullptr, nullptr, &node_g));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

} // namespace test_vshogi::test_engine::test_dfpn::test_node
