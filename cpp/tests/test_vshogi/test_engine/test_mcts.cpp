#include "vshogi/animal_shogi/game.hpp"
#include "vshogi/engine/mcts.hpp"
#include "vshogi/minishogi/game.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

namespace test_animal_shogi
{

using namespace vshogi::animal_shogi;
using Node = vshogi::engine::Node<Game, Move>;
constexpr auto SQ_A4 = vshogi::animal_shogi::Squares::SQ_A4; // NOLINT
constexpr auto SQ_A3 = vshogi::animal_shogi::Squares::SQ_A3; // NOLINT
constexpr auto SQ_A2 = vshogi::animal_shogi::Squares::SQ_A2; // NOLINT
constexpr auto SQ_A1 = vshogi::animal_shogi::Squares::SQ_A1; // NOLINT
constexpr auto SQ_B4 = vshogi::animal_shogi::Squares::SQ_B4; // NOLINT
constexpr auto SQ_B3 = vshogi::animal_shogi::Squares::SQ_B3; // NOLINT
constexpr auto SQ_B2 = vshogi::animal_shogi::Squares::SQ_B2; // NOLINT
constexpr auto SQ_B1 = vshogi::animal_shogi::Squares::SQ_B1; // NOLINT
constexpr auto SQ_C4 = vshogi::animal_shogi::Squares::SQ_C4; // NOLINT
constexpr auto SQ_C3 = vshogi::animal_shogi::Squares::SQ_C3; // NOLINT
constexpr auto SQ_C2 = vshogi::animal_shogi::Squares::SQ_C2; // NOLINT
constexpr auto SQ_C1 = vshogi::animal_shogi::Squares::SQ_C1; // NOLINT
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP(node_animal_shogi){};

TEST(node_animal_shogi, init_default)
{
    auto root = Node();
    CHECK_FALSE(root.is_valid());
    CHECK_EQUAL(0, root.get_visit_count());
    DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
}

TEST(node_animal_shogi, init_with_args)
{
    auto game = Game("3/3/3/3 b -");
    auto root = Node(game, 1.f, zeros);
    CHECK_TRUE(root.is_valid());
    CHECK_EQUAL(1, root.get_visit_count());
    DOUBLES_EQUAL(1.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(1.f, root.get_q_value(), 1e-2f);
}

TEST(node_animal_shogi, explore_no_child)
{
    auto g = Game("3/3/3/3 b -");
    auto root = Node(g, 1.f, zeros);

    const auto actual = root.explore(g, 1.f, 0.5f, 1);

    CHECK_EQUAL(2, root.get_visit_count());
    CHECK_TRUE(nullptr == actual);
}

TEST(node_animal_shogi, explore_game_end)
{
    auto g = Game("3/1l1/1C1/3 b -");
    auto root = Node(g, 0.f, zeros);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
    const auto actual = root.explore(g, 1.f, 0.f, 0);
    CHECK_TRUE(nullptr == actual);
    DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
    CHECK_TRUE(root.get_q_value() > 0.5f);
}

TEST(node_animal_shogi, explore_one_action)
{
    auto g = Game("1l1/3/1C1/3 b -");
    auto root = Node(g, 0.1f, zeros);

    const auto actual = root.explore(g, 1.f, 0.f, 0);
    {
        STRCMP_EQUAL("1l1/1C1/3/3 w - 2", g.to_sfen().c_str());

        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL(0.1f, root.get_value(), 1e-2f);
        DOUBLES_EQUAL(0.1f, root.get_q_value(), 1e-2f);

        CHECK_TRUE(actual != nullptr);
        CHECK_TRUE(actual != &root);
        CHECK_FALSE(actual->is_valid());
    }

    actual->set_value_policy_logits(g, -0.8f, zeros);
    {
        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL(0.1f, root.get_value(), 1e-2f);
        DOUBLES_EQUAL(
            std::tanh((std::atanh(0.1f) + std::atanh(0.8f)) * 0.5f),
            root.get_q_value(),
            1e-2f);

        CHECK_TRUE(actual->is_valid());
        CHECK_EQUAL(1, actual->get_visit_count());
        DOUBLES_EQUAL(-0.8f, actual->get_value(), 1e-2f);
        DOUBLES_EQUAL(-0.8f, actual->get_q_value(), 1e-2f);

        const auto& ch = root.get_child(Move(SQ_B2, SQ_B3));
        CHECK_TRUE(actual == &ch);
    }
}

TEST(node_animal_shogi, explore_two_action)
{

    /**
     *                        Node(v=0)
     *                        /     \
     *                       /       \
     *                    p=0.8     p=0.2
     *                     /           \
     *                    /             \
     *             Node(v=-0.3)     Node(v=0.2)
     *
     * PUCT scores (Q + U * c)
     * - step1
     *     - Move(SQ_A3, SQ_A4): 0 + 0.8 * 1 = 0.8 <-
     *     - Move(SQ_B4, SQ_A4): 0 + 0.2 * 1 = 0.2
     * - step2
     *     - Move(SQ_A3, SQ_A4): -0.3 + (0.8 * sqrt(2) / 2) * 1 = 0.266
     *     - Move(SQ_B4, SQ_A4): 0 + (0.2 * sqrt(2) / 1) * 1 = 0.282 <-
     * - step3
     *     - Move(SQ_A3, SQ_A4): -0.3 + (0.8 * sqrt(3) / 2) * 1 = 0.393 <-
     *     - Move(SQ_B4, SQ_A4): 0.2 + (0.2 * sqrt(3) / 2) * 1 = 0.373
     * - step4
     *     - Move(SQ_A3, SQ_A4): -0.3 + (0.8 * sqrt(4) / 3) * 1 = 0.233
     *     - Move(SQ_B4, SQ_A4): 0.2 + (0.2 * sqrt(4) / 2) * 1 = 0.400 <-
     * - step5
     *     - Move(SQ_A3, SQ_A4): -0.3 + (0.8 * sqrt(5) / 3) * 1 = 0.296
     *     - Move(SQ_B4, SQ_A4): 0.2 + (0.2 * sqrt(5) / 3) * 1 = 0.349 <-
     */

    std::vector<float> input_value = {0.3f, -0.2f};
    std::vector<bool> expected_leaf_nullptr = {false, false, true, true, true};
    const float atanh0p3 = std::atanh(0.3f);
    const float atanh0p2 = std::atanh(0.2f);
    std::vector<float> expected_q_value = {
        // At first, Move(SQ_A3, SQ_A4) is selected due to higher probability
        // clang-format off
        std::tanh((0.f + -atanh0p3) / 2.f),                        // Move(SQ_A3, SQ_A4) selected
        std::tanh((0.f + -atanh0p3 + atanh0p2) / 3.f),             // Move(SQ_B4, SQ_A4) selected
        std::tanh((0.f + -atanh0p3 * 2.f + atanh0p2) / 4.f),       // Move(SQ_A3, SQ_A4) selected
        std::tanh((0.f + -atanh0p3 * 2.f + atanh0p2 * 2.f) / 5.f), // Move(SQ_B4, SQ_A4) selected
        std::tanh((0.f + -atanh0p3 * 2.f + atanh0p2 * 3.f) / 6.f), // Move(SQ_B4, SQ_A4) selected
        // clang-format on
        // In the end, Move(SQ_B4, SQ_A4) is selected due to higher value
        // (lower value from the opponent perspective).
    };

    // softmax([-0.693, 0.693]) -> [0.2000471, 0.7999529]
    float logits[Game::num_dlshogi_policy()] = {0.f};
    logits[Move(SQ_A3, SQ_A4).to_dlshogi_policy_index()] = 0.693f;
    logits[Move(SQ_B4, SQ_A4).to_dlshogi_policy_index()] = -0.693f;
    auto g = Game("1l1/3/3/G2 b -");
    auto root = Node(g, 0.f, logits);

    for (std::size_t ii = 0; ii < 5; ++ii) {
        auto g_copy = Game(g);
        const auto actual = root.explore(g_copy, 1.f, 0.f, 0);
        if (actual != nullptr) {
            auto g_tmp = Game("3/1l1/1L1/3 w -");
            g_tmp.apply(Move(SQ_B3, SQ_B2));
            actual->set_value_policy_logits(g_tmp, input_value[ii], zeros);
        }

        CHECK_EQUAL(expected_leaf_nullptr[ii], (actual == nullptr));
        DOUBLES_EQUAL(expected_q_value[ii], root.get_q_value(), 1e-3f);
    }
}

TEST(node_animal_shogi, explore_two_layer)
{

    /**
     *                     Node(atanhv=0)
     *                        /     \
     *                       /       \
     *                    p=0.9     p=0.1
     *                     /           \
     *                    /             \
     *          Node(atanhv=-0.9) Node(atanhv=0.9)
     *                /     \
     *               /       \
     *           p=0.9      p=0.1
     *             /           \
     *            /             \
     *  Node(atanhv=-0.5) Node(atanhv=0.8)
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
     * - step3
     *     - Layer1
     *         - Move(SQ_A3, SQ_A4): tanh((atanh(0.9)-atanh(0.5))/2) + (0.9 * sqrt(3) / 3) * 1 = 0.951 <-
     *         - Move(SQ_B4, SQ_A4): 0 + (0.1 * sqrt(3) / 1) * 1 = 0.173
     *     - Layer2
     *         - Move(SQ_C2, SQ_C1): 0.5 + (0.9 * sqrt(2) / 2) * 1 = 1.136 <-
     *         - Move(SQ_B1, SQ_C1): 0 + 0.1 * 1
     */

    std::vector<float> input_value = {-0.9f, -0.5f};
    std::vector<bool> expected_leaf_nullptr = {false, false, true};

    // softmax([-1.099, 1.099]) -> [0.09993023, 0.90006977]
    float logits[Game::num_dlshogi_policy()] = {0.f};
    logits[Move(SQ_A3, SQ_A4).to_dlshogi_policy_index()] = 1.099f;
    logits[Move(SQ_B4, SQ_A4).to_dlshogi_policy_index()] = -1.099f;
    auto g = Game("2g/3/3/G2 b -");
    auto root = Node(g, 0.f, logits);

    {
        auto g_copy = Game("2g/3/3/G2 b -");
        const auto actual = root.explore(g_copy, 1.f, 0.f, 0);
        CHECK_TRUE(actual != nullptr);
        STRCMP_EQUAL("2g/3/G2/3 w - 2", g_copy.to_sfen().c_str());
        float policy[Game::num_dlshogi_policy()] = {0.f};
        policy[Move(SQ_C2, SQ_C1).rotate().to_dlshogi_policy_index()] = 1.099f;
        policy[Move(SQ_B1, SQ_C1).rotate().to_dlshogi_policy_index()] = -1.099f;
        actual->set_value_policy_logits(g_copy, -0.9f, policy);
        DOUBLES_EQUAL(
            std::tanh((0.f + std::atanh(0.9f)) / 2.f),
            root.get_q_value(),
            1e-3f);
    }
    {
        auto g_copy = Game("2g/3/3/G2 b -");
        const auto actual = root.explore(g_copy, 1.f, 0.f, 0);
        CHECK_TRUE(actual != nullptr);
        STRCMP_EQUAL("3/2g/G2/3 b - 3", g_copy.to_sfen().c_str());
        actual->set_value_policy_logits(
            Game("3/1L1/3/3 b -").apply(Move(SQ_B1, SQ_B2)), -0.5f, zeros);
        DOUBLES_EQUAL(
            std::tanh((0.f + std::atanh(0.9f) + std::atanh(-0.5f)) / 3.f),
            root.get_q_value(),
            1e-3f);
    }
    {
        auto g_copy = Game("2g/3/3/G2 b -");
        const auto actual = root.explore(g, 1.f, 0.f, 0);
        CHECK_TRUE(actual == nullptr);
        DOUBLES_EQUAL(
            std::tanh((0.f + std::atanh(0.9f) + std::atanh(-0.5f) * 2.f) / 4.f),
            root.get_q_value(),
            1e-3f);
    }
    {
        auto n = root.pop_child(Move(SQ_A3, SQ_A4));
        CHECK_EQUAL(1, root.get_visit_count());
        DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-3f);
        CHECK_EQUAL(3, n.get_visit_count());
        DOUBLES_EQUAL(0.9f, n.get_proba(Move(SQ_C2, SQ_C1)), 1e-2f);
        DOUBLES_EQUAL(0.1f, n.get_proba(Move(SQ_B1, SQ_C1)), 1e-2f);
        DOUBLES_EQUAL(
            std::tanh((std::atanh(-0.9f) + std::atanh(0.5f) * 2.f) / 3.f),
            n.get_q_value(),
            1e-3f);
    }
}

TEST(node_animal_shogi, explore_from_popped_child)
{
    auto g = Game();
    const auto moves = g.get_legal_moves();
    auto root = Node(g, 0.f, zeros);
    for (int ii = 100; ii--;) {
        auto g_copy = Game(g);
        const auto n = root.explore(g_copy, 4.f, 0.25f, 1);
        if (n != nullptr)
            n->set_value_policy_logits(g_copy, 0.f, zeros);
    }

    const auto selected = Move(SQ_B2, SQ_B3);
    root = root.pop_child(selected);
    g.apply(selected);
    for (int ii = 100; ii--;) {
        auto g_copy = Game(g);
        const auto n = root.explore(g_copy, 4.f, 0.25f, 1);
        if (n != nullptr)
            n->set_value_policy_logits(g_copy, 0.f, zeros);
    }
}

TEST(node_animal_shogi, explore_until_game_end)
{
    auto g = Game();
    auto root = Node(g, 0.f, zeros);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - root.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = root.explore(g_copy, 4.f, 0.25f, 1);
            if (n != nullptr)
                n->set_value_policy_logits(g_copy, 0.f, zeros);
        }

