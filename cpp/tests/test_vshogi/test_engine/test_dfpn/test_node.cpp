#include <set>

#include "vshogi/engine/dfpn/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn::test_node
{

using namespace vshogi::minishogi;
using MT = MoveTraits;
using move_t = vshogi::move_t;
namespace dfpn = vshogi::engine::dfpn;
using Node = dfpn::Node;
constexpr uint zero = dfpn::zero;
constexpr uint cent = dfpn::cent;
constexpr uint unit = dfpn::unit;
constexpr uint kilo = dfpn::kilo;
constexpr uint inf = dfpn::inf;

TEST_GROUP (dfpn_node_simulate) {
};

TEST(dfpn_node_simulate, using_game_ongoing)
{
    auto g = Game();
    auto n = Node();
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.simulate(g));
    CHECK_FALSE(n.proved());
}

TEST(dfpn_node_simulate, using_game_draw)
{
    auto g = Game("4k/5/5/5/4K b -");
    g.apply(MT::make_move(SQ_1E, SQ_1D))
        .apply(MT::make_move(SQ_1A, SQ_1B))
        .apply(MT::make_move(SQ_1D, SQ_1E))
        .apply(MT::make_move(SQ_1B, SQ_1A));

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn(true));
    CHECK_EQUAL(zero, n.dn(true));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate(true));
    CHECK_TRUE(n.proved_no_mate(true));
}

TEST(dfpn_node_simulate, offence_won_at_offence_turn)
{
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(zero, n.pn(true));
    CHECK_EQUAL(inf, n.dn(true));
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate(true));
    CHECK_FALSE(n.proved_no_mate(true));
}

TEST(dfpn_node_simulate, offence_won_at_defence_turn)
{
    const auto m = MT::make_move(KI, SQ_1B);
    auto g = Game("4k/5/4P/5/5 b G").apply(m);
    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(zero, n.pn(false));
    CHECK_EQUAL(inf, n.dn(false));
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate(false));
    CHECK_FALSE(n.proved_no_mate(false));
}

TEST(dfpn_node_simulate, defence_won_at_offence_turn)
{
    auto g = Game("5/5/3gk/5/4K b P")
                 .apply(MT::make_move("P*1d"))
                 .apply(MT::make_move(SQ_2C, SQ_1D));
    auto n = Node();

    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn(true));
    CHECK_EQUAL(zero, n.dn(true));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate(true));
    CHECK_TRUE(n.proved_no_mate(true));
}

TEST(dfpn_node_simulate, defence_won_at_defence_turn)
{
    auto g = Game("4k/5/4S/5/4K b -");
    g.apply(MT::make_move(SQ_1C, SQ_1B))
        .apply(MT::make_move(SQ_1A, SQ_2B))
        .apply(MT::make_move(SQ_1B, SQ_2C))
        .apply(MT::make_move(SQ_2B, SQ_1A))
        .apply(MT::make_move(SQ_2C, SQ_1B));

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn(false));
    CHECK_EQUAL(zero, n.dn(false));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate(false));
    CHECK_TRUE(n.proved_no_mate(false));
}

TEST(dfpn_node_simulate, using_offence_twin_l_mate)
{
    auto node_l = Node();
    node_l.init(nullptr, static_cast<move_t>(0), zero, inf);
    CHECK_TRUE(node_l.proved_mate(true));
    auto g = Game();
    auto n = Node();
    CHECK_TRUE(n.simulate(g, nullptr, &node_l));
    CHECK_EQUAL(zero, n.pn(true));
    CHECK_EQUAL(inf, n.dn(true));
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate(true));
    CHECK_FALSE(n.proved_no_mate(true));
}

TEST(dfpn_node_simulate, using_offence_twin_l_no_mate)
{
    auto node_l = Node();
    node_l.init(nullptr, static_cast<move_t>(0), inf, zero);
    CHECK_TRUE(node_l.proved_no_mate(true));
    auto g = Game();
    auto n = Node();
    CHECK_FALSE(n.simulate(g, nullptr, &node_l));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate(true));
    CHECK_FALSE(n.proved_no_mate(true));
}

