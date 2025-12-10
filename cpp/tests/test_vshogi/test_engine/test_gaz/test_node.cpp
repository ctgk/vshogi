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
    Node node{};
    void setup()
    {
        node.init();
    }
};

TEST(test_gaz_node, select_from)
{
    node.simulate_ongoing_and_expand(Game("g4/5/5/5/4G b -"), 0.f, nullptr);
    node.backprop(node.get_q_value(), nullptr);
    {
        const Node* child_nodes[10] = {};
        child_nodes[0] = node.get_child()->get_sibling();
        const auto actual = node.select_from(child_nodes);
        CHECK_EQUAL(node.get_child()->get_sibling(), actual);
        CHECK_EQUAL(&node, actual->get_parent());
    }
    {
        const auto actual = node.select();
        CHECK_EQUAL(node.get_child(), actual);
        CHECK_EQUAL(&node, actual->get_parent());
    }
}

TEST(test_gaz_node, select_given_policy)
{
    float policy_logits[State::num_dlshogi_policy()] = {};
    for (uint ii = State::num_dlshogi_policy(); ii--;)
        policy_logits[ii] = static_cast<float>(ii);

    node.simulate_ongoing_and_expand(
        Game("g4/5/5/5/4G b -"), 0.f, policy_logits);
    node.backprop(node.get_q_value(), nullptr);
    const auto actual = node.select();
    CHECK_EQUAL(node.get_child()->get_sibling()->get_sibling(), actual);
    CHECK_EQUAL(&node, actual->get_parent());
}

TEST(test_gaz_node, get_q_value)
{
    node.simulate_ongoing_and_expand(Game("g4/5/5/5/4G b -"), 0.f, nullptr);
    node.backprop(node.get_q_value(), nullptr);
    {
        const auto c = node.select();
        c->simulate_mate_and_expand(static_cast<move_t>(0));
        c->backprop(c->get_q_value(), nullptr);
        node.backprop(-c->get_q_value(), c);
    }
    DOUBLES_EQUAL(-0.5f, node.get_q_value(), 1e-3f);
    DOUBLES_EQUAL(-1.f, node.get_q_value(1u), 1e-3f);
}

} // namespace test_vshogi::test_engine::test_gaz
