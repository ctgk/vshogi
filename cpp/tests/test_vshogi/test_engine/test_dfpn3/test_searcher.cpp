#include "vshogi/engine/dfpn3/searcher.hpp"
#include "vshogi/variants/judkins_shogi.hpp"
#include "vshogi/variants/minishogi.hpp"
#include "vshogi/variants/shogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn::test_table
{

using namespace vshogi::minishogi;
namespace dfpn = vshogi::engine::dfpn3;
using Table = dfpn::Table<Parameters>;
using Node = dfpn::Node<Parameters>;
constexpr uint inf = dfpn::inf;

TEST_GROUP (dfpn3_table) {
};

TEST(dfpn3_table, look_up_e)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto t = Table();
    auto n = Node();
    auto g = Game();
    t.add(&n, g);

    const Node *node_ge, *node_e, *node_le;
    t.look_up(g, &node_ge, &node_e, &node_le);
    CHECK_TRUE(nullptr == node_le);
    CHECK_TRUE(&n == node_e);
    CHECK_TRUE(nullptr == node_ge);

    n.expand(next, g);

    t.look_up(g, &node_ge, &node_e, &node_le);
    CHECK_TRUE(&n == node_le);
    CHECK_TRUE(&n == node_e);
    CHECK_TRUE(&n == node_ge);
}

TEST(dfpn3_table, look_up_l)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto t = Table();
    auto n = Node();
    auto g = Game("4k/5/4P/5/5 b G");
    t.add(&n, g);

    const Node *node_ge, *node_e, *node_le;
    t.look_up(Game("4k/5/4P/5/5 b SG"), &node_ge, &node_e, &node_le);
    CHECK_TRUE(nullptr == node_le);
    CHECK_TRUE(nullptr == node_e);
    CHECK_TRUE(nullptr == node_ge);

    n.expand(next, g);
    t.look_up(Game("4k/5/4P/5/5 b SG"), &node_ge, &node_e, &node_le);
    CHECK_TRUE(&n == node_le);
    CHECK_TRUE(nullptr == node_e);
    CHECK_TRUE(nullptr == node_ge);
}

TEST(dfpn3_table, look_up_l_prefer_mate_at_offence)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto n1 = Node();
    auto g1 = Game("3rk/3p1/4P/5/5 b G");
    n1.expand(next, g1);
    n1.backprop(SQ_1A, SQ_NA);
    uint th_p_ch, th_d_ch;
    Node* const c1 = n1.select(inf, inf, th_p_ch, th_d_ch);
    g1.apply(c1->get_action());
    CHECK_EQUAL(vshogi::BLACK_WIN, g1.get_result());
    CHECK_TRUE(c1->simulate(g1));
    g1.undo();
    n1.backprop(SQ_1A, g1.get_checker_location());
    CHECK_TRUE(n1.fully_expanded());
    CHECK_TRUE(n1.proved_mate());

    auto n2 = Node();
    auto g2 = Game("3rk/3p1/4P/5/5 b S");
    n2.expand(next, g2);
    CHECK_TRUE(n2.fully_expanded());
    CHECK_FALSE(n2.proved());
    {
        Table t{};
        t.add(&n1, g1);
        t.add(&n2, g2);

        const Node *node_le, *node_e, *node_ge;
        t.look_up(Game("3rk/3p1/4P/5/5 b GS"), &node_ge, &node_e, &node_le);
        CHECK_TRUE(&n1 == node_le);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(nullptr == node_ge);
    }
    {
        Table t{};
        t.add(&n2, g2);
        t.add(&n1, g1);

        const Node *node_le, *node_e, *node_ge;
        t.look_up(Game("3rk/3p1/4P/5/5 b GS"), &node_ge, &node_e, &node_le);
        CHECK_TRUE(&n1 == node_le);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(nullptr == node_ge);
    }
}

