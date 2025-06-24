#include "vshogi/engine/dfpn/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn
{

TEST_GROUP (test_dfpn_node) {
};

TEST_GROUP (test_dfpn_node_simulate) {
    using Node = vshogi::engine::dfpn::Node<vshogi::minishogi::Parameters>;
    const vshogi::uint zero = vshogi::engine::dfpn::zero;
    const vshogi::uint inf = vshogi::engine::dfpn::inf;
};

TEST(test_dfpn_node_simulate, test_ongoing)
{
    using namespace vshogi::minishogi;
    auto g = Game("5/5/4k/5/4P b -");
    auto n = Node();
    CHECK_EQUAL(0u, n.num_edges());

    CHECK_FALSE(n.simulate(g));
    CHECK_EQUAL(0, n.num_edges());
}

TEST(test_dfpn_node_simulate, test_draw)
{
    using namespace vshogi::minishogi;
    auto g = Game("4k/5/5/5/4K b -");
    g.apply(Move(SQ_1D, SQ_1E))
        .apply(Move(SQ_1B, SQ_1A))
        .apply(Move(SQ_1E, SQ_1D))
        .apply(Move(SQ_1A, SQ_1B));
    g.update_result_dfpn(1u, true);
    CHECK_EQUAL(vshogi::DRAW, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn(true));
    CHECK_EQUAL(zero, n.dn(true));
    CHECK_TRUE(n.found_conclusion());
    CHECK_FALSE(n.found_mate(true));
    CHECK_TRUE(n.found_no_mate(true));
}

TEST(test_dfpn_node_simulate, test_won_by_offence_at_offence_turn)
{
    using namespace vshogi::minishogi;
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u, true);
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(zero, n.pn(true));
    CHECK_EQUAL(inf, n.dn(true));
    CHECK_TRUE(n.found_conclusion());
    CHECK_TRUE(n.found_mate(true));
    CHECK_FALSE(n.found_no_mate(true));
}

TEST(test_dfpn_node_simulate, test_won_by_defence_at_offence_turn)
{
    using namespace vshogi::minishogi;
    auto g = Game("5/5/3gk/5/4K w -").apply(Move(SQ_1D, SQ_2C));
    CHECK_EQUAL(vshogi::BLACK, g.get_turn());
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn(true));
    CHECK_EQUAL(zero, n.dn(true));
    CHECK_TRUE(n.found_conclusion());
    CHECK_FALSE(n.found_mate(true));
    CHECK_TRUE(n.found_no_mate(true));
}

TEST(test_dfpn_node_simulate, test_won_by_defence_at_defence_turn)
{
    using namespace vshogi::minishogi;
    auto g = Game("4k/4S/5/5/4K w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));
    g.update_result_dfpn(1u, true);
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn(false));
    CHECK_EQUAL(zero, n.dn(false));
    CHECK_TRUE(n.found_conclusion());
    CHECK_FALSE(n.found_mate(false));
    CHECK_TRUE(n.found_no_mate(false));
}

TEST_GROUP (test_dfpn_node_extend_edges) {
    using Node = vshogi::engine::dfpn::Node<vshogi::minishogi::Parameters>;
    const vshogi::uint unit = vshogi::engine::dfpn::unit;
};

TEST(test_dfpn_node_extend_edges, test_offence)
{
    using namespace vshogi::minishogi;
    Node *node_l = nullptr, *node_g = nullptr;
    auto g = Game("5/5/4k/5/4G b -");
    auto n = Node();
    n.extend_edges(g, true, node_l, node_g);
    CHECK_EQUAL(2u, n.num_edges());
    CHECK_EQUAL(unit, n.pn(true));
    CHECK_EQUAL(2u * unit, n.dn(true));
}

TEST(test_dfpn_node_extend_edges, test_defence)
{
    using namespace vshogi::minishogi;

    Node *node_l = nullptr, *node_g = nullptr;
    auto g = Game("5/5/4k/5/4G b -").apply(Move(SQ_2D, SQ_1E));
    auto n = Node();
    n.extend_edges(g, false, node_l, node_g);
    CHECK_EQUAL(3u, n.num_edges());
    CHECK_EQUAL(3u * unit, n.pn(false));
    CHECK_EQUAL(unit, n.dn(false));
}

TEST(test_dfpn_node_extend_edges, test_node_with_unextended_edges)
{
    using namespace vshogi::minishogi;
    // Turn: BLACK
    // White: FU,GI,KI
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |+RY|   |   |   |   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |   |   |   |
    //   *---*---*---*---*---*
    // D |   |   |   |-TO|   |
    //   *---*---*---*---*---*
    // E |   |   |   |-KI|-OU|
    //   *---*---*---*---*---*
    // Black: -
    auto g = Game("+R4/5/5/3+p1/3gk b psg")
                 .apply(Move(SQ_1A, SQ_5A))
                 .apply(Move(SQ_1B, FU))
                 .apply(Move(SQ_1B, SQ_1A))
                 .apply(Move(SQ_1C, GI))
                 .apply(Move(SQ_1C, SQ_1B));
    g.update_result_dfpn(1u, false);
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());

    auto n_unextended = Node();
    n_unextended.simulate(g);
    CHECK_TRUE(n_unextended.found_mate(false));
    CHECK_EQUAL(0u, n_unextended.num_edges());

    auto g2 = Game("+R4/5/5/3+p1/3gk b psg")
                  .apply(Move(SQ_1A, SQ_5A))
                  .apply(Move(SQ_1C, GI))
                  .apply(Move(SQ_1C, SQ_1A));
    g2.update_result_dfpn(1u, false);
    CHECK_EQUAL(g.get_board_turn_hash(), g2.get_board_turn_hash());

    auto n2 = Node();
    n2.extend_edges(g2, false, &n_unextended, nullptr);
    CHECK_EQUAL(3u, n2.num_edges());
}

TEST(test_dfpn_node, test_select)
{
    using namespace vshogi::minishogi;
    using Node = vshogi::engine::dfpn::Node<Parameters>;
    constexpr auto unit = vshogi::engine::dfpn::unit;
    {
        Node *node_l = nullptr, *node_g = nullptr;
        auto g = Game("5/5/4k/5/4P b -");
        auto n = Node();
        n.extend_edges(g, true, node_l, node_g);

        const auto actual = n.select_an_edge();
        CHECK_TRUE(Move(SQ_1D, SQ_1E) == actual->m_action);
        CHECK_EQUAL(unit, actual->m_pn);
        CHECK_EQUAL(unit, actual->m_dn);
        CHECK_TRUE(nullptr == n.get_edge_2nd());
    }
}

TEST(test_dfpn_node, test_backprop)
{
    using namespace vshogi::minishogi;
    using Node = vshogi::engine::dfpn::Node<Parameters>;
    constexpr auto unit = vshogi::engine::dfpn::unit;
    {
        // offence
        Node *node_l = nullptr, *node_g = nullptr;
        auto g = Game("5/5/4k/5/4G b -");
        auto n = Node();
        n.extend_edges(g, true, node_l, node_g);
        auto e = n.select_an_edge();
        CHECK_TRUE(e != nullptr);
        e->m_pn = 10u * unit;
        e->m_dn = 2u * unit;

        n.backprop(SQ_NA);
        CHECK_EQUAL(unit, n.pn(true));
        CHECK_EQUAL(3u * unit, n.dn(true));
        CHECK_TRUE(e != n.select_an_edge());
        CHECK_TRUE(e == n.get_edge_2nd());
    }
}

} // namespace test_vshogi::test_engine::test_dfpn
