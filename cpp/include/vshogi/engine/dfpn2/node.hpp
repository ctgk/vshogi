#ifndef VSHOGI_ENGINE_DFPN2_NODE_HPP
#define VSHOGI_ENGINE_DFPN2_NODE_HPP

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
inline void emplace_back_with_clamped_pn_dn(
    std::list<Node<P>>& children, const Node<P>& nibling)
{
    // Parameter names are name from the point of view of
    // the parent node.
    // Note that nibling node may have ended up in proving mate or no-mate
    // by special rules such as repetitions.
    children.emplace_back(
        nibling.offence(),
        nibling.get_action(),
        std::clamp(nibling.pn(), cent, kilo),
        std::clamp(nibling.dn(), cent, kilo));
}

template <class P>
inline void expand_children_at_offence_using_niblings(
    std::list<Node<P>>& children,
    const Game<P>& g,
    const std::list<Node<P>>& niblings)
{
    const Stand<P>& stand = g.get_stand(g.get_turn());
    for (const Node<P>& nibling : niblings) {
        const Move<P> m = nibling.get_action();
        if ((!m.is_drop()) || stand.exist(m.source_piece()))
            emplace_back_with_clamped_pn_dn(children, nibling);
    }
}

template <class P>
inline void expand_children_at_offence_using_board_niblings(
    std::list<Node<P>>& children, const std::list<Node<P>>& niblings)
{
    for (const Node<P>& nibling : niblings) {
        if (!nibling.get_action().is_drop())
            emplace_back_with_clamped_pn_dn(children, nibling);
    }
}

template <class P>
inline void
expand_children_at_offence(std::list<Node<P>>& children, const Game<P>& g)
{
    const State<P>& s = g.get_state();
    for (auto m : LegalMoveGenerator<P, true>(s)) {
        if (s.is_declined_promotion(m))
            children.emplace_back(false, m, kilo, cent);
        else
            children.emplace_back(false, m);
    }
}

template <class P>
inline void expand_children_at_offence(
    std::list<Node<P>>& children,
    const Game<P>& g,
    const std::list<Node<P>>* const niblings_l,
    const std::list<Node<P>>* const niblings_g)
{
    if (niblings_g)
        expand_children_at_offence_using_niblings(children, g, *niblings_g);
    else if (niblings_l) {
        expand_children_at_offence_using_board_niblings(children, *niblings_l);
        for (auto m : DropMoveGenerator<P, true>(g.get_state()))
            children.emplace_back(false, m);
    } else
        expand_children_at_offence(children, g);
}

template <class P>
inline void
expand_children_at_defence_board(std::list<Node<P>>& children, const Game<P>& g)
{
    const State<P>& s = g.get_state();
    for (Move<P> m : KingMoveGenerator<P>(s))
        children.emplace_back(true, m);
    for (Move<P> m : BlockMoveGenerator<P>(s))
        children.emplace_back(true, m);
}

template <class P>
inline void expand_children_at_defence_board(
    std::list<Node<P>>& children,
    const Game<P>& g,
    const std::list<Node<P>>* const niblings_gl)
{
    if (niblings_gl == nullptr)
        return expand_children_at_defence_board(children, g);
    for (const Node<P>& nibling : (*niblings_gl)) {
        if (nibling.get_action().is_drop())
            continue;
        emplace_back_with_clamped_pn_dn(children, nibling);
    }
}

template <class P>
inline void
expand_children_at_defence_drop(std::list<Node<P>>& children, const Game<P>& g)
{
    for (Move<P> m : DropMoveGenerator<P>(g.get_state()))
        children.emplace_back(true, m);
}

template <class P>
inline void expand_children_at_defence_drop(
    std::list<Node<P>>& children,
    const Game<P>& g,
    const std::list<Node<P>>* const niblings_g)
{
    if (niblings_g == nullptr)
        return expand_children_at_defence_drop(children, g);
    const Stand<P>& stand = g.get_stand(g.get_turn());
    for (const Node<P>& nibling : (*niblings_g)) {
        const auto m = nibling.get_action();
        if (m.is_drop() && stand.exist(m.source_piece()))
            emplace_back_with_clamped_pn_dn(children, nibling);
    }
}

template <class Parameters>
bool had_two_consecutive_sacrifice_drops(const Game<Parameters>& g)
{
    const uint n = g.record_length();
    if (n < 4u)
        return false;

    // first sacrifice drop
    const Move<Parameters> drop1st = g.get_record_action(n - 4u);
    if (!drop1st.is_drop())
        return false;

    // capture first sacrifice drop
    const Move<Parameters> capt1st = g.get_record_action(n - 3u);
    if (drop1st.destination() != capt1st.destination())
        return false;

    // second sacrifice drop
    const Move<Parameters> drop2nd = g.get_record_action(n - 2u);
    if (!drop2nd.is_drop())
        return false;

    // capture second sacrifice drop
    const Move<Parameters> capt2nd = g.get_record_action(n - 1u);
    return (drop2nd.destination() == capt2nd.destination())
           && (capt1st.destination() == capt2nd.source_square());
}