TEST(dfpn3_table, look_up_l_prefer_no_mate_at_defence)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto n1 = Node();
    n1.init(false, Move());
    auto g1 = Game("4k/4P/5/5/5 w -");
    n1.expand(next, g1);
    n1.backprop(SQ_1A, g1.get_checker_location());
    uint th_p_ch, th_d_ch;
    Node* const c1 = n1.select(inf, inf, th_p_ch, th_d_ch);
    g1.apply_dfpn(c1->get_action());
    c1->expand(next, g1);
    c1->backprop(SQ_1B, g1.get_checker_location());
    CHECK_TRUE(c1->proved_no_mate());
    g1.undo();
    n1.backprop(SQ_1A, g1.get_checker_location());
    CHECK_TRUE(n1.fully_expanded());
    CHECK_TRUE(n1.proved_no_mate());
    CHECK_FALSE(n1.proved_by_repetitions());

    auto n2 = Node();
    n2.init(false, Move());
    auto g2 = Game("4k/4P/5/5/5 w ps");
    n2.expand(next, g2);
    CHECK_TRUE(n2.fully_expanded());
    CHECK_FALSE(n2.proved());

    {
        Table t{};
        t.add(&n1, g1);
        t.add(&n2, g2);
        const Node *node_le, *node_e, *node_ge;
        t.look_up(Game("4k/4P/5/5/5 w psg"), &node_ge, &node_e, &node_le);
        CHECK_TRUE(&n1 == node_le);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(nullptr == node_ge);
    }
    {
        Table t{};
        t.add(&n2, g2);
        t.add(&n1, g1);
        const Node *node_le, *node_e, *node_ge;
        t.look_up(Game("4k/4P/5/5/5 w psg"), &node_ge, &node_e, &node_le);
        CHECK_TRUE(&n1 == node_le);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(nullptr == node_ge);
    }
}

TEST(dfpn3_table, look_up_g)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto t = Table();
    auto n = Node();
    auto g = Game("4k/5/4P/5/5 b G");
    t.add(&n, g);

    const Node *node_le, *node_e, *node_ge;
    t.look_up(Game("4k/5/4P/5/5 b -"), &node_ge, &node_e, &node_le);
    CHECK_TRUE(nullptr == node_le);
    CHECK_TRUE(nullptr == node_e);
    CHECK_TRUE(nullptr == node_ge);
    n.expand(next, g);
    t.look_up(Game("4k/5/4P/5/5 b -"), &node_ge, &node_e, &node_le);
    CHECK_TRUE(nullptr == node_le);
    CHECK_TRUE(nullptr == node_e);
    CHECK_TRUE(&n == node_ge);
}

TEST(dfpn3_table, look_up_g_prefer_no_mate_at_offence)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto n1 = Node();
    auto g1 = Game("3rk/3gs/5/5/5 b PSG");
    n1.expand(next, g1);
    n1.backprop(SQ_1A, g1.get_checker_location());
    CHECK_TRUE(n1.proved_no_mate());
    auto n2 = Node();
    auto g2 = Game("3rk/3gs/5/5/5 b PS");
    n2.expand(next, g2);
    CHECK_TRUE(n2.fully_expanded());
    CHECK_FALSE(n2.proved());
    {
        Table t{};
        t.add(&n1, g1);
        t.add(&n2, g2);
        const Node *node_le, *node_e, *node_ge;
        t.look_up(Game("3rk/3gs/5/5/5 b P"), &node_ge, &node_e, &node_le);
        CHECK_TRUE(nullptr == node_le);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(&n1 == node_ge);
    }
    {
        Table t{};
        t.add(&n2, g2);
        t.add(&n1, g1);
        const Node *node_le, *node_e, *node_ge;
        t.look_up(Game("3rk/3gs/5/5/5 b P"), &node_ge, &node_e, &node_le);
        CHECK_TRUE(nullptr == node_le);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(&n1 == node_ge);
    }
}

TEST(dfpn3_table, look_up_g_prefer_mate_at_defence)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto n1 = Node();
    n1.init(false, Move());
    auto g1 = Game("4k/4G/4P/5/5 w psg");
    n1.expand(next, g1);
    n1.backprop(SQ_1A, g1.get_checker_location());
    CHECK_TRUE(n1.fully_expanded());
    CHECK_TRUE(n1.proved_mate());
    auto n2 = Node();
    n2.init(false, Move());
    auto g2 = Game("4k/4G/4P/5/5 w ps");
    n2.expand(next, Game("4k/4S/4P/5/5 w ps")); // dummy
    CHECK_TRUE(n2.fully_expanded());
    CHECK_FALSE(n2.proved());
    {
        Table t{};
        t.add(&n1, g1);
        t.add(&n2, g2);
        const Node *node_le, *node_e, *node_ge;
        t.look_up(Game("4k/4G/4P/5/5 w p"), &node_ge, &node_e, &node_le);
        CHECK_TRUE(nullptr == node_le);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(&n1 == node_ge);
    }
    {
        Table t{};
        t.add(&n2, g2);
        t.add(&n1, g1);
        const Node *node_le, *node_e, *node_ge;
        t.look_up(Game("4k/4G/4P/5/5 w p"), &node_ge, &node_e, &node_le);
        CHECK_TRUE(nullptr == node_le);
        CHECK_TRUE(nullptr == node_e);
        CHECK_TRUE(&n1 == node_ge);
    }
}

} // namespace test_vshogi::test_engine::test_dfpn::test_table

