#ifndef VSHOGI_ENGINE_DFPN_EDGE_HPP
#define VSHOGI_ENGINE_DFPN_EDGE_HPP

#include <list>

#include "vshogi/common/config.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/move.hpp"
#include "vshogi/common/utils.hpp"

namespace vshogi::engine::dfpn
{

static constexpr uint zero = 0u;
static constexpr uint unit = 100u;
static constexpr uint cent = 1u;
static constexpr uint kilo = 1000u * unit;
static constexpr uint inf = std::numeric_limits<uint>::max();

template <class Parameters>
class Node;

template <class Parameters>
struct Edge
{

    /**
     * @brief `p_node -> m_action -> c_node`
     */
    const Move<Parameters> m_action;

    /**
     * @brief Number of searches required to prove child node leads to mate.
     */
    uint m_pn;

    /**
     * @brief Number of searches required to prove child node leads to no mate.
     */
    uint m_dn;

    Edge(const Move<Parameters>& m) : m_action(m), m_pn(unit), m_dn(unit)
    {
    }
};

namespace internal
{

template <class Parameters>
inline uint update_edges_1st_2nd_for_offence(
    Edge<Parameters>& e, Edge<Parameters>** e1, Edge<Parameters>** e2)
{
    if ((*e1 == nullptr) || (e.m_pn < (*e1)->m_pn)) {
        *e2 = *e1;
        *e1 = &e;
    } else if ((*e2 == nullptr) || (e.m_pn < (*e2)->m_pn)) {
        *e2 = &e;
    }
    return e.m_dn;
}

template <class Parameters>
inline bool is_better_dn_than(
    const Edge<Parameters>& a,
    const Edge<Parameters>* const b,
    const typename Configuration<Parameters>::Square checker_sq)
{
    if (b == nullptr)
        return true;
    if (a.m_dn < b->m_dn)
        return true;
    if (a.m_dn > b->m_dn)
        return false;
    return (a.m_action.destination() == checker_sq)
           && (b->m_action.destination() != checker_sq);
}

template <class Parameters>
inline uint update_edges_1st_2nd_for_defence(
    Edge<Parameters>& e,
    Edge<Parameters>** e1,
    Edge<Parameters>** e2,
    const typename Configuration<Parameters>::Square checker_sq)
{
    if (is_better_dn_than(e, *e1, checker_sq)) {
        *e2 = *e1;
        *e1 = &e;
    } else if (is_better_dn_than(e, *e2, checker_sq)) {
        *e2 = &e;
    }
    return e.m_pn;
}

template <class P>
inline uint append_for_offence(
    std::list<Edge<P>>& edges, const Move<P>& m, Edge<P>** e1, Edge<P>** e2)
{
    edges.emplace_back(m);
    Edge<P>& added = edges.back();
    return update_edges_1st_2nd_for_offence(added, e1, e2);
}

template <class P>
inline uint append_for_offence(
    std::list<Edge<P>>& edges,
    const Move<P>& m,
    Edge<P>** e1,
    Edge<P>** e2,
    const uint pn,
    const uint dn)
{
    edges.emplace_back(m);
    Edge<P>& added = edges.back();
    added.m_pn = pn;
    added.m_dn = dn;
    return update_edges_1st_2nd_for_offence(added, e1, e2);
}

template <class P>
inline uint append_for_offence(
    std::list<Edge<P>>& edges, const Edge<P>& e, Edge<P>** e1, Edge<P>** e2)
{
    edges.emplace_back(e.m_action);
    Edge<P>& added = edges.back();
    added.m_pn = std::clamp(e.m_pn, cent, kilo);
    added.m_dn = std::clamp(e.m_dn, cent, kilo);
    return update_edges_1st_2nd_for_offence(added, e1, e2);
}

template <class P>
inline uint append_for_defence(
    std::list<Edge<P>>& edges,
    const Move<P>& m,
    Edge<P>** e1,
    Edge<P>** e2,
    const typename Configuration<P>::Square checker_sq)
{
    edges.emplace_back(m);
    Edge<P>& added = edges.back();
    return update_edges_1st_2nd_for_defence(added, e1, e2, checker_sq);
}

template <class P>
inline uint append_for_defence(
    std::list<Edge<P>>& edges,
    const Edge<P>& e,
    Edge<P>** e1,
    Edge<P>** e2,
    const typename Configuration<P>::Square checker_sq)
{
    edges.emplace_back(e.m_action);
    Edge<P>& added = edges.back();
    added.m_pn = std::clamp(e.m_pn, cent, kilo);
    added.m_dn = std::clamp(e.m_dn, cent, kilo);
    return update_edges_1st_2nd_for_defence(added, e1, e2, checker_sq);
}

inline void increment_with_guard(uint& a, const uint b)
{
    if (a == inf)
        return;
    if (b == inf)
        a = inf;
    else
        a += b;
}

template <class P>
inline uint extend_edges_at_offence_drop(
    std::list<Edge<P>>& edges,
    const State<P>& state,
    Edge<P>** e1,
    Edge<P>** e2)
{
    uint total_dn = 0u;
    for (Move<P> m : DropMoveGenerator<P, true>(state))
        increment_with_guard(total_dn, append_for_offence(edges, m, e1, e2));
    return total_dn;
}

template <class P>
inline uint extend_edges_at_offence_drop(
    std::list<Edge<P>>& edges,
    const State<P>& state,
    Edge<P>** e1,
    Edge<P>** e2,
    const std::list<Edge<P>>* const edges_g)
{
    if (edges_g == nullptr)
        return extend_edges_at_offence_drop(edges, state, e1, e2);
    uint total_dn = 0u;
    const Stand<P>& stand = state.get_stand(state.get_turn());
    for (const Edge<P>& e : (*edges_g)) {
        if (!e.m_action.is_drop())
            break;
        if (!stand.exist(e.m_action.source_piece()))
            continue;
        increment_with_guard(total_dn, append_for_offence(edges, e, e1, e2));
    }
    return total_dn;
}

template <class P>
inline uint extend_edges_at_offence_board(
    std::list<Edge<P>>& edges,
    const State<P>& state,
    Edge<P>** e1,
    Edge<P>** e2)
{
    uint total_dn = 0u;
    for (Move<P> m : CheckBoardMoveGenerator<P>(state)) {
        if (state.is_declined_promotion(m))
            increment_with_guard(
                total_dn, append_for_offence(edges, m, e1, e2, kilo, cent));
        else
            increment_with_guard(
                total_dn, append_for_offence(edges, m, e1, e2));
    }
    return total_dn;
}

template <class Parameters>
inline uint extend_edges_at_offence_board(
    std::list<Edge<Parameters>>& edges,
    const State<Parameters>& state,
    Edge<Parameters>** e1,
    Edge<Parameters>** e2,
    const std::list<Edge<Parameters>>* const edges_gl)
{
    if (edges_gl == nullptr)
        return extend_edges_at_offence_board(edges, state, e1, e2);
    uint total_dn = 0u;
    for (const Edge<Parameters>& e : (*edges_gl)) {
        if (e.m_action.is_drop())
            continue;
        increment_with_guard(total_dn, append_for_offence(edges, e, e1, e2));
    }
    return total_dn;
}

template <class Parameters>
inline uint extend_edges_at_offence(
    std::list<Edge<Parameters>>& edges,
    const Game<Parameters>& g,
    Edge<Parameters>** e1,
    Edge<Parameters>** e2,
    const std::list<Edge<Parameters>>* const edges_l = nullptr,
    const std::list<Edge<Parameters>>* const edges_g = nullptr)
{
    uint total_dn = 0u;
    const auto& s = g.get_state();
    increment_with_guard(
        total_dn, extend_edges_at_offence_drop(edges, s, e1, e2, edges_g));
    increment_with_guard(
        total_dn,
        extend_edges_at_offence_board(
            edges, s, e1, e2, edges_g ? edges_g : edges_l));
    return total_dn;
}

template <class Parameters>
inline uint extend_edges_at_defence_board(
    std::list<Edge<Parameters>>& edges,
    const Game<Parameters>& g,
    Edge<Parameters>** e1,
    Edge<Parameters>** e2)
{
    uint total_pn = zero;
    const State<Parameters>& s = g.get_state();
    const auto c = s.get_checker_location();
    for (Move<Parameters> m : KingMoveGenerator<Parameters>(s))
        increment_with_guard(total_pn, append_for_defence(edges, m, e1, e2, c));
    for (Move<Parameters> m : BlockMoveGenerator<Parameters>(s))
        increment_with_guard(total_pn, append_for_defence(edges, m, e1, e2, c));
    return total_pn;
}

template <class Parameters>
inline uint extend_edges_at_defence_board(
    std::list<Edge<Parameters>>& edges,
    const Game<Parameters>& g,
    Edge<Parameters>** e1,
    Edge<Parameters>** e2,
    const std::list<Edge<Parameters>>* const edges_gl)
{
    if (edges_gl == nullptr)
        return extend_edges_at_defence_board(edges, g, e1, e2);
    uint total_pn = zero;
    const State<Parameters>& s = g.get_state();
    const auto c = s.get_checker_location();
    assert(c != Configuration<Parameters>::SQ_NA);

    for (const Edge<Parameters>& e : (*edges_gl)) {
        if (e.m_action.is_drop())
            break;
        increment_with_guard(total_pn, append_for_defence(edges, e, e1, e2, c));
    }
    return total_pn;
}

template <class P>
inline uint extend_edges_at_defence_drop(
    std::list<Edge<P>>& edges, const Game<P>& g, Edge<P>** e1, Edge<P>** e2)
{
    using C = Configuration<P>;
    const auto c = g.get_checker_location();
    uint pn_max[C::num_squares] = {0u};
    for (Move<P> m : DropMoveGenerator<P>(g.get_state())) {
        const uint pn = append_for_defence(edges, m, e1, e2, c);
        const auto d = m.destination();
        if (pn_max[d] < pn)
            pn_max[d] = pn;
    }

    uint total_pn = zero;
    for (uint ii = C::num_squares; ii--;)
        increment_with_guard(total_pn, pn_max[ii]);
    return total_pn;
}

template <class P>
inline uint extend_edges_at_defence_drop(
    std::list<Edge<P>>& edges,
    const Game<P>& g,
    Edge<P>** e1,
    Edge<P>** e2,
    const std::list<Edge<P>>* const edges_g)
{
    using C = Configuration<P>;

    if (edges_g == nullptr)
        return extend_edges_at_defence_drop(edges, g, e1, e2);
    // https://komorinfo.com/blog/proof-number-double-count/
    uint pn_max[C::num_squares] = {0u};
    const State<P>& s = g.get_state();
    const auto c = s.get_checker_location();
    const Stand<P>& stand = g.get_stand(g.get_turn());
    for (const Edge<P>& e : (*edges_g)) {
        if (!e.m_action.is_drop())
            continue;
        if (!stand.exist(e.m_action.source_piece()))
            continue;
        const uint pn = append_for_defence(edges, e, e1, e2, c);
        const auto d = e.m_action.destination();
        if (pn_max[d] < pn)
            pn_max[d] = pn;
    }
    uint total_pn = zero;
    for (uint ii = C::num_squares; ii--;)
        increment_with_guard(total_pn, pn_max[ii]);
    return total_pn;
}

template <class P>
inline uint extend_edges_at_defence(
    std::list<Edge<P>>& edges,
    const Game<P>& g,
    Edge<P>** e1,
    Edge<P>** e2,
    const std::list<Edge<P>>* const edges_l,
    const std::list<Edge<P>>* const edges_g)
{
    uint total_pn = 0u;
    increment_with_guard(
        total_pn,
        extend_edges_at_defence_board(
            edges, g, e1, e2, edges_l ? edges_l : edges_g));
    increment_with_guard(
        total_pn, extend_edges_at_defence_drop(edges, g, e1, e2, edges_g));
    return total_pn;
}

template <class P>
inline uint extend_edges(
    std::list<Edge<P>>& edges,
    const Game<P>& g,
    const bool offence,
    Edge<P>** e1,
    Edge<P>** e2,
    const std::list<Edge<P>>* const edges_l = nullptr,
    const std::list<Edge<P>>* const edges_g = nullptr)
{
    if (offence)
        return extend_edges_at_offence(edges, g, e1, e2, edges_l, edges_g);
    else
        return extend_edges_at_defence(edges, g, e1, e2, edges_l, edges_g);
}

} // namespace internal

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_EDGE_HPP