TEST(dfpn_node_simulate, using_defence_twin_l_mate)
{
    auto node_l = Node();
    node_l.init(nullptr, static_cast<move_t>(0), inf, zero);
    CHECK_TRUE(node_l.proved_mate(false));
    auto g = Game();
    auto n = Node();
    g.apply(MT::make_move(SQ_1E, SQ_1D));
    CHECK_FALSE(n.simulate(g, nullptr, &node_l));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate(false));
    CHECK_FALSE(n.proved_no_mate(false));
}

TEST(dfpn_node_simulate, using_defence_twin_l_no_mate)
{
    auto node_l = Node();
    node_l.init(nullptr, static_cast<move_t>(0), zero, inf);
    CHECK_TRUE(node_l.proved_no_mate(false));
    auto g = Game();
    auto n = Node();
    g.apply(MT::make_move(SQ_1E, SQ_1D));
    CHECK_TRUE(n.simulate(g, nullptr, &node_l));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate(false));
    CHECK_TRUE(n.proved_no_mate(false));
}

TEST(dfpn_node_simulate, using_offence_twin_g_mate)
{
    auto twin = Node();
    twin.init(nullptr, static_cast<move_t>(0), zero, inf);
    CHECK_TRUE(twin.proved_mate(true));
    auto g = Game();
    auto n = Node();
    CHECK_FALSE(n.simulate(g, &twin, nullptr));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate(true));
    CHECK_FALSE(n.proved_no_mate(true));
}

TEST(dfpn_node_simulate, using_offence_twin_g_no_mate)
{
    auto twin = Node();
    twin.init(nullptr, static_cast<move_t>(0), inf, zero);
    CHECK_TRUE(twin.proved_no_mate(true));
    auto g = Game();
    auto n = Node();
    CHECK_TRUE(n.simulate(g, &twin, nullptr));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate(true));
    CHECK_TRUE(n.proved_no_mate(true));
}

TEST(dfpn_node_simulate, using_defence_twin_g_mate)
{
    auto twin = Node();
    twin.init(nullptr, static_cast<move_t>(0), inf, zero);
    CHECK_TRUE(twin.proved_mate(false));
    auto g = Game();
    auto n = Node();
    g.apply(MT::make_move(SQ_1E, SQ_1D));
    CHECK_TRUE(n.simulate(g, &twin, nullptr));
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate(false));
    CHECK_FALSE(n.proved_no_mate(false));
}

TEST(dfpn_node_simulate, using_defence_twin_g_no_mate)
{
    auto twin = Node();
    twin.init(nullptr, static_cast<move_t>(0), zero, inf);
    CHECK_TRUE(twin.proved_no_mate(false));
    auto g = Game();
    auto n = Node();
    g.apply(MT::make_move(SQ_1E, SQ_1D));
    CHECK_FALSE(n.simulate(g, &twin, nullptr));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate(false));
    CHECK_FALSE(n.proved_no_mate(false));
}

TEST_GROUP (dfpn_node_expand) {
};