namespace test_vshogi::test_engine::test_dfpn
{

namespace dfpn = vshogi::engine::dfpn3;

TEST_GROUP (test_dfpn3_searcher) {
    template <class P>
    void search_mate(
        const std::string& sfen,
        const uint expect_search_count,
        const uint expect_mate_length = 0u)
    {
        auto g = vshogi::Game<P>(sfen);
        auto searcher = dfpn::Searcher<P>();
        searcher.search(g, (expect_search_count + 10u) * 2u);
        CHECK_TRUE(searcher.proved());
        CHECK_TRUE(searcher.proved_mate());
        CHECK_FALSE(searcher.proved_no_mate());
        CHECK_EQUAL(expect_search_count, searcher.get_search_count());

        const auto expect_result = (g.get_turn() == vshogi::BLACK)
                                       ? vshogi::BLACK_WIN
                                       : vshogi::WHITE_WIN;
        const auto mate_moves = searcher.get_mate_moves(g);
        CHECK_EQUAL(0u, g.ply());
        if (expect_mate_length > 0)
            CHECK_EQUAL(expect_mate_length, mate_moves.size());
        for (auto&& m : mate_moves) {
            CHECK_EQUAL(vshogi::ONGOING, g.get_result());
            g.apply(m);
        }
        CHECK_EQUAL(expect_result, g.get_result());
    }
    template <class P>
    void search_no_mate(const std::string& sfen, const uint expect_search_count)
    {
        auto g = vshogi::Game<P>(sfen);
        auto searcher = dfpn::Searcher<P>();
        searcher.search(g, (expect_search_count + 10u) * 2u);
        CHECK_TRUE(searcher.proved());
        CHECK_FALSE(searcher.proved_mate());
        CHECK_TRUE(searcher.proved_no_mate());
        CHECK_EQUAL(expect_search_count, searcher.get_search_count());
        CHECK_EQUAL(0u, searcher.get_mate_move().hash());
        CHECK_EQUAL(0u, searcher.get_mate_moves(g).size());
    }
};

namespace test_minishogi
{

using namespace vshogi::minishogi;

TEST(test_dfpn3_searcher, test_init)
{
    auto g = Game("4k/5/3P1/5/5 b G");
    auto searcher = dfpn::Searcher<Parameters>();
    searcher.search(g, 1000u);
    CHECK_TRUE(searcher.proved());
    searcher.init();
    CHECK_FALSE(searcher.proved());
}

TEST(test_dfpn3_searcher, test_small_num_nodes)
{
    auto g = Game("4k/5/3P1/5/5 b G");
    auto searcher = dfpn::Searcher<Parameters>(1u);
    searcher.search(g, 100u);
    CHECK_FALSE(searcher.proved());
    CHECK_EQUAL(0u, searcher.get_search_count());
    CHECK_EQUAL(0u, searcher.get_num_nodes_remain());
}

TEST(test_dfpn3_searcher, test_debug)
{
    auto g = Game("3k1/P3p/s1s1r/2B2/KG3 b RGb");
    auto searcher = dfpn::Searcher<Parameters>();
    searcher.search(g, 1000u);
    CHECK_TRUE(searcher.proved_mate());
    const auto actual = searcher.get_mate_moves(g);
    CHECK_EQUAL(0u, g.ply());
    for (auto&& m : actual) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
}

TEST(test_dfpn3_searcher, test_minishogi_no_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |-OU|   |   |
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |   |   |   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |+OU|   |   |
        //   *---*---*---*---*---*
        // Black: -
        {"2k2/5/5/5/2K2 b -", 0u},

        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |-FU|-FU|
        //   *---*---*---*---*---*
        // B |   |   |   |-KA|-OU|
        //   *---*---*---*---*---*
        // C |   |   |   |   |   |
        //   *---*---*---*---*---*
        //   *---*---*---*---*---*
        // D |   |   |+OU|   |+GI|
        //   *---*---*---*---*---*
        // E |   |   |   |+KA|   |
        //   *---*---*---*---*---*
        // Black: -
        {"3pp/3bk/5/2K1S/3B1 b -", 14u},

        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |-OU|   |   |
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |+TO|   |   |   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: GI
        {"2k2/5/1+P3/5/5 b S", 75u},

        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |-OU|   |   |
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |+TO|   |   |   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: GIx2
        {"2k2/5/1+P3/5/5 b 2S", 780u},
    };

