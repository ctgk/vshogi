#ifndef VSHOGI_ENGINE_DFPN_NODE_HPP
#define VSHOGI_ENGINE_DFPN_NODE_HPP

#include <list>

#include "vshogi/common/game.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/utils.hpp"

/**
 * @brief Namespace for DFPN algorithm.
 *
 * Depth-First Proof-Number algorithm.
 *
 * 1. Select a leaf node.
 *
 *      - Offence: argmin(#P of children).
 *
 *      - Defence: argmin(#D of children).
 *
 * 2. Simulate the game position.
 *
 *      - If checkmate proved, #P=zero,#D=inf
 *
 *      - If no mate proved, #P=inf,#D=zero
 *
 *      - otherwise, do nothing
 *
 * 3. Expand its children.
 *
 *      - Initialize all children by #P=unit,#D=unit
 *
 * 4. Back-propagation
 *
 *      - Offence: #P = min(#P of children), #D = sum(#D of children)
 *
 *      - Defence: #P = sum(#P of children), #D = min(#D of children)
 *
 * ref: https://qhapaq.hatenablog.com/entry/2020/07/18/201956
 */
namespace vshogi::engine::dfpn2
{

static constexpr uint zero = 0u;
static constexpr uint unit = 100u;
static constexpr uint cent = 1u;
static constexpr uint kilo = 1000u * unit;
static constexpr uint inf = std::numeric_limits<uint>::max();

template <class Parameters>
class Node;

namespace internal
{

template <class P>
inline void append_for_defence(
    std::list<Node<P>>& children,
    const Node<P>& cousin,
    const bool& stands_fewer)
{
    // note that the childs are offence node.
    // - (0, inf), if offence cousin proved mate with fewer stands.
    // - (inf, 0), if offence cousin proved no-mate with more stands.
    children.emplace_back(cousin.m_action);
    Node<P>& added = children.back();
    if (stands_fewer) {
        // - (0, inf), if offence cousin proved mate with fewer stands.
        added.m_pn = std::min(cousin.pn(), kilo);
        added.m_dn = std::max(cousin.dn(), unit);
    } else {
        // - (inf, 0), if offence cousin proved no-mate with more stands.
        added.m_pn = std::max(cousin.pn(), unit);
        added.m_dn = std::min(cousin.dn(), kilo);
    }
}

template <class P>
inline void
expand_children_at_defence_drop(std::list<Node<P>>& children, const Game<P>& g)
{
    for (Move<P> m : DropMoveGenerator<P>(g.get_state()))
        children.emplace_back(false, m);
}

template <class P>
inline void expand_children_at_defence_drop(
    std::list<Node<P>>& children,
    const Game<P>& g,
    const std::list<Node<P>>* const childs_g)
{
    if (childs_g == nullptr)
        return extend_edges_at_defence_drop(children, g);
    const State<P>& s = g.get_state();
    const Stand<P>& stand = g.get_stand(g.get_turn());
    for (const Node<P>& e : (*childs_g)) {
        if (!e.m_action.is_drop())
            continue;
        if (!stand.exist(e.m_action.source_piece()))
            continue;
        append_for_defence(children, e);
    }
}

template <class P>
inline uint expand_children_at_defence(
    std::list<Node<P>>& children,
    const Game<P>& g,
    const std::list<Node<P>>* const edges_l,
    const std::list<Node<P>>* const childs_g)
{
    uint total_pn = 0u;
    increment_with_guard(
        total_pn,
        extend_edges_at_defence_board(
            children, g, edges_l ? edges_l : childs_g));
    increment_with_guard(
        total_pn, extend_edges_at_defence_drop(children, g, childs_g));
    return total_pn;
}

template <class P>
inline void expand_children(
    std::list<Node<P>>& children,
    const Game<P>& g,
    const bool offence,
    const std::list<Node<P>>* const children_l = nullptr,
    const std::list<Node<P>>* const children_g = nullptr)
{
    if (offence)
        return expand_children_at_offence(children, g, children_l, children_g);
    else
        return expand_children_at_defence(children, g, children_l, children_g);
}

} // namespace internal

template <class Parameters>
class Node
{
private:
    using C = Configuration<Parameters>;
    using GameType = Game<Parameters>;
    using MoveType = Move<Parameters>;
    using Square = typename C::Square;