TEST(dfpn_node_expand, offence_no_twins)
{
    {
        vshogi::engine::ContiguousBuffer<Node> buffer{10u};
        auto& n = buffer.front();
        CHECK_FALSE(n.fully_expanded());
        n.expand(buffer, Game("2B1k/5/3P1/3GK/4R b P"));
        CHECK_TRUE(n.fully_expanded());
        std::set<uint16_t> moves;
        for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
            moves.insert(ch->get_action());
            if (ch->get_action() == MT::make_move(SQ_3A, SQ_2B, false)) {
                CHECK_EQUAL(kilo, ch->pn(false));
                CHECK_EQUAL(cent, ch->dn(false));
            } else {
                CHECK_EQUAL(unit, ch->pn(false));
                CHECK_EQUAL(unit, ch->dn(false));
            }
        }
        CHECK_TRUE(
            std::set<uint16_t>({
                MT::make_move(SQ_1D, SQ_2E), // king
                MT::make_move(SQ_3A, SQ_2B, false), // soldier
                MT::make_move(SQ_3A, SQ_2B, true), // soldier
                MT::make_move(FU, SQ_1B), // drop
            })
            == moves);
    }
    {
        vshogi::engine::ContiguousBuffer<Node> buffer{3u};
        CHECK_EQUAL(3u, buffer.remaining());
        auto& n = buffer.front();
        CHECK_FALSE(n.fully_expanded());
        n.expand(buffer, Game("2B1k/5/3P1/3GK/4R b P")); // 4 check moves
        CHECK_FALSE(n.fully_expanded());
        CHECK_TRUE(buffer.is_full());
        CHECK_EQUAL(0u, buffer.remaining());
    }
    {
        vshogi::engine::ContiguousBuffer<Node> buffer{4u};
        CHECK_EQUAL(4u, buffer.remaining());
        auto& n = buffer.front();
        CHECK_FALSE(n.fully_expanded());
        n.expand(buffer, Game("2B1k/5/3P1/3GK/4R b P")); // 4 check moves
        CHECK_TRUE(n.fully_expanded());
        CHECK_TRUE(buffer.is_full());
        CHECK_EQUAL(0u, buffer.remaining());
    }
}

TEST(dfpn_node_expand, offence_twin_g)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto twin_g = Node();
    twin_g.expand(buffer, Game("2B1k/5/4p/5/5 b RP"));
    auto& n = buffer.front();
    CHECK_FALSE(n.fully_expanded());
    n.expand(buffer, Game("2B1k/5/4p/5/5 b P"), &twin_g);
    CHECK_TRUE(n.fully_expanded());

    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        moves.insert(ch->get_action());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            MT::make_move(SQ_3A, SQ_2B, false), // soldier
            MT::make_move(SQ_3A, SQ_2B, true), // soldier
            MT::make_move(FU, SQ_1B), // drop
        })
        == moves);
}

TEST(dfpn_node_expand, offence_twin_l)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto twin_l = Node();
    twin_l.expand(buffer, Game("2B1k/5/4p/5/5 b P"));
    auto& n = buffer.front();
    CHECK_FALSE(n.fully_expanded());
    n.expand(buffer, Game("2B1k/5/4p/5/4R b SP"), nullptr, &twin_l);
    CHECK_TRUE(n.fully_expanded());

    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        moves.insert(ch->get_action());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            MT::make_move(SQ_3A, SQ_2B, false), // soldier
            MT::make_move(SQ_3A, SQ_2B, true), // soldier
            MT::make_move(FU, SQ_1B), // drop
            MT::make_move(GI, SQ_1B), // drop
            MT::make_move(GI, SQ_2B), // drop
            // note that there is no MT::make_move(SQ_1E, SQ_1C) here!
        })
        == moves);
}

TEST(dfpn_node_expand, defence_no_twins)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto& n = buffer.front();
    CHECK_FALSE(n.fully_expanded());
    {
        auto g = Game("3gk/5/R4/5/4K b p");
        g.apply(MT::make_move(SQ_5C, SQ_1C));
        n.expand(buffer, g);
    }
    CHECK_TRUE(n.fully_expanded());

    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        moves.insert(ch->get_action());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            MT::make_move(SQ_1A, SQ_2B), // king
            MT::make_move(SQ_2A, SQ_1B), // board
            MT::make_move(FU, SQ_1B), // drop
        })
        == moves);
}

TEST(dfpn_node_expand, defence_twin_l)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto twin_l = Node();
    {
        auto g = Game("3gk/5/R4/5/4K b p");
        g.apply(MT::make_move(SQ_5C, SQ_1C));
        twin_l.expand(buffer, g);
    }
    auto& n = buffer.front();
    CHECK_FALSE(n.fully_expanded());
    {
        auto g = Game("3gk/5/R4/5/4K b ps");
        g.apply(MT::make_move(SQ_5C, SQ_1C));
        n.expand(buffer, g, nullptr, &twin_l);
    }
    CHECK_TRUE(n.fully_expanded());
    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        moves.insert(ch->get_action());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            MT::make_move(SQ_1A, SQ_2B), // king
            MT::make_move(SQ_2A, SQ_1B), // board
            MT::make_move(FU, SQ_1B), // drop
            MT::make_move(GI, SQ_1B), // drop
        })
        == moves);
}

