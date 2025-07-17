#include <set>

#include "vshogi/engine/dfpn/node.hpp"
#include "vshogi/variants/minishogi.hpp"

#include <CppUTest/TestHarness.h>

namespace test_vshogi::test_engine::test_dfpn::test_node
{

using namespace vshogi::minishogi;
namespace dfpn = vshogi::engine::dfpn2;
using Node = dfpn::Node<Parameters>;
constexpr uint zero = dfpn::zero;
constexpr uint unit = dfpn::unit;
constexpr uint cent = dfpn::cent;
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
    g.apply(Move(SQ_1D, SQ_1E))
        .apply(Move(SQ_1B, SQ_1A))
        .apply(Move(SQ_1E, SQ_1D))
        .apply(Move(SQ_1A, SQ_1B));
    g.update_result_dfpn(1u);
    CHECK_EQUAL(vshogi::DRAW, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_game_won_by_offence_at_offence_turn)
{
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u);
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_game_won_by_defence_at_offence_turn)
{
    auto g = Game("5/5/3gk/5/4K w -").apply(Move(SQ_1D, SQ_2C));
    CHECK_EQUAL(vshogi::BLACK, g.get_turn());
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());

    auto n = Node();
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_game_won_by_defence_at_defence_turn)
{
    using namespace vshogi::minishogi;
    auto g = Game("4k/4S/5/5/4K w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));
    g.update_result_dfpn(1u);
    CHECK_EQUAL(vshogi::WHITE_WIN, g.get_result());

    auto n = Node(false, Move());
    CHECK_TRUE(n.simulate(g));
    CHECK_EQUAL(inf, n.pn());
    CHECK_EQUAL(zero, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_offence_cousin_l_mate)
{
    auto node_l = Node();
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u);
    node_l.simulate(g);

    auto n = Node();
    CHECK_TRUE(n.simulate(Game(), &node_l));
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_offence_cousin_l_no_mate)
{
    auto node_l = Node();
    auto g = Game("5/5/3gk/5/4K w -").apply(Move(SQ_1D, SQ_2C));
    node_l.simulate(g);
    CHECK_TRUE(node_l.proved_no_mate());

    auto n = Node();
    CHECK_FALSE(n.simulate(Game(), &node_l));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_defence_cousin_l_no_mate)
{
    auto g = Game("4k/4S/5/5/4K w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));
    g.update_result_dfpn(1u);
    auto node_l = Node(false, Move());
    node_l.simulate(g);
    CHECK_TRUE(node_l.proved_no_mate());

    auto n = Node(false, Move());
    CHECK_TRUE(n.simulate(Game(), &node_l));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_defence_cousin_l_ongoing)
{
    auto g = Game("4k/4S/5/5/4K w -");
    auto node_l = Node(false, Move());
    node_l.simulate(g);
    CHECK_FALSE(node_l.proved());

    auto n = Node(false, Move());
    CHECK_FALSE(n.simulate(Game(), &node_l));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_cousin_e_mate)
{
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u);
    auto node_e = Node();
    node_e.simulate(g);
    CHECK_TRUE(node_e.proved_mate());

    auto n = Node();
    CHECK_TRUE(n.simulate(Game(), nullptr, &node_e));
    CHECK_TRUE(n.proved());
    CHECK_TRUE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_cousin_e_no_mate)
{
    auto g = Game("4k/5/5/5/4K b -");
    g.apply(Move(SQ_1D, SQ_1E))
        .apply(Move(SQ_1B, SQ_1A))
        .apply(Move(SQ_1E, SQ_1D))
        .apply(Move(SQ_1A, SQ_1B));
    g.update_result_dfpn(1u);
    auto node_e = Node();
    node_e.simulate(g);
    CHECK_TRUE(node_e.proved_no_mate());

    auto n = Node();
    CHECK_TRUE(n.simulate(Game(), nullptr, &node_e));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_cousin_e_ongoing)
{
    auto g = Game();
    auto node_e = Node();
    node_e.simulate(g);
    CHECK_FALSE(node_e.proved());

    auto n = Node();
    CHECK_FALSE(n.simulate(Game(), nullptr, &node_e));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_offence_cousin_g_mate)
{
    auto node_g = Node();
    auto g = Game("BRBRK/5/5/5/4k b 2P2S2G");
    g.update_result_dfpn(1u);
    node_g.simulate(g);

    auto n = Node();
    CHECK_FALSE(n.simulate(Game(), nullptr, nullptr, &node_g));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_offence_cousin_g_no_mate)
{
    auto node_g = Node();
    auto g = Game("5/5/3gk/5/4K w -").apply(Move(SQ_1D, SQ_2C));
    node_g.simulate(g);
    CHECK_TRUE(node_g.proved_no_mate());

    auto n = Node();
    CHECK_TRUE(n.simulate(Game(), nullptr, nullptr, &node_g));
    CHECK_TRUE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_TRUE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_defence_cousin_g_no_mate)
{
    auto g = Game("4k/4S/5/5/4K w -");
    g.apply(Move(SQ_2B, SQ_1A))
        .apply(Move(SQ_2C, SQ_1B))
        .apply(Move(SQ_1A, SQ_2B))
        .apply(Move(SQ_1B, SQ_2C));
    g.update_result_dfpn(1u);
    auto node_g = Node(false, Move());
    node_g.simulate(g);
    CHECK_TRUE(node_g.proved_no_mate());

    auto n = Node(false, Move());
    CHECK_FALSE(n.simulate(Game(), nullptr, nullptr, &node_g));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST(dfpn_node_simulate, using_defence_cousin_g_ongoing)
{
    auto g = Game("4k/4S/5/5/4K w -");
    auto node_g = Node(false, Move());
    node_g.simulate(g);
    CHECK_FALSE(node_g.proved());

    auto n = Node(false, Move());
    CHECK_FALSE(n.simulate(Game(), nullptr, nullptr, &node_g));
    CHECK_FALSE(n.proved());
    CHECK_FALSE(n.proved_mate());
    CHECK_FALSE(n.proved_no_mate());
}

TEST_GROUP (dfpn_node_backprop) {
};

TEST(dfpn_node_backprop, offence_single_child)
{
    auto g = Game("4k/5/5/5/5 b P");
    auto n = Node();
    n.expand_children(g);
    n.backprop(g);
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(unit, n.dn());
}

TEST(dfpn_node_backprop, offence_with_proved_child)
{
    auto g = Game("3rk/3p1/4P/5/4K b G");
    auto n = Node();
    n.expand_children(g);
    n.backprop(g);
    CHECK_FALSE(n.proved());
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(2u * unit, n.dn());

    auto ch = n.select(); // Move(SQ_1B, KI)
    CHECK_FALSE(ch->offence());
    g.apply(Move(SQ_1B, KI));
    CHECK_EQUAL(vshogi::BLACK_WIN, g.get_result());
    ch->simulate(g);
    CHECK_TRUE(ch->proved_mate());

    n.backprop(g);
    CHECK_TRUE(n.proved_mate());
    CHECK_EQUAL(zero, n.pn());
    CHECK_EQUAL(inf, n.dn());
}

TEST(dfpn_node_backprop, offence_preference)
{
    auto g = Game("2B1k/5/5/5/5 b P");
    auto n = Node();
    n.expand_children(g);
    n.backprop(g);
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(2u * unit + cent, n.dn());

    // Prefer drop move over soldier move.
    CHECK_EQUAL(Move(SQ_1B, FU).hash(), n.get_child_1st()->get_action().hash());

    // Prefer promotion move over no-promotion move.
    CHECK_EQUAL(
        Move(SQ_2B, SQ_3A, true).hash(),
        n.get_child_2nd()->get_action().hash());
}

TEST(dfpn_node_backprop, defence_single_child)
{
    auto g = Game("4k/4P/4G/5/5 w -");
    auto n = Node(false, Move());
    n.expand_children(g);
    CHECK_EQUAL(1u, n.get_children().size());
    n.backprop(g);
    CHECK_EQUAL(unit, n.pn());
    CHECK_EQUAL(unit, n.dn());
}

TEST(dfpn_node_backprop, defence_with_proved_child)
{
    auto g = Game("4k/4P/5/5/5 w -");
    auto n = Node(false, Move());
    n.expand_children(g);
    CHECK_EQUAL(3u, n.num_children());
    n.backprop(g);

    const auto ch = n.select();
    CHECK_TRUE(ch->offence());
    CHECK_EQUAL(Move(SQ_1B, SQ_1A).hash(), ch->get_action().hash());
    g.apply_dfpn(ch->get_action());
    ch->expand_children(g);
    ch->backprop(g);
    CHECK_TRUE(ch->proved_no_mate());
    g.undo();

    n.backprop(g);
    CHECK_TRUE(ch->proved_no_mate());
}

TEST(dfpn_node_backprop, defence_preference)
{
    auto g = Game("s4/RR3/5/5/k4 w p");
    auto n = Node(false, Move());
    n.expand_children(g);
    n.backprop(g);

    // Prefer capture move
    CHECK_EQUAL(
        Move(SQ_5B, SQ_5A).hash(), n.get_child_1st()->get_action().hash());

    // Prefer drop near by king than drop far from king.
    CHECK_EQUAL(Move(SQ_5D, FU).hash(), n.get_child_2nd()->get_action().hash());
}

TEST_GROUP (dfpn_node_compute_child_th) {
};

TEST(dfpn_node_compute_child_th, offence)
{
    auto g = Game("2B1k/5/5/5/5 b -");
    auto n = Node();
    n.expand_children(g);
    n.backprop(g);
    CHECK_EQUAL(unit, n.compute_child_thpn(unit));
    CHECK_EQUAL(kilo + 1u, n.compute_child_thpn(10u * kilo));
}

TEST(dfpn_node_compute_child_th, defence)
{
    auto g = Game("4k/4P/5/5/5 w -");
    auto n = Node(false, Move());
    n.expand_children(g);
    n.backprop(g);
    CHECK_EQUAL(unit + 1u, n.compute_child_thdn(inf));
    CHECK_EQUAL(cent, n.compute_child_thdn(cent));
}

TEST_GROUP (dfpn_expand_children_at_defence_drop) {
};

TEST(dfpn_expand_children_at_defence_drop, no_niblings)
{
    std::list<Node> actual{};
    dfpn::internal::expand_children_at_defence_drop(
        actual, Game("4k/5/4R/5/4K w p"));
    CHECK_EQUAL(1u, actual.size());
    CHECK_EQUAL(Move(SQ_1B, FU).hash(), actual.front().get_action().hash());
    CHECK_TRUE(actual.front().offence());
    CHECK_TRUE(actual.front().is_first_arrival());
}

TEST(dfpn_expand_children_at_defence_drop, using_niblings)
{
    std::list<Node> niblings{};
    dfpn::internal::expand_children_at_defence(
        niblings, Game("3gk/5/4R/5/4K w psg"));

    std::list<Node> actual{};
    dfpn::internal::expand_children_at_defence_drop(
        actual, Game("3gk/5/4R/5/4K w s"), &niblings);
    CHECK_EQUAL(1u, actual.size());
    CHECK_EQUAL(Move(SQ_1B, GI).hash(), actual.front().get_action().hash());
    CHECK_TRUE(actual.front().offence());
    CHECK_TRUE(actual.front().is_first_arrival());
}

TEST_GROUP (dfpn_expand_children_at_defence_board) {
};

TEST(dfpn_expand_children_at_defence_board, no_niblings)
{
    std::list<Node> actual{};
    dfpn::internal::expand_children_at_defence_board(
        actual, Game("3gk/5/4R/5/4K w p"));
    CHECK_EQUAL(2u, actual.size());
    std::set<uint16_t> actual_moves{};
    auto it = actual.cbegin();
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_1A).hash(), // king
            Move(SQ_1B, SQ_2A).hash(), // board
        })
        == actual_moves);
}

