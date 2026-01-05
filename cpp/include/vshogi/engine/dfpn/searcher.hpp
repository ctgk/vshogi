#ifndef VSHOGI_ENGINE_DFPN_SEARCHER_HPP
#define VSHOGI_ENGINE_DFPN_SEARCHER_HPP

#include <memory>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/engine/dfpn/node.hpp"
#include "vshogi/engine/dfpn/table.hpp"
#include "vshogi/engine/tree/searcher.hpp"

namespace vshogi::engine::dfpn
{

template <class P>
class ScopedGame
{
private:
    Game<P>& m_game;
    std::vector<ZobristHashType> m_hash_list{};
    std::vector<std::uint32_t> m_captured_move_list{};

public:
    ScopedGame(Game<P>& g) : m_game(g)
    {
        m_hash_list.reserve(32u);
        m_captured_move_list.reserve(32u);
        m_game.swap_log(m_hash_list, m_captured_move_list);
    }
    ~ScopedGame()
    {
        m_game.swap_log(m_hash_list, m_captured_move_list);
    }
};

template <class P>
class Searcher : public tree::Searcher<Node>
{
    using C = Configuration<P>;
    using MT = MoveTraits<P>;
    using Square = typename C::Square;

private:
    Table<P> m_table;
    uint m_search_count;
    uint m_remaining_searches;

public:
    move_t search(Game<P>& g, const uint n)
    {
        ScopedGame scope{g};
        if ((m_search_count == 0u) && !m_nodes[0].simulate(g)
            && !m_next->is_end()) {
            m_nodes[0].expand(m_next, g);
            m_table.add(&m_nodes[0], g);
            m_nodes[0].template backprop<P>(true);
        }
        if (m_nodes[0].proved())
            return static_cast<move_t>(0);
        m_remaining_searches = n;
        const auto out = multiple_iterative_deepening(m_nodes[0], g, inf, inf);
        m_search_count += n - m_remaining_searches;
        return out;
    }

private:
    move_t multiple_iterative_deepening(
        Node& n, Game<P>& g, const uint th_p, const uint th_d)
    {
        const bool offence = (g.ply() % 2u == 0u);
        move_t out = n.get_action();
        assert(offence || g.in_check());
        const Node *twin_ge{}, *twin_e{}, *twin_le{};
        m_table.look_up(g, &twin_ge, &twin_e, &twin_le);
        if (n.simulate(g, twin_ge, twin_le)) {
            --m_remaining_searches;
            return out;
        }
        if (!n.has_child() && !m_next->is_end()) {
            n.expand(m_next, g, twin_ge, twin_le);
            if ((twin_e == nullptr) || !twin_e->fully_expanded())
                m_table.add(&n, g);
            --m_remaining_searches;
            n.backprop<P>(offence);
        }
        while (!m_next->is_end() && m_remaining_searches && (n.phi() < th_p)
               && (n.delta() < th_d)) {
            uint th_p_ch, th_d_ch;
            Node* const child = n.select(th_p, th_d, th_p_ch, th_d_ch);
            g.apply_dfpn(child->get_action());
            out = multiple_iterative_deepening(*child, g, th_p_ch, th_d_ch);
            g.undo();
            n.backprop<P>(offence);
        }
        return out;
    }

public: // utility
    Searcher(const uint tree_size = 100000u);

    // Rules of 5
    ~Searcher() = default; // 1/5 destructor
    Searcher(const Searcher& other) = delete; // 2/5 copy constructor
    Searcher& operator=(const Searcher& other) = delete; // 3/5 copy assignment
    Searcher(Searcher&& other) = delete; // 4/5 move constructor
    Searcher& operator=(Searcher&& other) = delete; // 5/5 move assignment