    for (auto&& arg : args)
        search_no_mate<Parameters>(std::get<0>(arg), std::get<1>(arg));
}

TEST(test_dfpn3_searcher, test_minishogi_1ply_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |-HI|-OU|
        //   *---*---*---*---*---*
        // B |   |   |   |-KA|   |
        //   *---*---*---*---*---*
        // C |   |   |   |   |+FU|
        //   *---*---*---*---*---*
        // D |   |   |   |   |+HI|
        //   *---*---*---*---*---*
        // E |   |   |   |   |+OU|
        //   *---*---*---*---*---*
        // Black: -
        {"3rk/3b1/4P/4R/4K b -", 1u},

        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |-OU|   |   |
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |+KI|+KA|   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |+OU|   |   |
        //   *---*---*---*---*---*
        // Black: -
        {"2k2/5/2GB1/5/2K2 b -", 3u},
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg), 1u);
}

TEST(test_dfpn3_searcher, test_minishogi_3ply_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |-KA|-OU|
        //   *---*---*---*---*---*
        // B |   |   |   |-GI|   |
        //   *---*---*---*---*---*
        // C |   |   |   |+KA|+FU|
        //   *---*---*---*---*---*
        // D |   |   |   |   |+RY|
        //   *---*---*---*---*---*
        // E |   |   |   |   |+OU|
        //   *---*---*---*---*---*
        // Black: HI
        {"3bk/3s1/3BP/4+R/4K b -", 3u},

        // Turn: White
        // White: KIx2
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |   |   |
        //   *---*---*---*---*---*
        // B |   |   |-FU|   |   |
        //   *---*---*---*---*---*
        // C |   |   |   |   |   |
        //   *---*---*---*---*---*
        // D |   |   |+OU|   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: -
        {"5/2p2/5/2K2/5 w 2g", 26u},

        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |-GI|-KI|-OU|
        //   *---*---*---*---*---*
        // B |   |   |   |   |   |
        //   *---*---*---*---*---*
        // C |   |   |   |+HI|+KI|
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |+OU|
        //   *---*---*---*---*---*
        // Black: HI
        {"2sgk/5/3RG/5/4K b R", 204u},
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg), 3u);
}

TEST(test_dfpn3_searcher, test_minishogi_5ply_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // DISCOVERED CHECK
        // Turn: BLACK
        // White: -
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |   |-KA|   |
        //   *---*---*---*---*---*
        // B |   |   |-HI|   |-OU|
        //   *---*---*---*---*---*
        // C |   |   |   |-FU|+KA|
        //   *---*---*---*---*---*
        // D |   |   |   |-KI|+HI|
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: KI
        {"3b1/2r1k/3pB/3gR/5 b G", 16u},

        // Turn: BLACK
        // White: FU
        //     5   4   3   2   1
        //   *---*---*---*---*---*
        // A |   |   |-FU|-OU|-KA|
        //   *---*---*---*---*---*
        // B |   |   |   |   |+HI|
        //   *---*---*---*---*---*
        // C |   |   |-UM|+KI|   |
        //   *---*---*---*---*---*
        // D |   |   |   |   |   |
        //   *---*---*---*---*---*
        // E |   |   |   |   |   |
        //   *---*---*---*---*---*
        // Black: GIx2
        {"2pkb/4R/2+bG1/5/5 b 2S", 387u},
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg), 5u);
}

TEST(test_dfpn3_searcher, test_minishogi_nply_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: WHITE
        // White: FU,GI,KA
        //     5   4   3   2   1
        //   +---+---+---+---+---+
        // A |   |   |+UM|   |-OU|
        //   +---+---+---+---+---+
        // B |   |   |   |-HI|   |
        //   +---+---+---+---+---+
        // C |   |   |   |+GI|-FU|
        //   +---+---+---+---+---+
        // D |+KI|   |+OU|   |   |
        //   +---+---+---+---+---+
        // E |   |   |   |-HI|   |
        //   +---+---+---+---+---+
        // Black: KI
        {"2+B1k/3r1/3Sp/G1K2/3r1 w Gbsp 30", 2228u},

        // Turn: BLACK
        // White: KI
        //     5   4   3   2   1
        //   +---+---+---+---+---+
        // A |   |   |   |   |-OU|
        //   +---+---+---+---+---+
        // B |-HI|   |   |   |-FU|
        //   +---+---+---+---+---+
        // C |   |   |-GI|   |   |
        //   +---+---+---+---+---+
        // D |+FU|+UM|   |+GI|   |
        //   +---+---+---+---+---+
        // E |+OU|   |   |   |+HI|
        //   +---+---+---+---+---+
        // Black: KA,KI
        {"4k/r3p/2s2/P+B1S1/K3R b BGg 17", 4599u},
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg));
}

} // namespace test_minishogi