TEST(dfpn_expand_children_at_defence_board, using_niblings)
{
    std::list<Node> niblings{};
    dfpn::internal::expand_children_at_defence(
        niblings, Game("3gk/5/4R/5/4K w p"));

    std::list<Node> actual{};
    dfpn::internal::expand_children_at_defence_board(
        actual, Game("3gk/5/4R/5/4K w ps"));
    CHECK_EQUAL(2u, actual.size());
    std::set<uint16_t> actual_moves{};
    auto it = actual.cbegin();
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_1A).hash(), // king
            Move(SQ_1B, SQ_2A).hash(), // board
        })
        == actual_moves);
}

TEST_GROUP (dfpn_expand_children_at_defence) {
};

TEST(dfpn_expand_children_at_defence, no_niblings)
{
    std::list<Node> actual{};
    dfpn::internal::expand_children_at_defence(
        actual, Game("3gk/5/4R/5/4K w p"));
    CHECK_EQUAL(3u, actual.size());

    std::set<uint16_t> actual_moves{};
    auto it = actual.cbegin();
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    CHECK_TRUE(
        std::set<uint16_t>({
            Move(SQ_2B, SQ_1A).hash(), // king
            Move(SQ_1B, SQ_2A).hash(), // board
            Move(SQ_1B, FU).hash(), // drop
        })
        == actual_moves);
}

