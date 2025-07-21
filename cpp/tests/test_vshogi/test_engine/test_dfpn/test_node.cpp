#include "vshogi/engine/dfpn/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn::test_node
{

using namespace vshogi::minishogi;
namespace dfpn = vshogi::engine::dfpn;
using Node = dfpn::Node<Parameters>;
constexpr uint zero = dfpn::zero;
constexpr uint unit = dfpn::unit;
constexpr uint kilo = dfpn::kilo;
constexpr uint inf = dfpn::inf;

TEST_GROUP (dfpn_node_select) {
};

TEST(dfpn_node_select, offence_single_child)
{
    auto g = Game("4k/5/5/5/5 b P");
    auto n = Node();
    n.expand(g);
    uint thpn_ch, thdn_ch;
    Node* const c1 = n.select(inf, inf, thpn_ch, thdn_ch);
    CHECK_EQUAL(Move(SQ_1B, FU).hash(), c1->get_action().hash());
    CHECK_EQUAL(inf, thpn_ch);
    CHECK_EQUAL(inf, thdn_ch);
}

TEST(dfpn_node_select, offence_prefer_promotion)
{
    auto g = Game("2B1k/5/5/5/5 b -");
    auto n = Node();
    n.expand(g);
    uint thpn_ch, thdn_ch;
    Node* const c1 = n.select(inf, inf, thpn_ch, thdn_ch);
    CHECK_EQUAL(Move(SQ_2B, SQ_3A, true).hash(), c1->get_action().hash());
    CHECK_EQUAL(kilo + 1u, thpn_ch);
    CHECK_EQUAL(inf, thdn_ch);
}

TEST(dfpn_node_select, defence_prefer_capture)
{
    auto g = Game("+R4/5/pr3/5/k4 b -");
    auto r = Node();
    r.expand(g);
    uint thpn_ch, thdn_ch;
    Node* const n = r.select(inf, inf, thpn_ch, thdn_ch);
    g.apply_dfpn(n->get_action());
    n->expand(g);
    Node* const c1 = n->select(inf, inf, thpn_ch, thdn_ch);
    CHECK_EQUAL(SQ_5C, c1->get_action().destination());
    CHECK_EQUAL(Move(SQ_5C, SQ_4C).hash(), c1->get_action().hash());
    CHECK_EQUAL(inf, thpn_ch);
    CHECK_EQUAL(unit + 1u, thdn_ch);
}

TEST_GROUP (dfpn_node_backprop) {
};

TEST(dfpn_node_backprop, offence_single_child)
{
    auto g = Game("4k/5/5/5/5 b P");
    auto n = Node();
    n.expand(g, nullptr, nullptr);
    n.backprop();
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(unit, n.dn());
}

TEST(dfpn_node_backprop, offence_with_proved_child)
{
    auto g = Game("3rk/3p1/4P/5/4K b G");
    auto n = Node();
    n.expand(g, nullptr, nullptr);
    CHECK_FALSE(n.proved());
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(2u * unit, n.dn());

    auto ch = n.select(); // Move(SQ_1B, KI)
    CHECK_FALSE(ch->offence());
    g.apply(Move(SQ_1B, KI));
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
    ch->simulate(g);
    CHECK_TRUE(ch->proved_mate());

    n.backprop();
    CHECK_TRUE(n.proved_mate());
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
}

} // namespace test_vshogi::test_engine::test_dfpn::test_node