TEST(dfpn_node_expand, defence_twin_g)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto twin_g = Node();
    {
        auto g = Game("3gk/5/R4/5/4K b ps");
        g.apply(MT::make_move(SQ_5C, SQ_1C));
        twin_g.expand(buffer, g);
    }
    auto& n = buffer.front();
    CHECK_FALSE(n.fully_expanded());
    {
        auto g = Game("3gk/5/R4/5/4K b s");
        g.apply(MT::make_move(SQ_5C, SQ_1C));
        n.expand(buffer, g, &twin_g);
    }
    CHECK_TRUE(n.fully_expanded());
    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        moves.insert(ch->get_action());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            MT::make_move(SQ_1A, SQ_2B), // king
            MT::make_move(SQ_2A, SQ_1B), // board
            MT::make_move(GI, SQ_1B), // drop
        })
        == moves);
}

TEST(dfpn_node_expand, defence_partial_expansion)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto g = Game("3gk/5/5/5/R4 b 2p2s");
    g.apply(MT::make_move(SQ_5E, SQ_1E))
        .apply(MT::make_move(FU, SQ_1D))
        .apply(MT::make_move(SQ_1E, SQ_1D))
        .apply(MT::make_move(FU, SQ_1C))
        .apply(MT::make_move(SQ_1D, SQ_1C));
    auto& n = buffer.front();
    CHECK_FALSE(n.fully_expanded());
    n.expand(buffer, g);
    CHECK_FALSE(n.fully_expanded());
    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        moves.insert(ch->get_action());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            MT::make_move(SQ_1A, SQ_2B), // king
            MT::make_move(SQ_2A, SQ_1B), // board
            // MT::make_move(GI, SQ_1B), // drop
        })
        == moves);
}

TEST_GROUP (dfpn_node_backprop) {
};

TEST(dfpn_node_backprop, offence_preference)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto g = Game("2B1k/5/5/5/5 b P");
    auto& n = buffer.front();
    n.expand(buffer, g);
    n.backprop<Parameters>(true);
    CHECK_EQUAL(unit, n.pn(true));
    CHECK_EQUAL(2u * unit + cent, n.dn(true));

    // Prefer drop move over board move.
    CHECK_EQUAL(MT::make_move(FU, SQ_1B), n.get_child_1st()->get_action());

    // Prefer promotion move over no-promotion move.
    CHECK_EQUAL(
        MT::make_move(SQ_3A, SQ_2B, true), n.get_child_2nd()->get_action());
}

TEST(dfpn_node_backprop, offence_with_proved_child)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto g = Game("3rk/3p1/4P/5/4K b G");
    auto& n = buffer.front();
    n.expand(buffer, g);
    n.backprop<Parameters>(true);
    CHECK_FALSE(n.proved());
    CHECK_EQUAL(unit, n.pn(true));
    CHECK_EQUAL(2u * unit, n.dn(true));

    uint th_p_ch, th_d_ch;
    auto ch = n.select(inf, inf, th_p_ch, th_d_ch); // MT::make_move(KI, SQ_1B)
    CHECK_EQUAL(MT::make_move(KI, SQ_1B),
                ch->get_action()); // prefer drop
    g.apply(ch->get_action());
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
    ch->simulate(g);
    CHECK_TRUE(ch->proved_mate(false));

    n.backprop<Parameters>(true);
    CHECK_TRUE(n.proved_mate(true));
    CHECK_EQUAL(zero, n.pn(true));
    CHECK_EQUAL(inf, n.dn(true));
}

TEST(dfpn_node_backprop, defence_preference)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    // auto g = Game("s4/RR3/5/5/k4 w p");
    auto g = Game("s4/1R3/5/5/k4 b Rp");
    auto& n = buffer.front();
    g.apply(MT::make_move(HI, SQ_5B));
    n.expand(buffer, g);
    n.backprop<Parameters>(g.get_state().find_checker_square());

    // Prefer capture move
    CHECK_EQUAL(MT::make_move(SQ_5A, SQ_5B), n.get_child_1st()->get_action());

    // Prefer drop near by king than drop far from king.
    CHECK_EQUAL(MT::make_move(FU, SQ_5D), n.get_child_2nd()->get_action());
}