TEST(dfpn_expand_children_at_defence, using_niblings)
{
    std::list<Node> niblings{};
    dfpn::internal::expand_children_at_defence(
        niblings, Game("3gk/3p1/4R/5/4K w psg"));

    std::list<Node> actual{};
    dfpn::internal::expand_children_at_defence(
        actual,
        Game("3gk/5/4R/5/4K w p"),
        static_cast<const std::list<Node>*>(nullptr),
        &niblings);
    CHECK_EQUAL(2u, actual.size());

    std::set<uint16_t> actual_moves{};
    auto it = actual.cbegin();
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_TRUE(it->offence());
    actual_moves.insert(it->get_action().hash());
    CHECK_TRUE(
        std::set<uint16_t>({
            // Move(SQ_2B, SQ_1A).hash(), // no king move
            Move(SQ_1B, SQ_2A).hash(), // board
            Move(SQ_1B, FU).hash(), // drop
        })
        == actual_moves);
}

TEST(dfpn_expand_children_at_defence, partial_expansion)
{
    auto g = Game("4k/4p/5/5/B3K w psgr")
                 .apply(Move(SQ_4D, FU))
                 .apply(Move(SQ_4D, SQ_5E))
                 .apply(Move(SQ_3C, GI))
                 .apply(Move(SQ_3C, SQ_4D));
    std::list<Node> actual{};
    CHECK_FALSE(dfpn::internal::expand_children_at_defence(actual, g));
    CHECK_EQUAL(1u, actual.size());
    CHECK_EQUAL(Move(SQ_2A, SQ_1A).hash(), actual.front().get_action().hash());
}

