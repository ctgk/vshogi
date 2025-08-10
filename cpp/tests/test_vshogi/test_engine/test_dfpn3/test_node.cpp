#include <set>

#include "vshogi/engine/dfpn3/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn::test_node
{

using namespace vshogi::minishogi;
namespace dfpn = vshogi::engine::dfpn3;
using Node = dfpn::Node<Parameters>;
constexpr uint zero = dfpn::zero;
constexpr uint cent = dfpn::cent;
constexpr uint unit = dfpn::unit;
constexpr uint kilo = dfpn::kilo;
constexpr uint inf = dfpn::inf;

TEST_GROUP (dfpn3_node_simulate) {
};

TEST(dfpn3_node_simulate, using_game_ongoing)
{
    auto g = Game();
    auto n = Node();
    CHECK_FALSE(n.proved());

    CHECK_FALSE(n.simulate(g));
    CHECK_FALSE(n.proved());
}

TEST(dfpn3_node_simulate, using_game_draw)
{
    auto g = Game("4k/5/5/5/4K b -");
    g.apply(Move(SQ_1D, SQ_1E))
        .apply(Move(SQ_1B, SQ_1A))
        .apply(Move(SQ_1E, SQ_1D))
        .apply(Move(SQ_1A, SQ_1B));

    auto n = Node();
    CHECK_TRUE(n.simulate(g, &n));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn3_node_simulate, offence_won_at_offence_turn)
{
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn3_node_simulate, offence_won_at_defence_turn)
{
    const auto m = Move(SQ_1B, KI);
    auto g = Game("4k/5/4P/5/5 b G").apply(m);
    auto n = Node();
    n.init(false, m);
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn3_node_simulate, defence_won_at_offence_turn)
{
    auto g = Game("5/5/3gk/4P/4K w -").apply(Move(SQ_1D, SQ_2C));
    auto n = Node();

    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, defence_won_at_defence_turn)
{
    auto g = Game("4k/4S/5/5/4K w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));

    auto n = Node();
    n.init(false, Move());
    CHECK_TRUE(n.simulate(g, &n));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
    CHECK_TRUE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, using_offence_twin_l_mate)
{
    auto node_l = Node();
    node_l.init(true, Move(), zero, inf);
    CHECK_TRUE(node_l.proved_mate());
    auto g = Game();
    auto n = Node();
    CHECK_TRUE(n.simulate(g, nullptr, nullptr, &node_l));
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, using_offence_twin_l_no_mate)
{
    auto node_l = Node();
    node_l.init(true, Move(), inf, zero);
    CHECK_TRUE(node_l.proved_no_mate());
    auto g = Game();
    auto n = Node();
    CHECK_FALSE(n.simulate(g, nullptr, nullptr, &node_l));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, using_defence_twin_l_mate)
{
    auto node_l = Node();
    node_l.init(false, Move(), inf, zero);
    CHECK_TRUE(node_l.proved_mate());
    auto g = Game();
    auto n = Node();
    n.init(false, Move());
    CHECK_FALSE(n.simulate(g, nullptr, nullptr, &node_l));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, using_defence_twin_l_no_mate)
{
    auto node_l = Node();
    node_l.init(false, Move(), zero, inf);
    CHECK_TRUE(node_l.proved_no_mate());
    auto g = Game();
    auto n = Node();
    n.init(false, Move());
    CHECK_TRUE(n.simulate(g, nullptr, nullptr, &node_l));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, using_offence_twin_g_mate)
{
    auto twin = Node();
    twin.init(true, Move(), zero, inf);
    CHECK_TRUE(twin.proved_mate());
    auto g = Game();
    auto n = Node();
    CHECK_FALSE(n.simulate(g, nullptr, &twin));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, using_offence_twin_g_no_mate)
{
    auto twin = Node();
    twin.init(true, Move(), inf, zero);
    CHECK_TRUE(twin.proved_no_mate());
    auto g = Game();
    auto n = Node();
    CHECK_TRUE(n.simulate(g, nullptr, &twin));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, using_defence_twin_g_mate)
{
    auto twin = Node();
    twin.init(false, Move(), inf, zero);
    CHECK_TRUE(twin.proved_mate());
    auto g = Game();
    auto n = Node();
    n.init(false, Move());
    CHECK_TRUE(n.simulate(g, nullptr, &twin));
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST(dfpn3_node_simulate, using_defence_twin_g_no_mate)
{
    auto twin = Node();
    twin.init(false, Move(), zero, inf);
    CHECK_TRUE(twin.proved_no_mate());
    auto g = Game();
    auto n = Node();
    n.init(false, Move());
    CHECK_FALSE(n.simulate(g, nullptr, &twin));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
    CHECK_FALSE(n.proved_by_repetitions());
}

TEST_GROUP (dfpn3_node_expand) {
};

TEST(dfpn3_node_expand, offence_no_twins)
{
    {
        auto buffer = std::vector<Node>(10);
        auto next = buffer.data();
        auto n = Node();
        CHECK_FALSE(n.fully_expanded());
        n.expand(next, Game("2B1k/5/3P1/3GK/4R b P"));
        CHECK_TRUE(n.fully_expanded());
        std::set<uint16_t> moves;
        for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
            CHECK_FALSE(ch->offence());
            moves.insert(ch->get_action().hash());
            if (ch->get_action() == Move(SQ_2B, SQ_3A, false)) {
                CHECK_EQUAL(kilo, ch->pn());
                CHECK_EQUAL(cent, ch->dn());
            } else {
                CHECK_EQUAL(unit, ch->pn());
                CHECK_EQUAL(unit, ch->dn());
            }
        }
        CHECK_TRUE(
            std::set<uint16_t>({
                Move(SQ_2E, SQ_1D).hash(), // king
                Move(SQ_2B, SQ_3A, false).hash(), // soldier
                Move(SQ_2B, SQ_3A, true).hash(), // soldier
                Move(SQ_1B, FU).hash(), // drop
            })
            == moves);
    }
    {
        auto buffer = std::vector<Node>(3);
        buffer[2].init(false, Move());
        auto next = buffer.data();
        auto n = Node();
        CHECK_FALSE(n.fully_expanded());
        n.expand(next, Game("2B1k/5/3P1/3GK/4R b P"));
        CHECK_FALSE(n.fully_expanded());
    }
    {
        auto buffer = std::vector<Node>(4);
        buffer[3].init(false, Move());
        auto next = buffer.data();
        auto n = Node();
        CHECK_FALSE(n.fully_expanded());
        n.expand(next, Game("2B1k/5/3P1/3GK/4R b P"));
        CHECK_TRUE(n.fully_expanded());
        CHECK_TRUE(next == nullptr);
    }
}

TEST(dfpn3_node_expand, offence_twin_g)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto twin_g = Node();
    twin_g.expand(next, Game("2B1k/5/4p/5/5 b RP"));
    auto n = Node();
    CHECK_FALSE(n.fully_expanded());
    n.expand(next, Game("2B1k/5/4p/5/5 b P"), &twin_g);
    CHECK_TRUE(n.fully_expanded());

    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        CHECK_FALSE(ch->offence());
        moves.insert(ch->get_action().hash());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_3A, false).hash(), // soldier
            Move(SQ_2B, SQ_3A, true).hash(), // soldier
            Move(SQ_1B, FU).hash(), // drop
        })
        == moves);
}

