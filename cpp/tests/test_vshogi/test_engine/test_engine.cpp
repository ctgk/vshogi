#include "vshogi/engine/dfpn.hpp"
#include "vshogi/engine/mcts.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

namespace test_shogi
{

using namespace vshogi::shogi;
using Node = vshogi::engine::mcts::Node<Game, Move>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP(shogi_engine){};

TEST(shogi_engine, mcts_with_dfpn)
{
    auto g = Game();
    auto root = Node(g, 0.f, zeros);
    while (true) {
        if (g.get_result() != vshogi::ONGOING)
            break;

        auto dfpn = vshogi::engine::dfpn::Searcher<Game, Move>(g, 10000);
        if (dfpn.explore())
            break;

        for (int ii = (100 - root.get_visit_count()); ii--;) {
            auto g_copy = Game(g);
            const auto n = root.select(g_copy, 4.f, 3, 1, 100);
            if (n != nullptr)
                n->simulate_expand_and_backprop(g_copy, 0.f, zeros);
        }

        const auto action = root.get_best_action();
        g.apply(action);
        root.apply(action);
    }
}

} // namespace test_shogi

} // namespace test_vshogi::test_engine
