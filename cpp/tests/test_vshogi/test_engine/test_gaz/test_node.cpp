#include "vshogi/engine/gaz/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_gaz
{

using namespace vshogi;
using namespace vshogi::minishogi;
using Game = vshogi::minishogi::Game;
using State = vshogi::minishogi::State;
using Node = vshogi::engine::gaz::Node;
using MT = vshogi::minishogi::MoveTraits;

TEST_GROUP (test_gaz_node) {
    Node root{};
    Node nodes[1000] = {};
    Node* next{};
    void setup()
    {
        root.init();
        nodes[0].init();
        nodes[999].init_as_end();
        next = nodes;
    }
};

TEST(test_gaz_node, select_from)
{
    root.simulate_ongoing_and_expand(
        next, Game("g4/5/5/5/4G b -"), 0.f, nullptr);
    root.backprop(root.get_q_value(), nullptr);
    {
        const Node* child_nodes[10] = {};
        child_nodes[0] = root.get_child()->get_sibling();
        const auto actual = root.select_from(child_nodes);
        CHECK_EQUAL(root.get_child()->get_sibling(), actual);
        CHECK_EQUAL(&root, actual->get_parent());
    }
    {
        const auto actual = root.select();
        CHECK_EQUAL(root.get_child(), actual);
        CHECK_EQUAL(&root, actual->get_parent());
    }
}

TEST(test_gaz_node, select_given_policy)
{
    float policy_logits[State::num_dlshogi_policy()] = {};
    for (uint ii = State::num_dlshogi_policy(); ii--;)
        policy_logits[ii] = static_cast<float>(ii);

    root.simulate_ongoing_and_expand(
        next, Game("g4/5/5/5/4G b -"), 0.f, policy_logits);
    root.backprop(root.get_q_value(), nullptr);
    const auto actual = root.select();
    CHECK_EQUAL(root.get_child()->get_sibling()->get_sibling(), actual);
    CHECK_EQUAL(&root, actual->get_parent());
}

TEST(test_gaz_node, get_q_value)
{
    CHECK_EQUAL(nodes, next);
    root.simulate_ongoing_and_expand(
        next, Game("g4/5/5/5/4G b -"), 0.f, nullptr);
    CHECK_EQUAL(nodes + 3, next);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-3f);
    root.backprop(root.get_q_value(), nullptr);
    {
        const auto c = root.select();
        c->simulate_mate_and_expand(next, static_cast<move_t>(0));
        DOUBLES_EQUAL(1.f, c->get_q_value(), 1e-3f);
        c->backprop(c->get_q_value(), nullptr);
        root.backprop(-c->get_q_value(), c);
    }
    DOUBLES_EQUAL(-0.5f, root.get_q_value(), 1e-3f);
    // DOUBLES_EQUAL(-1.f, root.get_q_value(1u, 0u), 1e-3f);
    DOUBLES_EQUAL(-1.f, root.get_q_value(1u, 10u), 1e-3f);
}

TEST(test_gaz_node, backprop)
{
    root.backprop(0.5f, nullptr);
    CHECK_FALSE(root.is_mate());
    DOUBLES_EQUAL(0.5f, root.get_q_value(), 1e-3f);
}

} // namespace test_vshogi::test_engine::test_gaz
