#include <set>

#include "vshogi/engine/gaz/searcher.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_gaz
{

using namespace vshogi;
using P = vshogi::minishogi::Parameters;
using Game = vshogi::minishogi::Game;
using Searcher = vshogi::engine::gaz::Searcher<P>;

TEST_GROUP (test_gaz_searcher) {
};

TEST(test_gaz_searcher, keep_top_n_actions)
{
    Searcher searcher{};
    Game g{};
    {
        const auto n = searcher.search(g);
        searcher.simulate_expand_backprop(n, g, 0.f);
    }
    searcher.keep_top_n_actions(2u);
    CHECK_EQUAL(2u, searcher.count_active_childs());
    std::set<move_t> actual{};
    for (uint ii = 2u; ii--;) {
        auto g_copy = Game(g);
        const auto c = searcher.search(g_copy);
        CHECK_EQUAL(&searcher.get_root(), c->backprop(0.f, nullptr));
        CHECK_FALSE(c->is_mate());
        actual.emplace(c->get_action());
    }
    CHECK_EQUAL(2u, actual.size());
    for (uint ii = 98u; ii--;) {
        auto g_copy = Game(g);
        const auto c = searcher.search(g_copy);
        CHECK_EQUAL(&searcher.get_root(), c->backprop(0.f, nullptr));
        actual.emplace(c->get_action());
    }
    CHECK_EQUAL(2u, actual.size());
    for (auto c = searcher.get_root().get_child(); c; c = c->get_sibling()) {
        if (actual.find(c->get_action()) != actual.cend())
            CHECK_EQUAL(50u, c->get_visit_count());
        else
            CHECK_EQUAL(0u, c->get_visit_count());
    }
}

TEST(test_gaz_searcher, explore_until_game_end_no_sample)
{
    Searcher searcher{};
    Game g{};
    for (uint num_ply = 0u;; ++num_ply) {
        CHECK_EQUAL(num_ply, g.ply());
        if (g.get_result() != ONGOING)
            break;
        CHECK_COMPARE(searcher.get_search_count(), <, 100u);
        for (uint ii = 100u - searcher.get_search_count(); ii--;) {
            const auto n = searcher.search(g);
            searcher.simulate_expand_backprop(n, g, 0.f, nullptr);
        }
        CHECK_EQUAL(100u, searcher.get_search_count());
        const auto action = searcher.select_action();
        CHECK_TRUE(g.is_legal(action));
        searcher.apply(g, action);
    }
}

TEST(test_gaz_searcher, explore_until_game_end_sampled)
{
    Searcher searcher{};
    Game g{};
    for (uint num_ply = 0u;; ++num_ply) {
        CHECK_EQUAL(num_ply, g.ply());
        if (g.get_result() != ONGOING)
            break;
        searcher.init();
        {
            const auto n = searcher.search(g);
            searcher.simulate_expand_backprop(n, g, 0.f, nullptr);
        }
        uint num_actions = 4u;
        uint num_simulations = 10u;
        const uint num_phase = std::max(
            static_cast<uint>(std::ceil(std::log2(num_actions))), 1u);
        for (uint phase = num_phase; phase--;) {
            CHECK_TRUE(num_actions > 1u);
            searcher.keep_top_n_actions(num_actions);
            if (g.get_legal_moves().size() >= num_actions)
                CHECK_EQUAL(num_actions, searcher.count_active_childs());
            num_actions /= 2u;
            uint budget = phase ? num_simulations / num_phase : num_simulations;
            for (uint sim = budget; sim--;) {
                const auto n = searcher.search(g);
                if (n) {
                    searcher.simulate_expand_backprop(n, g, 0.f, nullptr);
                }
            }
            num_simulations -= budget;
        }
        CHECK_EQUAL(11u, searcher.get_search_count());
        const auto action = searcher.select_action();
        CHECK_TRUE(g.is_legal(action));
        g.apply(action);
    }
}

TEST(test_gaz_searcher, dfpn)
{
    Searcher searcher_with_dfpn(1000000u, 10000u, 100u);
    Game g("5/4k/4G/4K/5 w G");
    {
        {
            const auto n = searcher_with_dfpn.search(g);
            CHECK_TRUE(n != nullptr);
            searcher_with_dfpn.simulate_expand_backprop(n, g, 0.f, nullptr);
        }
        uint num_actions = 4u;
        uint num_simulations = 10u;
        const uint num_phase = std::max(
            static_cast<uint>(std::ceil(std::log2(num_actions))), 1u);
        for (uint phase = num_phase; phase--;) {
            searcher_with_dfpn.keep_top_n_actions(num_actions);
            num_actions /= 2u;
            uint budget = phase ? num_simulations / num_phase : num_simulations;
            for (uint sim = budget; sim--;) {
                const auto n = searcher_with_dfpn.search(g);
                if (n)
                    searcher_with_dfpn.simulate_expand_backprop(
                        n, g, 0.f, nullptr);
            }
            num_simulations -= budget;
        }
        CHECK_EQUAL(11u, searcher_with_dfpn.get_search_count());
        const auto action = searcher_with_dfpn.select_action();
        g.apply(action);
    }
    CHECK_EQUAL(ONGOING, g.get_result());
    {
        searcher_with_dfpn.init();
        CHECK_EQUAL(nullptr, searcher_with_dfpn.search(g));
        CHECK_TRUE(searcher_with_dfpn.proved_mate());
        const auto action = searcher_with_dfpn.select_action();
        g.apply(action);
    }
    CHECK_EQUAL(BLACK_WIN, g.get_result());
}

TEST(test_gaz_searcher, select_action)
{
    Game g{};
    {
        Searcher searcher{1000u};
        const auto n = searcher.search(g);
        CHECK_EQUAL(&searcher.get_root(), n);
        searcher.simulate_expand_backprop(n, g, 0.f);
        searcher.keep_top_n_actions(2u);
        CHECK_EQUAL(2u, searcher.count_active_childs());
        std::set<move_t> actual{};
        for (uint ii = 1000; ii--;)
            actual.emplace(searcher.select_action());
        CHECK_EQUAL(1u, actual.size());
    }
    {
        Searcher searcher{1000u};
        searcher.simulate_expand_backprop(searcher.search(g), g, 0.f);
        CHECK_EQUAL(14u, searcher.get_root().count_childs());
        searcher.keep_top_n_actions(2u);
        std::set<move_t> actual{};
        for (uint ii = 1000; ii--;)
            actual.emplace(searcher.select_action(10.f));
        CHECK_EQUAL(14u, actual.size());
    }
    {
        Searcher searcher{1000u};
        searcher.simulate_expand_backprop(searcher.search(g), g, 0.f);
        std::set<move_t> actual{};
        for (uint ii = 1000; ii--;)
            actual.emplace(searcher.select_action());
        CHECK_EQUAL(1u, actual.size());
    }
    {
        Searcher searcher{1000u};
        searcher.simulate_expand_backprop(searcher.search(g), g, 0.f);
        std::set<move_t> actual{};
        for (uint ii = 1000; ii--;)
            actual.emplace(searcher.select_action(10.f));
        CHECK_EQUAL(g.get_legal_moves().size(), actual.size());
    }
}

} // namespace test_vshogi::test_engine::test_gaz
