#ifndef VSHOGI_ENGINE_DFPN_NODE_HPP
#define VSHOGI_ENGINE_DFPN_NODE_HPP

#include "vshogi/common/game.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/engine/dfpn/edge.hpp"

namespace vshogi::engine::dfpn
{

template <class Parameters>
class Node
{
private:
    using C = Configuration<Parameters>;
    using EdgeType = Edge<Parameters>;
    using GameType = Game<Parameters>;
    using Square = typename C::Square;

    std::list<EdgeType> m_edges;
    EdgeType* m_edge_1st; //!< The best edge to select.
    EdgeType* m_edge_2nd;

    /**
     * @brief Total counter number, which is DN if offence otherwise PN.
     */
    uint m_total_cn;

public:
    void extend_edges(
        const Game<Parameters>& g,
        const bool& offence,
        const Node* node_l,
        const Node* node_g)
    {
        if (node_l && node_l->found_conclusion() && (node_l->num_edges() == 0u))
            node_l = nullptr;
        if (node_g && node_g->found_conclusion() && (node_g->num_edges() == 0u))
            node_g = nullptr;

        m_total_cn = internal::extend_edges(
            m_edges,
            g,
            offence,
            &m_edge_1st,
            &m_edge_2nd,
            node_l ? &node_l->m_edges : nullptr,
            node_g ? &node_g->m_edges : nullptr);
    }
    bool simulate(const Game<Parameters>& g)
    {
        const auto r = g.get_result();
        if (r == ONGOING)
            return false;
        if (r == DRAW) {
            m_total_cn = 0u;
            return true;
        }

        const auto winner = (r == BLACK_WIN) ? BLACK : WHITE;
        const auto turn = g.get_turn();
        m_total_cn = (winner == turn) ? inf : zero;
        return true;
    }
    /**
     * @brief Select a best edge to explore.
     * @note User must take care that this node has expanded its edges.
     * @return Edge<Parameters>*
     */
    Edge<Parameters>* select_an_edge()
    {
        return m_edge_1st;
    }
    void backprop(const Square& checker_sq)
    {
        assert(m_edge_1st != nullptr);
        m_total_cn = zero;
        if (checker_sq == C::SQ_NA)
            backprop_at_offence();
        else
            backprop_at_defence(checker_sq);
    }

private:
    void backprop_at_offence()
    {
        if (m_edge_1st->m_pn == zero) {
            m_total_cn = inf;
            return;
        }
        for (EdgeType& e : m_edges)
            internal::increment_with_guard(
                m_total_cn,
                internal::update_edges_1st_2nd_for_offence(
                    e, &m_edge_1st, &m_edge_2nd));
    }
    void backprop_at_defence(const Square& checker_sq)
    {
        if (m_edge_1st->m_dn == zero) {
            m_total_cn = inf;
            return;
        }
        backprop_at_defence_board(checker_sq);
        backprop_at_defence_drop(checker_sq);
    }
    void backprop_at_defence_board(const Square& checker_sq)
    {
        for (EdgeType& e : m_edges) {
            if (e.m_action.is_drop())
                break;
            internal::increment_with_guard(
                m_total_cn,
                internal::update_edges_1st_2nd_for_defence(
                    e, &m_edge_1st, &m_edge_2nd, checker_sq));
        }
    }
    void backprop_at_defence_drop(const Square& checker_sq)
    {
        // https://komorinfo.com/blog/proof-number-double-count/
        uint pn_max[C::num_squares] = {0u};
        for (EdgeType& e : m_edges) {
            if (!e.m_action.is_drop())
                continue;
            internal::update_edges_1st_2nd_for_defence(
                e, &m_edge_1st, &m_edge_2nd, checker_sq);
            const auto d = e.m_action.destination();
            if (pn_max[d] < e.m_pn)
                pn_max[d] = e.m_pn;
        }
        for (uint ii = C::num_squares; ii--;)
            internal::increment_with_guard(m_total_cn, pn_max[ii]);
    }

public: // utilities
    Node()
        : m_edges(), m_edge_1st(nullptr), m_edge_2nd(nullptr), m_total_cn(unit)
    {
    }
    uint num_edges() const
    {
        return m_edges.size();
    }
    uint pn(const bool& is_offence) const
    {
        if (is_offence) {
            if (m_total_cn == inf)
                return zero;
            return (m_edge_1st) ? m_edge_1st->m_pn : inf;
        } else {
            return m_total_cn;
        }
    }
    uint dn(const bool& is_offence) const
    {
        if (is_offence) {
            return m_total_cn;
        } else {
            if (m_total_cn == inf)
                return zero;
            return (m_edge_1st) ? m_edge_1st->m_dn : inf;
        }
    }
    bool found_mate(const bool& is_offence) const
    {
        return pn(is_offence) == zero;
    }
    bool found_no_mate(const bool& is_offence) const
    {
        return dn(is_offence) == zero;
    }
    bool found_conclusion() const
    {
        return (m_total_cn == zero) || (m_total_cn == inf);
    }
    const Edge<Parameters>* get_edge_1st() const
    {
        return m_edge_1st;
    }
    const Edge<Parameters>* get_edge_2nd() const
    {
        return m_edge_2nd;
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_NODE_HPP
