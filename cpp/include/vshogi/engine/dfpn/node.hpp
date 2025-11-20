#ifndef VSHOGI_ENGINE_DFPN_NODE_HPP
#define VSHOGI_ENGINE_DFPN_NODE_HPP

#include <memory>

#include "vshogi/common/config.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/iterator.hpp"
#include "vshogi/common/iterator/drop.hpp"
#include "vshogi/common/squares.hpp"
#include "vshogi/common/state.hpp"
#include "vshogi/common/utils.hpp"

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

template <class P>
class Node
{
    using C = Configuration<P>;
    using Square = typename C::Square;
    using SHelper = Squares<P>;

private:
    Move<P> m_action;

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

    Node* m_parent;
    Node* m_child;
    Node* m_child_1st;
    Node* m_child_2nd;

public:
    bool simulate(
        const Game<P>& g,
        const Node* const twin_ge = nullptr,
        const Node* const twin_le = nullptr)
    {
        if (simulate_using_twins(twin_ge, twin_le))
            return true;
        if (has_child())
            return false;
        return simulate_using_game(g);
    }
    void expand(
        Node<P>*& next,
        const Game<P>& g,
        const Node* const twin_ge = nullptr,
        const Node* const twin_le = nullptr)
    {
        const bool offence = (g.ply() % 2u == 0u);
        const State<P>& s = g.get_state();
        const Node* nibling = nullptr;
        if (twin_ge)
            nibling = twin_ge->get_child();
        else if (twin_le)
            nibling = twin_le->get_child();

        m_fully_expanded = false;
        m_child = next;
        expand_board_moves(offence, next, s, &nibling);
        if (offence || (!g.had_two_consecutive_sacrifice_drops())) {
            if ((twin_ge == nullptr) || (!twin_ge->fully_expanded()))
                nibling = nullptr;
            m_fully_expanded = expand_drop_moves(offence, next, s, &nibling);
        }
        if (m_child == next)
            m_child = nullptr;
        assert(next);
        if (!next->is_end())
            next->m_parent = nullptr;

        const int offset = offence ? 19 : 10;
        const Square king_sq
            = g.get_king_square(offence ? ~g.get_turn() : g.get_turn());
        const Square checker = g.find_checker_square();
        for (Node* c = m_child; c && (c->m_parent == this); ++c) {
            uint delta_plus = 0u;
            const auto m = c->get_action();
            const auto dst = m.destination();
            if ((offence && !m.is_drop()) || (!offence && (dst != checker)))
                delta_plus
                    = 0x40u; // offence (defence) prefers drop (capturing)
            const auto d
                = static_cast<int>(SHelper::chebyshev_distance(dst, king_sq));
            delta_plus ^= std::min(
                0x3fu, static_cast<uint>(std::abs(10 * d - offset)));
            c->m_delta ^= delta_plus;
        }
    }
    Node* select(const uint th_p, const uint th_d, uint& th_p_ch, uint& th_d_ch)
    {
        assert(m_child_1st);
        constexpr uint d2_max = inf - 1u;
        const uint d2 = m_child_2nd ? m_child_2nd->delta() : inf;
        th_p_ch = th_d - delta() + m_child_1st->m_phi;
        th_d_ch = std::min(th_p, std::min(d2, d2_max) + 1u);
        return m_child_1st;
    }

    /**
     * @brief Update #P and #D based on the values of child nodes.
     *
     * @param checker_sq Location of checker piece attacking the target king.
     * Make sure to pass `SQ_NA` if the node is an offence node.
     */
    void backprop(const Square& checker_sq)
    {
        const bool offence = (checker_sq == C::SQ_NA);
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
            if (offence || (!ch->m_action.is_drop()))
                m_delta += ch->m_phi;
            else {
                const auto cd = ch->m_action.destination();
                delta_max[cd] = std::max(delta_max[cd], ch->m_phi);
                cd_max = std::max(cd_max, static_cast<uint>(cd));
            }
            if ((m_child_1st == nullptr)
                || ch->m_delta < m_child_1st->m_delta) {
                m_child_2nd = m_child_1st;
                m_child_1st = ch;
            } else if (
                (m_child_2nd == nullptr)
                || (ch->m_delta < m_child_2nd->m_delta)) {
                m_child_2nd = ch;
            }
        }
        for (uint ii = cd_max + 1u; ii--;)
            m_delta += delta_max[ii];
        if (m_delta <= 0xffu)
            m_delta = zero;
        m_phi = m_child_1st ? m_child_1st->delta() : inf;
    }