    const bool m_offence;
    const MoveType m_action;

    /**
     * @brief The first two childs are the best and second best.
     * @note There are no guarantees that the rest of the nodes are in order.
     */
    std::list<Node> m_children;

    /**
     * @brief Total counter number, which is #D if offence otherwise #P.
     */
    uint m_total_cn;

public:
    /**
     * @brief Select a best edge to explore.
     * @note
     *
     * - Offence: argmin(#P of children).
     *
     * - Defence: argmin(#D of children).
     *
     * @note Users must take care that this node has expanded its children.
     * @return Node<Parameters>*
     */
    Node* select()
    {
        return &m_children.front();
    }

    /**
     * @brief Simulate the game position.
     * @note
     *
     * - If proved checkmate, #P=0, #D=inf
     *
     * - If proved no mate, #P=inf, #D=0
     *
     * - Otherwise, do nothing
     *
     * @param g
     * @return true Proved checkmate or no-mate.
     * @return false Proved nothing yet.
     */
    bool simulate(
        const Game<Parameters>& g,
        const Node<Parameters>* const node_l = nullptr,
        const Node<Parameters>* const node_e = nullptr,
        const Node<Parameters>* const node_g = nullptr)
    {
        if (simulate_using_game(g))
            return true;
        return simulate_using_cousins(node_l, node_e, node_g);
    }

    /**
     * @brief Expand children under the node.
     * @note #P and #D of the node is not updated until you call `backprop()`.
     * @note Each of a child is initialized by (#P,#D)=
     *
     * - (unit, unit), if no cousin
     *
     * - (0, inf), if offence cousin proved mate with fewer stands.
     *
     * - (inf, 0), if offence cousin proved no-mate with more stands.
     *
     * - (0, inf), if defence cousin proved mate with more stands.
     *
     * - (inf, 0), if defence cousin proved no-mate with fewer stands.
     *
     * - (clamp(#P of cousin), clamp(#D of cousin)), otherwise.
     *
     * @param g
     * @param node_l
     * @param node_e
     * @param node_g
     * @return true Full expansion.
     * @return false Partial expansion.
     */
    bool expand_children(
        const Game<Parameters>& g,
        const Node* node_l = nullptr,
        const Node* node_ge = nullptr)
    {
        return internal::expand_children(
            m_children,
            g,
            m_offence,
            node_l ? &node_l->m_children : nullptr,
            node_ge ? &node_ge->m_children : nullptr);
    }

