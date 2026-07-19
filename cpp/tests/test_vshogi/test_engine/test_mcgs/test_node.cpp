#include "vshogi/engine/contiguous_buffer.hpp"
#include "vshogi/engine/mcgs/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_mcgs
{

using namespace vshogi;
using C = vshogi::minishogi::Config;
using Game = vshogi::minishogi::Game;
using MT = vshogi::minishogi::MoveTraits;
template <class T>
using ContiguousBuffer = vshogi::engine::ContiguousBuffer<T>;
using Edge = vshogi::engine::mcgs::Edge;
using Node = vshogi::engine::mcgs::Node;

TEST_GROUP (test_mcgs_node) {
};

TEST(test_mcgs_node, test_no_child)
{
    Node n{};
    CHECK_EQUAL(nullptr, n.select());
}

TEST(test_mcgs_node, test_backprop_mate_to_loss_and_no_mate)
{
    Node n{}, c1{}, c2{};
    ContiguousBuffer<Edge> buffer{10u};
    n.simulate_ongoing_and_expand(buffer, Game("4K/2g2/5/5/5 b -"), 0.2f);
    CHECK_EQUAL(0u, n.get_visits());
    n.backprop();
    CHECK_EQUAL(1u, n.get_visits());

    CHECK_EQUAL(2u, n.count_childs());
    Edge* e1 = n.select();
    CHECK_EQUAL(n.get_child(), e1);

    e1->set_child(c1);
    c1.simulate_mate_and_expand(buffer, static_cast<move_t>(1));
    CHECK_TRUE(c1.is_mate_to_win());

    Edge* e2 = e1 + 1;
    e2->set_child(c2);
    c2.simulate_ongoing_and_expand(buffer, Game("5/5/p3P/5/5 w -"), 0.4f);
    CHECK_FALSE(c2.is_mate());

    CHECK_FALSE(e1->is_mate());
    CHECK_EQUAL(0u, e1->get_visits());
    e1->backprop();
    CHECK_TRUE(e1->is_mate_to_lose());
    CHECK_EQUAL(1u, e1->get_visits());

    CHECK_FALSE(e2->is_mate());
    CHECK_EQUAL(0u, e2->get_visits());
    e2->backprop();
    CHECK_FALSE(e2->is_mate());
    CHECK_EQUAL(1u, e2->get_visits());

    CHECK_FALSE(n.is_mate());
    CHECK_EQUAL(1u, n.get_visits());
    n.backprop();
    CHECK_EQUAL(3u, n.get_visits());
    CHECK_FALSE(n.is_mate());
    DOUBLES_EQUAL((0.2f - 0.4f - 1.f) / 3.f, n.get_value(), 1e-5f);
}

TEST(test_mcgs_node, test_backprop_all_mate_to_loss)
{
    Node n{}, c1{}, c2{};
    ContiguousBuffer<Edge> buffer{10u};
    n.simulate_ongoing_and_expand(buffer, Game("4K/2g2/5/5/5 b -"), 0.2f);
    CHECK_EQUAL(0u, n.get_visits());
    n.backprop();
    CHECK_EQUAL(1u, n.get_visits());

    CHECK_EQUAL(2u, n.count_childs());
    Edge* e1 = n.select();
    CHECK_EQUAL(n.get_child(), e1);

    e1->set_child(c1);
    c1.simulate_mate_and_expand(buffer, static_cast<move_t>(1));
    CHECK_TRUE(c1.is_mate_to_win());

    Edge* e2 = e1 + 1;
    e2->set_child(c2);
    c2.simulate_mate_and_expand(buffer, static_cast<move_t>(2));
    CHECK_TRUE(c2.is_mate_to_win());

    for (auto e : {e1, e2}) {
        CHECK_FALSE(e->is_mate());
        CHECK_EQUAL(0u, e->get_visits());
        e->backprop();
        CHECK_TRUE(e->is_mate_to_lose());
        CHECK_EQUAL(1u, e->get_visits());
    }

    CHECK_FALSE(n.is_mate());
    CHECK_EQUAL(1u, n.get_visits());
    n.backprop();
    CHECK_EQUAL(3u, n.get_visits());
    CHECK_TRUE(n.is_mate_to_lose());
    DOUBLES_EQUAL(-1.f, n.get_value(), 1e-5f);
}

TEST(test_mcgs_node, test_backprop_mate_to_win)
{
    Node n{}, c{};
    ContiguousBuffer<Edge> buffer{10u};
    n.expand(buffer, Game("5/4P/5/5/5 b -"), nullptr);
    Edge* e = n.select();
    e->set_child(c);
    c.simulate(Game("4+P/5/5/5/5 w -"));
    CHECK_TRUE(c.is_mate_to_lose());

    CHECK_FALSE(e->is_mate());
    CHECK_EQUAL(0u, e->get_visits());
    e->backprop();
    CHECK_TRUE(e->is_mate());
    CHECK_TRUE(e->is_mate_to_win());
    CHECK_EQUAL(1u, e->get_visits());

    CHECK_FALSE(n.is_mate());
    n.backprop();
    CHECK_TRUE(n.is_mate());
    CHECK_TRUE(n.is_mate_to_win());
}

TEST(test_mcgs_node, test_simulate_mate_and_expand)
{
    Node n{};
    ContiguousBuffer<Edge> buffer{10u};
    CHECK_FALSE(n.is_mate());
    CHECK_FALSE(n.is_mate_to_win());
    CHECK_FALSE(n.is_mate_to_lose());

    n.simulate_mate_and_expand(buffer, MT::make_move("G*1b"));

    CHECK_EQUAL(0u, n.get_visits());
    DOUBLES_EQUAL(1.f, n.get_value(), 1e-4f);
    CHECK_EQUAL(1u, n.count_childs());
    CHECK_TRUE(n.is_mate());
    CHECK_TRUE(n.is_mate_to_win());
    CHECK_FALSE(n.is_mate_to_lose());

    Node c{};
    CHECK_FALSE(c.is_mate());
    n.child()->set_child(c);
    CHECK_TRUE(c.is_mate());
    CHECK_FALSE(c.is_mate_to_win());
    CHECK_TRUE(c.is_mate_to_lose());
    DOUBLES_EQUAL(-1.f, c.get_value(), 1e-5f);
}

TEST(test_mcgs_node, test_select_unexplored)
{
    ContiguousBuffer<Edge> buffer{10u};
    Node n{};
    float policy_logits[C::dlshogi_policy_size] = {0.f};
    policy_logits[MT::to_policy_index(MT::make_move("1e1d"), BLACK)] = 0.f;
    policy_logits[MT::to_policy_index(MT::make_move("1e2d"), BLACK)] = 9.f;
    policy_logits[MT::to_policy_index(MT::make_move("1e2e"), BLACK)] = 1.f;
    n.expand(buffer, Game("k4/5/5/5/4K b -"), policy_logits);
    n.backprop(); // to increment visit counts of node "n"
    {
        const auto e = n.select();
        CHECK_EQUAL(MT::make_move("1e2d"), e->get_action());
        e->backprop();
        CHECK_EQUAL(1u, e->get_visits());
    }
    {
        const auto e_best = n.select();
        CHECK_EQUAL(MT::make_move("1e2d"), e_best->get_action());
        CHECK_EQUAL(1u, e_best->get_visits());

        const auto e = n.select_unexplored();
        CHECK_TRUE(e);
        CHECK_EQUAL(MT::make_move("1e2e"), e->get_action());
        CHECK_EQUAL(0u, e->get_visits());
        e->backprop();
    }
    {
        const auto e_best = n.select();
        CHECK_EQUAL(MT::make_move("1e2d"), e_best->get_action());
        CHECK_EQUAL(1u, e_best->get_visits());

        const auto e = n.select_unexplored();
        CHECK_TRUE(e);
        CHECK_EQUAL(MT::make_move("1e1d"), e->get_action());
        CHECK_EQUAL(0u, e->get_visits());
        e->backprop();
    }
    {
        const auto e_best = n.select();
        CHECK_EQUAL(MT::make_move("1e2d"), e_best->get_action());
        CHECK_EQUAL(1u, e_best->get_visits());

        const auto e = n.select_unexplored();
        CHECK_EQUAL(nullptr, e);
    }
}

TEST(test_mcgs_node, test_select_with_policy)
{
    constexpr auto SQ_2D = vshogi::minishogi::SQ_2D;
    constexpr auto SQ_2E = vshogi::minishogi::SQ_2E;
    ContiguousBuffer<Edge> buffer{10u};
    Node n{};
    Game game("k4/5/5/5/4K b -");
    float policy_logits[C::dlshogi_policy_size] = {0.f};
    policy_logits[MT::to_policy_index(MT::make_move("1e1d"), BLACK)] = -0.01f;
    policy_logits[MT::to_policy_index(MT::make_move("1e2d"), BLACK)] = 0.01f;
    policy_logits[MT::to_policy_index(MT::make_move("1e2e"), BLACK)] = 0.0f;
    n.simulate_ongoing_and_expand(buffer, game, 0.9f, policy_logits);
    n.backprop();
    DOUBLES_EQUAL(0.9f, n.get_value(), 1e-4f);
    CHECK_EQUAL(1u, n.get_visits());
    {
        Edge* const e = n.select();
        CHECK_EQUAL(SQ_2D, MT::get_dst(e->get_action()));
        auto leaf = std::make_unique<Node>();
        leaf->simulate_ongoing_and_expand(buffer, Game("5/5/5/5/5 w -"), 0.9f);
        e->set_child(*leaf);
        e->backprop();
        CHECK_EQUAL(1u, e->get_visits());
        DOUBLES_EQUAL(-0.9f, e->get_value(), 1e-4f);

        n.backprop();
        DOUBLES_EQUAL(0.0f, n.get_value(), 1e-4f);
        CHECK_EQUAL(2u, n.get_visits());
    }
    {
        Edge* const e = n.select();
        CHECK_EQUAL(SQ_2E, MT::get_dst(e->get_action()));
        auto leaf = std::make_unique<Node>();
        leaf->simulate_ongoing_and_expand(buffer, Game("5/5/5/5/5 w -"), 0.9f);
        e->set_child(*leaf);
        e->backprop();
        CHECK_EQUAL(1u, e->get_visits());
        DOUBLES_EQUAL(-0.9f, e->get_value(), 1e-4f);

        n.backprop();
        DOUBLES_EQUAL(-0.3f, n.get_value(), 1e-4f);
        CHECK_EQUAL(3u, n.get_visits());
    }
}

TEST(test_mcgs_node, test_expand_enhanced_checks)
{
    {
        ContiguousBuffer<Edge> buffer{10u};
        Node node{};
        Game game("4k/5/5/5/2B1K b -");
        node.expand<false>(buffer, game, nullptr);
        CHECK_EQUAL(7, node.count_childs());
        CHECK_COMPARE(
            MT::make_move("3e4d"), !=, node.get_child()->get_action());
    }
    {
        ContiguousBuffer<Edge> buffer{10u};
        Node node{};
        Game game("4k/5/5/5/2B1K b -");
        node.expand<true>(buffer, game, nullptr);
        CHECK_EQUAL(7, node.count_childs());
        CHECK_EQUAL(MT::make_move("3e4d"), node.get_child()->get_action());
    }
}

TEST(test_mcgs_node, test_expand_with_policy)
{
    ContiguousBuffer<Edge> buffer{10u};
    Node n{};
    Game game("k4/5/5/5/4K b -");
    float policy_logits[C::dlshogi_policy_size] = {0.f};
    policy_logits[MT::to_policy_index(MT::make_move("1e1d"), BLACK)] = -1.f;
    policy_logits[MT::to_policy_index(MT::make_move("1e2d"), BLACK)] = 1.f;
    policy_logits[MT::to_policy_index(MT::make_move("1e2e"), BLACK)] = 0.f;
    n.expand(buffer, game, policy_logits);
    const Edge* const e1 = n.get_child();
    const Edge* const e2 = e1->get_sibling();
    const Edge* const e3 = e2->get_sibling();
    CHECK_TRUE(e1 != nullptr);
    CHECK_TRUE(e2 != nullptr);
    CHECK_TRUE(e3 != nullptr);
    CHECK_EQUAL(nullptr, e3->get_sibling());
    DOUBLES_EQUAL(
        1.f,
        e1->get_prior_proba() + e2->get_prior_proba() + e3->get_prior_proba(),
        1e-4f);

    DOUBLES_EQUAL(0.66524f, e1->get_prior_proba(), 1e-4f);
    DOUBLES_EQUAL(0.24473f, e2->get_prior_proba(), 1e-4f);
    DOUBLES_EQUAL(0.09003f, e3->get_prior_proba(), 1e-4f);
    CHECK_EQUAL(MT::make_move("1e2d"), e1->get_action());
    CHECK_EQUAL(MT::make_move("1e2e"), e2->get_action());
    CHECK_EQUAL(MT::make_move("1e1d"), e3->get_action());
}

TEST(test_mcgs_node, test_expand)
{
    ContiguousBuffer<Edge> buffer(100u);
    Node n{};
    Game game{};
    n.expand(buffer, game, nullptr);
}

TEST(test_mcgs_node, test_destruct_no_child)
{
    ContiguousBuffer<Edge> buffer(0u);
    buffer.emplace_next(nullptr, 0.f, static_cast<move_t>(0));
    CHECK_TRUE(buffer.is_full());

    Node n{};
    Game game{};
    n.simulate_mate_and_expand(buffer, static_cast<move_t>(0));
    DOUBLES_EQUAL(1.f, n.get_value(), 1e-5f);
    CHECK_TRUE(n.is_mate_to_win());

    n.destruct();

    DOUBLES_EQUAL(0.f, n.get_value(), 1e-5f);
    CHECK_FALSE(n.is_mate_to_win());
}

TEST(test_mcgs_node, test_destruct_with_child)
{
    ContiguousBuffer<Edge> buffer(2u);
    Node node{};
    node.simulate_mate_and_expand(buffer, static_cast<move_t>(1));
    CHECK_TRUE(node.has_child());
    const Edge* const edge = node.get_child();

    CHECK_EQUAL(&node, edge->get_parent());
    CHECK_EQUAL(static_cast<move_t>(1), edge->get_action());
    DOUBLES_EQUAL(1.f, edge->get_value(), 1e-5f);
    CHECK_TRUE(edge->is_mate_to_win());

    node.destruct();

    CHECK_EQUAL(nullptr, edge->get_parent());
    CHECK_EQUAL(static_cast<move_t>(0), edge->get_action());
    DOUBLES_EQUAL(-1.f, edge->get_value(), 1e-5f);
    CHECK_FALSE(edge->is_mate_to_win());
}

} // namespace test_vshogi::test_engine::test_mcgs
