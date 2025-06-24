#ifndef VSHOGI_ENGINE_DFPN_SEARCHER_HPP
#define VSHOGI_ENGINE_DFPN_SEARCHER_HPP

#include <memory>

#include "vshogi/engine/dfpn/node.hpp"
#include "vshogi/engine/dfpn/table.hpp"

namespace vshogi::engine::dfpn
{

template <class Parameters>
class Searcher
{
private:
    using C = Configuration<Parameters>;
    using EdgeType = Edge<Parameters>;
    using GameType = Game<Parameters>;
    using NodeType = Node<Parameters>;
    using TableType = Table<Parameters>;

    TableType m_table;
    std::unique_ptr<GameType> m_game;
    NodeType* m_root;
    uint m_num_searched;

public:
    void set_game(const Game<Parameters>& g)
    {
        m_table.clear();
        m_table.set_offence(g.get_turn());
        m_game = std::make_unique<GameType>(g);
        NodeType& n = m_table.get_or_expand(g);
        if (!n.simulate(g))
            n.extend_edges(g, true, nullptr, nullptr);
        m_root = &n;
        m_num_searched = 0u;
    }
    bool search(const uint n)
    {
        GameType& game = *m_game;
        uint remains = n;
        search_under_node(game, *m_root, remains, inf, inf);
        m_num_searched += n - remains;
        return m_root->found_mate(true);
    }

private:
    void search_under_node(
        GameType& g,
        NodeType& n,
        uint& remains,
        const uint thpn,
        const uint thdn)
    {
        const bool is_atk = (m_table.offence() == g.get_turn());
        while (remains && (n.pn(is_atk) < thpn) && (n.dn(is_atk) < thdn)) {
            const EdgeType* const e2 = n.get_edge_2nd();
            uint thpn_ch = thpn, thdn_ch = thdn;
            if (is_atk)
                thpn_ch = compute_th_child(thpn, e2 ? e2->m_pn : inf);
            else
                thdn_ch = compute_th_child(thdn, e2 ? e2->m_dn : inf);
            EdgeType* const e1 = n.select_an_edge();
            assert(e1 != nullptr);
            search_under_edge(g, *e1, remains, thpn_ch, thdn_ch);
            n.backprop(is_atk ? C::SQ_NA : g.get_checker_location());
        }
    }
    void search_under_edge(
        GameType& g,
        EdgeType& e,
        uint& remains,
        const uint thpn,
        const uint thdn)
    {
        g.apply_dfpn(e.m_action);
        const bool is_atk = (m_table.offence() == g.get_turn());
        NodeType& n = expand_and_simulate_if_necessary(g, is_atk, remains);
        search_under_node(g, n, remains, thpn, thdn);
        e.m_pn = n.pn(is_atk);
        e.m_dn = n.dn(is_atk);
        g.undo();
    }
    NodeType& expand_and_simulate_if_necessary(
        GameType& g, const bool offence, uint& remains)
    {
        // TODO: remove `offence` parameter
        NodeType& n = m_table.get_or_expand(g);
        if (n.num_edges() == 0u) {
            g.update_result_dfpn(1u, offence);
            if (!n.simulate(g)) {
                const NodeType *node_l = nullptr, *node_g = nullptr;
                m_table.lookup_lgstand(g, &node_l, &node_g);
                n.extend_edges(g, offence, node_l, node_g);
            }
            --remains;
        }
        return n;
    }

public: // utilities
    Searcher()
        : m_table(BLACK), m_game(nullptr), m_root(nullptr), m_num_searched(0u)
    {
    }
    bool is_ready() const
    {
        return static_cast<bool>(m_game);
    }
    bool found_conclusion() const
    {
        if (m_root == nullptr)
            return false;
        return m_root->found_conclusion();
    }
    bool found_mate() const
    {
        if (m_root == nullptr)
            return false;
        return m_root->found_mate(true);
    }
    bool found_no_mate() const
    {
        if (m_root == nullptr)
            return false;
        return m_root->found_no_mate(true);
    }
    uint get_search_count() const
    {
        return m_num_searched;
    }
    Move<Parameters> get_mate_move() const
    {
        if (found_mate())
            return m_root->get_edge_1st()->m_action;
        return Move<Parameters>();
    }
    std::vector<Move<Parameters>> get_mate_moves()
    {
        std::vector<Move<Parameters>> out{};
        if (found_mate())
            append_mate_moves(out, *m_game, *m_root->get_edge_1st());
        return out;
    }

private: // utilities
    static uint compute_th_child(const uint th, const uint n2)
    {
        return std::min(th, (n2 != inf) ? n2 + 1u : inf);
    }
    void append_mate_moves(
        std::vector<Move<Parameters>>& out, GameType& g, const EdgeType& e)
    {
        g.apply_nocheck(e.m_action);
        out.emplace_back(e.m_action);
        if (g.get_result() == ONGOING) {
            NodeType& n = m_table.get_or_expand(g);
            assert(n.num_edges() > 0);
            append_mate_moves(out, g, *n.get_edge_1st());
        }
        g.undo();
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_SEARCHER_HPP
