#ifndef VSHOGI_ENGINE_DFPN_NODE_HPP
#define VSHOGI_ENGINE_DFPN_NODE_HPP

#include <memory>

#include "vshogi/common/config.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/generators/drop.hpp"
#include "vshogi/common/square_traits.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/engine/tree/node.hpp"

/**
 * @brief Df-pn algorithm
 *
 * https://webdocs.cs.ualberta.ca/~mmueller/ps/ICGA2012PNS.pdf
 */
namespace vshogi::engine::dfpn
{

static constexpr uint zero = 0u;
static constexpr uint cent = 1u << 8u;
static constexpr uint unit = 100u * cent;
static constexpr uint kilo = 1000u * unit;
static constexpr uint inf = std::numeric_limits<uint>::max();

class Node : public tree::Node<Node>
{
public: // utility
    Node();
    void init();
    void init(
        Node* const parent,
        const move_t& action,
        const uint phi,
        const uint delta);
    void init(Node* const parent, const Node* const twin);
    // clang-format off
    uint phi() const { return m_phi; }
    uint delta() const { return (m_delta == inf) ? m_delta : (m_delta >> 8u) << 8u; }
    uint pn(const bool offence) const { return offence ? m_phi : delta(); }
    uint dn(const bool offence) const { return offence ? delta() : m_phi; }
    bool proved() const { return (m_phi == zero) || (m_delta == zero); }
    bool proved_mate(const bool offence) const { return pn(offence) == zero; }
    bool proved_no_mate(const bool offence) const { return dn(offence) == zero; }
    bool fully_expanded() const { return m_fully_expanded; }
    const Node* get_child_2nd() const { return m_child_2nd; }
    // clang-format on

    template <class P>
    bool simulate(
        const Game<P>& g,
        const Node* const twin_ge = nullptr,
        const Node* const twin_le = nullptr);
    template <class P>
    void expand(
        Node*& next,
        const Game<P>& g,
        const Node* const twin_ge = nullptr,
        const Node* const twin_le = nullptr);
    Node*
    select(const uint th_p, const uint th_d, uint& th_p_ch, uint& th_d_ch);

    /**
     * @brief Update #P and #D based on the values of child nodes.
     */
    template <class P>
    void backprop(const bool& offence);

private:
    /**
     * @brief #P (or #D).
     * @details
     *           fedcba98 76543210 fedcba98 76543210
     *           ******** ******** ******** ________       Valid delta value
     * (MSB) ... xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx (LSB)
     */
    uint m_phi;

    /**
     * @brief #D (or #P) with some extra in the least significant 8 bits.
     * @details
     *           fedcba98 76543210 fedcba98 76543210
     *           ________ ________ ________ __******       chebyshev(king, dst)
     *           ________ ________ ________ _*______       drop (or capture)
     *           ******** ******** ******** ________       Valid delta value
     * (MSB) ... xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx (LSB)
     */
    uint m_delta;
    bool m_fully_expanded; //!< Omitted drop moves if false.

    Node* m_child_2nd;

    bool
    simulate_using_twins(const Node* const twin_ge, const Node* const twin_le);
    void set_mate(const bool offence);
    void set_no_mate(const bool offence);
    template <class P>
    bool simulate_using_game(const Game<P>& g);

