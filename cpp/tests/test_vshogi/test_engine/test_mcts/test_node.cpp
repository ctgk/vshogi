#include "vshogi/common/notation.hpp"
#include "vshogi/engine/mcts/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

namespace test_minishogi
{

using namespace vshogi;
using namespace vshogi::minishogi;
using MT = vshogi::minishogi::MoveTraits;
using NT = vshogi::Notation<Parameters>;
using Node = vshogi::engine::mcts::Node;
using Game = vshogi::minishogi::Game;

TEST_GROUP (minishogi_node) {
    Node root{};
    void setup()
    {
        root.init();
    }
};

TEST(minishogi_node, init_default)
{
    CHECK_EQUAL(0, root.get_visit_count());
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
}

TEST(minishogi_node, init_with_args)
{
    root.simulate_ongoing_and_expand(Game("5/5/5/5/5 b -"), -1.f, nullptr);
    root.backprop(root.get_q_value(), nullptr);
    CHECK_EQUAL(1, root.get_visit_count());
    DOUBLES_EQUAL(-1.f, root.get_q_value(), 1e-2f);
}

TEST(minishogi_node, explore_no_child)
{
    auto g = Game("5/5/5/5/5 b -");
    CHECK_TRUE(g.get_result() != ONGOING);
    root.simulate(g);
    root.backprop(root.get_q_value(), nullptr);
    root.simulate(g);
    root.backprop(root.get_q_value(), nullptr);
    CHECK_EQUAL(2, root.get_visit_count());
}

TEST(minishogi_node, explore_game_end)
{
    auto g = Game("b2pk/3b1/4P/2gRR/4K b -");
    root.simulate_ongoing_and_expand(g, 0.f, nullptr);
    root.backprop(root.get_q_value(), nullptr);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
    Node* const child = root.select(g, 1.f, 0.f); // 1c1b
    CHECK_TRUE(nullptr != child);
    child->simulate(g);
    auto p = child->backprop(child->get_q_value(), nullptr);
    CHECK_EQUAL(&root, p);
    p->backprop(-child->get_q_value(), child);
    DOUBLES_EQUAL(1.f, root.get_q_value(), 1e-2f);
    CHECK_EQUAL(
        MT::make_move("1c1b"), root.get_most_visited_child()->get_action());
}

TEST(minishogi_node, explore_one_action)
{
    auto g = Game("4k/5/4P/5/5 b -");
    root.simulate_ongoing_and_expand(g, 0.1f, nullptr);
    auto p = root.backprop(root.get_q_value(), nullptr);
    CHECK_EQUAL(nullptr, p);
    DOUBLES_EQUAL(0.1f, root.get_q_value(100), 1e-2f);

    const auto actual = root.select(g, 1.f, 0.f);
    {
        STRCMP_EQUAL("4k/4P/5/5/5 w - 2", NT::to_sfen(g).c_str());

        CHECK_EQUAL(1, root.get_visit_count());
        DOUBLES_EQUAL(0.1f, root.get_q_value(), 1e-2f);

        CHECK_EQUAL(0, actual->get_visit_count());
        CHECK_TRUE(actual != nullptr);
        CHECK_TRUE(actual != &root);
    }
    actual->simulate_ongoing_and_expand(g, -0.8f, nullptr);
    p = actual->backprop(actual->get_q_value(), nullptr);
    CHECK_EQUAL(&root, p);
    CHECK_EQUAL(nullptr, p->backprop(-actual->get_q_value(), actual));
    {
        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL((0.1f + 0.8f) / 2.f, root.get_q_value(), 1e-2f);

        CHECK_EQUAL(1, actual->get_visit_count());
        DOUBLES_EQUAL(-0.8f, actual->get_q_value(), 1e-2f);

        const auto ch = root.get_child(MT::make_move(SQ_1C, SQ_1B));
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
     *     - Move(SQ_1E, SQ_1D): 0 + 0.6 * 1 = 0.6 <-
     *     - Move(SQ_1E, SQ_2D): 0 + 0.4 * 1 = 0.4
     * - step2
     *     - Move(SQ_1E, SQ_1D): -0.3 + (0.6 * sqrt(2) / 2) * 1 = 0.124
     *     - Move(SQ_1E, SQ_2D): -0.15 + (0.4 * sqrt(2) / 1) * 1 = 0.416 <-
     * - step3
     *     - Move(SQ_1E, SQ_1D): -0.3 + (0.6 * sqrt(3) / 2) * 1 = 0.220
     *     - Move(SQ_1E, SQ_2D): 0.8 + (0.4 * sqrt(3) / 2) * 1 = 1.15 <-
     */
    auto g = Game("4k/5/5/5/4S b -");
    std::vector<float> input_value = {0.3f, -0.8f, -0.8f};
    const move_t moves[]
        = {MT::make_move(SQ_1E, SQ_1D),
           MT::make_move(SQ_1E, SQ_2D),
           MT::make_move(SQ_1E, SQ_2D)};
    std::vector<float> expected_q_value = {
        // At first, Move(SQ_1E, SQ_1D) is selected due to higher probability
        // clang-format off
        (0.f + -0.3f) / 2.f,                // Move(SQ_1E, SQ_1D) selected
        (0.f + -0.3f + 0.8f) / 3.f,         // Move(SQ_1E, SQ_2D) selected
        (0.f + -0.3f + 0.8f + 0.8f) / 4.f,  // Move(SQ_1E, SQ_2D) selected
        // clang-format on
    };
    std::vector<move_t> expected_most_selected_moves
        = {MT::make_move(SQ_1E, SQ_1D),
           MT::make_move(SQ_1E, SQ_2D),
           MT::make_move(SQ_1E, SQ_2D)};
    const float expected_greedy_q_values[] = {-0.3f, 0.8f, 0.8f};

    // softmax([-0.202, 0.202]) -> [0.5996, 0.4003]
    float logits[Game::num_dlshogi_policy()] = {0.f};
    logits[MT::to_policy_index(MT::make_move(SQ_1E, SQ_1D), BLACK)] = 0.202f;
    logits[MT::to_policy_index(MT::make_move(SQ_1E, SQ_2D), BLACK)] = -0.202f;
    root.simulate_ongoing_and_expand(g, 0.f, logits);
    CHECK_EQUAL(nullptr, root.backprop(root.get_q_value(), nullptr));

    for (std::size_t ii = 0; ii < 3; ++ii) {
        auto g_copy = Game(g);
        const auto actual = root.select(g_copy, 1.f, 0.f);
        actual->simulate_ongoing_and_expand(g_copy, input_value[ii], nullptr);
        CHECK_EQUAL(&root, actual->backprop(actual->get_q_value(), nullptr));
        CHECK_EQUAL(nullptr, root.backprop(-actual->get_q_value(), actual));

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
     *         - Move(SQ_1E, SQ_1D): 0 + 0.9 * 1 = 0.9 <-
     *         - Move(SQ_1E, SQ_2D): 0 + 0.1 * 1 = 0.1
     * - step2
     *     - Layer1
     *         - Move(SQ_1E, SQ_1D): 0.9 + (0.9 * sqrt(2) / 2) * 1 = 1.536 <-
     *         - Move(SQ_1E, SQ_2D): 0 + (0.1 * sqrt(2) / 1) * 1 = 0.141
     *     - Layer2
     *         - Move(SQ_5A, SQ_5B): 0 + 0.9 * 1 <-
     *         - Move(SQ_5A, SQ_4B): 0 + 0.1 * 1
     */

    std::vector<float> input_value = {-0.9f, -0.5f};
    std::vector<bool> expected_leaf_nullptr = {false, false, true};

    // softmax([-1.099, 1.099]) -> [0.09993023, 0.90006977]
    float logits[Game::num_dlshogi_policy()] = {0.f};
    logits[MT::to_policy_index(MT::make_move(SQ_1E, SQ_1D), BLACK)] = 1.099f;
    logits[MT::to_policy_index(MT::make_move(SQ_1E, SQ_2D), BLACK)] = -1.099f;
    auto g = Game("s4/5/5/5/4S b -");
    root.simulate_ongoing_and_expand(g, 0.f, logits);
    CHECK_EQUAL(nullptr, root.backprop(root.get_q_value(), nullptr));

    {
        auto g_copy = Game(g);
        const auto actual = root.select(g_copy, 1.f, 0.f);
        CHECK_EQUAL(root.get_child(MT::make_move(SQ_1E, SQ_1D)), actual);
        STRCMP_EQUAL("s4/5/5/4S/5 w - 2", NT::to_sfen(g_copy).c_str());
        float policy[Game::num_dlshogi_policy()] = {0.f};
        policy[MT::to_policy_index(MT::make_move(SQ_5A, SQ_5B), WHITE)]
            = 1.099f;
        policy[MT::to_policy_index(MT::make_move(SQ_5A, SQ_4B), WHITE)]
            = -1.099f;
        actual->simulate_ongoing_and_expand(g_copy, -0.9f, policy);
        CHECK_EQUAL(&root, actual->backprop(actual->get_q_value(), nullptr));
        CHECK_EQUAL(nullptr, root.backprop(-actual->get_q_value(), actual));
        DOUBLES_EQUAL((0.f + 0.9f) / 2.f, root.get_q_value(), 1e-3f);
        DOUBLES_EQUAL(0.9f, root.get_q_value(100), 1e-2f);
    }
    {
        auto g_copy = Game(g);
        Node* const child = root.select(g_copy, 1.f, 0.f);
        CHECK_EQUAL(1u, g_copy.ply());
        CHECK_EQUAL(root.get_child(MT::make_move(SQ_1E, SQ_1D)), child);
        Node* const grand_child = child->select(g_copy, 1.f, 0.f);
        CHECK_EQUAL(2u, g_copy.ply());
        CHECK_EQUAL(
            root.get_child(MT::make_move(SQ_1E, SQ_1D))
                ->get_child(MT::make_move(SQ_5A, SQ_5B)),
            grand_child);
        STRCMP_EQUAL("5/s4/5/4S/5 b - 3", NT::to_sfen(g_copy).c_str());
        grand_child->simulate_ongoing_and_expand(g_copy, -0.5f, nullptr);
        CHECK_EQUAL(
            child, grand_child->backprop(grand_child->get_q_value(), nullptr));
        CHECK_EQUAL(
            &root, child->backprop(-grand_child->get_q_value(), grand_child));
        CHECK_EQUAL(nullptr, root.backprop(grand_child->get_q_value(), child));
        DOUBLES_EQUAL((0.f + 0.9f + -0.5f) / 3.f, root.get_q_value(), 1e-3f);
        CHECK_TRUE(
            root.get_most_visited_child()->get_action()
            == MT::make_move(SQ_1E, SQ_1D));
        DOUBLES_EQUAL((0.f + 0.9f + -0.5f) / 3.f, root.get_q_value(0), 1e-2f);
        DOUBLES_EQUAL((0.9f + -0.5f) / 2.f, root.get_q_value(1), 1e-2f);
        DOUBLES_EQUAL(-0.5f, root.get_q_value(2), 1e-2f);
        DOUBLES_EQUAL(-0.5f, root.get_q_value(100), 1e-2f);
    }
    {
        CHECK_EQUAL(3, root.get_visit_count());
        DOUBLES_EQUAL((0.f + 0.9f + -0.5f) / 3.f, root.get_q_value(), 1e-2f);
        root.apply(MT::make_move(SQ_1E, SQ_1D));
        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL((-0.9f + 0.5f) / 2.f, root.get_q_value(), 1e-2f);

        DOUBLES_EQUAL(
            0.9f,
            root.get_child(MT::make_move(SQ_5A, SQ_5B))->get_proba(),
            1e-2f);
        DOUBLES_EQUAL(
            0.1f,
            root.get_child(MT::make_move(SQ_5A, SQ_4B))->get_proba(),
            1e-2f);
        DOUBLES_EQUAL((-0.9f + 0.5f) / 2.f, root.get_q_value(), 1e-3f);
        CHECK_TRUE(
            root.get_most_visited_child()->get_action()
            == MT::make_move(SQ_5A, SQ_5B));
        DOUBLES_EQUAL((-0.9f + 0.5f) / 2.f, root.get_q_value(0), 1e-2f);
        DOUBLES_EQUAL(0.5f, root.get_q_value(1), 1e-2f);
        DOUBLES_EQUAL(0.5f, root.get_q_value(100), 1e-2f);
    }
}

TEST(minishogi_node, test_apply)
{
    auto g = Game("4p/5/5/5/P4 b -");
    {
        root.simulate_ongoing_and_expand(g, 0.f, nullptr);
        CHECK_EQUAL(nullptr, root.backprop(root.get_q_value(), nullptr));
    }
    {
        auto g_copy = Game(g);
        auto n = root.select(g_copy, 1.f, 0.f);
        CHECK_EQUAL(root.get_child(), n);
        n->simulate_ongoing_and_expand(g_copy, 0.f, nullptr);
        CHECK_EQUAL(&root, n->backprop(n->get_q_value(), nullptr));
    }
    {
        auto g_copy = Game(g);
        auto n = root.select(g_copy, 1.f, 0.f);
        CHECK_EQUAL(root.get_child(), n);
        n = n->select(g_copy, 1.f, 0);
        CHECK_EQUAL(root.get_child()->get_child(), n);
        n->simulate_ongoing_and_expand(g_copy, 0.f, nullptr);
        CHECK_EQUAL(
            &root,
            n->backprop(n->get_q_value(), nullptr)
                ->backprop(-n->get_q_value(), n));
    }
    auto c = root.get_child();
    auto gc = root.get_child()->get_child();
    CHECK_EQUAL(c->get_most_visited_child(), gc);
    CHECK_COMPARE(gc->get_parent(), ==, c);

    root.apply(root.get_child()->get_action());
    CHECK_EQUAL(0u, root.get_action());
    CHECK_EQUAL(root.get_most_visited_child(), gc);
    CHECK_COMPARE(gc->get_parent(), !=, &root);

    root.select(g, 1.f, 0.f);
    CHECK_COMPARE(gc->get_parent(), ==, &root);
}

} // namespace test_minishogi

} // namespace test_vshogi::test_engine