namespace test_judkins_shogi
{

using namespace vshogi::judkins_shogi;

TEST(test_dfpn3_searcher, test_judkins_shogi_no_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: -
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // B |   |   |   |-KI|-GI|-HI|
        //   +---+---+---+---+---+---+
        // C |   |   |   |-KA|   |-KI|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+GI|+KA|-OU|
        //   +---+---+---+---+---+---+
        // E |   |   |   |+FU|   |   |
        //   +---+---+---+---+---+---+
        // F |   |   |   |   |+FU|+KE|
        //   +---+---+---+---+---+---+
        // Black: -
        {"6/3gsr/3b1g/3SBk/3P2/4PN b -", 8u},
    };
    for (auto&& arg : args)
        search_no_mate<Parameters>(std::get<0>(arg), std::get<1>(arg));
}

TEST(test_dfpn3_searcher, test_judkins_shogi_3ply_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: -
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |-GI|+GI|-HI|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // C |   |   |   |-FU|-OU|-KE|
        //   +---+---+---+---+---+---+
        // D |   |   |   |   |+KE|-FU|
        //   +---+---+---+---+---+---+
        // E |   |   |   |   |+RY|   |
        //   +---+---+---+---+---+---+
        // F |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // Black: KI
        {"3sSr/6/3pkn/4Np/4+R1/6 b G", 36u}, // was 46

        // Turn: BLACK
        // White: FU,KI,KA,HI
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |-GI|+GI|-HI|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // C |   |   |   |-FU|-OU|-KE|
        //   +---+---+---+---+---+---+
        // D |   |   |   |   |+KE|-FU|
        //   +---+---+---+---+---+---+
        // E |   |   |   |   |+RY|   |
        //   +---+---+---+---+---+---+
        // F |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // Black: KI
        {"3sSr/6/3pkn/4Np/4+R1/6 b Gpgbr", 20u}, // was 30
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg), 3u);
}

TEST(test_dfpn3_searcher, test_judkins_shogi_3ply_mate_by_king_move)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: FU
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |+HI|-KE|-HI|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |-OU|
        //   +---+---+---+---+---+---+
        // C |   |-UM|   |   |   |-FU|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+OU|   |   |
        //   +---+---+---+---+---+---+
        // E |   |   |+KA|   |   |-KI|
        //   +---+---+---+---+---+---+
        // F |   |   |   |-KI|   |   |
        //   +---+---+---+---+---+---+
        // Black: -
        {"3Rnr/5k/1+b3p/3K2/2B2g/3g2 b p", 4u}, // was 5

        // COUNTER CHECK
        // Turn: BLACK
        // White: KE
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |+HI|-KE|-HI|
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |-OU|
        //   +---+---+---+---+---+---+
        // C |   |-UM|   |   |   |-FU|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+OU|   |   |
        //   +---+---+---+---+---+---+
        // E |   |   |+KA|   |   |   |
        //   +---+---+---+---+---+---+
        // F |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // Black: -
        {"3Rnr/5k/1+b3p/3K2/2B3/6 b n", 4u}, // was 6
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg), 3u);
}

TEST(test_dfpn3_searcher, test_judkins_shogi_5ply_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: -
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |   |-FU|-FU|
        //   +---+---+---+---+---+---+
        // B |   |   |   |-FU|-KA|-OU|
        //   +---+---+---+---+---+---+
        // C |   |   |   |-KE|   |+KE|
        //   +---+---+---+---+---+---+
        // D |   |   |   |+FU|   |+FU|
        //   +---+---+---+---+---+---+
        // E |   |   |+GI|   |+GI|+HI|
        //   +---+---+---+---+---+---+
        // F |   |   |   |+KI|+HI|+KI|
        //   +---+---+---+---+---+---+
        // Black: KA
        {"4pp/3pbk/3n1N/3P1P/2S1SR/3GRG b B", 5u},
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg), 5u);
}