    void init();
    uint get_search_count() const
    {
        return m_search_count;
    }
    uint get_num_nodes_remain() const
    {
        assert(m_next);
        return static_cast<uint>(m_nodes.size())
               - static_cast<uint>(m_next - m_nodes.data()) - 1u;
    }
    bool proved() const
    {
        return m_nodes[0].proved();
    }
    bool proved_mate() const
    {
        return m_nodes[0].proved_mate(true);
    }
    bool proved_no_mate() const
    {
        return m_nodes[0].proved_no_mate(true);
    }
    std::vector<move_t> get_mate_moves(Game<P>& game) const
    {
        ScopedGame scope{game};
        follow_line(game, m_nodes[0].get_child_1st());
        std::vector<move_t> out{};
        if (game.ply() > 0u) {
            for (uint ii = 0u; ii < game.ply(); ++ii)
                out.emplace_back(game.get_record_action(ii));
            for (uint ii = game.ply(); ii--;)
                game.undo();
        }
        assert(game.ply() == 0u);
        return out;
    }

private: // utility
    bool follow_line(Game<P>& game, const Node* node) const
    {
        if ((node == nullptr) || (!game.is_legal(node->get_action())))
            return false;
        game.apply_nocheck(node->get_action());
        if ((game.count_repetitions() > 1u) || (game.ply() > 31u)) {
            game.undo();
            return false;
        }
        const bool offence = (game.ply() % 2u == 0u);
        if (is_mate_end(game))
            return true;
        if (!node->proved_mate(offence))
            node = lookup_in_table(game);
        if (node) {
            const Node* c = node->get_child_1st();
            if (follow_line(game, c))
                return true;
            for (c = node->get_child(); c; c = c->get_sibling()) {
                if (c == node->get_child_1st())
                    continue;
                if (follow_line(game, c))
                    return true;
            }
        }
        game.undo();
        return false;
    }
    static bool is_mate_end(const Game<P>& game)
    {
        const bool offence = (game.ply() % 2u == 0u);
        const auto r = game.get_result();
        if ((r == ONGOING) || (r == DRAW))
            return false;
        const auto turn = game.get_turn();
        const auto winner = (r == BLACK_WIN) ? BLACK : WHITE;
        return offence ? (turn == winner) : (turn != winner);
    }
    const Node* lookup_in_table(Game<P>& game) const
    {
        const bool offence = (game.ply() % 2u == 0u);
        const Node *node_ge{}, *node_e{}, *node_le{};
        m_table.look_up(game, &node_ge, &node_e, &node_le);
        const Node* const node = (offence) ? node_le : node_ge;
        if (node && node->proved_mate(offence))
            return node;
        return nullptr;
    }
};

template <class P>
Searcher<P>::Searcher(const uint tree_size)
    : tree::Searcher<Node>(tree_size), m_table{}, m_search_count(0u),
      m_remaining_searches(0u)
{
    init();
}

template <class P>
void Searcher<P>::init()
{
    tree::Searcher<Node>::init();
    m_table.clear();
    m_search_count = 0u;
}

template <class P, class N>
class DfpnAugmentedSearcher : public tree::Searcher<N>
{
public:
    /**
     * @brief Construct a new Dfpn Augmenter object
     *
     * @param tree_size Maximum size of the derived search tree.
     * @param budget_root Budget for DFPN search at root of the base tree.
     * @param budget_leaf Budget for DFPN search at leaf of the base tree.
     */
    DfpnAugmentedSearcher(
        const uint tree_size, const uint budget_root, const uint budget_leaf);
    void apply(Game<P>& game, const move_t& action);

private:
    Searcher<P> m_dfpn;
    const uint m_budget_root;
    const uint m_budget_leaf;

protected:
    using tree::Searcher<N>::m_nodes;
    using tree::Searcher<N>::m_next;
    using tree::Searcher<N>::backprop_to_root;
    N* simulate_backprop_if_possible(Game<P>& game, N* const leaf);
    bool dfpn_proved_mate(Game<P>& game, N* const node);
};

template <class P, class N>
DfpnAugmentedSearcher<P, N>::DfpnAugmentedSearcher(
    const uint tree_size, const uint budget_root, const uint budget_leaf)
    : tree::Searcher<N>(tree_size),
      m_dfpn{std::max(budget_root, budget_leaf) * 10u},
      m_budget_root{budget_root}, m_budget_leaf{budget_leaf}
{
}

template <class P, class N>
void DfpnAugmentedSearcher<P, N>::apply(Game<P>& game, const move_t& action)
{
    tree::Searcher<N>::apply(action);
    game.apply(action);
    dfpn_proved_mate(game, &m_nodes[0]);
}

template <class P, class N>
N* DfpnAugmentedSearcher<P, N>::simulate_backprop_if_possible(
    Game<P>& game, N* const leaf)
{
    assert(leaf != nullptr);
    assert(!leaf->has_child());
    if (game.get_result() != ONGOING) {
        leaf->simulate(game);
        backprop_to_root(game, leaf);
        return nullptr;
    }
    if (leaf->is_mate_to_lose()) {
        leaf->expand(m_next, game, nullptr);
        backprop_to_root(game, leaf);
        return nullptr;
    }
    assert(!leaf->is_mate_to_win());
    if (dfpn_proved_mate(game, leaf))
        return nullptr;
    return leaf;
}

template <class P, class N>
bool DfpnAugmentedSearcher<P, N>::dfpn_proved_mate(Game<P>& game, N* const node)
{
    using C = Configuration<P>;
    const uint budget = (node == &m_nodes[0]) ? m_budget_root : m_budget_leaf;
    if (budget == 0u)
        return false;
    if (game.get_state().get_king_square(~game.get_turn()) == C::SQ_NA)
        return false;
    m_dfpn.init();
    m_dfpn.search(game, budget);
    if (m_dfpn.proved_mate()) {
        const move_t action = m_dfpn.select_action();
        if (action) {
            node->simulate_mate_and_expand(m_next, action);
            backprop_to_root(game, node);
            return true;
        }
    }
    return false;
}

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_SEARCHER_HPP