template <class P>
inline bool expand_children_at_defence(
    std::list<Node<P>>& children,
    const Game<P>& g,
    const std::list<Node<P>>* const niblings_l = nullptr,
    const std::list<Node<P>>* const niblings_g = nullptr)
{
    expand_children_at_defence_board(
        children, g, niblings_l ? niblings_l : niblings_g);
    if (!had_two_consecutive_sacrifice_drops(g)) {
        expand_children_at_defence_drop(children, g, niblings_g);
        return true;
    }
    return false;
}

} // namespace internal

template <class Parameters>
class Node
{
private:
    using C = Configuration<Parameters>;
    using SHelper = Squares<Parameters>;
    using GameType = Game<Parameters>;
    using MoveType = Move<Parameters>;
    using Square = typename C::Square;

    const bool m_offence;
    const MoveType m_action;
    uint m_pn;
    uint m_dn;

    /**
     * @brief The first two childs are the best and second best.
     * @note There are no guarantees that the rest of the nodes are in order.
     */
    std::list<Node> m_children;

public:
    /**
     * @brief Select a best child to explore.
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
    uint compute_child_thpn(const uint thpn) const
    {
        // https://webdocs.cs.ualberta.ca/~mmueller/ps/ICGA2012PNS.pdf pg6
        if (m_offence) {
            const auto c2 = get_child_2nd();
            if ((c2 == nullptr) || (c2->m_pn == inf))
                return thpn;
            return std::min(thpn, c2->m_pn + 1u);
        } else {
            return thpn - m_pn + get_child_1st()->pn();
        }
    }
    uint compute_child_thdn(const uint thdn) const
    {
        // https://webdocs.cs.ualberta.ca/~mmueller/ps/ICGA2012PNS.pdf pg6
        if (m_offence) {
            return thdn - m_dn + get_child_1st()->dn();
        } else {
            const auto c2 = get_child_2nd();
            if ((c2 == nullptr) || (c2->m_dn == inf))
                return thdn;
            return std::min(thdn, c2->m_dn + 1u);
        }
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
        const auto niblings_l = node_l ? &node_l->m_children : nullptr;
        const auto niblings_ge = node_ge ? &node_ge->m_children : nullptr;
        if (m_offence) {
            internal::expand_children_at_offence(
                m_children, g, niblings_l, niblings_ge);
            assert(std::all_of(
                m_children.cbegin(), m_children.cend(), [](const Node& n) {
                    return !n.offence();
                }));
            return true;
        } else {
            const bool out = internal::expand_children_at_defence(
                m_children, g, niblings_l, niblings_ge);
            assert(std::all_of(
                m_children.cbegin(), m_children.cend(), [](const Node& n) {
                    return n.offence();
                }));
            return out;
        }
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
    void backprop(const Game<Parameters>& g)
    {
        if (m_children.size() == 1u) {
            m_pn = m_children.front().pn();
            m_dn = m_children.front().dn();
        } else if (m_offence)
            backprop_at_offence();
        else
            backprop_at_defence(
                g.get_checker_location(), g.get_king_location());
    }

private:
    bool simulate_using_game(const GameType& g)
    {
        const auto r = g.get_result();
        if (r == ONGOING)
            return false;
        if (r == DRAW) {
            m_pn = inf;
            m_dn = zero;
            return true;
        }

        const auto winner = (r == BLACK_WIN) ? BLACK : WHITE;
        const auto turn = g.get_turn();
        if (m_offence == (winner == turn)) {
            m_pn = zero;
            m_dn = inf;
        } else {
            m_pn = inf;
            m_dn = zero;
        }
        return true;
    }
    bool simulate_using_cousins(
        const Node* const node_l,
        const Node* const node_e,
        const Node* const node_g)
    {
        if (node_l && m_offence && node_l->proved_mate()) {
            m_pn = zero;
            m_dn = inf;
            return true;
        } else if (node_l && (!m_offence) && node_l->proved_no_mate()) {
            m_pn = inf;
            m_dn = zero;
            return true;
        } else if (node_e && node_e->proved()) {
            m_pn = node_e->m_pn;
            m_dn = node_e->m_dn;
            return true;
        } else if (node_g && m_offence && node_g->proved_no_mate()) {
            m_pn = inf;
            m_dn = zero;
            return true;
        } else if (node_g && (!m_offence) && node_g->proved_mate()) {
            m_pn = zero;
            m_dn = inf;
            return true;
        }
        return false;
    }
    void backprop_at_offence()
    {
        if (m_children.empty()) {
            m_pn = inf;
            m_dn = zero;
            return;
        }
        if (m_children.front().m_pn == zero) {
            m_pn = zero;
            m_dn = inf;
            return;
        }
        auto it_1st = m_children.end();
        auto it_2nd = m_children.end();
        m_dn = zero;
        for (auto it = m_children.begin(); it != m_children.end(); ++it) {
            assert(it->dn() != inf);
            m_dn += it->dn();
            if ((it_1st == m_children.end()) || it->is_better_than(*it_1st)) {
                it_2nd = it_1st;
                it_1st = it;
            } else if (
                (it_2nd == m_children.end()) || it->is_better_than(*it_2nd)) {
                it_2nd = it;
            }
        }
        if (it_2nd != m_children.begin())
            m_children.splice(m_children.begin(), m_children, it_2nd);
        if (it_1st != m_children.begin())
            m_children.splice(m_children.begin(), m_children, it_1st);
        m_pn = m_children.front().pn();
    }
    bool is_better_than(const Node& other) const
    {
        if (m_pn < other.m_pn)
            return true;
        if (m_pn > other.m_pn)
            return false;
        return m_action.is_drop() && (!other.m_action.is_drop());
    }
    void backprop_at_defence(const Square& checker_sq, const Square& king_sq)
    {
        if (m_children.empty()) {
            m_pn = zero;
            m_dn = inf;
            return;
        }
        if (m_children.front().m_dn == zero) {
            m_pn = inf;
            m_dn = zero;
            return;
        }
        auto it_1st = m_children.end();
        auto it_2nd = m_children.end();
        m_pn = zero;
        uint pn_max_drop_at[C::num_squares] = {zero};
        for (auto it = m_children.begin(); it != m_children.end(); ++it) {
            assert(it->pn() != inf);
            const auto m = it->get_action();
            if (m.is_drop()) {
                const auto d = m.destination();
                if (pn_max_drop_at[d] < it->pn())
                    pn_max_drop_at[d] = it->pn();
            } else {
                m_pn += it->pn();
            }
            if ((it_1st == m_children.end())
                || it->is_better_than(*it_1st, checker_sq, king_sq)) {
                it_2nd = it_1st;
                it_1st = it;
            } else if (
                (it_2nd == m_children.end())
                || it->is_better_than(*it_2nd, checker_sq, king_sq)) {
                it_2nd = it;
            }
        }
        for (uint ii = C::num_squares; ii--;)
            m_pn += pn_max_drop_at[ii];
        if (m_pn == zero) {
            m_children.splice(
                m_children.begin(), m_children, std::prev(m_children.end()));
        } else {
            if (it_2nd != m_children.begin())
                m_children.splice(m_children.begin(), m_children, it_2nd);
            if (it_1st != m_children.begin())
                m_children.splice(m_children.begin(), m_children, it_1st);
        }
        m_dn = m_children.front().dn();
    }
    bool is_better_than(
        const Node& other,
        const Square& checker_sq,
        const Square& king_sq) const
    {
        if (m_dn < other.m_dn)
            return true;
        if (m_dn > other.m_dn)
            return false;
        const auto td = this->m_action.destination();
        const auto od = other.m_action.destination();
        if ((td == checker_sq) && (od != checker_sq))
            return true;
        if ((td != checker_sq) && (od == checker_sq))
            return false;
        return (
            SHelper::chebyshev_distance(td, king_sq)
            < SHelper::chebyshev_distance(od, king_sq));
    }

public: // utilities
    Node() : m_offence(true), m_action(), m_pn(unit), m_dn(unit), m_children()
    {
    }
    Node(const bool offence, const Move<Parameters>& action)
        : m_offence(offence), m_action(action), m_pn(unit), m_dn(unit),
          m_children()
    {
    }
    Node(
        const bool offence,
        const Move<Parameters>& action,
        const uint pn,
        const uint dn)
        : m_offence(offence), m_action(action), m_pn(pn), m_dn(dn), m_children()
    {
    }
    void init()
    {
        assert(m_offence);
        m_children.clear();
        m_pn = unit;
        m_dn = unit;
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
        return m_children.empty() && (!proved());
    }
    uint num_children() const
    {
        return static_cast<uint>(m_children.size());
    }
    uint pn() const
    {
        return m_pn;
    }
    uint dn() const
    {
        return m_dn;
    }
    bool proved_mate() const
    {
        return m_pn == zero;
    }
    bool proved_no_mate() const
    {
        return m_dn == zero;
    }
    bool proved() const
    {
        return (m_pn == zero) || (m_dn == zero);
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
    const std::list<Node>& get_children() const
    {
        return m_children;
    }
};

} // namespace vshogi::engine::dfpn2

#endif // VSHOGI_ENGINE_DFPN2_NODE_HPP