    template <class P>
    bool expand_board_moves(
        const bool offence,
        Node*& next,
        const State<P>& s,
        const Node** const nibling);
    template <class P>
    bool expand_board_moves(Node*& next, const Node** const nibling);
    template <class P>
    bool expand_drop_moves(
        const bool offence,
        Node*& next,
        const State<P>& s,
        const Node** const nibling);
    template <class P>
    bool expand_drop_moves(
        Node*& next, const Node** const nibling, const Stand<P>& s);
    template <class G, class P>
    bool expand_by_generator(Node*& next, const State<P>& s);
};

inline Node::Node()
    : tree::Node<Node>(), m_phi{unit}, m_delta{unit}, m_fully_expanded{},
      m_child_2nd{}
{
}

inline void Node::init()
{
    tree::Node<Node>::init();
    m_phi = unit;
    m_delta = unit;
    m_fully_expanded = false;
    m_child_2nd = nullptr;
}

inline void Node::init(
    Node* const parent, const move_t& action, const uint phi, const uint delta)
{
    init();
    m_parent = parent;
    m_action = action;
    m_phi = phi;
    m_delta = delta;
}

inline void Node::init(Node* const parent, const Node* const twin)
{
    init();
    m_parent = parent;
    m_action = twin->get_action();
    m_phi = std::clamp(twin->phi(), cent, kilo);
    m_delta = std::clamp(twin->delta(), cent, kilo);
}

inline Node*
Node::select(const uint th_p, const uint th_d, uint& th_p_ch, uint& th_d_ch)
{
    assert(m_child_1st);
    constexpr uint d2_max = inf - 1u;
    const uint d2 = m_child_2nd ? m_child_2nd->delta() : inf;
    th_p_ch = th_d - delta() + m_child_1st->m_phi;
    th_d_ch = std::min(th_p, std::min(d2, d2_max) + 1u);
    return m_child_1st;
}

template <class P>
bool Node::simulate(
    const Game<P>& g, const Node* const twin_ge, const Node* const twin_le)
{
    if (simulate_using_twins(twin_ge, twin_le))
        return true;
    if (has_child())
        return false;
    return simulate_using_game(g);
}

inline bool
Node::simulate_using_twins(const Node* const twin_ge, const Node* const twin_le)
{
    // proved_no_mate_at_offence() == (dn() == zero) == (delta == zero)
    // proved_mate_at_defence() == (pn() == zero) == (delta == zero)
    if (twin_ge && (twin_ge->m_delta == zero)) {
        m_phi = inf;
        m_delta = zero;
        return true;
    }

    // proved_mate_at_offence() == (pn() == zero) == (phi == zero)
    // proved_no_mate_at_defence() == (dn() == zero) == (phi == zero)
    if (twin_le && (twin_le->m_phi == zero)) {
        m_phi = zero;
        m_delta = inf;
        return true;
    }
    return false;
}

template <class P>
bool Node::simulate_using_game(const Game<P>& g)
{
    const bool offence = (g.ply() % 2u == 0u);
    const auto turn = g.get_turn();
    auto result = g.get_result(); // this is usually ONGOING
    if (g.is_repetitions(1u)) {
        if (g.in_check())
            result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
        else
            result = DRAW;
    } else if (g.can_declare_win_by_king_enter())
        result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;

    if (result == ONGOING)
        return false;
    if (result == DRAW) {
        set_no_mate(offence);
        return true;
    }
    const auto winner = (result == BLACK_WIN) ? BLACK : WHITE;
    if (offence == (winner == turn)) {
        set_mate(offence);
    } else {
        set_no_mate(offence);
    }
    return true;
}

inline void Node::set_mate(const bool offence)
{
    if (offence) {
        m_phi = zero;
        m_delta = inf;
    } else {
        m_phi = inf;
        m_delta = zero;
    }
}
inline void Node::set_no_mate(const bool offence)
{
    if (offence) {
        m_phi = inf;
        m_delta = zero;
    } else {
        m_phi = zero;
        m_delta = inf;
    }
}

template <class P>
void Node::expand(
    Node*& next,
    const Game<P>& g,
    const Node* const twin_ge,
    const Node* const twin_le)
{
    using Square = typename Configuration<P>::Square;
    using MT = MoveTraits<P>;
    using ST = SquareTraits<P>;
    const bool offence = (g.ply() % 2u == 0u);
    const State<P>& s = g.get_state();
    const Node* nibling = nullptr;
    if (twin_ge)
        nibling = twin_ge->get_child();
    else if (twin_le)
        nibling = twin_le->get_child();

    m_fully_expanded = false;
    m_child = next;
    expand_board_moves<P>(offence, next, s, &nibling);
    if (offence || (!g.had_two_consecutive_sacrifice_drops())) {
        if ((twin_ge == nullptr) || (!twin_ge->fully_expanded()))
            nibling = nullptr;
        m_fully_expanded = expand_drop_moves<P>(offence, next, s, &nibling);
    }
    if (m_child == next)
        m_child = nullptr;
    assert(next);
    if (!next->is_end())
        next->m_parent = nullptr;

    const int offset = offence ? 19 : 10;
    const Square king_sq
        = g.get_state().get_king_square(offence ? ~g.get_turn() : g.get_turn());
    const Square checker = g.get_state().find_checker_square();
    for (Node* c = m_child; c && (c->m_parent == this); ++c) {
        uint delta_plus = 0u;
        const auto m = c->get_action();
        const auto dst = MT::get_dst(m);
        if ((offence && !MT::is_drop(m)) || (!offence && (dst != checker)))
            delta_plus = 0x40u; // offence (defence) prefers drop (capturing)
        const auto d = static_cast<int>(ST::chebyshev_distance(dst, king_sq));
        delta_plus
            ^= std::min(0x3fu, static_cast<uint>(std::abs(10 * d - offset)));
        c->m_delta ^= delta_plus;
    }
}

template <class P>
bool Node::expand_board_moves(
    const bool offence,
    Node*& next,
    const State<P>& s,
    const Node** const nibling)
{
    if ((*nibling) != nullptr)
        return expand_board_moves<P>(next, nibling);
    if (offence)
        return expand_by_generator<BoardMoveGenerator<P, GenEnum::CHECK>>(
            next, s);
    expand_by_generator<KingMoveGenerator<P>>(next, s);
    return expand_by_generator<SoldierMoveGenerator<P, GenEnum::EVADE>>(
        next, s);
}

template <class P>
bool Node::expand_board_moves(Node*& next, const Node** const nibling)
{
    using MT = MoveTraits<P>;
    for (; *nibling; *nibling = (*nibling)->get_sibling()) {
        if (next->is_end())
            return false;
        if (MT::is_drop((*nibling)->get_action()))
            break;
        next->init(this, *nibling);
        ++next;
    }
    return true;
}

template <class P>
bool Node::expand_drop_moves(
    const bool offence,
    Node*& next,
    const State<P>& s,
    const Node** const nibling)
{
    if ((*nibling) != nullptr)
        return expand_drop_moves(next, nibling, s.get_stand(s.get_turn()));
    if (offence)
        return expand_by_generator<DropMoveGenerator<P, GenEnum::CHECK>>(
            next, s);
    return expand_by_generator<DropMoveGenerator<P, GenEnum::EVADE>>(next, s);
}

template <class P>
bool Node::expand_drop_moves(
    Node*& next, const Node** const nibling, const Stand<P>& s)
{
    using MT = MoveTraits<P>;
    for (; *nibling; *nibling = (*nibling)->get_sibling()) {
        if (next->is_end())
            return false;
        assert(MT::is_drop((*nibling)->get_action()));
        if (!s.exist(MT::get_src_pt((*nibling)->get_action())))
            continue;
        next->init(this, *nibling);
        ++next;
    }
    return true;
}

template <class G, class P>
bool Node::expand_by_generator(Node*& next, const State<P>& s)
{
    for (auto it = G(s); it; ++it) {
        const auto m = *it;
        if (next->is_end())
            return false;
        if (s.is_declined_promotion(m))
            next->init(this, m, cent, kilo);
        else
            next->init(this, m, unit, unit);
        ++next;
    }
    return true;
}

template <class P>
void Node::backprop(const bool& offence)
{
    using C = Configuration<P>;
    using MT = MoveTraits<P>;
    if (m_child_1st && (m_child_1st->delta() == zero)) {
        m_phi = zero;
        m_delta = inf;
        return;
    }
    uint cd_max = 0u;
    uint delta_max[C::num_squares] = {zero};
    m_delta &= 0xffu; // keep the least significant 8 bits.
    m_child_1st = nullptr;
    m_child_2nd = nullptr;
    for (Node* ch = m_child; ch && (ch->m_parent == this); ++ch) {
        assert(ch->m_phi != inf);
        if (offence || (!MT::is_drop(ch->m_action)))
            m_delta += ch->m_phi;
        else {
            const auto cd = MT::get_dst(ch->m_action);
            delta_max[cd] = std::max(delta_max[cd], ch->m_phi);
            cd_max = std::max(cd_max, static_cast<uint>(cd));
        }
        if ((m_child_1st == nullptr) || ch->m_delta < m_child_1st->m_delta) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if (
            (m_child_2nd == nullptr) || (ch->m_delta < m_child_2nd->m_delta)) {
            m_child_2nd = ch;
        }
    }
    for (uint ii = cd_max + 1u; ii--;)
        m_delta += delta_max[ii];
    if (m_delta <= 0xffu)
        m_delta = zero;
    m_phi = m_child_1st ? m_child_1st->delta() : inf;
}

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_NODE_HPP
