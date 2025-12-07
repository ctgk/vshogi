#include "vshogi/common/notation.hpp"
#include "vshogi/engine/az/searcher.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine
{

namespace test_minishogi
{

using namespace vshogi::minishogi;
using namespace vshogi;
using Game = vshogi::minishogi::Game;
using MT = vshogi::MoveTraits<Parameters>;
using NT = vshogi::Notation<Parameters>;
using Node = vshogi::engine::az::Node;
using Searcher = vshogi::engine::az::Searcher<Parameters>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP (minishogi_searcher) {
};

TEST(minishogi_searcher, explore_after_apply)
{
    auto g = Game();
    auto az = Searcher(4.f, 1.f);
    for (int ii = 100; ii--;) {
        const auto n = az.search(g);
        if (n != nullptr) {
            az.simulate_expand_backprop(n, g, 0.f, zeros);
        }
        CHECK_EQUAL(0u, g.ply());
    }

    const auto move = az.get_action_by_visit_max();
    az.apply(g, move);
    const auto current_visit_count = az.get_search_count();
    CHECK_TRUE(current_visit_count > 0);
    for (int ii = 100; ii--;) {
        const auto n = az.search(g);
        if (n != nullptr) {
            CHECK_COMPARE(1u, <=, g.ply());
            az.simulate_expand_backprop(n, g, 0.f, zeros);
        }
        CHECK_EQUAL(1u, g.ply());
    }
    CHECK_EQUAL(current_visit_count + 100, az.get_search_count());
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
    auto g = Game("1r3/2k1G/5/2PG1/5 b -");
    auto az = Searcher(4.f, 0.f);
    for (int ii = 100; ii--;) {
        Node* const n = az.search(g);
        if (n)
            az.simulate_expand_backprop(n, g, 0.f, nullptr);
    }

    const auto m = MT::make_move(SQ_2D, SQ_3C);
    CHECK_EQUAL(m, az.get_action_by_visit_max());
    DOUBLES_EQUAL(-1.f, az.get_root().get_child(m)->get_q_value(), 1e-3f);
    const auto expected_visits
        = az.get_root().get_child(m)->get_visit_count() + 100u;

    for (int ii = 100; ii--;) {
        Node* const n = az.search(g);
        if (n)
            az.simulate_expand_backprop(n, g, 0.f, nullptr);
    }
    CHECK_EQUAL(expected_visits, az.get_root().get_child(m)->get_visit_count());
}

TEST(minishogi_searcher, test_dfpn_root)
{
    auto az = Searcher(4.f, 0.f, 10000u, 0u);
    {
        auto g = Game("5/4k/5/4P/4K b 2G");
        az.search(g);
        CHECK_TRUE(az.proved_mate());
        CHECK_EQUAL(MT::make_move("G*1c"), az.get_action_by_visit_max());
    }
    {
        az.init_root();
        auto game = Game("5/5/4k/4P/4K w 2G");
        for (int ii = 100; ii--;) {
            auto g = Game(game);
            const auto leaf = az.search(g);
            if (leaf)
                az.simulate_expand_backprop(leaf, g, 0.f, nullptr);
        }
        CHECK_FALSE(az.proved_mate());
        CHECK_FALSE(az.get_root().get_child(MT::make_move("1c1b"))->is_mate());
        az.apply(game, MT::make_move("1c1b"));
        CHECK_TRUE(az.proved_mate());
        CHECK_EQUAL(MT::make_move("G*1c"), az.get_action_by_visit_max());
    }
}

TEST(minishogi_searcher, test_dfpn_vertex)
{
    // Turn: BLACK
    // White: -
    //     5   4   3   2   1
    //   +---+---+---+---+---+
    // A |   |   |-OU|   |   |
    //   +---+---+---+---+---+
    // B |   |   |-HI|-KI|   |
    //   +---+---+---+---+---+
    // C |   |   |-GI|-FU|   |
    //   +---+---+---+---+---+
    // D |   |   |   |   |   |
    //   +---+---+---+---+---+
    // E |   |   |+OU|   |   |
    //   +---+---+---+---+---+
    // Black: -
    auto game = Game("2k2/2rg1/2sp1/5/2K2 b -");
    auto az = Searcher(4.f, 0.f, 0u, 100u);
    for (int ii = 3; ii--;) {
        Node* const n = az.search(game);
        if (n)
            az.simulate_expand_backprop(n, game, 0.f, nullptr);
        CHECK_EQUAL(0u, game.ply());
    }
    CHECK_EQUAL(MT::make_move("3e4e"), az.get_action_by_visit_max());
    az.apply(game, MT::make_move("3e2e"));
    CHECK_EQUAL(1u, game.ply());
    DOUBLES_EQUAL(1.f, az.get_root().get_q_value(), 1e-3f);
    for (int ii = 2; ii--;) {
        Node* const n = az.search(game);
        CHECK_EQUAL(1u, game.ply());
        CHECK_EQUAL(nullptr, n);
        DOUBLES_EQUAL(1.f, az.get_root().get_q_value(), 1e-3f);
    }
    {
        Node* const n = az.search(game);
        CHECK_EQUAL(1u, game.ply());
        CHECK_EQUAL(nullptr, n);
        DOUBLES_EQUAL(1.f, az.get_root().get_q_value(), 1e-3f);
    }
    CHECK_EQUAL(MT::make_move("3c2d"), az.get_action_by_visit_max());
}

TEST(minishogi_searcher, test_dfpn_root_vertex)
{
    // Turn: WHITE
    // White: GI,KI
    //     5   4   3   2   1
    //   +---+---+---+---+---+
    // A |-HI|   |   |   |   |
    //   +---+---+---+---+---+
    // B |   |   |-OU|   |   |
    //   +---+---+---+---+---+
    // C |   |   |-UM|   |+FU|
    //   +---+---+---+---+---+
    // D |+FU|+KI|+KA|   |   |
    //   +---+---+---+---+---+
    // E |+OU|   |+GI|   |   |
    //   +---+---+---+---+---+
    // Black: HI
    auto game = Game("r4/2k2/2+b1P/PGB2/K1S2 w Rgs 22");
    auto az = Searcher(4.f, 0.f, 10000u, 100u);
    for (int ii = 4; ii--;) {
        const auto leaf = az.search(game);
        if (leaf)
            az.simulate_expand_backprop(leaf, game, 0.f, nullptr);
        CHECK_EQUAL(0u, game.ply());
    }
    CHECK_TRUE(az.proved_mate());
    DOUBLES_EQUAL(1.f, az.get_root().get_q_value(), 1e-3f);
}

TEST(minishogi_searcher, explore_until_game_end)
{
    auto g = Game();
    auto az = Searcher(4.f, 1.f);
    for (uint num_ply = 0u;; ++num_ply) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        CHECK_COMPARE(100u, >=, az.get_search_count());
        for (uint ii = (100 - az.get_search_count()); ii--;) {
            const auto n = az.search(g);
            if (n != nullptr) {
                CHECK_COMPARE(num_ply, <=, g.ply());
                az.simulate_expand_backprop(n, g, 0.f, zeros);
            }
            CHECK_EQUAL(num_ply, g.ply());
        }

        const auto action = az.get_action_by_visit_max();
        az.apply(g, action);
    }
}

} // namespace test_minishogi

