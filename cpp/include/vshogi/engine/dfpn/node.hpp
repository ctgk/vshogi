#ifndef VSHOGI_ENGINE_DFPN_NODE_HPP
#define VSHOGI_ENGINE_DFPN_NODE_HPP

#include <memory>

#include "vshogi/common/config.hpp"
#include "vshogi/common/game.hpp"
#include "vshogi/common/generator.hpp"
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
static constexpr uint cent = 1u;
static constexpr uint unit = 100u;
static constexpr uint kilo = 1000u * unit;
static constexpr uint inf = std::numeric_limits<uint>::max();

template <class P>
class Node
{
    using C = Configuration<P>;
    using Square = typename C::Square;
    using SHelper = Squares<P>;

private:
    bool m_offence;
    Move<P> m_action;

    /**
     * @brief Threshold value during search, #P (or #D) after search.
     */
    uint m_phi;

    /**
     * @brief Threshold value during search, #D (or #P) after search.
     */
    uint m_delta;
    bool m_proved_by_repetition;

    Node* m_sibling;
    Node* m_child;
    bool m_fully_expanded; //!< Omitted drop moves if false.
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
        const State<P>& s = g.get_state();
        const Node* nibling = nullptr;
        if (twin_ge)
            nibling = twin_ge->get_child();
        else if (twin_le)
            nibling = twin_le->get_child();

        m_fully_expanded = false;
        m_child = next;
        expand_board_moves(next, s, &nibling);
        if (m_offence || (!g.had_two_consecutive_sacrifice_drops())) {
            if (twin_ge == nullptr)
                nibling = nullptr;
            m_fully_expanded = expand_drop_moves(next, s, &nibling);
        }
        if (m_child == next)
            m_child = nullptr;
    }
    Node* select(const uint th_p, const uint th_d, uint& th_p_ch, uint& th_d_ch)
    {
        assert(m_child_1st);
        constexpr uint d2_max = inf - 1u;
        const uint d2 = m_child_2nd ? m_child_2nd->m_delta : inf;
        th_p_ch = th_d - m_delta + m_child_1st->m_phi;
        th_d_ch = std::min(th_p, std::min(d2, d2_max) + 1u);
        return m_child_1st;
    }
    void backprop(const Square& king_sq, const Square& checker_sq)
    {
        assert(offence() || (checker_sq != C::SQ_NA));
        if (m_child_1st && (m_child_1st->m_delta == zero)) {
            m_phi = zero;
            m_delta = inf;
            m_proved_by_repetition = m_child_1st->proved_by_repetitions();
            return;
        }
        uint delta_max[C::num_squares] = {zero};
        m_delta = zero;
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
        m_proved_by_repetition = static_cast<bool>(m_child);
        for (Node* ch = child(); ch; ch = ch->sibling()) {
            assert(ch->m_phi != inf);
            m_proved_by_repetition &= ch->proved_by_repetitions();
            if (m_offence || (!ch->m_action.is_drop()))
                m_delta += ch->m_phi;
            else {
                const auto cd = ch->m_action.destination();
                delta_max[cd] = std::max(delta_max[cd], ch->m_phi);
            }
            if (ch->is_better_child_than(m_child_1st, checker_sq, king_sq)) {
                m_child_2nd = m_child_1st;
                m_child_1st = ch;
            } else if (ch->is_better_child_than(
                           m_child_2nd, checker_sq, king_sq)) {
                m_child_2nd = ch;
            }
        }
        for (uint ii = C::num_squares; ii--;)
            m_delta += delta_max[ii];
        m_phi = m_child_1st ? m_child_1st->m_delta : inf;
    }