TEST(dfpn_node_backprop, defence_with_proved_child)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto g = Game("4k/5/4P/5/5 b -");
    g.apply(MT::make_move(SQ_1C, SQ_1B));
    auto& n = buffer.front();
    n.expand(buffer, g);
    n.backprop<Parameters>(false);

    uint th_p_ch, th_d_ch;
    const auto ch = n.select(inf, inf, th_p_ch, th_d_ch);
    CHECK_EQUAL(MT::make_move(SQ_1A, SQ_1B), ch->get_action());
    g.apply_dfpn(ch->get_action());
    ch->expand(buffer, g);
    ch->backprop<Parameters>(true);
    CHECK_TRUE(ch->proved_no_mate(true));
    g.undo();

    n.backprop<Parameters>(false);
    CHECK_TRUE(ch->proved_no_mate(true));
}

TEST(dfpn_node_backprop, offence_proved_by_repetitions)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto g = Game("4k/5/4S/5/5 b -");
    g.apply(MT::make_move(SQ_1C, SQ_2B))
        .apply(MT::make_move(SQ_1A, SQ_1B))
        .apply(MT::make_move(SQ_2B, SQ_1C))
        .apply(MT::make_move(SQ_1B, SQ_1A));
    g.apply(MT::make_move(SQ_1C, SQ_1B))
        .apply(MT::make_move(SQ_1A, SQ_2B))
        .apply(MT::make_move(SQ_1B, SQ_1A))
        .apply(MT::make_move(SQ_2B, SQ_2A))
        .apply(MT::make_move(SQ_1A, SQ_2B))
        .apply(MT::make_move(SQ_2A, SQ_1B))
        .apply(MT::make_move(SQ_2B, SQ_1C))
        .apply(MT::make_move(SQ_1B, SQ_1A));
    auto& n = buffer.front();
    n.expand(buffer, g);
    n.backprop<Parameters>(true);

    uint th_p_ch, th_d_ch;
    auto c = n.select(inf, inf, th_p_ch, th_d_ch);
    g.apply_dfpn(c->get_action());
    CHECK_TRUE(c->simulate(g)); // repetition
    CHECK_TRUE(c->proved_no_mate(false));
    g.undo();
    n.backprop<Parameters>(true);
    CHECK_FALSE(n.proved());
    c = n.select(inf, inf, th_p_ch, th_d_ch);
    g.apply_dfpn(c->get_action());
    CHECK_TRUE(c->simulate(g)); // repetition
    CHECK_TRUE(c->proved_no_mate(false));
    g.undo();
    n.backprop<Parameters>(true);
    CHECK_TRUE(n.proved_no_mate(true));
}

TEST(dfpn_node_backprop, defence_proved_by_repetitions)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto g = Game("4k/5/4P/5/5 b S");
    g.apply(MT::make_move("S*1b"))
        .apply(MT::make_move(SQ_1A, SQ_2B))
        .apply(MT::make_move(SQ_1B, SQ_2C))
        .apply(MT::make_move(SQ_2B, SQ_1A))
        .apply(MT::make_move(SQ_2C, SQ_1B));
    auto& n = buffer.front();
    n.expand(buffer, g);
    n.backprop<Parameters>(g.get_state().find_checker_square());

    uint th_p_ch, th_d_ch;
    auto c = n.select(inf, inf, th_p_ch, th_d_ch);
    g.apply_dfpn(c->get_action());
    CHECK_TRUE(c->simulate(g)); // repetition
    CHECK_TRUE(c->proved_no_mate(true));
    g.undo();
    n.backprop<Parameters>(g.get_state().find_checker_square());
    CHECK_TRUE(n.proved_no_mate(false));
}

TEST_GROUP (dfpn_node_select) {
};