TEST(test_dfpn3_searcher, test_judkins_shogi_2ply_mate_by_king_entering)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: WHITE
        // White: GI
        //     6   5   4   3   2   1
        //   +---+---+---+---+---+---+
        // A |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // B |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // C |   |   |   |-KE|   |   |
        //   +---+---+---+---+---+---+
        // D |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // E |+FU|   |   |   |-KI|-RY|
        //   +---+---+---+---+---+---+
        // F |   |+OU|   |-OU|-TO|-UM|
        //   +---+---+---+---+---+---+
        // Black: -
        {"6/6/3n2/6/P3g+r/1K1k+p+b w s", 7u},
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg), 2u);
}

} // namespace test_judkins_shogi

namespace test_shogi
{

using namespace vshogi::shogi;

TEST(test_dfpn3_searcher, test_shogi_no_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: -
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |-KY|-KE|   |   |   |   |   |-KE|-KY|
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |   |-HI|-KI|-OU|-KI|-GI|   |
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |-FU|-FU|   |-FU|-FU|   |-FU|-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // D |-FU|   |   |   |-GI|   |-FU|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |+FU|   |   |   |+FU|-KA|
        //   +---+---+---+---+---+---+---+---+---+
        // F |+FU|   |+FU|   |+FU|   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |+FU|+KA|+GI|+GI|+FU|+FU|   |+FU|
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |+KI|   |+KI|   |   |+HI|   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |+KY|+KE|   |   |+OU|   |   |+KE|+KY|
        //   +---+---+---+---+---+---+---+---+---+
        // Black: FU
        {"ln5nl/3rgkgs1/1pp1pp1pp/p3s1p2/3P3Pb/P1P1P4/1PBSSPP1P/2G1G2R1/"
         "LN2K2NL b P 29",
         4u},

        // Turn: BLACK
        // White: FUx10
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |+RY|   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |   |   |   |   |-FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |   |   |   |   |-KI|-OU|
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |   |   |   |   |   |   |+FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |   |   |   |   |   |+KI|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |   |   |   |   |   |+KE|   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // Black: -
        {"+R8/9/9/7p1/7gk/7P1/6G2/7N1/9 b 10p", 6u},

        // Turn: WHITE
        // White: FUx3,KY,KE,GI
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |-KY|   |   |   |   |   |-KI|   |-KY|
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |   |   |   |   |-KI|-OU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |-FU|   |   |   |   |-KE|-FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |+RY|   |-FU|-FU|   |-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |-FU|   |-FU|   |   |+FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |-FU|   |   |   |   |+KE|+FU|   |+FU|
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |+FU|   |+GI|   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |+FU|+KI|   |+GI|+KA|   |-TO|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |   |+KE|+OU|   |+KA|   |   |-RY|+KY|
        //   +---+---+---+---+---+---+---+---+---+
        // Black: FU,GI,KI
        {"l5g1l/6gk1/1p4np1/3+R1pp1p/2p1p2P1/p4NP1P/1P1S5/PG1SB1+p2/1NK1B2+rL "
         "w GSPsnl3p 134",
         1412u},
    };

    for (auto&& arg : args)
        search_no_mate<Parameters>(std::get<0>(arg), std::get<1>(arg));
}

TEST(test_dfpn3_searcher, test_shogi_1ply_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: FUx3,KI
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |   |-KA|   |-KE|   |   |   |   |-OU|
        //   +---+---+---+---+---+---+---+---+---+
        // B |-KY|   |-GI|   |   |   |-FU|   |-GI|
        //   +---+---+---+---+---+---+---+---+---+
        // C |-FU|-HI|-FU|   |   |-FU|-KE|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |-FU|   |+UM|-FU|   |   |   |+HI|
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |   |+FU|   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |   |   |   |   |   |+KI|-FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |+FU|+FU|+FU|+FU|   |+FU|+KE|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |   |   |+OU|+KI|+GI|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |+KY|+KE|+GI|   |   |+KI|   |   |+KY|
        //   +---+---+---+---+---+---+---+---+---+
        // Black: FUx2,KY
        {"1b1n4k/l1s3p1s/prp2pn2/1p1+Bp3R/4P4/6Gp1/PPPP1PN2/4KGS2/"
         "LNS2G2L b L2Pg3p",
         1u},
        // Turn: BLACK
        // White: KE,GI,KA,HI,KI
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |   |   |   |   |   |   |   |-KA|-OU|
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |   |   |   |   |   |-FU|-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |   |+KA|   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // Black: KY,KE
        {"7bk/7pp/9/9/4B4/9/9/9/9 b NLrbgsn", 1u},
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg), 1u);
}