private:
    bool
    simulate_using_twins(const Node* const twin_ge, const Node* const twin_le)
    {
        // proved_no_mate_at_offence() == (dn() == zero) == (delta == zero)
        // proved_mate_at_defence() == (pn() == zero) == (delta == zero)
        if (twin_ge && (twin_ge->m_delta == zero)) {
            assert(m_offence == twin_ge->m_offence);
            assert(!twin_ge->proved_by_repetitions());
            m_phi = inf;
            m_delta = zero;
            return true;
        }

        // proved_mate_at_offence() == (pn() == zero) == (phi == zero)
        // proved_no_mate_at_defence() == (dn() == zero) == (phi == zero)
        if (twin_le && (twin_le->m_phi == zero)) {
            assert(m_offence == twin_le->m_offence);
            assert(!twin_le->proved_by_repetitions());
            m_phi = zero;
            m_delta = inf;
            return true;
        }
        return false;
    }
    void set_mate()
    {
        if (m_offence) {
            m_phi = zero;
            m_delta = inf;
        } else {
            m_phi = inf;
            m_delta = zero;
        }
    }
    void set_no_mate()
    {
        if (m_offence) {
            m_phi = inf;
            m_delta = zero;
        } else {
            m_phi = zero;
            m_delta = inf;
        }
    }
    bool simulate_using_game(const Game<P>& g)
    {
        const auto turn = g.get_turn();
        auto result = g.get_result(); // this is usually ONGOING
        if (g.is_repetitions(1u)) {
            m_proved_by_repetition = true;
            if (g.in_check())
                result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;
            else
                result = DRAW;
        } else if (g.can_declare_win_by_king_enter())
            result = (turn == BLACK) ? BLACK_WIN : WHITE_WIN;

        if (result == ONGOING)
            return false;
        if (result == DRAW) {
            set_no_mate();
            return true;
        }
        const auto winner = (result == BLACK_WIN) ? BLACK : WHITE;
        if (m_offence == (winner == turn)) {
            set_mate();
        } else {
            set_no_mate();
        }
        return true;
    }

    /**
     * @brief expand children by generator
     *
     * @return true fully expanded
     * @return false partially expanded
     */
    template <class Generator>
    bool expand_by_generator(Node<P>*& next, const State<P>& s)
    {
        for (Move<P> m : Generator(s)) {
            if (next == nullptr)
                return false;
            const bool is_last
                = (!next->m_offence) && (next->get_action().hash() == 0u);
            if (s.is_declined_promotion(m))
                next->init(!m_offence, m, cent, kilo);
            else
                next->init(!m_offence, m);
            if (m_child != next) {
                auto prev = next - 1;
                prev->m_sibling = next;
            }
            if (is_last) {
                next = nullptr;
            } else {
                ++next;
            }
        }
        return true;
    }
    bool expand_board_moves(
        Node<P>*& next, const State<P>& s, const Node** const nibling)
    {
        if ((*nibling) != nullptr)
            return expand_board_moves(next, nibling);
        if (m_offence)
            return expand_by_generator<BoardMoveGenerator<P, true>>(next, s);
        expand_by_generator<KingMoveGenerator<P>>(next, s);
        return expand_by_generator<BlockMoveGenerator<P>>(next, s);
    }
    bool expand_board_moves(Node<P>*& next, const Node** const nibling)
    {
        for (; *nibling; *nibling = (*nibling)->get_sibling()) {
            if (next == nullptr)
                return false;
            if ((*nibling)->get_action().is_drop())
                break;
            const bool end
                = (!next->m_offence) && (next->get_action().hash() == 0u);
            init_from_nibling(*next, **nibling);
            if (m_child != next) {
                auto prev = next - 1;
                prev->m_sibling = next;
            }
            if (end) {
                next = nullptr;
            } else {
                ++next;
            }
        }
        return true;
    }
    bool expand_drop_moves(
        Node<P>*& next, const State<P>& s, const Node** const nibling)
    {
        if ((*nibling) == nullptr) {
            if (m_offence)
                return expand_by_generator<DropMoveGenerator<P, true>>(next, s);
            return expand_by_generator<DropMoveGenerator<P, false>>(next, s);
        }
        return expand_drop_moves(next, nibling, s.get_stand(s.get_turn()));
    }
    bool expand_drop_moves(
        Node<P>*& next, const Node** const nibling, const Stand<P>& s)
    {
        for (; *nibling; *nibling = (*nibling)->get_sibling()) {
            if (next == nullptr)
                return false;
            assert((*nibling)->get_action().is_drop());
            if (!s.exist((*nibling)->get_action().source_piece()))
                continue;
            const bool end
                = (!next->m_offence) && (next->get_action().hash() == 0u);
            init_from_nibling(*next, **nibling);
            if (m_child != next) {
                auto prev = next - 1;
                prev->m_sibling = next;
            }
            if (end) {
                next = nullptr;
                break;
            } else {
                ++next;
            }
        }
        return true;
    }
    void init_from_nibling(Node<P>& n, const Node& nibling)
    {
        n.init(
            nibling.offence(),
            nibling.get_action(),
            std::clamp(nibling.phi(), cent, kilo),
            std::clamp(nibling.delta(), cent, kilo));
    }
    bool is_better_child_than(
        const Node* const other,
        const Square& dst_prev,
        const Square& king_sq) const
    {
        if (other == nullptr)
            return true;
        if (m_delta != other->m_delta)
            return m_delta < other->m_delta;
        if (m_child_1st && other->m_child_1st && m_child_1st->proved()
            && (!other->m_child_1st->proved()))
            return true;

        const auto td = m_action.destination();
        const auto od = other->m_action.destination();

        if (m_offence && ((td == dst_prev) != (od == dst_prev)))
            return (td == dst_prev) && (od != dst_prev); // defence at parent
        if (!m_offence && (m_action.is_drop() != other->m_action.is_drop()))
            return m_action.is_drop()
                   && !other->m_action.is_drop(); // offence at parent

        const int offset = m_offence ? 10 : 19;
        const int tcd
            = static_cast<int>(SHelper::chebyshev_distance(td, king_sq));
        const int ocd
            = static_cast<int>(SHelper::chebyshev_distance(od, king_sq));
        return std::abs(10 * tcd - offset) < std::abs(10 * ocd - offset);
    }
    Node* child()
    {
        return m_child;
    }
    Node* sibling()
    {
        return m_sibling;
    }

