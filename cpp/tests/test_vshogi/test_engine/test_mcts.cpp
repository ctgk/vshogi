#include "vshogi/engine/mcts.hpp"
#include "vshogi/variants/animal_shogi.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

namespace test_animal_shogi
{

using namespace vshogi::animal_shogi;
using Node = vshogi::engine::mcts::Node<Game, Move>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP(animal_shogi_node){};

TEST(animal_shogi_node, init_default)
{
    auto root = Node();
    CHECK_EQUAL(0, root.get_visit_count());
    DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
}

TEST(animal_shogi_node, init_with_args)
{
    auto root = Node({}, vshogi::BLACK, -1.f, zeros);
    CHECK_EQUAL(1, root.get_visit_count());
    DOUBLES_EQUAL(-1.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(-1.f, root.get_q_value(), 1e-2f);
}

TEST(animal_shogi_node, explore_no_child)
{
    auto g = Game("3/3/3/3 b -");
    auto root = Node(g.get_legal_moves(), g.get_turn(), 1.f, zeros);

    const auto actual = root.select(g, 1.f, 1, 1);

    CHECK_EQUAL(2, root.get_visit_count());
    CHECK_TRUE(nullptr == actual);
}

TEST(animal_shogi_node, explore_game_end)
{
    auto g = Game("3/1l1/1C1/3 b -");
    auto root = Node(g.get_legal_moves(), g.get_turn(), 0.f, zeros);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
    const auto actual = root.select(g, 1.f, 0.f, 0);
    CHECK_TRUE(nullptr == actual);
    DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(1.f, root.get_q_value(), 1e-2f);
}

TEST(animal_shogi_node, explore_one_action)
{
    auto g = Game("1l1/3/1C1/3 b -");
    auto root = Node(g.get_legal_moves(), g.get_turn(), 0.1f, zeros);
    DOUBLES_EQUAL(0.1f, root.get_q_value(100), 1e-2f);

    const auto actual = root.select(g, 1.f, 0.f, 0);
    {
        STRCMP_EQUAL("1l1/1C1/3/3 w - 2", g.to_sfen().c_str());

        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL(0.1f, root.get_value(), 1e-2f);
        DOUBLES_EQUAL(0.1f, root.get_q_value(), 1e-2f);

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

        const auto ch = root.get_child(Move(SQ_B2, SQ_B3));
        CHECK_TRUE(actual == ch);
    }
    DOUBLES_EQUAL(0.8f, root.get_q_value(100), 1e-2f);
}

TEST(animal_shogi_node, explore_two_action)
{

    /**
     *                        Node(v=0)
     *                        /     \
     *                       /       \
     *                    p=0.8     p=0.2
     *                     /           \
     *                    /             \
     *             Node(v=-0.3)     Node(v=0.8)
     *
     * PUCT scores (Q + U * c)
     * - step1
     *     - Move(SQ_A3, SQ_A4): 0 + 0.8 * 1 = 0.8 <-
     *     - Move(SQ_B4, SQ_A4): 0 + 0.2 * 1 = 0.2
     * - step2
     *     - Move(SQ_A3, SQ_A4): -0.3 + (0.8 * sqrt(2) / 2) * 1 = 0.266
     *     - Move(SQ_B4, SQ_A4): 0 + (0.2 * sqrt(2) / 1) * 1 = 0.282 <-
     * - step3
     *     - Move(SQ_A3, SQ_A4): -0.3 + (0.8 * sqrt(3) / 2) * 1 = 0.392
     *     - Move(SQ_B4, SQ_A4): 0.8 + (0.2 * sqrt(3) / 2) * 1 = 0.973 <-
     */

    std::vector<float> input_value = {0.3f, -0.8f, -0.8f};
    const float atanh0p3 = std::atanh(0.3f);
    const float atanh0p2 = std::atanh(0.2f);
    const Move moves[]
        = {Move(SQ_A3, SQ_A4), Move(SQ_B4, SQ_A4), Move(SQ_B4, SQ_A4)};
    std::vector<float> expected_q_value = {
        // At first, Move(SQ_A3, SQ_A4) is selected due to higher probability
        // clang-format off
        (0.f + -0.3f) / 2.f,                // Move(SQ_A3, SQ_A4) selected
        (0.f + -0.3f + 0.8f) / 3.f,         // Move(SQ_B4, SQ_A4) selected
        (0.f + -0.3f + 0.8f + 0.8f) / 4.f,  // Move(SQ_B4, SQ_A4) selected
        // clang-format on
    };
    std::vector<Move> expected_most_selected_moves
        = {Move(SQ_A3, SQ_A4), Move(SQ_B4, SQ_A4), Move(SQ_B4, SQ_A4)};
    const float expected_greedy_q_values[] = {-0.3f, 0.8f, 0.8f};

    // softmax([-0.693, 0.693]) -> [0.2000471, 0.7999529]
    float logits[Game::num_dlshogi_policy()] = {0.f};
    logits[Move(SQ_A3, SQ_A4).to_dlshogi_policy_index()] = 0.693f;
    logits[Move(SQ_B4, SQ_A4).to_dlshogi_policy_index()] = -0.693f;
    auto g = Game("1l1/3/3/G2 b -");
    auto root = Node(
        {Move(SQ_A3, SQ_A4), Move(SQ_B4, SQ_A4)}, vshogi::BLACK, 0.f, logits);

    for (std::size_t ii = 0; ii < 3; ++ii) {
        auto g_copy = Game(g);
        const auto actual = root.select(g_copy, 1.f, -1, 0);
        actual->simulate_expand_and_backprop(
            {}, vshogi::WHITE, input_value[ii], zeros);

        CHECK_EQUAL(root.get_child(moves[ii]), actual);
        DOUBLES_EQUAL(expected_q_value[ii], root.get_q_value(), 1e-3f);
        CHECK_TRUE(
            root.get_action_by_visit_max() == expected_most_selected_moves[ii]);
        DOUBLES_EQUAL(
            expected_greedy_q_values[ii], root.get_q_value(100), 1e-2f);
    }
}

TEST(animal_shogi_node, explore_two_layer)
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
     *         - Move(SQ_A3, SQ_A4): 0 + 0.9 * 1 = 0.9 <-
     *         - Move(SQ_B4, SQ_A4): 0 + 0.1 * 1 = 0.1
     * - step2
     *     - Layer1
     *         - Move(SQ_A3, SQ_A4): 0.9 + (0.9 * sqrt(2) / 2) * 1 = 1.536 <-
     *         - Move(SQ_B4, SQ_A4): 0 + (0.1 * sqrt(2) / 1) * 1 = 0.141
     *     - Layer2
     *         - Move(SQ_C2, SQ_C1): 0 + 0.9 * 1 <-
     *         - Move(SQ_B1, SQ_C1): 0 + 0.1 * 1
     */

    std::vector<float> input_value = {-0.9f, -0.5f};
    std::vector<bool> expected_leaf_nullptr = {false, false, true};

    // softmax([-1.099, 1.099]) -> [0.09993023, 0.90006977]
    float logits[Game::num_dlshogi_policy()] = {0.f};
    logits[Move(SQ_A3, SQ_A4).to_dlshogi_policy_index()] = 1.099f;
    logits[Move(SQ_B4, SQ_A4).to_dlshogi_policy_index()] = -1.099f;
    auto g = Game("2g/3/3/G2 b -");
    auto root = Node(
        {Move(SQ_A3, SQ_A4), Move(SQ_B4, SQ_A4)}, vshogi::BLACK, 0.f, logits);

    {
        auto g_copy = Game(g);
        const auto actual = root.select(g_copy, 1.f, -1, 0);
        CHECK_EQUAL(root.get_child(Move(SQ_A3, SQ_A4)), actual);
        STRCMP_EQUAL("2g/3/G2/3 w - 2", g_copy.to_sfen().c_str());
        float policy[Game::num_dlshogi_policy()] = {0.f};
        policy[Move(SQ_C2, SQ_C1).rotate().to_dlshogi_policy_index()] = 1.099f;
        policy[Move(SQ_B1, SQ_C1).rotate().to_dlshogi_policy_index()] = -1.099f;
        actual->simulate_expand_and_backprop(
            {Move(SQ_C2, SQ_C1), Move(SQ_B1, SQ_C1)},
            vshogi::WHITE,
            -0.9f,
            policy);
        DOUBLES_EQUAL((0.f + 0.9f) / 2.f, root.get_q_value(), 1e-3f);
        DOUBLES_EQUAL(0.9f, root.get_q_value(100), 1e-2f);
    }
    {
        auto g_copy = Game("2g/3/3/G2 b -");
        const auto actual = root.select(g_copy, 1.f, -1, 0);
        CHECK_EQUAL(
            root.get_child(Move(SQ_A3, SQ_A4))->get_child(Move(SQ_C2, SQ_C1)),
            actual);
        STRCMP_EQUAL("3/2g/G2/3 b - 3", g_copy.to_sfen().c_str());
        actual->simulate_expand_and_backprop(
            g_copy.get_legal_moves(), g_copy.get_turn(), -0.5f, zeros);
        DOUBLES_EQUAL((0.f + 0.9f + -0.5f) / 3.f, root.get_q_value(), 1e-3f);
        CHECK_TRUE(root.get_action_by_visit_max() == Move(SQ_A3, SQ_A4));
        DOUBLES_EQUAL((0.f + 0.9f + -0.5f) / 3.f, root.get_q_value(0), 1e-2f);
        DOUBLES_EQUAL((0.9f + -0.5f) / 2.f, root.get_q_value(1), 1e-2f);
        DOUBLES_EQUAL(-0.5f, root.get_q_value(2), 1e-2f);
        DOUBLES_EQUAL(-0.5f, root.get_q_value(100), 1e-2f);
    }
    {
        CHECK_EQUAL(3, root.get_visit_count());
        DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
        root.apply(Move(SQ_A3, SQ_A4));
        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL(-0.9f, root.get_value(), 1e-2f);

        DOUBLES_EQUAL(
            0.9f, root.get_child(Move(SQ_C2, SQ_C1))->get_proba(), 1e-2f);
        DOUBLES_EQUAL(
            0.1f, root.get_child(Move(SQ_B1, SQ_C1))->get_proba(), 1e-2f);
        DOUBLES_EQUAL((-0.9f + 0.5f) / 2.f, root.get_q_value(), 1e-3f);
        CHECK_TRUE(root.get_action_by_visit_max() == Move(SQ_C2, SQ_C1));
        DOUBLES_EQUAL((-0.9f + 0.5f) / 2.f, root.get_q_value(0), 1e-2f);
        DOUBLES_EQUAL(0.5f, root.get_q_value(1), 1e-2f);
        DOUBLES_EQUAL(0.5f, root.get_q_value(100), 1e-2f);
    }
}

TEST(animal_shogi_node, explore_after_apply)
{
    auto g = Game();
    auto root = Node(g.get_legal_moves(), g.get_turn(), 0.f, zeros);
    for (int ii = 100; ii--;) {
        auto g_copy = Game(g);
        const auto n = root.select(g_copy, 4.f, 3, 1);
        if (n != nullptr)
            n->simulate_expand_and_backprop(
                g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
    }

    const auto move = Move(SQ_B2, SQ_B3);
    root.apply(move);
    g.apply(move);
    const auto current_visit_count = root.get_visit_count();
    for (int ii = 100; ii--;) {
        auto g_copy = Game(g);
        const auto n = root.select(g_copy, 4.f, 3, 1);
        if (n != nullptr)
            n->simulate_expand_and_backprop(
                g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
    }
    CHECK_EQUAL(current_visit_count + 100, root.get_visit_count());
}

TEST(animal_shogi_node, explore_until_game_end)
{
    auto g = Game();
    auto root = Node(g.get_legal_moves(), g.get_turn(), 0.f, zeros);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - root.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = root.select(g_copy, 4.f, 3, 1);
            if (n != nullptr)
                n->simulate_expand_and_backprop(
                    g_copy.get_legal_moves(), g.get_turn(), 0.f, zeros);
        }

        const auto action = root.get_action_by_visit_max();
        g.apply(action);
        root.apply(action);
    }
}

} // namespace test_animal_shogi