TEST(test_dfpn3_searcher, test_shogi_nply_mate)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: WHITE
        // White: FUx5,GI
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |-HI|   |   |   |   |   |-FU|+KI|   |
        //   +---+---+---+---+---+---+---+---+---+
        // B |-KY|   |   |-FU|-GI|   |-KI|-FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // C |-KE|   |-OU|   |-FU|   |-GI|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |-FU|   |-KA|+FU|   |   |-KY|
        //   +---+---+---+---+---+---+---+---+---+
        // E |-FU|   |   |   |+HI|   |   |+KE|+FU|
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |   |   |+KI|   |-KE|+FU|-KY|   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |+FU|-FU|   |+KI|   |   |+KE|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |+KY|   |   |-KA|   |+GI|   |   |+OU|
        //   +---+---+---+---+---+---+---+---+---+
        // I |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // Black: FUx2
        {"r5pG1/l2ps1gp1/n1k1p1s2/2p1bP2l/p3R2NP/3G1nPl1/Pp1G2N2/"
         "L2b1S2K/9 w 2Ps5p 162",
         48u},

        // Turn: WHITE
        // White: FUx2,KEx2,GI
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |-KY|   |   |   |   |-KI|   |   |-KY|
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |-GI|-OU|   |   |   |-GI|-KA|   |
        //   +---+---+---+---+---+---+---+---+---+
        // C |-FU|-FU|-FU|-FU|   |-FU|-FU|   |-KE|
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |   |   |   |   |-KY|-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |+FU|   |   |   |+FU|   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |   |+FU|+FU|+KI|+FU|+FU|+FU|+FU|
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |-RY|   |+KI|   |+OU|+KI|+GI|   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |   |-RY|   |+KA|   |   |   |+KE|+KY|
        //   +---+---+---+---+---+---+---+---+---+
        // Black: FU
        {"l4g2l/1sk3sb1/pppp1pp1n/7lp/9/P3P4/2PPGPPPP/1+r1G1KGS1/"
         "1+r1B3NL w Ps2n2p 72",
         351u},

        // Turn: WHITE
        // White: FU,KY,KIx2
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |-KY|-KE|-GI|-KI|-OU|-KI|-GI|-KE|-KY|
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |   |   |   |   |   |-KA|   |
        //   +---+---+---+---+---+---+---+---+---+
        // C |-FU|+TO|-FU|-FU|-FU|-FU|-FU|-FU|-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |   |   |+KA|   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |   |   |   |   |+FU|   |+FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |+FU|   |+FU|+FU|+FU|+OU|+FU|   |+FU|
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |   |-RY|+GI|   |   |   |+KY|
        //   +---+---+---+---+---+---+---+---+---+
        // I |-RY|+KE|   |   |   |   |-GI|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // Black: KE
        {"lnsgkgsnl/7b1/p+Pppppppp/9/5B3/5P1P1/P1PPPKP1P/3+rS3L/"
         "+rN4s2 w N2glp 44",
         486u},

        // Turn: WHITE
        // White: FUx4,KE,KIx2
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |-KY|-KE|-GI|   |   |   |-KI|-KE|   |
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |-KI|   |-OU|   |   |-KA|-KY|
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |   |-FU|-FU|-FU|-FU|-FU|-GI|   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |+FU|   |   |   |   |   |   |   |-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |+KE|   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |+OU|+FU|   |   |+KA|+HI|   |+FU|
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |+FU|   |+FU|+FU|+FU|+FU|   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |+KY|   |+GI|   |-RY|   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |   |   |   |   |-GI|   |   |   |+KY|
        //   +---+---+---+---+---+---+---+---+---+
        // Black: -
        {"lns3gn1/2g1k2bl/2ppppps1/P7p/3N5/1KP2BR1P/1P1PPPP2/L1S1+r4/"
         "4s3L w 2gn4p 74",
         1629u},
    };

    for (auto&& arg : {args[2]})
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg));
}