    /**
     * @brief Back-propagation.
     * @note Sort child nodes and update #P and #D.
     *
     * - Offence: #P = min(#P of children), #D = sum(#D of children)
     *
     * - Defence: #P = sum(#P of children), #D = min(#D of children)
     *
     * @param checker_sq
     */
    // void backprop(const Square& checker_sq)
    // {
    //     assert(m_edge_1st != nullptr);
    //     m_total_cn = zero;
    //     if (m_offence)
    //         backprop_at_offence();
    //     else
    //         backprop_at_defence(checker_sq);
    // }

private:
    bool simulate_using_game(const GameType& g)
    {
        const auto r = g.get_result();
        if (r == ONGOING)
            return false;
        if (r == DRAW) {
            m_total_cn = zero;
            return true;
        }

        const auto winner = (r == BLACK_WIN) ? BLACK : WHITE;
        const auto turn = g.get_turn();
        m_total_cn = (winner == turn) ? inf : zero;
        return true;
    }
    bool simulate_using_cousins(
        const Node* const node_l,
        const Node* const node_e,
        const Node* const node_g)
    {
        if (node_l && m_offence && node_l->proved_mate()) {
            m_total_cn = inf; // (#P, #D) = (0, inf)
            return true;
        } else if (node_l && (!m_offence) && node_l->proved_no_mate()) {
            m_total_cn = inf; // (#P, #D) = (inf, 0)
            return true;
        } else if (node_e && node_e->proved()) {
            m_total_cn = node_e->m_total_cn;
            return true;
        } else if (node_g && m_offence && node_g->proved_no_mate()) {
            m_total_cn = zero; // (#P, #D) = (inf, 0)
            return true;
        } else if (node_g && (!m_offence) && node_g->proved_mate()) {
            m_total_cn = zero; // (#P, #D) = (0, inf)
            return true;
        }
        return false;
    }
    // void backprop_at_offence()
    // {
    //     if (m_edge_1st->m_pn == zero) {
    //         m_total_cn = inf;
    //         return;
    //     }
    //     for (EdgeType& e : m_edges)
    //         internal::increment_with_guard(
    //             m_total_cn,
    //             internal::update_edges_1st_2nd_for_offence(
    //                 e, &m_edge_1st, &m_edge_2nd));
    // }
    // void backprop_at_defence(const Square& checker_sq)
    // {
    //     if (m_edge_1st->m_dn == zero) {
    //         m_total_cn = inf;
    //         return;
    //     }
    //     backprop_at_defence_board(checker_sq);
    //     backprop_at_defence_drop(checker_sq);
    // }
    // void backprop_at_defence_board(const Square& checker_sq)
    // {
    //     for (EdgeType& e : m_edges) {
    //         if (e.m_action.is_drop())
    //             break;
    //         internal::increment_with_guard(
    //             m_total_cn,
    //             internal::update_edges_1st_2nd_for_defence(
    //                 e, &m_edge_1st, &m_edge_2nd, checker_sq));
    //     }
    // }
    // void backprop_at_defence_drop(const Square& checker_sq)
    // {
    //     // https://komorinfo.com/blog/proof-number-double-count/
    //     uint pn_max[C::num_squares] = {0u};
    //     for (EdgeType& e : m_edges) {
    //         if (!e.m_action.is_drop())
    //             continue;
    //         internal::update_edges_1st_2nd_for_defence(
    //             e, &m_edge_1st, &m_edge_2nd, checker_sq);
    //         const auto d = e.m_action.destination();
    //         if (pn_max[d] < e.m_pn)
    //             pn_max[d] = e.m_pn;
    //     }
    //     for (uint ii = C::num_squares; ii--;)
    //         internal::increment_with_guard(m_total_cn, pn_max[ii]);
    // }

public: // utilities
    Node() : m_offence(true), m_action(), m_children(), m_total_cn(unit)
    {
    }
    Node(const bool offence, const Move<Parameters>& action)
        : m_offence(offence), m_action(action), m_children(), m_total_cn(unit)
    {
    }
    void init()
    {
        assert(m_offence);
        m_children.clear();
        m_total_cn = unit;
    }
    bool offence() const
    {
        return m_offence;
    }
    Move<Parameters> get_action() const
    {
        return m_action;
    }
    bool is_first_arrival() const
    {
        return m_children.empty() && (m_total_cn == unit);
    }
    uint num_children() const
    {
        return m_children.size();
    }
    uint pn() const
    {
        if (m_offence) {
            if (m_total_cn == inf)
                return zero;
            return (m_children.empty()) ? inf : get_child_1st()->pn();
        } else {
            return m_total_cn;
        }
    }
    uint dn() const
    {
        if (m_offence) {
            return m_total_cn;
        } else {
            if (m_total_cn == inf)
                return zero;
            return (m_children.empty()) ? inf : get_child_1st()->dn();
        }
    }
    bool proved_mate() const
    {
        return m_total_cn == (m_offence ? inf : zero);
    }
    bool proved_no_mate() const
    {
        return m_total_cn == (m_offence ? zero : inf);
    }
    bool proved() const
    {
        return (m_total_cn == zero) || (m_total_cn == inf);
    }
    const Node* get_child_1st() const
    {
        if (m_children.empty())
            return nullptr;
        return &m_children.front();
    }
    const Node* get_child_2nd() const
    {
        if (m_children.size() < 2u)
            return nullptr;
        return &*std::next(m_children.cbegin());
    }
};

} // namespace vshogi::engine::dfpn2

#endif // VSHOGI_ENGINE_DFPN_NODE_HPP