TEST(dfpn3_node_expand, offence_twin_l)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto twin_l = Node();
    twin_l.expand(next, Game("2B1k/5/4p/5/5 b P"));
    auto n = Node();
    CHECK_FALSE(n.fully_expanded());
    n.expand(next, Game("2B1k/5/4p/5/4R b SP"), nullptr, &twin_l);
    CHECK_TRUE(n.fully_expanded());

    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        CHECK_FALSE(ch->offence());
        moves.insert(ch->get_action().hash());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_3A, false).hash(), // soldier
            Move(SQ_2B, SQ_3A, true).hash(), // soldier
            Move(SQ_1B, FU).hash(), // drop
            Move(SQ_1B, GI).hash(), // drop
            Move(SQ_2B, GI).hash(), // drop
            // note that there is no Move(SQ_1C, SQ_1E) here!
        })
        == moves);
}

TEST(dfpn3_node_expand, defence_no_twins)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto n = Node();
    n.init(false, Move());
    CHECK_FALSE(n.fully_expanded());
    n.expand(next, Game("3gk/5/4R/5/4K w p"));
    CHECK_TRUE(n.fully_expanded());

    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        CHECK_TRUE(ch->offence());
        moves.insert(ch->get_action().hash());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_1A).hash(), // king
            Move(SQ_1B, SQ_2A).hash(), // board
            Move(SQ_1B, FU).hash(), // drop
        })
        == moves);
}

