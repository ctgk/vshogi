#include "vshogi/animal_shogi/game.hpp"
#include "vshogi/engine/mcts.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

using namespace vshogi::animal_shogi;
using Node = vshogi::engine::Node<Game, Move>;

TEST_GROUP(node){};

TEST(node, init_default)
{
    auto root = Node();
    CHECK_FALSE(root.is_valid());
    CHECK_EQUAL(0, root.get_visit_count());
    DOUBLES_EQUAL(0.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-2f);
}

TEST(node, init_with_args)
{
    auto root = Node(1.f, {}, {});
    CHECK_TRUE(root.is_valid());
    CHECK_EQUAL(1, root.get_visit_count());
    DOUBLES_EQUAL(1.f, root.get_value(), 1e-2f);
    DOUBLES_EQUAL(1.f, root.get_q_value(), 1e-2f);
}

TEST(node, explore_no_child)
{
    auto g = Game();
    auto root = Node(1., {}, {});

    const auto actual = root.explore(g, 1.f, 0.5f, 1);

    CHECK_EQUAL(2, root.get_visit_count());
    CHECK_TRUE(nullptr == actual);
}

TEST(node, explore_one_action)
{
    auto g = Game();
    auto root = Node(1.f, {Move(SQ_B2, SQ_B3)}, {1.f});

    const auto actual = root.explore(g, 1.f, 0.f, 0);
    {
        STRCMP_EQUAL("gle/1C1/3/ELG w C 2", g.to_sfen().c_str());

        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL(1.f, root.get_value(), 1e-2f);
        DOUBLES_EQUAL(1.f, root.get_q_value(), 1e-2f);

        CHECK_TRUE(actual != nullptr);
        CHECK_TRUE(actual != &root);
        CHECK_FALSE(actual->is_valid());
    }

    actual->set_value_action_proba(-0.8f, {}, {});
    {
        CHECK_EQUAL(2, root.get_visit_count());
        DOUBLES_EQUAL(1.f, root.get_value(), 1e-2f);
        DOUBLES_EQUAL(
            std::tanh((5.f + std::atanh(0.8f)) * 0.5f),
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

TEST(node, explore_two_action)
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
     *     - Move(SQ_B2, SQ_B3): 0 + 0.8 * 1 = 0.8 <-
     *     - Move(SQ_C3, SQ_C4): 0 + 0.2 * 1 = 0.2
     * - step2
     *     - Move(SQ_B2, SQ_B3): -0.3 + (0.8 * sqrt(2) / 2) * 1 = 0.266
     *     - Move(SQ_C3, SQ_C4): 0 + (0.2 * sqrt(2) / 1) * 1 = 0.282 <-
     * - step3
     *     - Move(SQ_B2, SQ_B3): -0.3 + (0.8 * sqrt(3) / 2) * 1 = 0.393 <-
     *     - Move(SQ_C3, SQ_C4): 0.2 + (0.2 * sqrt(3) / 2) * 1 = 0.373
     * - step4
     *     - Move(SQ_B2, SQ_B3): -0.3 + (0.8 * sqrt(4) / 3) * 1 = 0.233
     *     - Move(SQ_C3, SQ_C4): 0.2 + (0.2 * sqrt(4) / 2) * 1 = 0.400 <-
     * - step5
     *     - Move(SQ_B2, SQ_B3): -0.3 + (0.8 * sqrt(5) / 3) * 1 = 0.296
     *     - Move(SQ_C3, SQ_C4): 0.2 + (0.2 * sqrt(5) / 3) * 1 = 0.349 <-
     */

    std::vector<float> input_value = {0.3f, -0.2f};
    std::vector<bool> expected_leaf_nullptr = {false, false, true, true, true};
    const float atanh0p3 = std::atanh(0.3f);
    const float atanh0p2 = std::atanh(0.2f);
    std::vector<float> expected_q_value = {
        // At first, Move(SQ_B2, SQ_B3) is selected due to higher probability
        // clang-format off
        std::tanh((0.f + -atanh0p3) / 2.f),                        // Move(SQ_B2, SQ_B3) selected
        std::tanh((0.f + -atanh0p3 + atanh0p2) / 3.f),             // Move(SQ_C3, SQ_C4) selected
        std::tanh((0.f + -atanh0p3 * 2.f + atanh0p2) / 4.f),       // Move(SQ_B2, SQ_B3) selected
        std::tanh((0.f + -atanh0p3 * 2.f + atanh0p2 * 2.f) / 5.f), // Move(SQ_C3, SQ_C4) selected
        std::tanh((0.f + -atanh0p3 * 2.f + atanh0p2 * 3.f) / 6.f), // Move(SQ_C3, SQ_C4) selected
        // clang-format on
        // In the end, Move(SQ_C3, SQ_C4) is selected due to higher value
        // (lower value from the opponent perspective).
    };

    auto root
        = Node(0.f, {Move(SQ_B2, SQ_B3), Move(SQ_C3, SQ_C4)}, {0.8f, 0.2f});

    for (std::size_t ii = 0; ii < 5; ++ii) {
        auto g = Game();
        const auto actual = root.explore(g, 1.f, 0.f, 0);
        if (actual != nullptr) {
            actual->set_value_action_proba(input_value[ii], {}, {});
        }

        CHECK_EQUAL(expected_leaf_nullptr[ii], (actual == nullptr));
        DOUBLES_EQUAL(expected_q_value[ii], root.get_q_value(), 1e-3f);
    }
}

TEST(node, explore_two_layer)
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
     *         - Move(SQ_B2, SQ_B3): 0 + 0.9 * 1 = 0.9 <-
     *         - Move(SQ_C3, SQ_C4): 0 + 0.1 * 1 = 0.1
     * - step2
     *     - Layer1
     *         - Move(SQ_B2, SQ_B3): 0.9 + (0.9 * sqrt(2) / 2) * 1 = 1.536 <-
     *         - Move(SQ_C3, SQ_C4): 0 + (0.1 * sqrt(2) / 1) * 1 = 0.141
     *     - Layer2
     *         - Move(SQ_B2, SQ_B1): 0 + 0.9 * 1 <-
     *         - Move(SQ_A2, SQ_B1): 0 + 0.1 * 1
     * - step3
     *     - Layer1
     *         - Move(SQ_B2, SQ_B3): tanh((atanh(0.9)-atanh(0.5))/2) + (0.9 * sqrt(3) / 3) * 1 = 0.951 <-
     *         - Move(SQ_C3, SQ_C4): 0 + (0.1 * sqrt(3) / 1) * 1 = 0.173
     *     - Layer2
     *         - Move(SQ_B2, SQ_B1): 0.5 + (0.9 * sqrt(2) / 2) * 1 = 1.136 <-
     *         - Move(SQ_A2, SQ_B1): 0 + 0.1 * 1
     */

    std::vector<float> input_value = {-0.9f, -0.5f};
    std::vector<bool> expected_leaf_nullptr = {false, false, true};

    auto root
        = Node(0.f, {Move(SQ_B2, SQ_B3), Move(SQ_C3, SQ_C4)}, {0.8f, 0.2f});

    {
        auto g = Game();
        const auto actual = root.explore(g, 1.f, 0.f, 0);
        CHECK_TRUE(actual != nullptr);
        actual->set_value_action_proba(
            -0.9f, {Move(SQ_B2, SQ_B1), Move(SQ_A2, SQ_B1)}, {0.9f, 0.1f});
        DOUBLES_EQUAL(
            std::tanh((0.f + std::atanh(0.9f)) / 2.f),
            root.get_q_value(),
            1e-3f);
    }
    {
        auto g = Game();
        const auto actual = root.explore(g, 1.f, 0.f, 0);
        CHECK_TRUE(actual != nullptr);
        actual->set_value_action_proba(-0.5f, {}, {});
        DOUBLES_EQUAL(
            std::tanh((0.f + std::atanh(0.9f) + std::atanh(-0.5f)) / 3.f),
            root.get_q_value(),
            1e-3f);
    }
    {
        auto g = Game();
        const auto actual = root.explore(g, 1.f, 0.f, 0);
        CHECK_TRUE(actual == nullptr);
        DOUBLES_EQUAL(
            std::tanh((0.f + std::atanh(0.9f) + std::atanh(-0.5f) * 2.f) / 4.f),
            root.get_q_value(),
            1e-3f);
    }
    {
        auto n = root.pop_child(Move(SQ_B2, SQ_B3));
        CHECK_EQUAL(1, root.get_visit_count());
        DOUBLES_EQUAL(0.f, root.get_q_value(), 1e-3f);
        CHECK_EQUAL(3, n.get_visit_count());
        DOUBLES_EQUAL(
            std::tanh((std::atanh(-0.9f) + std::atanh(0.5f) * 2.f) / 3.f),
            n.get_q_value(),
            1e-3f);
    }
}

TEST(node, get_child_pointer_after_pop)
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
     *         - Move(SQ_B2, SQ_B3): 0 + 0.9 * 1 = 0.9 <-
     *         - Move(SQ_C3, SQ_C4): 0 + 0.1 * 1 = 0.1
     * - step2
     *     - Layer1
     *         - Move(SQ_B2, SQ_B3): tanh(0.9) + (0.9 * sqrt(2) / 2) * 1 = 1.353 <-
     *         - Move(SQ_C3, SQ_C4): 0 + (0.1 * sqrt(2) / 1) * 1 = 0.141
     *     - Layer2
     *         - Move(SQ_B2, SQ_B1): 0 + 0.9 * 1 <-
     *         - Move(SQ_A2, SQ_B1): 0 + 0.1 * 1
     */

    auto root
        = Node(0.f, {Move(SQ_B2, SQ_B3), Move(SQ_C3, SQ_C4)}, {0.8f, 0.2f});

    {
        auto g = Game();
        const auto actual = root.explore(g, 1.f, 0.f, 0);
        CHECK_TRUE(actual != nullptr);
        actual->set_value_action_proba(
            -0.9f, {Move(SQ_B2, SQ_B1), Move(SQ_A2, SQ_B1)}, {0.9f, 0.1f});
        DOUBLES_EQUAL(
            std::tanh((0.f + std::atanh(0.9f)) / 2.f),
            root.get_q_value(),
            1e-3f);
    }
    const Node* expected = nullptr;
    {
        auto g = Game();
        const auto actual = root.explore(g, 1.f, 0.f, 0);
        CHECK_TRUE(actual != nullptr);
        actual->set_value_action_proba(-0.5f, {}, {});
        DOUBLES_EQUAL(
            std::tanh((0.f + std::atanh(0.9f) + std::atanh(-0.5f)) / 3.f),
            root.get_q_value(),
            1e-3f);
        expected = actual;
    }
    root = root.pop_child(Move(SQ_B2, SQ_B3));
    CHECK_TRUE(expected == &root.get_child(Move(SQ_B2, SQ_B1)));
}