namespace test_minishogi
{

using namespace vshogi::minishogi;
using Node = vshogi::engine::mcts::Node<Game, Move>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP(minishogi_node){};

TEST(minishogi_node, explore_until_game_end)
{
    auto g = Game();
    auto root = Node(g.get_legal_moves(), g.get_turn(), 0.f, zeros);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - root.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = root.select(g_copy, 4.f, 3, 1);
            if (n != nullptr)
                n->simulate_expand_and_backprop(
                    g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
        }

        const auto action = root.get_action_by_visit_max();
        g.apply(action);
        root.apply(action);
    }
}

} // namespace test_minishogi

namespace test_judkins_shogi
{

using namespace vshogi::judkins_shogi;
using Node = vshogi::engine::mcts::Node<Game, Move>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP(judkins_shogi_node){};

TEST(judkins_shogi_node, explore_until_game_end)
{
    auto g = Game();
    auto root = Node(g.get_legal_moves(), g.get_turn(), 0.f, zeros);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - root.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = root.select(g_copy, 4.f, 3, 1);
            if (n != nullptr)
                n->simulate_expand_and_backprop(
                    g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
        }

        const auto action = root.get_action_by_visit_max();
        g.apply(action);
        root.apply(action);
    }
}

} // namespace test_judkins_shogi

namespace test_shogi
{

using namespace vshogi::shogi;
using Node = vshogi::engine::mcts::Node<Game, Move>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP(shogi_node){};

TEST(shogi_node, explore_until_game_end)
{
    auto g = Game();
    auto root = Node(g.get_legal_moves(), g.get_turn(), 0.f, zeros);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - root.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = root.select(g_copy, 4.f, 3, 1);
            if (n != nullptr)
                n->simulate_expand_and_backprop(
                    g_copy.get_legal_moves(), g_copy.get_turn(), 0.f, zeros);
        }

        const auto action = root.get_action_by_visit_max();
        g.apply(action);
        root.apply(action);
    }
}

} // namespace test_shogi

} // namespace test_vshogi::test_engine