TEST(dfpn3_node_expand, defence_twin_l)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto twin_l = Node();
    twin_l.init(false, Move());
    twin_l.expand(next, Game("3gk/5/4R/5/4K w p"));
    auto n = Node();
    n.init(false, Move());
    CHECK_FALSE(n.fully_expanded());
    n.expand(next, Game("3gk/5/4R/5/4K w ps"), nullptr, &twin_l);
    CHECK_TRUE(n.fully_expanded());
    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        CHECK_TRUE(ch->offence());
        moves.insert(ch->get_action().hash());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_1A).hash(), // king
            Move(SQ_1B, SQ_2A).hash(), // board
            Move(SQ_1B, FU).hash(), // drop
            Move(SQ_1B, GI).hash(), // drop
        })
        == moves);
}

TEST(dfpn3_node_expand, defence_twin_g)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto twin_g = Node();
    twin_g.init(false, Move());
    twin_g.expand(next, Game("3gk/5/4R/5/4K w ps"));
    auto n = Node();
    n.init(false, Move());
    CHECK_FALSE(n.fully_expanded());
    n.expand(next, Game("3gk/5/4R/5/4K w s"), &twin_g);
    CHECK_TRUE(n.fully_expanded());
    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        CHECK_TRUE(ch->offence());
        moves.insert(ch->get_action().hash());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_1A).hash(), // king
            Move(SQ_1B, SQ_2A).hash(), // board
            Move(SQ_1B, GI).hash(), // drop
        })
        == moves);
}

TEST(dfpn3_node_expand, defence_partial_expansion)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto g = Game("3gk/5/5/5/4R w 2p2s")
                 .apply(Move(SQ_1D, FU))
                 .apply(Move(SQ_1D, SQ_1E))
                 .apply(Move(SQ_1C, FU))
                 .apply(Move(SQ_1C, SQ_1D));
    auto n = Node();
    n.init(false, Move());
    CHECK_FALSE(n.fully_expanded());
    n.expand(next, g);
    CHECK_FALSE(n.fully_expanded());
    std::set<uint16_t> moves;
    for (auto ch = n.get_child(); ch; ch = ch->get_sibling()) {
        CHECK_TRUE(ch->offence());
        moves.insert(ch->get_action().hash());
    }
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_1A).hash(), // king
            Move(SQ_1B, SQ_2A).hash(), // board
            // Move(SQ_1B, GI).hash(), // drop
        })
        == moves);
}

TEST_GROUP (dfpn3_node_backprop) {
};

TEST(dfpn3_node_backprop, offence_preference)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto g = Game("2B1k/5/5/5/5 b P");
    auto n = Node();
    n.expand(next, g);
    n.backprop(SQ_1A);
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(2u * unit + cent, n.dn());

    // Prefer drop move over board move.
    CHECK_EQUAL(Move(SQ_1B, FU).hash(), n.get_child_1st()->get_action().hash());

    // Prefer promotion move over no-promotion move.
    CHECK_EQUAL(
        Move(SQ_2B, SQ_3A, true).hash(),
        n.get_child_2nd()->get_action().hash());
}

