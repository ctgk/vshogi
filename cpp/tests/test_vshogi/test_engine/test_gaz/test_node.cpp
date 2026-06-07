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

TEST(test_gaz_node, get_q_value_returns_compute_v_pi)
{
    /**
     * Test that get_q_value() returns the result of compute_v_pi()
     * when greedy_depth is 0 or child visits < min_visits
     *
     * Setup:
     *                    root (v=0.2)
     *                    /         \
     *                   /           \
     *              logit=1.0     logit=-1.0
     *                 /               \
     *                /                 \
     *         child1(v=-0.5, n=5)   child2(v=-0.3, n=3)
     *
     * compute_v_pi() should return weighted average:
     * pi = softmax([1.0, -1.0]) = [0.88, 0.12]
     * v_pi = 0.88 * 0.5 + 0.12 * 0.3 = 0.476
     */

    auto g = Game("4k/5/5/5/4S b -");

    // Setup root with two children having different logits and visit counts
    float logits[Config::dlshogi_policy_size] = {0.f};
    logits[MT::to_policy_index(MT::make_move(SQ_1E, SQ_1D), BLACK)] = 1.0f;
    logits[MT::to_policy_index(MT::make_move(SQ_1E, SQ_2D), BLACK)] = -1.0f;

    root.simulate_ongoing_and_expand(next, g, 0.2f, logits);
    root.backprop(root.get_q_value(), nullptr);
    const Node* childs[3] = {&nodes[0], &nodes[1], nullptr};

    {
        auto g_copy = Game(g);
        Node* child1 = root.select_from(childs);
        g_copy.apply_nocheck(child1->get_action());
        child1->simulate_ongoing_and_expand(next, g_copy, -0.5f, nullptr);
        child1->backprop(child1->get_q_value(), nullptr);
        root.backprop(-child1->get_q_value(), child1);
    }
    {
        auto g_copy = Game(g);
        Node* child2 = root.select_from(childs);
        g_copy.apply_nocheck(child2->get_action());
        child2->simulate_ongoing_and_expand(next, g_copy, -0.3f, nullptr);
        child2->backprop(child2->get_q_value(), nullptr);
        root.backprop(-child2->get_q_value(), child2);
    }

    // Test with greedy_depth = 0 should use compute_v_pi()
    // Expected: softmax([1.0, -1.0]) = [0.88, 0.12]
    // v_pi = 0.88 * 0.5 + 0.12 * 0.3 = 0.476
    const float result_greedy_0 = root.get_q_value(0u, 0u);
    DOUBLES_EQUAL(0.476f, result_greedy_0, 1e-2f);

    // Test with min_visits > child's visit count use compute_v_pi()
    const float result_min_visits = root.get_q_value(100u, 10u);
    DOUBLES_EQUAL(0.476f, result_min_visits, 1e-2f);

    // Test with greedy_depth > 0 and sufficient visits use child's q_value
    const float result_greedy = root.get_q_value(1u, 0u);
    DOUBLES_EQUAL(0.5f, result_greedy, 1e-2f); // Should return -(-0.5) = 0.5
}

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
    float policy_logits[Config::dlshogi_policy_size] = {};
    for (uint ii = Config::dlshogi_policy_size; ii--;)
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