TEST(test_dfpn3_searcher, test_shogi_unnecessary_interposition)
{
    const std::vector<std::tuple<std::string, vshogi::uint>> args = {
        // Turn: BLACK
        // White: FUx10
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |+RY|   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |   |   |   |   |-FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |   |   |   |   |-KI|-OU|
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |   |   |   |   |   |   |+KE|   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |   |   |   |   |   |   |   |+FU|   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |   |   |   |   |   |+KE|   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // Black: -
        {"+R8/9/9/7p1/7gk/7N1/7P1/7N1/9 b 10p", 20u},

        // Turn: BLACK
        // White: FUx10,KYx4,KEx3,GIx4,KA,HIx2,KIx4
        //     9   8   7   6   5   4   3   2   1
        //   +---+---+---+---+---+---+---+---+---+
        // A |   |   |   |   |   |   |   |-KA|-OU|
        //   +---+---+---+---+---+---+---+---+---+
        // B |   |   |   |   |   |   |   |   |-FU|
        //   +---+---+---+---+---+---+---+---+---+
        // C |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // D |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // E |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // F |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // G |+KA|   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // H |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // I |   |   |   |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+---+---+---+
        // Black: KE
        {"7bk/8p/9/9/9/9/B8/9/9 b N10p4l3n4s4gb2r", 5596u},
    };

    for (auto&& arg : args)
        search_mate<Parameters>(std::get<0>(arg), std::get<1>(arg));
}

TEST(test_dfpn3_searcher, test_shogi_avoid_consecutive_checks_1)
{
    // Turn: WHITE
    // White: GI
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // C |-FU|-FU|   |   |+KA|-OU|   |-FU|+FU|
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |+FU|+FU|   |+FU|+FU|+FU|+FU|+GI|+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // H |+KY|   |-UM|   |   |+OU|+KI|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |+KE|   |-HI|-KI|   |   |+KE|   |
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FUx3
    auto g = Game("9/9/pp2Bk1pP/9/9/9/PP1PPPPSL/L1+b3G2/1N3K1N1 w 3Prgs 44");
    auto searcher = dfpn::Searcher<Parameters>();
    searcher.search(g, 10000);
    CHECK_TRUE(searcher.proved_mate());
    const auto moves = searcher.get_mate_moves(g);
    CHECK_EQUAL(0u, g.ply());
    for (auto&& m : moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());
}

TEST(test_dfpn3_searcher, test_shogi_avoid_consecutive_checks_2)
{
    // Turn: WHITE
    // White: FU,HI
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |   |-KE|-GI|   |   |-OU|   |-KE|   |
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |   |   |   |-KA|   |-KE|-KY|
    //   +---+---+---+---+---+---+---+---+---+
    // C |   |   |   |   |   |-FU|   |   |-FU|
    //   +---+---+---+---+---+---+---+---+---+
    // D |+KI|   |   |   |   |   |-KI|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // E |+FU|+FU|   |-FU|-FU|+FU|-FU|+FU|-GI|
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |+FU|+FU|+GI|+FU|+KY|+FU|
    //   +---+---+---+---+---+---+---+---+---+
    // G |   |   |+KI|   |   |   |+KE|   |+KI|
    //   +---+---+---+---+---+---+---+---+---+
    // H |+KY|-NG|   |   |+OU|   |   |   |+KY|
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |   |-RY|   |   |   |   |   |+KA|
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FUx4
    auto g = Game("1ns2k1n1/5b1nl/5p2p/G5g2/PP1ppPpPs/3PPSPLP/2G3N1G/L+s2K3L/"
                  "2+r5B w 4Prp");
    auto searcher = dfpn::Searcher<Parameters>();
    searcher.search(g, 100u);
    CHECK_TRUE(searcher.proved_mate());
    const auto moves = searcher.get_mate_moves(g);
    for (auto&& m : moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());
}

TEST(test_dfpn3_searcher, test_shogi_debug)
{
    // Turn: BLACK
    // White: FUx10,HIx2,KIx4
    //     9   8   7   6   5   4   3   2   1
    //   +---+---+---+---+---+---+---+---+---+
    // A |   |   |   |   |   |   |+TO|   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // B |   |   |   |   |   |   |   |   |-OU|
    //   +---+---+---+---+---+---+---+---+---+
    // C |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // D |   |   |   |   |   |   |   |+GI|+KA|
    //   +---+---+---+---+---+---+---+---+---+
    // E |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // F |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // G |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // H |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // I |   |   |   |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+---+---+---+
    // Black: FU
    auto g = Game("6+P2/8k/9/7SB/9/9/9/9/9 b P10p4g2r");
    auto searcher = dfpn::Searcher<Parameters>();
    searcher.search(g, 10000u);
    CHECK_TRUE(searcher.proved_mate());
    const auto moves = searcher.get_mate_moves(g);
    for (auto&& m : moves) {
        CHECK_EQUAL(vshogi::ONGOING, g.get_result());
        g.apply(m);
    }
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
}

} // namespace test_shogi

} // namespace test_vshogi::test_engine::test_dfpn