TEST(dfpn3_node_backprop, offence_with_proved_child)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto g = Game("3rk/3p1/4P/5/4K b G");
    auto n = Node();
    n.expand(next, g);
    n.backprop(SQ_1A);
    CHECK_FALSE(n.proved());
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(2u * unit, n.dn());

    uint th_p_ch, th_d_ch;
    auto ch = n.select(inf, inf, th_p_ch, th_d_ch); // Move(SQ_1B, KI)
    CHECK_FALSE(ch->offence());
    CHECK_EQUAL(Move(SQ_1B, KI).hash(), ch->get_action().hash()); // prefer drop
    g.apply(ch->get_action());
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
    ch->simulate(g);
    CHECK_TRUE(ch->proved_mate());

    n.backprop(SQ_1A);
    CHECK_TRUE(n.proved_mate());
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
}

TEST(dfpn3_node_backprop, defence_preference)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto g = Game("s4/RR3/5/5/k4 w p");
    auto n = Node();
    n.init(false, Move(SQ_5B, HI));
    n.expand(next, g);
    n.backprop(g.get_king_location());

    // Prefer capture move
    CHECK_EQUAL(
        Move(SQ_5B, SQ_5A).hash(), n.get_child_1st()->get_action().hash());

    // Prefer drop near by king than drop far from king.
    CHECK_EQUAL(Move(SQ_5D, FU).hash(), n.get_child_2nd()->get_action().hash());
}

TEST(dfpn3_node_backprop, defence_with_proved_child)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto g = Game("4k/4P/5/5/5 w -");
    auto n = Node();
    n.init(false, Move());
    n.expand(next, g);
    n.backprop(SQ_1A);

    uint th_p_ch, th_d_ch;
    const auto ch = n.select(inf, inf, th_p_ch, th_d_ch);
    CHECK_TRUE(ch->offence());
    CHECK_EQUAL(Move(SQ_1B, SQ_1A).hash(), ch->get_action().hash());
    g.apply_dfpn(ch->get_action());
    ch->expand(next, g);
    ch->backprop(SQ_1B);
    CHECK_TRUE(ch->proved_no_mate());
    g.undo();

    n.backprop(SQ_1A);
    CHECK_TRUE(ch->proved_no_mate());
}

TEST(dfpn3_node_backprop, offence_proved_by_repetitions)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto g = Game("4k/5/4S/5/5 b -");
    g.apply(Move(SQ_2B, SQ_1C))
        .apply(Move(SQ_1B, SQ_1A))
        .apply(Move(SQ_1C, SQ_2B))
        .apply(Move(SQ_1A, SQ_1B));
    g.apply(Move(SQ_1B, SQ_1C))
        .apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_1A, SQ_1B))
        .apply(Move(SQ_2A, SQ_2B))
        .apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_1B, SQ_2A))
        .apply(Move(SQ_1C, SQ_2B))
        .apply(Move(SQ_1A, SQ_1B));
    auto n = Node();
    n.expand(next, g);
    n.backprop(SQ_1A);

    uint th_p_ch, th_d_ch;
    auto c = n.select(inf, inf, th_p_ch, th_d_ch);
    g.apply_dfpn(c->get_action());
    CHECK_TRUE(c->simulate(g, c)); // repetition
    CHECK_TRUE(c->proved_no_mate());
    CHECK_TRUE(c->proved_by_repetitions());
    g.undo();
    n.backprop(SQ_1A);
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_by_repetitions());
    c = n.select(inf, inf, th_p_ch, th_d_ch);
    g.apply_dfpn(c->get_action());
    CHECK_TRUE(c->simulate(g, c)); // repetition
    CHECK_TRUE(c->proved_no_mate());
    CHECK_TRUE(c->proved_by_repetitions());
    g.undo();
    n.backprop(SQ_1A);
    CHECK_TRUE(n.proved_no_mate());
    CHECK_TRUE(n.proved_by_repetitions());
}

