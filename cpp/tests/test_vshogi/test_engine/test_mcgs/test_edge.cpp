#include "vshogi/engine/mcgs/edge.hpp"
#include "vshogi/engine/mcgs/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_mcgs
{

using namespace vshogi;
using Game = vshogi::minishogi::Game;
using Edge = vshogi::engine::mcgs::Edge;
using Node = vshogi::engine::mcgs::Node;

TEST_GROUP (test_mcgs_edge) {
};

TEST(test_mcgs_edge, test_destruct_with_no_child)
{
    Node node{};
    Edge edge{};
    edge.init(&node, 0.5f, static_cast<move_t>(1));

    CHECK_EQUAL(&node, edge.get_parent());
    DOUBLES_EQUAL(0.5f, edge.get_prior_proba(), 1e-5f);
    CHECK_EQUAL(static_cast<move_t>(1), edge.get_action());

    edge.destruct();

    CHECK_EQUAL(nullptr, edge.get_parent());
    DOUBLES_EQUAL(0.f, edge.get_prior_proba(), 1e-5f);
    CHECK_EQUAL(static_cast<move_t>(0), edge.get_action());
}

TEST(test_mcgs_edge, test_destruct_with_single_parent_child)
{
    Edge edge{};
    Node node{};
    edge.init(nullptr, 0.1f, static_cast<move_t>(1));
    edge.set_child(node);
    node.simulate(Game("4k/4G/4G/5/4K w - "));
    edge.backprop();

    CHECK_TRUE(edge.is_mate());
    CHECK_EQUAL(&node, edge.get_child());
    DOUBLES_EQUAL(1.f, edge.get_value(), 1e-5f);
    CHECK_TRUE(node.is_mate());
    CHECK_EQUAL(1u, node.num_parents());

    edge.destruct();

    CHECK_FALSE(edge.is_mate());
    CHECK_EQUAL(nullptr, edge.get_child());
    DOUBLES_EQUAL(-1.f, edge.get_value(), 1e-5f);
    CHECK_FALSE(node.is_mate());
    CHECK_EQUAL(0u, node.num_parents());
}

TEST(test_mcgs_edge, test_destruct_with_transposed_child)
{
    Edge edge1{};
    Edge edge2{};
    Node node{};
    edge1.set_child(node);
    node.simulate(Game("4k/4G/4G/5/4K w -"));
    edge1.backprop();

    edge2.set_child(node);
    edge2.backprop();

    CHECK_TRUE(node.is_mate());
    CHECK_EQUAL(2u, node.num_parents());

    edge1.destruct();

    CHECK_TRUE(edge2.is_mate());
    CHECK_TRUE(node.is_mate());
    CHECK_EQUAL(1u, node.num_parents());
}

TEST(test_mcgs_edge, test_init_default)
{
    const Edge e{};
    CHECK_EQUAL(nullptr, e.get_parent());
    CHECK_EQUAL(nullptr, e.get_child());
    CHECK_EQUAL(nullptr, e.get_sibling());
    DOUBLES_EQUAL(-1.f, e.get_value(), 1e-3f);
    DOUBLES_EQUAL(0.f, e.get_prior_proba(), 1e-3f);
    CHECK_EQUAL(0u, e.get_visits());
    CHECK_EQUAL(0u, e.get_action());
}

} // namespace test_vshogi::test_engine::test_mcgs