private:
    bool
    simulate_using_twins(const Node* const twin_ge, const Node* const twin_le)
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
    void set_mate(const bool offence)
    {
        if (offence) {
            m_phi = zero;
            m_delta = inf;
        } else {
            m_phi = inf;
            m_delta = zero;
        }
    }
    void set_no_mate(const bool offence)
    {
        if (offence) {
            m_phi = inf;
            m_delta = zero;
        } else {
            m_phi = zero;
            m_delta = inf;
        }
    }
    bool simulate_using_game(const Game<P>& g)
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

    /**
     * @brief expand children by generator
     *
     * @return true fully expanded
     * @return false partially expanded
     */
    template <class Iter>
    bool expand_by_generator(Node<P>*& next, const State<P>& s)
    {
        for (auto it = Iter(s); it; ++it) {
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
    bool expand_board_moves(
        const bool offence,
        Node<P>*& next,
        const State<P>& s,
        const Node** const nibling)
    {
        if ((*nibling) != nullptr)
            return expand_board_moves(next, nibling);
        if (offence)
            return expand_by_generator<BoardMoveIterator<P, IterEnum::CHECK>>(
                next, s);
        expand_by_generator<KingMoveIterator<P>>(next, s);
        return expand_by_generator<SoldierMoveIterator<P, IterEnum::EVADE>>(
            next, s);
    }
    bool expand_board_moves(Node<P>*& next, const Node** const nibling)
    {
        for (; *nibling; *nibling = (*nibling)->get_sibling()) {
            if (next->is_end())
                return false;
            if ((*nibling)->get_action().is_drop())
                break;
            init_from_nibling(*next, **nibling);
            ++next;
        }
        return true;
    }
    bool expand_drop_moves(
        const bool offence,
        Node<P>*& next,
        const State<P>& s,
        const Node** const nibling)
    {
        if ((*nibling) == nullptr) {
            if (offence)
                return expand_by_generator<
                    DropMoveIterator<P, IterEnum::CHECK>>(next, s);
            return expand_by_generator<DropMoveIterator<P, IterEnum::EVADE>>(
                next, s);
        }
        return expand_drop_moves(next, nibling, s.get_stand(s.get_turn()));
    }
    bool expand_drop_moves(
        Node<P>*& next, const Node** const nibling, const Stand<P>& s)
    {
        for (; *nibling; *nibling = (*nibling)->get_sibling()) {
            if (next->is_end())
                return false;
            assert((*nibling)->get_action().is_drop());
            if (!s.exist((*nibling)->get_action().source_piece()))
                continue;
            init_from_nibling(*next, **nibling);
            ++next;
        }
        return true;
    }
    void init_from_nibling(Node<P>& n, const Node& nibling)
    {
        n.init(
            this,
            nibling.get_action(),
            std::clamp(nibling.phi(), cent, kilo),
            std::clamp(nibling.delta(), cent, kilo));
    }

public: // utility
    Node()
        : m_action(), m_phi(unit), m_delta(unit), m_fully_expanded(false),
          m_parent(nullptr), m_child(nullptr), m_child_1st(nullptr),
          m_child_2nd(nullptr)

    {
    }

    // Rules of 5
    ~Node() = default; // 1/5 destructor
    Node(const Node& other) = default; // 2/5 copy constructor
    Node& operator=(const Node& other) = default; // 3/5 copy assignment
    Node(Node&& other) = default; // 4/5 move constructor
    Node& operator=(Node&& other) = default; // 5/5 move assignment

    void init()
    {
        m_phi = unit;
        m_delta = unit;
        m_fully_expanded = false;
        m_parent = nullptr;
        m_child = nullptr;
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
    }
    void init_end()
    {
        init();
        m_parent = this;
    }
    void init(
        Node* const parent,
        const Move<P>& action,
        const uint phi,
        const uint delta)
    {
        m_action = action;
        m_phi = phi;
        m_delta = delta;
        m_fully_expanded = false;
        m_parent = parent;
        m_child = nullptr;
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
    }
    Move<P> get_action() const
    {
        return m_action;
    }
    uint phi() const
    {
        return m_phi;
    }
    uint delta() const
    {
        return (m_delta == inf) ? m_delta : (m_delta >> 8u) << 8u;
    }
    uint pn(const bool offence) const
    {
        return offence ? m_phi : delta();
    }
    uint dn(const bool offence) const
    {
        return offence ? delta() : m_phi;
    }
    bool proved() const
    {
        return (m_phi == zero) || (m_delta == zero);
    }
    bool proved_mate(const bool offence) const
    {
        return pn(offence) == zero;
    }
    bool proved_no_mate(const bool offence) const
    {
        return dn(offence) == zero;
    }
    bool has_child() const
    {
        return static_cast<bool>(m_child);
    }
    const Node* get_child() const
    {
        return m_child;
    }
    const Node* get_sibling() const
    {
        const Node* const sibling = this + 1;
        if (m_parent == sibling->m_parent)
            return sibling;
        return nullptr;
    }
    bool fully_expanded() const
    {
        return m_fully_expanded;
    }
    const Node* get_child_1st() const
    {
        return m_child_1st;
    }
    const Node* get_child_2nd() const
    {
        return m_child_2nd;
    }
    bool is_end() const
    {
        return m_parent == this;
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_NODE_HPP
