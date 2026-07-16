#include "vshogi/engine/mcgs/searcher.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_mcgs
{

namespace test_minishogi
{

using namespace vshogi;
using C = vshogi::minishogi::Config;
using Game = vshogi::minishogi::Game;
using MT = vshogi::minishogi::MoveTraits;
using Node = vshogi::engine::mcgs::Node;
using Searcher = vshogi::engine::mcgs::Searcher<vshogi::minishogi::Parameters>;

TEST_GROUP (test_minishogi_mcgs_mock_dist01) {
    std::uniform_real_distribution<float> dist01_backup;
    void setup()
    {
        dist01_backup = vshogi::dist01;
    }
    void teardown()
    {
        vshogi::dist01 = dist01_backup;
    }
};

TEST(test_minishogi_mcgs_mock_dist01, test_select_unexplored)
{
    // Mock to make random exploration depth equals to 1.
    vshogi::dist01 = std::uniform_real_distribution<float>(0.f, 0.01f);

    Game game{};
    Searcher searcher(100u);
    {
        Node* const n = searcher.search(game, 1.f);
        CHECK_EQUAL(&searcher.get_root(), n);
        CHECK_EQUAL(0u, game.ply());
        searcher.simulate_expand_backprop(n, game, 0.f);
        CHECK_EQUAL(1u, searcher.get_search_count());
    }
    {
        Node* const n = searcher.search(game, 1.f);
        const auto c = searcher.get_root().get_child_node();
        CHECK_EQUAL(c, n);
        CHECK_EQUAL(1u, game.ply());

        // Note that backprop runs in this scope
        searcher.simulate_expand_backprop(n, game, -0.3f);

        CHECK_EQUAL(0u, game.ply());
        CHECK_EQUAL(2u, searcher.get_search_count());
        DOUBLES_EQUAL(0.15f, searcher.get_root().get_value(), 1e-5f);
        DOUBLES_EQUAL(-0.3f, c->get_value(), 1e-5f);
        CHECK_EQUAL(1u, c->get_visits());
    }
    {
        Node* const n = searcher.search(game, 1.f);
        const auto c
            = searcher.get_root().get_child()->get_sibling()->get_child();
        CHECK_EQUAL(c, n);
        CHECK_EQUAL(1u, game.ply());

        // Note that backprop does not run in this scope
        searcher.simulate_expand_backprop(n, game, -0.9f);

        CHECK_EQUAL(0u, game.ply());
        CHECK_EQUAL(2u, searcher.get_search_count());
        DOUBLES_EQUAL(0.15f, searcher.get_root().get_value(), 1e-5f);
        DOUBLES_EQUAL(-0.9f, c->get_value(), 1e-5f);
        CHECK_EQUAL(0u, c->get_visits());
        CHECK_TRUE(c->has_child());
    }
    {
        // Note that auto-backprop runs in this scope
        Node* const n = searcher.search(game); // exploit
        CHECK_EQUAL(nullptr, n);
        CHECK_EQUAL(0u, game.ply());
        CHECK_EQUAL(3u, searcher.get_search_count());
        DOUBLES_EQUAL(0.4f, searcher.get_root().get_value(), 1e-5f);

        const auto c
            = searcher.get_root().get_child()->get_sibling()->get_child();
        DOUBLES_EQUAL(-0.9f, c->get_value(), 1e-5f);
        CHECK_EQUAL(1u, c->get_visits());
    }
}

TEST_GROUP (test_minishogi_mcgs_searcher) {
};

TEST(test_minishogi_mcgs_searcher, explore_until_game_end_with_fifty_budget)
{
    constexpr uint search_budget = 50u;
    auto g = Game();
    auto searcher = Searcher(2000u);
    for (uint ply = 0u; g.get_result() == ONGOING; ++ply) {
        CHECK_COMPARE(search_budget, >=, searcher.get_search_count());
        for (uint ii = search_budget - searcher.get_search_count(); ii--;) {
            const auto n = searcher.search(g);
            CHECK_COMPARE(ply, <=, g.ply());
            searcher.simulate_expand_backprop(n, g, 0.f);
            CHECK_EQUAL(ply, g.ply());
        }
        const auto action = searcher.select_action();
        CHECK_TRUE(g.is_legal(action));
        searcher.apply(g, action);
    }
}

TEST(test_minishogi_mcgs_searcher, explore_until_game_end_with_one_budget)
{
    constexpr uint search_budget = 1u;
    auto g = Game();
    auto searcher = Searcher(1000u);
    for (uint ply = 0u; g.get_result() == ONGOING; ++ply) {
        CHECK_EQUAL(0u, searcher.get_search_count());
        for (uint ii = search_budget; ii--;) {
            const auto n = searcher.search(g);
            CHECK_EQUAL(ply, g.ply());
            CHECK_EQUAL(&searcher.get_root(), n);
            searcher.simulate_expand_backprop(n, g, 0.f);
            CHECK_EQUAL(ply, g.ply());
        }
        const auto action = searcher.select_action();
        CHECK_TRUE(g.is_legal(action));
        searcher.apply(g, action);
    }
}

TEST(test_minishogi_mcgs_searcher, avoid_cyclings)
{
    // Target: Identify and differentiate duplicate states in a path.
    //
    // Initial State (#): "GGPPP/PP1K1/5/pp1k1/ggppp b -"
    // Target State  (*): "GGPPP/PP1K1/5/pp2k/ggppp w -"
    //
    // Path 1: # -> 2b1b -> 2d1d -> 1b2b -> * -> 1d2d
    // Path 2: # -> 2b3b -> 2d1d -> 3b2b -> *
    //
    // Combined Path:
    // # -> 2b1b -> 2d1d -> 1b2b -> * -> 1d2d -> 2b3b -> 2d1d -> 3b2b -> *
    //
    // NOTE: The combined path visits the target node (*) twice.
    // These duplicate states must be explicitly differentiated, because the
    // the former is the 1st-fold while the latter is the 2nd-fold.

    auto g = Game("GGPPP/PP1K1/5/pp1k1/ggppp b -");
    auto searcher = Searcher(1000u);

    {
        const auto n = searcher.search(g);
        CHECK_EQUAL(&searcher.get_root(), n);
        CHECK_EQUAL(0, g.ply());
        float logits[C::dlshogi_policy_size] = {};
        logits[MT::to_policy_index(MT::make_move("2b1b"), BLACK)] = 0.1f;
        searcher.simulate_expand_backprop(n, g, 0.f, logits);
        CHECK_EQUAL(0, g.ply());
    }
    for (uint ii = 27; ii--;) {
        const auto n = searcher.search(g);
        if (g.ply()) {
            CHECK_TRUE(&searcher.get_root() != n);
        } else {
            CHECK_EQUAL(nullptr, n);
        }
        searcher.simulate_expand_backprop(n, g, 0.f);
        CHECK_EQUAL(0, g.ply());
    }
    const Node* c3 = nullptr;
    {
        // Check existence of the 1st path:
        // Path 1: # -> 2b1b -> 2d1d -> 1b2b -> * -> 1d2d
        const auto c1
            = searcher.get_root().get_child_node_of(MT::make_move("2b1b"));
        CHECK_TRUE(c1);
        const auto c2 = c1->get_child_node_of(MT::make_move("2d1d"));
        CHECK_TRUE(c2);
        c3 = c2->get_child_node_of(MT::make_move("1b2b"));
        CHECK_TRUE(c3);
        CHECK_EQUAL(2u, c3->num_parents());
    }
    {
        // Check existence of the 2nd path:
        // Path 2: # -> 2b3b -> 2d1d -> 3b2b -> *
        const auto c1
            = searcher.get_root().get_child_node_of(MT::make_move("2b3b"));
        CHECK_TRUE(c1);
        const auto c2 = c1->get_child_node_of(MT::make_move("2d1d"));
        CHECK_TRUE(c2);
        const auto c3_ = c2->get_child_node_of(MT::make_move("3b2b"));
        CHECK_EQUAL(c3, c3_);
    }
    {
        // Check the combined path:
        // # -> 2b1b -> 2d1d -> 1b2b -> * -> 1d2d -> 2b3b -> 2d1d -> 3b2b -> *
        const auto c4 = c3->get_child_node_of(MT::make_move("1d2d"));
        CHECK_TRUE(c4);
        CHECK_TRUE(c4 != &searcher.get_root());

        const auto c5 = c4->get_child_node_of(MT::make_move("2b3b"));
        CHECK_TRUE(c5);
        // Two nodes, c5 and #->2b1b, are both the 1st fold of "GGPPP/PPK2/5/pp1k1/ggppp w -"
        // But they must be differentiated to avoid cycles.
        CHECK_TRUE(
            c5 != searcher.get_root().get_child_node_of(MT::make_move("2b3b")));
    }
    {
        const auto action = searcher.select_action();
        CHECK_EQUAL(MT::make_move("2b1b"), action);
        CHECK_TRUE(g.is_legal(action));
        searcher.apply(g, action);
        const auto c2
            = searcher.get_root().get_child_node_of(MT::make_move("2d1d"));
        CHECK_TRUE(c2);
        const auto c3_ = c2->get_child_node_of(MT::make_move("1b2b"));
        CHECK_EQUAL(c3, c3_);
        CHECK_EQUAL(1u, c3->num_parents());
    }
    {
        CHECK_EQUAL(3u, searcher.get_root().count_childs());
        const auto action = searcher.select_action();
        CHECK_TRUE(g.is_legal(action));
        searcher.apply(g, action);
    }
}

TEST(test_minishogi_mcgs_searcher, test_search_after_mate_proved)
{
    Searcher searcher{10000u};
    Game game("1r3/2k1G/5/2PG1/5 b -");
    const auto m = MT::make_move("2d3c");
    for (uint ii = 100u; ii--;) {
        Node* const n = searcher.search(game);
        searcher.simulate_expand_backprop(n, game, -0.9f * game.in_check());
        if (searcher.proved_mate())
            break;
    }
    CHECK_TRUE(searcher.proved_mate());
    const uint visits = searcher.get_root().get_child_node_of(m)->get_visits();

    for (uint ii = 50u; ii--;) {
        Node* const n = searcher.search(game, 0.99f);
        CHECK_EQUAL(nullptr, n);
    }
    const uint actual = searcher.get_root().get_child_node_of(m)->get_visits();
    CHECK_EQUAL(visits + 50u, actual);
}

TEST(test_minishogi_mcgs_searcher, test_prove_mate)
{
    Searcher searcher{1000u};
    Game game("5/4k/5/4P/5 b 2G");
    for (uint ii = 100u; ii--;) {
        Node* const n = searcher.search(game, 0.2f);
        float logits[C::dlshogi_policy_size] = {};
        logits[MT::to_policy_index(MT::make_move("G*1c"), BLACK)] = 10.f;
        logits[MT::to_policy_index(MT::make_move("G*1b"), BLACK)] = 10.f;
        logits[MT::to_policy_index(MT::make_move("G*2b"), BLACK)] = 9.f;
        searcher.simulate_expand_backprop(
            n, game, -0.9f * game.in_check(), logits);
        if (searcher.proved_mate())
            break;
    }
    const auto c1
        = searcher.get_root().get_child_node_of(MT::make_move("G*1c"));
    CHECK_TRUE(c1);
    DOUBLES_EQUAL(-0.9f, c1->get_prior_value(), 1e-5f);
    DOUBLES_EQUAL(-1.f, c1->get_value(), 1e-5f);
    const auto c2 = c1->get_child_node_of(MT::make_move("1b1a"));
    CHECK_TRUE(c2);
    DOUBLES_EQUAL(0.f, c2->get_prior_value(), 1e-5f);
    DOUBLES_EQUAL(1.f, c2->get_value(), 1e-5f);
    CHECK_FALSE(searcher.edge_buffer().is_full());
    CHECK_TRUE(searcher.proved_mate());
}

TEST(test_minishogi_mcgs_searcher, test_dfpn_root)
{
    Searcher searcher{100u, 1000u, 0u};
    Game game("3b1/2r1k/3pB/3gR/5 b G");
    {
        CHECK_EQUAL(nullptr, searcher.search(game));
        CHECK_EQUAL(1u, searcher.get_search_count());
        DOUBLES_EQUAL(1.f, searcher.get_root().get_value(), 1e-5f);
        CHECK_TRUE(searcher.proved_mate());
        CHECK_EQUAL(1u, searcher.get_root().count_childs());
        CHECK_TRUE(searcher.get_root().get_child()->is_mate_to_win());
        CHECK_FALSE(searcher.get_root().get_child()->has_child());
        const auto mate_moves = searcher.get_mate_moves(game);
        CHECK_EQUAL(5u, mate_moves.size());
        CHECK_EQUAL(MT::make_move("G*2b"), mate_moves[0]);
        CHECK_EQUAL(0u, game.ply());
        CHECK_EQUAL(MT::make_move("G*2b"), searcher.select_action());
    }
    {
        CHECK_EQUAL(nullptr, searcher.search(game));
        CHECK_EQUAL(0u, game.ply());
        CHECK_EQUAL(2u, searcher.get_search_count());
        CHECK_TRUE(searcher.get_root().get_child()->has_child());
        const Node* const c = searcher.get_root().get_child_node();
        CHECK_EQUAL(1u, c->get_visits());
        CHECK_TRUE(c->is_mate_to_lose());
        DOUBLES_EQUAL(-1.f, c->get_value(), 1e-5f);
        CHECK_EQUAL(1u, c->count_childs());
        CHECK_TRUE(c->get_child()->is_leaf());
        CHECK_FALSE(c->get_child()->is_mate());
    }
    {
        Node* const n = searcher.search(game);
        CHECK_EQUAL(2u, game.ply());
        CHECK_EQUAL(minishogi::W_HI, game.get_board()[minishogi::SQ_2B]);

        searcher.simulate_expand_backprop(n, game, 0.9f);

        CHECK_EQUAL(0u, game.ply());
        CHECK_EQUAL(3u, searcher.get_search_count());
        const Node* const c1 = searcher.get_root().get_child_node();
        CHECK_TRUE(c1);
        CHECK_EQUAL(2u, c1->get_visits());
        CHECK_TRUE(c1->is_mate_to_lose());
        DOUBLES_EQUAL(-1.f, c1->get_value(), 1e-5f);
        const Node* const c2 = c1->get_child_node();
        CHECK_TRUE(c2);
        CHECK_EQUAL(1u, c2->get_visits());
        CHECK_FALSE(c2->is_mate());
        DOUBLES_EQUAL(0.9f, c2->get_value(), 1e-5f);
    }
}

TEST(test_minishogi_mcgs_searcher, test_apply_with_repetitions)
{
    auto g = Game("PPPPP/4K/5/4k/ppppp b -");
    auto searcher = Searcher(10u);
    for (uint ii = 5u; ii--;) {
        Node* const n = searcher.search(g);
        searcher.simulate_expand_backprop(n, g, 0.f);
    }
    {
        const Node& root = searcher.get_root();
        CHECK_EQUAL(5u, root.get_visits());
        CHECK_EQUAL(1u, root.count_childs());
        const Node* const c1 = root.get_child()->get_child();
        CHECK_TRUE(c1);
        CHECK_EQUAL(4u, c1->get_visits());
        CHECK_EQUAL(1u, c1->count_childs());
        const Node* const c2 = c1->get_child()->get_child();
        CHECK_TRUE(c2);
        CHECK_EQUAL(3u, c2->get_visits());
        CHECK_EQUAL(2u, c2->count_childs());
        const Node* const c31 = c2->get_child()->get_child();
        const Node* const c32 = c2->get_child()->get_sibling()->get_child();
        CHECK_TRUE(c31);
        CHECK_EQUAL(1u, c31->get_visits());
        CHECK_EQUAL(3u, c31->count_childs());
        CHECK_TRUE(c32);
        CHECK_EQUAL(1u, c32->get_visits());
        CHECK_EQUAL(3u, c32->count_childs());
    }
    searcher.apply(g, MT::make_move("1b2b"));
    searcher.apply(g, MT::make_move("1d2d"));
    {
        const Node* const c2 = &(searcher.get_root());
        CHECK_TRUE(c2);
        CHECK_EQUAL(3u, c2->get_visits());
        CHECK_EQUAL(2u, c2->count_childs());
        const Node* const c31 = c2->get_child()->get_child();
        const Node* const c32 = c2->get_child()->get_sibling()->get_child();
        CHECK_TRUE(c31);
        CHECK_EQUAL(1u, c31->get_visits());
        CHECK_EQUAL(3u, c31->count_childs());
        CHECK_TRUE(c32);
        CHECK_EQUAL(1u, c32->get_visits());
        CHECK_EQUAL(3u, c32->count_childs());
    }
    searcher.apply(g, MT::make_move("2b1b"));
    {
        const Node* const c31 = &(searcher.get_root());
        CHECK_TRUE(c31);
        CHECK_EQUAL(1u, c31->get_visits());
        CHECK_EQUAL(3u, c31->count_childs());
    }
}

TEST(test_minishogi_mcgs_searcher, test_apply_transposition_parent_count)
{
    auto g = Game("3pS/4P/5/5/5 b -");
    auto searcher = Searcher(10u);
    for (uint ii = 5u; ii--;) {
        Node* const n = searcher.search(g);
        searcher.simulate_expand_backprop(n, g, 0.f);
    }

    // c1 node should have 2 parents before we apply the move
    const Node* const c = searcher.get_root().get_child_node();
    CHECK_TRUE(c);
    const Node* const c1 = c->get_child_node();
    CHECK_TRUE(c1);
    CHECK_EQUAL(2u, c1->num_parents());

    // Apply "1a2b+", which makes c1 the new root of the searcher.
    // The other parent (from S4's edge) should now point to the new root (m_root).
    searcher.apply(g, MT::make_move("1a2b+"));

    // "1a2b" parent has been removed
    CHECK_EQUAL(1u, c1->num_parents());

    // The root node of the searcher now has 1 parent (from S4's edge)
    CHECK_EQUAL(0u, searcher.get_root().num_parents());
}

TEST(test_minishogi_mcgs_searcher, test_apply_2)
{
    Game game("2k2/5/5/5/4K b -");
    Searcher searcher(50u);
    for (uint ii = 2u; ii--;) {
        Node* const n = searcher.search(game);
        float logits[C::dlshogi_policy_size] = {0.f};
        logits[MT::to_policy_index(MT::make_move("1e2e"), BLACK)] = 100.f;
        searcher.simulate_expand_backprop(
            n, game, static_cast<float>(1u - ii) / 2.f, logits);
        CHECK_EQUAL(0u, game.ply());
    }
    const auto action = searcher.select_action();
    CHECK_EQUAL(MT::make_move("1e2e"), action);
    CHECK_EQUAL(2u, searcher.get_search_count());
    DOUBLES_EQUAL(-0.25f, searcher.get_root().get_value(), 1e-5f);
    CHECK_EQUAL(3u, searcher.get_root().count_childs());

    searcher.apply(game, action);

    CHECK_EQUAL(1u, searcher.get_search_count());
    DOUBLES_EQUAL(0.5f, searcher.get_root().get_value(), 1e-5f);
    CHECK_EQUAL(5u, searcher.get_root().count_childs());
}

TEST(test_minishogi_mcgs_searcher, test_apply_action_not_in_childs)
{
    Game game("4k/5/4P/5/4K b G");
    Searcher searcher(100u, 100u, 0u);
    {
        Node* const n = searcher.search(game);
        CHECK_EQUAL(nullptr, n);
    }
    const auto e = searcher.get_root().get_child();
    CHECK_TRUE(e);
    CHECK_EQUAL(MT::make_move("G*1b"), e->get_action());

    searcher.apply(game, MT::make_move("1e1d"));
    CHECK_EQUAL(1u, game.ply());
    CHECK_EQUAL(ONGOING, game.get_result());
    CHECK_EQUAL(0u, searcher.get_search_count());
    CHECK_EQUAL(0u, searcher.get_root().count_childs());
    CHECK_EQUAL(0u, searcher.edge_buffer().size());
    CHECK_EQUAL(0u, searcher.num_nodes_in_table());
}

TEST(test_minishogi_mcgs_searcher, test_apply_illegal_action)
{
    {
        // apply legal action
        Game game{};
        Searcher searcher(1000u);
        for (uint ii = 10u; ii--;) {
            Node* const n = searcher.search(game);
            searcher.simulate_expand_backprop(n, game, 0.1f);
        }
        const auto action = searcher.select_action();
        CHECK_TRUE(game.is_legal(action));
        const auto c = searcher.get_root().get_child_node_of(action);
        CHECK_TRUE(c);
        CHECK_TRUE(c->get_visits());
        CHECK_TRUE(c->count_childs());
        const auto expected_child_counts = c->count_childs();
        const auto expected_visits = c->get_visits();

        searcher.apply(game, action);
        CHECK_EQUAL(expected_child_counts, searcher.get_root().count_childs());
        CHECK_EQUAL(expected_visits, searcher.get_search_count());
    }
    {
        // apply legal action
        Game game{};
        Searcher searcher(1000u);
        for (uint ii = 10u; ii--;) {
            Node* const n = searcher.search(game);
            searcher.simulate_expand_backprop(n, game, 0.1f);
        }
        const auto action = MT::make_move("1e1a");
        CHECK_FALSE(game.is_legal(action));

        searcher.apply(game, action);
        CHECK_EQUAL(0u, searcher.get_root().count_childs());
        CHECK_EQUAL(0u, searcher.get_search_count());
        CHECK_EQUAL(0u, searcher.edge_buffer().size());
        CHECK_EQUAL(0u, searcher.num_nodes_in_table());
    }
}

TEST(test_minishogi_mcgs_searcher, test_apply)
{
    constexpr auto SQ_2B = vshogi::minishogi::SQ_2B;
    constexpr auto B_GI = vshogi::minishogi::B_GI;
    Game game("3pS/5/5/5/P4 b -");
    Searcher searcher(10u);
    {
        Node* const node = searcher.search(game);
        float logits[C::dlshogi_policy_size] = {0.f};
        logits[MT::to_policy_index(MT::make_move("1a2b+"), BLACK)] = 1.1f;
        logits[MT::to_policy_index(MT::make_move("1a2b"), BLACK)] = 1.0f;
        logits[MT::to_policy_index(MT::make_move("1e1d"), BLACK)] = -100.f;
        searcher.simulate_expand_backprop(node, game, 0.0f, logits);
    }
    for (auto v : {0.9f, 0.9f, -1.f, -1.f}) {
        Node* const n = searcher.search(game);
        searcher.simulate_expand_backprop(n, game, v);
    }
    CHECK_EQUAL(3u, searcher.num_nodes_in_table());
    CHECK_EQUAL(6u, searcher.num_edges_in_buffer());

    searcher.apply(game, MT::make_move("1a2b"));

    CHECK_EQUAL(B_GI, game.get_board()[SQ_2B]);
    CHECK_EQUAL(2u, searcher.get_search_count());
    CHECK_EQUAL(1u, searcher.get_root().get_child()->get_visits());
    CHECK_EQUAL(1u, searcher.num_nodes_in_table());
    CHECK_EQUAL(2u, searcher.num_edges_in_buffer());
}

TEST(test_minishogi_mcgs_searcher, test_select_action)
{
    Game game("4k/4p/4P/5/K3R w -");
    Searcher searcher(1000u);
    for (uint ii = 100u; ii--;) {
        Node* const n = searcher.search(game, 0.1f);
        const float v = 0.9f * (game.get_board().get_king_square(WHITE) != 0);
        float logits[C::dlshogi_policy_size] = {};
        logits[MT::to_policy_index(MT::make_move("1b1c"), WHITE)] = 100.f;
        searcher.simulate_expand_backprop(n, game, v, logits);
    }
    const Node& root = searcher.get_root();
    CHECK_TRUE(root.has_child());
    for (auto e = root.get_child(); e->get_parent() == &root; ++e) {
        if (e->get_action() == MT::make_move("1b1c"))
            CHECK_TRUE(e->get_visits());
        else
            CHECK_EQUAL(0u, e->get_visits());
    }
    {
        const auto actual = searcher.select_action();
        CHECK_EQUAL(MT::make_move("1b1c"), actual);
    }
    for (uint ii = 10000u; ii--;) {
        const auto actual = searcher.select_action(1.f);
        CHECK_EQUAL(MT::make_move("1b1c"), actual);
    }
}

TEST(test_minishogi_mcgs_searcher, test_transposition_nodes)
{
    constexpr auto SQ_2B = vshogi::minishogi::SQ_2B;
    constexpr auto B_NG = vshogi::minishogi::B_NG;
    constexpr auto B_GI = vshogi::minishogi::B_GI;
    constexpr auto W_FU = vshogi::minishogi::W_FU;
    Game game("3pS/5/5/5/P4 b -");
    Searcher searcher(10u);
    {
        Node* const root = searcher.search(game);
        CHECK_EQUAL(&searcher.get_root(), root);
        float logits[C::dlshogi_policy_size] = {0.f};
        logits[MT::to_policy_index(MT::make_move("1a2b+"), BLACK)] = 1.1f;
        logits[MT::to_policy_index(MT::make_move("1a2b"), BLACK)] = 1.0f;
        logits[MT::to_policy_index(MT::make_move("1e1d"), BLACK)] = -100.f;
        searcher.simulate_expand_backprop(root, game, 0.0f, logits);
        CHECK_EQUAL(1u, searcher.get_search_count());
        CHECK_EQUAL(0u, searcher.num_nodes_in_table());
    }
    {
        Node* const c = searcher.search(game);
        CHECK_EQUAL(1u, searcher.num_nodes_in_table());
        CHECK_EQUAL(0u, c->get_visits());
        CHECK_EQUAL(1u, game.ply());
        CHECK_EQUAL(B_NG, game.get_board()[SQ_2B]);
        CHECK_EQUAL(1u, searcher.get_search_count());

        searcher.simulate_expand_backprop(c, game, 0.9f);

        CHECK_EQUAL(1u, c->get_visits());
        DOUBLES_EQUAL(0.9f, c->get_value(), 1e-5f);
        CHECK_EQUAL(2u, searcher.get_search_count());
        CHECK_EQUAL(0u, game.ply());
    }
    {
        Node* const c = searcher.search(game);
        CHECK_EQUAL(2u, searcher.num_nodes_in_table());
        CHECK_EQUAL(B_GI, game.get_board()[SQ_2B]);

        searcher.simulate_expand_backprop(c, game, 0.9f);

        CHECK_EQUAL(3u, searcher.get_search_count());
        CHECK_EQUAL(0u, game.ply());
    }
    {
        Node* const c = searcher.search(game);
        CHECK_EQUAL(3u, searcher.num_nodes_in_table());
        CHECK_EQUAL(0u, c->get_visits());
        CHECK_EQUAL(2u, game.ply());
        CHECK_EQUAL(W_FU, game.get_board()[SQ_2B]);
        searcher.simulate_expand_backprop(c, game, -0.8f);
        CHECK_EQUAL(0u, game.ply());
        CHECK_EQUAL(4u, searcher.get_search_count());
        DOUBLES_EQUAL(
            (0.f + -0.9f + -0.9f + -0.8f) / 4.f,
            searcher.get_root().get_value(),
            1e-5f);
    }
    {
        Node* const c = searcher.search(game);
        CHECK_EQUAL(nullptr, c);
        CHECK_EQUAL(0u, game.ply());
        CHECK_EQUAL(5u, searcher.get_search_count());
        DOUBLES_EQUAL(
            (0.f + -0.9f + -0.9f + -0.8f + -0.8f) / 5.f,
            searcher.get_root().get_value(),
            1e-5f);
    }
}

TEST(test_minishogi_mcgs_searcher, test_backprop_two_nodes)
{
    Game game("p4/5/5/5/4P b -");
    Searcher searcher(10u);
    {
        Node* const n = searcher.search(game);
        CHECK_EQUAL(&searcher.get_root(), n);
        searcher.simulate_expand_backprop(n, game, 0.2f, nullptr);
        CHECK_EQUAL(1u, searcher.get_search_count());
        DOUBLES_EQUAL(0.2f, searcher.get_root().get_value(), 1e-4f);
        CHECK_EQUAL(1u, searcher.get_root().count_childs());
    }
    {
        const Node& root = searcher.get_root();
        Node* const n = searcher.search(game);
        CHECK_EQUAL(root.get_child()->get_child(), n);
        searcher.simulate_expand_backprop(n, game, -0.4f, nullptr);
        CHECK_EQUAL(2u, searcher.get_search_count());
        DOUBLES_EQUAL(0.3f, root.get_value(), 1e-4f);
        DOUBLES_EQUAL(-0.4f, root.get_child()->get_child()->get_value(), 1e-4f);
        DOUBLES_EQUAL(0.4f, root.get_child()->get_value(), 1e-4f);
    }
}

TEST(test_minishogi_mcgs_searcher, test_search_backprop_root_node)
{
    Game game("4k/5/5/5/4K b -");
    Searcher searcher{10u};
    Node* const n = searcher.search(game);
    CHECK_EQUAL(&searcher.get_root(), n);
    searcher.simulate_expand_backprop(n, game, 0.2f, nullptr);
    CHECK_EQUAL(1u, searcher.get_search_count());
    DOUBLES_EQUAL(0.2f, searcher.get_root().get_value(), 1e-4f);
    CHECK_EQUAL(3u, searcher.get_root().count_childs());
}

TEST(test_minishogi_mcgs_searcher, search_with_no_remaining_capacity)
{
    Game game("4k/5/5/5/4K b -");
    Searcher searcher{0u};
    {
        Node* const n = searcher.search(game);
        CHECK_EQUAL(&searcher.get_root(), n);
        searcher.simulate_expand_backprop(n, game, 0.5f, nullptr);
        CHECK_EQUAL(1u, searcher.get_search_count());
        DOUBLES_EQUAL(0.5f, searcher.get_root().get_value(), 1e-4f);
        CHECK_EQUAL(0u, searcher.get_root().count_childs());
    }
    {
        Node* const n = searcher.search(game);
        CHECK_EQUAL(nullptr, n);
        DOUBLES_EQUAL(0.5f, searcher.get_root().get_value(), 1e-4f);
        CHECK_EQUAL(1u, searcher.get_search_count());
    }
}

} // namespace test_minishogi

} // namespace test_vshogi::test_engine::test_mcgs