TEST(node, explore_from_popped_child)
{
    auto g = Game();
    const auto moves = g.get_legal_moves();
    auto root = Node(
        0.f,
        moves,
        std::vector<float>(
            moves.size(), 1.f / static_cast<float>(moves.size())));
    for (int ii = 100; ii--;) {
        auto g_copy = Game(g);
        const auto n = root.explore(g_copy, 4.f, 0.25f, 1);
        if (n != nullptr) {
            const float v
                = (g_copy.get_result() == vshogi::ONGOING) ? 0.f : -1.f;
            const auto m = g_copy.get_legal_moves();
            n->set_value_action_proba(
                v,
                m,
                std::vector<float>(
                    m.size(), 1.f / static_cast<float>(m.size())));
        }
    }

    const auto selected = Move(SQ_B2, SQ_B3);
    root = root.pop_child(selected);
    g.apply(selected);
    for (int ii = 100; ii--;) {
        auto g_copy = Game(g);
        const auto n = root.explore(g_copy, 4.f, 0.25f, 1);
        if (n != nullptr) {
            const float v
                = (g_copy.get_result() == vshogi::ONGOING) ? 0.f : -1.f;
            const auto m = g_copy.get_legal_moves();
            n->set_value_action_proba(
                v,
                m,
                std::vector<float>(
                    m.size(), 1.f / static_cast<float>(m.size())));
        }
    }
}

} // namespace test_vshogi::test_engine