TEST(dfpn_node_select, offence_single_child)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    {
        auto& n = buffer.front();
        n.expand(buffer, Game("4k/5/4P/5/5 b -"));
        n.backprop<Parameters>(true);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(inf, inf, th_p_ch, th_d_ch);
        CHECK_EQUAL(inf, th_p_ch);
        CHECK_EQUAL(inf, th_d_ch);
        CHECK_EQUAL(MT::make_move(SQ_1C, SQ_1B), c->get_action());
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
    {
        auto& n = buffer.front();
        n.expand(buffer, Game("4k/5/4P/5/5 b -"));
        n.backprop<Parameters>(true);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(25, 10, th_p_ch, th_d_ch);
        CHECK_EQUAL(10, th_p_ch);
        CHECK_EQUAL(25, th_d_ch);
        CHECK_EQUAL(MT::make_move(SQ_1C, SQ_1B), c->get_action());
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
}

TEST(dfpn_node_select, offence_multiple_child)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    {
        auto& n = buffer.front();
        n.expand(buffer, Game("4k/5/5/5/5 b S"));
        n.backprop<Parameters>(true);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(inf, inf, th_p_ch, th_d_ch);
        CHECK_EQUAL(inf - unit, th_p_ch);
        CHECK_EQUAL(unit + 1u, th_d_ch);
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
    {
        // offence prefers promotion
        auto& n = buffer.front();
        n.expand(buffer, Game("2B1k/5/5/5/5 b -"));
        n.backprop<Parameters>(true);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(inf, inf, th_p_ch, th_d_ch);
        CHECK_EQUAL(MT::make_move(SQ_3A, SQ_2B, true), c->get_action());
    }
}

TEST(dfpn_node_select, offence_with_disproved_child)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    auto g = Game("4k/5/4G/5/5 b -");
    auto& n = buffer.front();
    n.expand(buffer, g);
    n.backprop<Parameters>(true);

    uint th_p_ch, th_d_ch;
    auto c = n.select(inf, inf, th_p_ch, th_d_ch);
    g.apply_dfpn(c->get_action());
    auto twin_l = Node();
    twin_l.init(nullptr, static_cast<move_t>(0), zero, inf);
    c->simulate(g, nullptr, &twin_l);
    CHECK_TRUE(c->proved_no_mate(false));
    g.undo();

    n.backprop<Parameters>(true);
    c = n.select(inf, inf, th_p_ch, th_d_ch);
    CHECK_EQUAL(inf, th_p_ch);
    CHECK_EQUAL(inf, th_d_ch);
}

TEST(dfpn_node_select, defence_single_child)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    {
        auto g = Game("4k/5/4G/5/5 b -");
        auto& n = buffer.front();
        g.apply(MT::make_move(SQ_1C, SQ_1B));
        n.expand(buffer, g);
        n.backprop<Parameters>(false);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(inf, inf, th_p_ch, th_d_ch);
        CHECK_EQUAL(inf, th_p_ch);
        CHECK_EQUAL(inf, th_d_ch);
        CHECK_EQUAL(MT::make_move(SQ_1A, SQ_1B), c->get_action());
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
    {
        auto g = Game("4k/5/4G/5/5 b -");
        auto& n = buffer.front();
        g.apply(MT::make_move(SQ_1C, SQ_1B));
        n.expand(buffer, g);
        n.backprop<Parameters>(false);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(25, 10, th_p_ch, th_d_ch);
        CHECK_EQUAL(10, th_p_ch);
        CHECK_EQUAL(25, th_d_ch);
        CHECK_EQUAL(MT::make_move(SQ_1A, SQ_1B), c->get_action());
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
}

TEST(dfpn_node_select, defence_multiple_child)
{
    vshogi::engine::ContiguousBuffer<Node> buffer{100u};
    {
        auto g = Game("5/1+R3/1r3/5/k4 b -");
        auto& n = buffer.front();
        g.apply(MT::make_move(SQ_4B, SQ_5C));
        n.expand(buffer, g);
        n.backprop<Parameters>(false);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(inf, inf, th_p_ch, th_d_ch);
        CHECK_EQUAL(MT::make_move("4c5c"), c->get_action());
    }
}

} // namespace test_vshogi::test_engine::test_dfpn::test_node
