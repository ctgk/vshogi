#include "vshogi/engine/dfpn/edge.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn
{

TEST_GROUP (test_dfpn_extend_edges) {
};

TEST(test_dfpn_extend_edges, test_offence_board)
{
    using namespace vshogi::minishogi;
    namespace dp = vshogi::engine::dfpn;
    using Edge = dp::Edge<Parameters>;
    {
        auto g = Game("4k/5/4G/5/5 b -");
        auto edges = std::list<Edge>();
        Edge *e1 = nullptr, *e2 = nullptr;
        const uint dn = dp::internal::extend_edges(edges, g, true, &e1, &e2);
        CHECK_EQUAL(2u * dp::unit, dn);
        CHECK_EQUAL(2u, edges.size());
    }
    {
        std::list<Edge>* edges_l = nullptr;
        const auto edges_g = std::list<Edge>({Move(SQ_1B, SQ_1C)});

        auto g = Game("4k/5/4G/5/5 b -");
        auto edges = std::list<Edge>();
        Edge *e1 = nullptr, *e2 = nullptr;
        const uint dn = dp::internal::extend_edges(
            edges, g, true, &e1, &e2, edges_l, &edges_g);
        CHECK_EQUAL(1u * dp::unit, dn);
        CHECK_EQUAL(1u, edges.size());
    }
    {
        std::list<Edge>* edges_g = nullptr;
        const auto edges_l = std::list<Edge>({Move(SQ_1B, SQ_1C)});

        auto g = Game("4k/5/4G/5/5 b -");
        auto edges = std::list<Edge>();
        Edge *e1 = nullptr, *e2 = nullptr;
        const uint dn = dp::internal::extend_edges(
            edges, g, true, &e1, &e2, &edges_l, edges_g);
        CHECK_EQUAL(1u * dp::unit, dn);
        CHECK_EQUAL(1u, edges.size());
    }
    {
        auto g = Game("2B1k/5/5/5/5 b -");
        auto edges = std::list<Edge>();
        Edge *e1 = nullptr, *e2 = nullptr;
        const uint dn = dp::internal::extend_edges(edges, g, true, &e1, &e2);
        CHECK_EQUAL(dp::unit + dp::cent, dn);

        CHECK_EQUAL(Move(SQ_2B, SQ_3A, true).hash(), e1->m_action.hash());
        CHECK_EQUAL(dp::unit, e1->m_pn);
        CHECK_EQUAL(dp::unit, e1->m_dn);

        CHECK_EQUAL(Move(SQ_2B, SQ_3A, false).hash(), e2->m_action.hash());
        CHECK_EQUAL(dp::kilo, e2->m_pn);
        CHECK_EQUAL(dp::cent, e2->m_dn);
    }
}

TEST(test_dfpn_extend_edges, test_offence_drop)
{
    using namespace vshogi::minishogi;
    namespace dp = vshogi::engine::dfpn;
    using Edge = dp::Edge<Parameters>;
    {
        auto g = Game("4k/5/5/5/5 b G");
        auto edges = std::list<Edge>();
        Edge *e1 = nullptr, *e2 = nullptr;
        const uint dn = dp::internal::extend_edges(edges, g, true, &e1, &e2);
        CHECK_EQUAL(3u * dp::unit, dn);
        CHECK_EQUAL(3u, edges.size());
    }
    {
        std::list<Edge>* edges_l = nullptr;
        const auto edges_g
            = std::list<Edge>({Move(SQ_1B, KI), Move(SQ_2B, KI)});

        auto g = Game("4k/5/5/5/5 b G");
        auto edges = std::list<Edge>();
        Edge *e1 = nullptr, *e2 = nullptr;
        const uint dn = dp::internal::extend_edges(
            edges, g, true, &e1, &e2, edges_l, &edges_g);
        CHECK_EQUAL(2u * dp::unit, dn);
        CHECK_EQUAL(2u, edges.size());
    }
    {
        std::list<Edge>* edges_g = nullptr;
        const auto edges_l
            = std::list<Edge>({Move(SQ_1B, KI), Move(SQ_2B, KI)});

        auto g = Game("4k/5/5/5/5 b G");
        auto edges = std::list<Edge>();
        Edge *e1 = nullptr, *e2 = nullptr;
        const uint dn = dp::internal::extend_edges(
            edges, g, true, &e1, &e2, &edges_l, edges_g);
        CHECK_EQUAL(3u * dp::unit, dn);
        CHECK_EQUAL(3u, edges.size());
    }
}

TEST(test_dfpn_extend_edges, test_offence)
{
    using namespace vshogi::minishogi;
    namespace dp = vshogi::engine::dfpn;
    using Edge = dp::Edge<Parameters>;
    {
        auto g = Game("3pk/5/4P/5/5 b R");
        auto edges = std::list<Edge>();
        Edge *e1 = nullptr, *e2 = nullptr;
        const uint dn = dp::internal::extend_edges(edges, g, true, &e1, &e2);
        CHECK_EQUAL(2u * dp::unit, dn);
        CHECK_EQUAL(2u, edges.size());
    }
}

TEST(test_dfpn_extend_edges, test_defence_checker_sq)
{
    using namespace vshogi::minishogi;
    namespace dp = vshogi::engine::dfpn;
    using Edge = dp::Edge<Parameters>;
    constexpr auto unit = dp::unit;
    {
        auto g = Game("4k/5/4S/5/5 b -").apply(Move(SQ_1B, SQ_1C));
        auto actual = std::list<Edge>();
        Edge* edge_1st = nullptr;
        Edge* edge_2nd = nullptr;
        const uint pn = dp::internal::extend_edges(
            actual, g, false, &edge_1st, &edge_2nd);
        CHECK_EQUAL(2u * unit, pn);
        CHECK_TRUE(Move(SQ_1B, SQ_1A) == edge_1st->m_action);
        CHECK_TRUE(Move(SQ_2B, SQ_1A) == edge_2nd->m_action);
    }
    {
        auto g = Game("4k/5/4S/5/5 b -").apply(Move(SQ_2B, SQ_1C));
        auto actual = std::list<Edge>();
        Edge* edge_1st = nullptr;
        Edge* edge_2nd = nullptr;
        const uint pn = dp::internal::extend_edges(
            actual, g, false, &edge_1st, &edge_2nd);
        CHECK_EQUAL(2u * unit, pn);
        CHECK_TRUE(Move(SQ_2B, SQ_1A) == edge_1st->m_action);
        CHECK_TRUE(Move(SQ_1B, SQ_1A) == edge_2nd->m_action);
    }
}

TEST(test_dfpn_extend_edges, test_defence_drop)
{
    using namespace vshogi::minishogi;
    namespace dp = vshogi::engine::dfpn;
    using Edge = dp::Edge<Parameters>;
    {
        auto g = Game("3rk/3p1/5/4R/5 w psgb");
        auto edges = std::list<Edge>();
        Edge* e1 = nullptr;
        Edge* e2 = nullptr;
        const uint pn = dp::internal::extend_edges(edges, g, false, &e1, &e2);
        CHECK_EQUAL(2u * dp::unit, pn);
        CHECK_EQUAL(8u, edges.size());
    }
    {
        std::list<Edge>* edges_l = nullptr;
        auto edges_g = std::list<Edge>(
            {Move(SQ_1B, FU),
             Move(SQ_1B, GI),
             Move(SQ_1C, KI),
             Move(SQ_1C, KA),
             Move(SQ_1C, FU)});
        for (auto& e : edges_g) {
            e.m_pn = (e.m_action.destination() == SQ_1B) ? dp::zero : dp::inf;
        }

        auto g = Game("3rk/3p1/5/4R/5 w psgb");
        auto edges = std::list<Edge>();
        Edge* e1 = nullptr;
        Edge* e2 = nullptr;
        const uint pn = dp::internal::extend_edges(
            edges, g, false, &e1, &e2, edges_l, &edges_g);
        CHECK_EQUAL(dp::cent + dp::kilo, pn);
        CHECK_EQUAL(5u, edges.size());
    }
}

} // namespace test_vshogi::test_engine::test_dfpn