namespace test_judkins_shogi
{

using namespace vshogi::judkins_shogi;
using Node = vshogi::engine::az::Node;
using Searcher = vshogi::engine::az::Searcher<Parameters>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP (judkins_shogi_searcher) {
};

TEST(judkins_shogi_searcher, explore_until_game_end)
{
    auto g = Game();
    auto az = Searcher(4.f, 0.25f);
    for (uint num_ply = 0u; g.get_result() == vshogi::ONGOING; ++num_ply) {
        CHECK_COMPARE(100u, >=, az.get_search_count());
        for (uint ii = (100 - az.get_search_count()); ii--;) {
            const auto n = az.search(g);
            if (n != nullptr) {
                CHECK_COMPARE(num_ply, <=, g.ply());
                az.simulate_expand_backprop(n, g, 0.f, zeros);
            }
            CHECK_EQUAL(num_ply, g.ply());
        }
        const auto action = az.get_action_by_visit_max();
        az.apply(g, action);
    }
}

} // namespace test_judkins_shogi

namespace test_shogi
{

using namespace vshogi::shogi;
using Node = vshogi::engine::az::Node;
using Searcher = vshogi::engine::az::Searcher<Parameters>;
static constexpr float zeros[Game::num_dlshogi_policy()] = {0.f};

TEST_GROUP (shogi_searcher) {
};

TEST(shogi_searcher, explore_until_game_end)
{
    auto g = Game();
    auto az = Searcher(4.f, 0.25f);
    for (uint num_ply = 0u; g.get_result() == vshogi::ONGOING; ++num_ply) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        CHECK_COMPARE(100u, >=, az.get_search_count());
        for (uint ii = (100 - az.get_search_count()); ii--;) {
            const auto n = az.search(g);
            if (n != nullptr) {
                CHECK_COMPARE(num_ply, <=, g.ply());
                az.simulate_expand_backprop(n, g, 0.f, zeros);
            }
            CHECK_EQUAL(num_ply, g.ply());
        }

        const auto action = az.get_action_by_visit_max();
        az.apply(g, action);
    }
}

TEST(shogi_searcher, dfpn)
{
    const char kifu[][6] = {"S*2b", "3a4a", "L*4b"};
    auto g = Game("l5k1l/3+R5/1p3Gnp1/5pp1p/2p1p2P1/p5P1P/1P1S5/PG2+p4/1NK1S3L "
                  "b RGSLP2bgs2n3p 165");
    auto az = Searcher(4.f, 0.25f, 10000u, 100u);
    for (uint ii = 0u; ii < 3u; ++ii) {
        if (g.get_result() != vshogi::ONGOING)
            break;
        CHECK_COMPARE(1u, >=, az.get_search_count());
        for (uint jj = 1u - az.get_search_count(); jj--;) {
            const auto n = az.search(g);
            if (n == nullptr)
                continue;
            CHECK_TRUE((n == &az.get_root()) || (g.ply() > ii));
            az.simulate_expand_backprop(n, g, 0.f, zeros);
            CHECK_EQUAL(ii, g.ply());
        }

        az.get_action_by_visit_max();
        const auto m = MoveTraits::make_move(kifu[ii]);
        az.apply(g, m);
        if (ii == 2u)
            CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
        else
            CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        CHECK_EQUAL(1u, g.count_repetitions());
    }
}

} // namespace test_shogi

} // namespace test_vshogi::test_engine