        const auto& actions = root.get_actions();
        auto visit_counts = std::vector<int>();
        for (auto&& a : actions) {
            visit_counts.emplace_back(root.get_child(a).get_visit_count());
        }
        const auto index = static_cast<std::size_t>(std::distance(
            visit_counts.cbegin(),
            std::max_element(visit_counts.cbegin(), visit_counts.cend())));
        const auto action = actions[index];
        g.apply(action);
        root = root.pop_child(action);
    }
}

} // namespace test_animal_shogi

namespace test_minishogi
{

using namespace vshogi::minishogi;
using Node = vshogi::engine::Node<Game, Move>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP(node_minishogi){};

TEST(node_minishogi, explore_until_game_end)
{
    auto g = Game();
    auto root = Node(g, 0.f, zeros);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        for (int ii = (100 - root.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = root.explore(g_copy, 4.f, 0.25f, 1);
            if (n != nullptr)
                n->set_value_policy_logits(g_copy, 0.f, zeros);
        }

        const auto& actions = root.get_actions();
        auto visit_counts = std::vector<int>();
        for (auto&& a : actions) {
            visit_counts.emplace_back(root.get_child(a).get_visit_count());
        }
        const auto index = static_cast<std::size_t>(std::distance(
            visit_counts.cbegin(),
            std::max_element(visit_counts.cbegin(), visit_counts.cend())));
        const auto action = actions[index];
        g.apply(action);
        root = root.pop_child(action);
    }
}

} // namespace test_minishogi

} // namespace test_vshogi::test_engine