TEST_GROUP (dfpn_expand_children_at_offence) {
};

TEST(dfpn_expand_children_at_offence, no_niblings)
{
    std::list<Node> actual;
    dfpn::internal::expand_children_at_offence(
        actual, Game("2B1k/5/3P1/3GK/4R b P"));
    CHECK_EQUAL(4u, actual.size());

    std::set<uint16_t> moves;
    for (auto it = actual.cbegin(); it != actual.cend(); ++it) {
        CHECK_TRUE(it->is_first_arrival());
        CHECK_FALSE(it->offence());
        moves.insert(it->get_action().hash());
        if (it->get_action() == Move(SQ_2B, SQ_3A, false)) {
            CHECK_EQUAL(kilo, it->pn());
            CHECK_EQUAL(cent, it->dn());
        } else {
            CHECK_EQUAL(unit, it->pn());
            CHECK_EQUAL(unit, it->dn());
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

TEST(dfpn_expand_children_at_offence, niblings_g)
{
    std::list<Node> niblings{};
    niblings.emplace_back(false, Move(SQ_2B, SQ_3A, false), zero, inf);
    niblings.emplace_back(false, Move(SQ_2B, SQ_3A, true));
    niblings.emplace_back(false, Move(SQ_1B, FU));
    niblings.emplace_back(false, Move(SQ_1B, GI));

    std::list<Node> actual{};
    dfpn::internal::expand_children_at_offence(
        actual,
        Game("2B1k/5/5/4K/4R b P"),
        static_cast<const std::list<Node>*>(nullptr),
        &niblings);
    CHECK_EQUAL(3u, actual.size());

    auto it = actual.cbegin();
    CHECK_TRUE(it->is_first_arrival());
    CHECK_FALSE(it->offence());
    CHECK_EQUAL(Move(SQ_2B, SQ_3A, false).hash(), it->get_action().hash());
    CHECK_EQUAL(cent, it->pn());
    CHECK_EQUAL(kilo, it->dn());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_FALSE(it->offence());
    CHECK_EQUAL(Move(SQ_2B, SQ_3A, true).hash(), it->get_action().hash());
    CHECK_EQUAL(unit, it->pn());
    CHECK_EQUAL(unit, it->dn());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_FALSE(it->offence());
    CHECK_EQUAL(Move(SQ_1B, FU).hash(), it->get_action().hash());
    CHECK_EQUAL(unit, it->pn());
    CHECK_EQUAL(unit, it->dn());
}

TEST(dfpn_expand_children_at_offence, niblings_l)
{
    std::list<Node> niblings{};
    niblings.emplace_back(false, Move(SQ_2B, SQ_3A, false), zero, inf);
    niblings.emplace_back(false, Move(SQ_2B, SQ_3A, true));
    niblings.emplace_back(false, Move(SQ_1B, GI));

    std::list<Node> actual{};
    dfpn::internal::expand_children_at_offence(
        actual,
        Game("2B1k/5/5/4K/4R b P"),
        &niblings,
        static_cast<const std::list<Node>*>(nullptr));
    CHECK_EQUAL(3u, actual.size());

    auto it = actual.cbegin();
    CHECK_TRUE(it->is_first_arrival());
    CHECK_FALSE(it->offence());
    CHECK_EQUAL(Move(SQ_2B, SQ_3A, false).hash(), it->get_action().hash());
    CHECK_EQUAL(cent, it->pn());
    CHECK_EQUAL(kilo, it->dn());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_FALSE(it->offence());
    CHECK_EQUAL(Move(SQ_2B, SQ_3A, true).hash(), it->get_action().hash());
    CHECK_EQUAL(unit, it->pn());
    CHECK_EQUAL(unit, it->dn());
    ++it;
    CHECK_TRUE(it->is_first_arrival());
    CHECK_FALSE(it->offence());
    CHECK_EQUAL(Move(SQ_1B, FU).hash(), it->get_action().hash());
    CHECK_EQUAL(unit, it->pn());
    CHECK_EQUAL(unit, it->dn());
}

} // namespace test_vshogi::test_engine::test_dfpn::test_node