public: // utility
    Node()
        : m_offence(true), m_action(), m_phi(inf), m_delta(inf),
          m_proved_by_repetition(false), m_sibling(nullptr), m_child(nullptr),
          m_fully_expanded(false), m_child_1st(nullptr), m_child_2nd(nullptr)

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
        m_offence = true;
        m_phi = inf;
        m_delta = inf;
        m_proved_by_repetition = false;
        m_sibling = nullptr;
        m_child = nullptr;
        m_fully_expanded = false;
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
    }
    void init(const bool offence, const Move<P>& action)
    {
        m_offence = offence;
        m_action = action;
        m_phi = unit;
        m_delta = unit;
        m_proved_by_repetition = false;
        m_sibling = nullptr;
        m_child = nullptr;
        m_fully_expanded = false;
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
    }
    void init(
        const bool offence,
        const Move<P>& action,
        const uint phi,
        const uint delta)
    {
        m_offence = offence;
        m_action = action;
        m_phi = phi;
        m_delta = delta;
        m_proved_by_repetition = false;
        m_sibling = nullptr;
        m_child = nullptr;
        m_fully_expanded = false;
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
    }
    bool offence() const
    {
        return m_offence;
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
        return m_delta;
    }
    uint pn() const
    {
        return m_offence ? m_phi : m_delta;
    }
    uint dn() const
    {
        return m_offence ? m_delta : m_phi;
    }
    bool proved() const
    {
        return (m_phi == zero) || (m_delta == zero);
    }
    bool proved_mate() const
    {
        return pn() == zero;
    }
    bool proved_no_mate() const
    {
        return dn() == zero;
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
        return m_sibling;
    }
    bool fully_expanded() const
    {
        return m_fully_expanded;
    }
    bool proved_by_repetitions() const
    {
        return m_proved_by_repetition;
    }
    const Node* get_child_1st() const
    {
        return m_child_1st;
    }
    const Node* get_child_2nd() const
    {
        return m_child_2nd;
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_NODE_HPP
