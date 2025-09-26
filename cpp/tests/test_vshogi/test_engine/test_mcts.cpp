#include "vshogi/engine/mcts.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

namespace test_minishogi
{

using namespace vshogi::minishogi;
using Node = vshogi::engine::mcts::Node<Parameters>;
using Searcher = vshogi::engine::mcts::Searcher<Parameters>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP (minishogi_node) {
};

TEST(minishogi_node, init_default)
{
    auto root = Node();
    CHECK_EQUAL(0, root.get_visit_count());
    DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
}

TEST(minishogi_node, init_with_args)
{
    auto root = Node();
    root.simulate_expand_and_backprop({}, vshogi::BLACK, -1.f, zeros);
    CHECK_EQUAL(1, root.get_visit_count());
    DOUBLES_EQUAL(-1.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(-1.f, root.get_q_value(), 1e-2f);
}

TEST(minishogi_node, explore_no_child)
{
    auto g = Game("5/5/5/5/5 b -");
    auto root = Node();
    CHECK_TRUE(g.get_result() != vshogi::ResultEnum::ONGOING);
    root.simulate(g);
    root.backprop_to_root();
    root.simulate(g);
    root.backprop_to_root();
    CHECK_EQUAL(2, root.get_visit_count());
}

TEST(minishogi_node, explore_game_end)
{
    auto g = Game("b2pk/3b1/4P/2gRR/4K b -");
    auto root = Node();
    root.simulate_expand_and_backprop(
        g.get_legal_moves(), g.get_turn(), 0.f, zeros);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
    Node* const child = root.select_nocheck(g, 1.f, 0.f); // 1b1c
    CHECK_TRUE(nullptr != child);
    child->simulate(g);
    child->backprop_to_root();
    DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(1.f, root.get_q_value(), 1e-2f);
}

TEST(minishogi_node, explore_one_action)
{
    auto g = Game("4k/5/4P/5/5 b -");
    auto root = Node();
    root.simulate_expand_and_backprop(
        g.get_legal_moves(), g.get_turn(), 0.1f, zeros);
    DOUBLES_EQUAL(0.1f, root.get_q_value(100), 1e-2f);

    const auto actual = root.select_nocheck(g, 1.f, 0.f);
    {
        STRCMP_EQUAL("4k/4P/5/5/5 w - 2", g.to_sfen().c_str());

        CHECK_EQUAL(1, root.get_visit_count());
        DOUBLES_EQUAL(0.1f, root.get_value(), 1e-2f);
        DOUBLES_EQUAL(0.1f, root.get_q_value(), 1e-2f);

        CHECK_EQUAL(0, actual->get_visit_count());
        CHECK_TRUE(actual != nullptr);
        CHECK_TRUE(actual != &root);
    }
    actual->simulate_expand_and_backprop(
        g.get_legal_moves(), g.get_turn(), -0.8f, zeros);
    {
        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL(0.1f, root.get_value(), 1e-2f);
        DOUBLES_EQUAL((0.1f + 0.8f) / 2.f, root.get_q_value(), 1e-2f);

        CHECK_EQUAL(1, actual->get_visit_count());
        DOUBLES_EQUAL(-0.8f, actual->get_value(), 1e-2f);
        DOUBLES_EQUAL(-0.8f, actual->get_q_value(), 1e-2f);

        const auto ch = root.get_child(Move(SQ_1B, SQ_1C));
        CHECK_TRUE(actual == ch);
    }
    DOUBLES_EQUAL(0.8f, root.get_q_value(100), 1e-2f);
}

TEST(minishogi_node, explore_two_action)
{

    /**
     *                        Node(v=0)
     *                        /     \
     *                       /       \
     *                    p=0.6     p=0.4
     *                     /           \
     *                    /             \
     *             Node(v=-0.3)     Node(v=0.8)
     *
     * PUCT scores (Q + U * c)
     * - step1
     *     - Move(SQ_1D, SQ_1E): 0 + 0.6 * 1 = 0.6 <-
     *     - Move(SQ_2D, SQ_1E): 0 + 0.4 * 1 = 0.4
     * - step2
     *     - Move(SQ_1D, SQ_1E): -0.3 + (0.6 * sqrt(2) / 2) * 1 = 0.124
     *     - Move(SQ_2D, SQ_1E): -0.15 + (0.4 * sqrt(2) / 1) * 1 = 0.416 <-
     * - step3
     *     - Move(SQ_1D, SQ_1E): -0.3 + (0.6 * sqrt(3) / 2) * 1 = 0.220
     *     - Move(SQ_2D, SQ_1E): 0.8 + (0.4 * sqrt(3) / 2) * 1 = 1.15 <-
     */

    std::vector<float> input_value = {0.3f, -0.8f, -0.8f};
    const Move moves[]
        = {Move(SQ_1D, SQ_1E), Move(SQ_2D, SQ_1E), Move(SQ_2D, SQ_1E)};
    std::vector<float> expected_q_value = {
        // At first, Move(SQ_1D, SQ_1E) is selected due to higher probability
        // clang-format off
        (0.f + -0.3f) / 2.f,                // Move(SQ_1D, SQ_1E) selected
        (0.f + -0.3f + 0.8f) / 3.f,         // Move(SQ_2D, SQ_1E) selected
        (0.f + -0.3f + 0.8f + 0.8f) / 4.f,  // Move(SQ_2D, SQ_1E) selected
        // clang-format on
    };
    std::vector<Move> expected_most_selected_moves
        = {Move(SQ_1D, SQ_1E), Move(SQ_2D, SQ_1E), Move(SQ_2D, SQ_1E)};
    const float expected_greedy_q_values[] = {-0.3f, 0.8f, 0.8f};

    // softmax([-0.202, 0.202]) -> [0.5996, 0.4003]
    float logits[Game::num_dlshogi_policy()] = {0.f};
    logits[Move(SQ_1D, SQ_1E).to_dlshogi_policy_index()] = 0.202f;
    logits[Move(SQ_2D, SQ_1E).to_dlshogi_policy_index()] = -0.202f;
    auto g = Game("4k/5/5/5/4S b -");
    auto root = Node();
    root.simulate_expand_and_backprop(
        {Move(SQ_1D, SQ_1E), Move(SQ_2D, SQ_1E)}, vshogi::BLACK, 0.f, logits);

    for (std::size_t ii = 0; ii < 3; ++ii) {
        auto g_copy = Game(g);
        const auto actual = root.select_nocheck(g_copy, 1.f, 0.f);
        actual->simulate_expand_and_backprop(
            {}, vshogi::WHITE, input_value[ii], zeros);

        CHECK_EQUAL(root.get_child(moves[ii]), actual);
        DOUBLES_EQUAL(expected_q_value[ii], root.get_q_value(), 1e-3f);
        CHECK_TRUE(
            root.get_most_visited_child()->get_action()
            == expected_most_selected_moves[ii]);
        DOUBLES_EQUAL(
            expected_greedy_q_values[ii], root.get_q_value(100), 1e-2f);
    }
}

TEST(minishogi_node, explore_two_layer)
{

    /**
     *                        Node(v=0)
     *                        /     \
     *                       /       \
     *                    p=0.9     p=0.1
     *                     /           \
     *                    /             \
     *             Node(v=-0.9)     Node(v=0.9)
     *                /     \
     *               /       \
     *           p=0.9      p=0.1
     *             /           \
     *            /             \
     *      Node(v=-0.5)    Node(v=0.8)
     *
     * PUCT scores (Q + U * c)
     * - step1
     *     - Layer1
     *         - Move(SQ_1D, SQ_1E): 0 + 0.9 * 1 = 0.9 <-
     *         - Move(SQ_2D, SQ_1E): 0 + 0.1 * 1 = 0.1
     * - step2
     *     - Layer1
     *         - Move(SQ_1D, SQ_1E): 0.9 + (0.9 * sqrt(2) / 2) * 1 = 1.536 <-
     *         - Move(SQ_2D, SQ_1E): 0 + (0.1 * sqrt(2) / 1) * 1 = 0.141
     *     - Layer2
     *         - Move(SQ_5B, SQ_5A): 0 + 0.9 * 1 <-
     *         - Move(SQ_4B, SQ_5A): 0 + 0.1 * 1
     */

    std::vector<float> input_value = {-0.9f, -0.5f};
    std::vector<bool> expected_leaf_nullptr = {false, false, true};

    // softmax([-1.099, 1.099]) -> [0.09993023, 0.90006977]
    float logits[Game::num_dlshogi_policy()] = {0.f};
    logits[Move(SQ_1D, SQ_1E).to_dlshogi_policy_index()] = 1.099f;
    logits[Move(SQ_2D, SQ_1E).to_dlshogi_policy_index()] = -1.099f;
    auto g = Game("s4/5/5/5/4S b -");
    auto root = Node();
    root.simulate_expand_and_backprop(
        {Move(SQ_1D, SQ_1E), Move(SQ_2D, SQ_1E)}, vshogi::BLACK, 0.f, logits);

    {
        auto g_copy = Game(g);
        const auto actual = root.select_nocheck(g_copy, 1.f, 0.f);
        CHECK_EQUAL(root.get_child(Move(SQ_1D, SQ_1E)), actual);
        STRCMP_EQUAL("s4/5/5/4S/5 w - 2", g_copy.to_sfen().c_str());
        float policy[Game::num_dlshogi_policy()] = {0.f};
        policy[Move(SQ_5B, SQ_5A).rotate().to_dlshogi_policy_index()] = 1.099f;
        policy[Move(SQ_4B, SQ_5A).rotate().to_dlshogi_policy_index()] = -1.099f;
        actual->simulate_expand_and_backprop(
            {Move(SQ_5B, SQ_5A), Move(SQ_4B, SQ_5A)},
            vshogi::WHITE,
            -0.9f,
            policy);
        DOUBLES_EQUAL((0.f + 0.9f) / 2.f, root.get_q_value(), 1e-3f);
        DOUBLES_EQUAL(0.9f, root.get_q_value(100), 1e-2f);
    }
    {
        auto g_copy = Game("s4/5/5/5/4S b -");
        auto actual = root.select_nocheck(g_copy, 1.f, 0.f);
        CHECK_EQUAL(1u, g_copy.record_length());
        CHECK_EQUAL(root.get_child(Move(SQ_1D, SQ_1E)), actual);
        actual = actual->select_nocheck(g_copy, 1.f, 0.f);
        CHECK_EQUAL(2u, g_copy.record_length());
        CHECK_EQUAL(
            root.get_child(Move(SQ_1D, SQ_1E))->get_child(Move(SQ_5B, SQ_5A)),
            actual);
        STRCMP_EQUAL("5/s4/5/4S/5 b - 3", g_copy.to_sfen().c_str());
        actual->simulate_expand_and_backprop(
            g_copy.get_legal_moves(), g_copy.get_turn(), -0.5f, zeros);
        DOUBLES_EQUAL((0.f + 0.9f + -0.5f) / 3.f, root.get_q_value(), 1e-3f);
        CHECK_TRUE(
            root.get_most_visited_child()->get_action() == Move(SQ_1D, SQ_1E));
        DOUBLES_EQUAL((0.f + 0.9f + -0.5f) / 3.f, root.get_q_value(0), 1e-2f);
        DOUBLES_EQUAL((0.9f + -0.5f) / 2.f, root.get_q_value(1), 1e-2f);
        DOUBLES_EQUAL(-0.5f, root.get_q_value(2), 1e-2f);
        DOUBLES_EQUAL(-0.5f, root.get_q_value(100), 1e-2f);
    }
    {
        CHECK_EQUAL(3, root.get_visit_count());
        DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
        root.apply(Move(SQ_1D, SQ_1E));
        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL(-0.9f, root.get_value(), 1e-2f);

        DOUBLES_EQUAL(
            0.9f, root.get_child(Move(SQ_5B, SQ_5A))->get_proba(), 1e-2f);
        DOUBLES_EQUAL(
            0.1f, root.get_child(Move(SQ_4B, SQ_5A))->get_proba(), 1e-2f);
        DOUBLES_EQUAL((-0.9f + 0.5f) / 2.f, root.get_q_value(), 1e-3f);
        CHECK_TRUE(
            root.get_most_visited_child()->get_action() == Move(SQ_5B, SQ_5A));
        DOUBLES_EQUAL((-0.9f + 0.5f) / 2.f, root.get_q_value(0), 1e-2f);
        DOUBLES_EQUAL(0.5f, root.get_q_value(1), 1e-2f);
        DOUBLES_EQUAL(0.5f, root.get_q_value(100), 1e-2f);
    }
}

TEST(minishogi_node, test_apply)
{
    auto root = Node();
    {
        auto g = Game("4p/5/5/5/P4 b -");
        root.simulate_expand_and_backprop(
            g.get_legal_moves(), g.get_turn(), 0.f, nullptr);
    }
    {
        auto g = Game("4p/5/5/5/P4 b -");
        auto n = root.select_nocheck(g, 1.f, 0.f);
        CHECK_EQUAL(root.get_child(), n);
        n->simulate_expand_and_backprop(
            g.get_legal_moves(), g.get_turn(), 0.f, nullptr);
    }
    {
        auto g = Game("4p/5/5/5/P4 b -");
        auto n = root.select_nocheck(g, 1.f, 0.f);
        CHECK_EQUAL(root.get_child(), n);
        n = n->select_nocheck(g, 1.f, 0);
        CHECK_EQUAL(root.get_child()->get_child(), n);
        n->simulate_expand_and_backprop(
            g.get_legal_moves(), g.get_turn(), 0.f, nullptr);
    }
    auto c = root.get_child();
    auto gc = root.get_child()->get_child();
    CHECK_EQUAL(c->get_most_visited_child(), gc);
    CHECK_COMPARE(gc->get_parent(), ==, c);

    root.apply(root.get_child()->get_action());
    CHECK_EQUAL(root.get_most_visited_child(), gc);
    CHECK_COMPARE(gc->get_parent(), !=, &root);

    auto g = Game("4p/5/5/P4/5 b -");
    root.select_nocheck(g, 1.f, 0.f);
    CHECK_COMPARE(gc->get_parent(), ==, &root);
}

TEST(minishogi_node, explore_until_game_end)
{
    auto g = Game();
    auto mcts = Searcher(4.f, 3, 1);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - mcts.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = mcts.search(g_copy);
            if (n != nullptr) {
                n->simulate_expand_and_backprop(
                    g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
            }
        }

        const auto action = mcts.get_action_by_visit_max();
        g.apply(action);
        mcts.apply(action);
    }
}

TEST_GROUP (minishogi_searcher) {
};

TEST(minishogi_searcher, explore_after_apply)
{
    auto g = Game();
    auto mcts = Searcher(4.f, 3, 1);
    for (int ii = 100; ii--;) {
        auto g_copy = Game(g);
        const auto n = mcts.search(g_copy);
        if (n != nullptr)
            n->simulate_expand_and_backprop(
                g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
    }

    const auto move = mcts.get_action_by_visit_max();
    mcts.apply(move);
    g.apply(move);
    const auto current_visit_count = mcts.get_visit_count();
    CHECK_TRUE(current_visit_count > 0);
    for (int ii = 100; ii--;) {
        auto g_copy = Game(g);
        const auto n = mcts.search(g_copy);
        if (n != nullptr)
            n->simulate_expand_and_backprop(
                g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
    }
    CHECK_EQUAL(current_visit_count + 100, mcts.get_visit_count());
}

TEST(minishogi_searcher, test_mate_in_three)
{
    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   +---+---+---+---+---+
    // A |   |-HI|   |   |   |
    //   +---+---+---+---+---+
    // B |   |   |-OU|   |+KI|
    //   +---+---+---+---+---+
    // C |   |   |   |   |   |
    //   +---+---+---+---+---+
    // D |   |   |+FU|+KI|   |
    //   +---+---+---+---+---+
    // E |   |   |   |   |   |
    //   +---+---+---+---+---+
    // Black: -
    auto mcts = Searcher(4.f, 0.f, 0);
    for (int ii = 100; ii--;) {
        auto g = Game("1r3/2k1G/5/2PG1/5 b -");
        Node* const n = mcts.search(g);
        if (n)
            n->simulate_expand_and_backprop(
                g.get_legal_moves(), g.get_turn(), 0.f, nullptr);
    }

    const auto m = Move(SQ_3C, SQ_2D);
    CHECK_EQUAL(m.hash(), mcts.get_action_by_visit_max().hash());
    DOUBLES_EQUAL(-1.f, mcts.get_root()->get_child(m)->get_q_value(), 1e-3f);
    const int expected_visits
        = mcts.get_root()->get_child(m)->get_visit_count() + 100;

    for (int ii = 100; ii--;) {
        auto g = Game("1r3/2k1G/5/2PG1/5 b -");
        Node* const n = mcts.search(g);
        if (n)
            n->simulate_expand_and_backprop(
                g.get_legal_moves(), g.get_turn(), 0.f, nullptr);
    }
    CHECK_EQUAL(
        expected_visits, mcts.get_root()->get_child(m)->get_visit_count());
}

TEST(minishogi_searcher, test_dfpn_vertex)
{
    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   *---*---*---*---*---*
    // A |   |   |-OU|   |   |
    //   *---*---*---*---*---*
    // B |   |   |   |   |   |
    //   *---*---*---*---*---*
    // C |   |   |-KI|-FU|   |
    //   *---*---*---*---*---*
    // D |   |   |   |   |   |
    //   *---*---*---*---*---*
    // E |   |   |+OU|   |   |
    //   *---*---*---*---*---*
    // Black: -
    auto mcts = Searcher(4.f, 0.f, 0, 100u);
    for (int ii = 3; ii--;) {
        auto g = Game("2k2/2rg1/2sp1/5/2K2 b -");
        Node* const n = mcts.search(g);
        if (n)
            n->simulate_expand_and_backprop(
                g.get_legal_moves(), g.get_turn(), 0.f, nullptr);
    }
    CHECK_EQUAL(Move("3e4e").hash(), mcts.get_action_by_visit_max().hash());
    mcts.apply(Move("3e2e"));
    DOUBLES_EQUAL(1.f, mcts.get_root()->get_q_value(), 1e-3f);
    for (int ii = 2; ii--;) {
        auto g = Game("2k2/2rg1/2sp1/5/3K1 w -");
        Node* const n = mcts.search(g);
        CHECK_TRUE(g.get_result() == vshogi::ONGOING);
        CHECK_EQUAL(nullptr, n);
        DOUBLES_EQUAL(1.f, mcts.get_root()->get_q_value(), 1e-3f);
    }
    {
        auto g = Game("2k2/2rg1/2sp1/5/3K1 w -");
        Node* const n = mcts.search(g);
        CHECK_TRUE(g.get_result() == vshogi::WHITE_WIN);
        CHECK_EQUAL(nullptr, n);
        DOUBLES_EQUAL(1.f, mcts.get_root()->get_q_value(), 1e-3f);
    }
    CHECK_EQUAL(Move("3c2d").hash(), mcts.get_action_by_visit_max().hash());
}

} // namespace test_minishogi

namespace test_judkins_shogi
{

using namespace vshogi::judkins_shogi;
using Node = vshogi::engine::mcts::Node<Parameters>;
using Searcher = vshogi::engine::mcts::Searcher<Parameters>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP (judkins_shogi_searcher) {
};

TEST(judkins_shogi_searcher, explore_until_game_end)
{
    auto g = Game();
    auto mcts = Searcher(4.f, 0.25f, 1);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - mcts.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = mcts.search(g_copy);
            if (g.record_length() == g_copy.record_length())
                CHECK_TRUE((n == nullptr) || (n == mcts.get_root()));
            if (n != nullptr) {
                CHECK_COMPARE(g_copy.get_legal_moves().size(), >, 0);
                n->simulate_expand_and_backprop(
                    g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
            }
        }

        const auto action = mcts.get_action_by_visit_max();
        g.apply(action);
        mcts.apply(action);
    }
}

} // namespace test_judkins_shogi

namespace test_shogi
{

using namespace vshogi::shogi;
using Node = vshogi::engine::mcts::Node<Parameters>;
using Searcher = vshogi::engine::mcts::Searcher<Parameters>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP (shogi_searcher) {
};

TEST(shogi_searcher, explore_until_game_end)
{
    auto g = Game();
    auto mcts = Searcher(4.f, 0.25f, 1);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - mcts.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = mcts.search(g_copy);
            if (n != nullptr)
                n->simulate_expand_and_backprop(
                    g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
        }

        const auto action = mcts.get_action_by_visit_max();
        g.apply(action);
        mcts.apply(action);
    }
}

} // namespace test_shogi

} // namespace test_vshogi::test_engine