TEST(dfpn3_node_backprop, defence_proved_by_repetitions)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto g = Game("4k/4S/4P/5/5 w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));
    auto n = Node();
    n.init(false, Move());
    n.expand(next, g);
    n.backprop(SQ_1A);

    uint th_p_ch, th_d_ch;
    auto c = n.select(inf, inf, th_p_ch, th_d_ch);
    g.apply_dfpn(c->get_action());
    CHECK_TRUE(c->simulate(g, c)); // repetition
    CHECK_TRUE(c->proved_no_mate());
    CHECK_TRUE(c->proved_by_repetitions());
    g.undo();
    n.backprop(SQ_1A);
    CHECK_TRUE(n.proved_no_mate());
    CHECK_TRUE(n.proved_by_repetitions());
}

TEST_GROUP (dfpn3_node_select) {
};

TEST(dfpn3_node_select, offence_single_child)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    {
        auto n = Node();
        n.expand(next, Game("4k/5/4P/5/5 b -"));
        n.backprop(SQ_1A);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(inf, inf, th_p_ch, th_d_ch);
        CHECK_EQUAL(inf, th_p_ch);
        CHECK_EQUAL(inf, th_d_ch);
        CHECK_EQUAL(Move(SQ_1B, SQ_1C).hash(), c->get_action().hash());
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
    {
        auto n = Node();
        n.expand(next, Game("4k/5/4P/5/5 b -"));
        n.backprop(SQ_1A);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(25, 10, th_p_ch, th_d_ch);
        CHECK_EQUAL(10, th_p_ch);
        CHECK_EQUAL(25, th_d_ch);
        CHECK_EQUAL(Move(SQ_1B, SQ_1C).hash(), c->get_action().hash());
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
}

TEST(dfpn3_node_select, offence_multiple_child)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    {
        auto n = Node();
        n.expand(next, Game("4k/5/5/5/5 b S"));
        n.backprop(SQ_1A);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(inf, inf, th_p_ch, th_d_ch);
        CHECK_EQUAL(inf - unit, th_p_ch);
        CHECK_EQUAL(unit + 1u, th_d_ch);
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
}

TEST(dfpn3_node_select, offence_with_disproved_child)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    auto g = Game("4k/5/4G/5/5 b -");
    auto n = Node();
    n.expand(next, g);
    n.backprop(SQ_1A);

    uint th_p_ch, th_d_ch;
    auto c = n.select(inf, inf, th_p_ch, th_d_ch);
    g.apply_dfpn(c->get_action());
    auto twin_l = Node();
    twin_l.init(false, Move(), zero, inf);
    c->simulate(g, nullptr, nullptr, &twin_l);
    CHECK_TRUE(c->proved_no_mate());
    g.undo();

    n.backprop(SQ_1A);
    c = n.select(inf, inf, th_p_ch, th_d_ch);
    CHECK_EQUAL(inf, th_p_ch);
    CHECK_EQUAL(inf, th_d_ch);
}

TEST(dfpn3_node_select, defence_single_child)
{
    auto buffer = std::vector<Node>(100);
    auto next = buffer.data();
    {
        auto n = Node();
        n.init(false, Move());
        n.expand(next, Game("4k/4G/5/5/5 w -"));
        n.backprop(SQ_1A);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(inf, inf, th_p_ch, th_d_ch);
        CHECK_EQUAL(inf, th_p_ch);
        CHECK_EQUAL(inf, th_d_ch);
        CHECK_EQUAL(Move(SQ_1B, SQ_1A).hash(), c->get_action().hash());
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
    {
        auto n = Node();
        n.init(false, Move());
        n.expand(next, Game("4k/4G/5/5/5 w -"));
        n.backprop(SQ_1A);
        uint th_p_ch, th_d_ch;
        const auto c = n.select(25, 10, th_p_ch, th_d_ch);
        CHECK_EQUAL(10, th_p_ch);
        CHECK_EQUAL(25, th_d_ch);
        CHECK_EQUAL(Move(SQ_1B, SQ_1A).hash(), c->get_action().hash());
        CHECK_EQUAL(unit, c->phi());
        CHECK_EQUAL(unit, c->delta());
    }
}

} // namespace test_vshogi::test_engine::test_dfpn::test_node
