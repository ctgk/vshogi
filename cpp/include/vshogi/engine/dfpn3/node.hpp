#ifndef VSHOGI_ENGINE_DFPN3_NODE_HPP
#define VSHOGI_ENGINE_DFPN3_NODE_HPP

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
namespace vshogi::engine::dfpn3
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
    using NodeOwner = std::unique_ptr<Node>;
    using SHelper = Squares<P>;

private:
    const bool m_offence;
    const Move<P> m_action;

    /**
     * @brief Threshold value during search, #P (or #D) after search.
     */
    uint m_phi;

    /**
     * @brief Threshold value during search, #D (or #P) after search.
     */
    uint m_delta;
    bool m_proved_by_repetition;

    std::unique_ptr<Node> m_sibling;
    std::unique_ptr<Node> m_child;
    bool m_fully_expanded; //!< Omitted drop moves if false.
    Node* m_child_1st;
    Node* m_child_2nd;

public:
    bool simulate(
        Game<P>& g,
        const Node* const twin_e = nullptr,
        const Node* const twin_ge = nullptr,
        const Node* const twin_le = nullptr)
    {
        if (simulate_using_twins(twin_ge, twin_le))
            return true;
        if (has_child())
            return false;
        return simulate_using_game(g, twin_e != nullptr);
    }
    void expand(
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
        NodeOwner* const c = expand_board_moves(&m_child, s, &nibling);
        if (m_offence || (!g.had_two_consecutive_sacrifice_drops())) {
            if (twin_ge == nullptr)
                nibling = nullptr;
            expand_drop_moves(c, s, &nibling);
            m_fully_expanded = true;
        }
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
    void backprop(const Square& king_sq)
    {
        if (m_child_1st && (m_child_1st->m_delta == zero)) {
            m_phi = zero;
            m_delta = inf;
            m_proved_by_repetition = m_child_1st->proved_by_repetitions();
            return;
        }
        const auto dst = m_action.destination();
        m_delta = zero;
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
        m_proved_by_repetition = static_cast<bool>(m_child);
        for (Node* ch = child(); ch; ch = ch->sibling()) {
            assert(ch->m_phi != inf);
            m_proved_by_repetition &= ch->proved_by_repetitions();
            m_delta += ch->m_phi;
            if (ch->is_better_child_than(m_child_1st, dst, king_sq)) {
                m_child_2nd = m_child_1st;
                m_child_1st = ch;
            } else if (ch->is_better_child_than(m_child_2nd, dst, king_sq)) {
                m_child_2nd = ch;
            }
        }
        m_phi = m_child_1st ? m_child_1st->m_delta : inf;
    }

private:
    bool proved_mate_at_offence() const
    {
        return m_offence && proved_mate();
    }
    bool proved_mate_at_defence() const
    {
        return (!m_offence) && proved_mate();
    }
    bool proved_no_mate_at_offence() const
    {
        return m_offence && proved_no_mate();
    }
    bool proved_no_mate_at_defence() const
    {
        return (!m_offence) && proved_no_mate();
    }
    bool
    simulate_using_twins(const Node* const twin_ge, const Node* const twin_le)
    {
        if (twin_ge
            && (twin_ge->proved_no_mate_at_offence()
                || twin_ge->proved_mate_at_defence())) {
            assert(m_offence == twin_ge->m_offence);
            assert(!twin_ge->proved_by_repetitions());
            m_phi = inf;
            m_delta = zero;
            return true;
        }
        if (twin_le
            && (twin_le->proved_mate_at_offence()
                || twin_le->proved_no_mate_at_defence())) {
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
    bool simulate_using_game(Game<P>& g, const bool check_repetition)
    {
        const auto turn = g.get_turn();
        auto result = g.get_result(); // this is usually ONGOING
        if ((check_repetition && g.is_repetitions(1u))) {
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
    template <class Generator>
    NodeOwner* expand_by_generator(NodeOwner* c, const State<P>& s)
    {
        for (Move<P> m : Generator(s)) {
            if (s.is_declined_promotion(m))
                *c = std::make_unique<Node>(!m_offence, m, cent, kilo);
            else
                *c = std::make_unique<Node>(!m_offence, m);
            c = &((*c)->m_sibling);
        }
        return c;
    }
    NodeOwner* expand_board_moves(
        NodeOwner* c, const State<P>& s, const Node** const nibling)
    {
        if ((*nibling) == nullptr) {
            if (m_offence)
                return expand_by_generator<BoardMoveGenerator<P, true>>(c, s);
            c = expand_by_generator<KingMoveGenerator<P>>(c, s);
            return expand_by_generator<BlockMoveGenerator<P>>(c, s);
        }
        return expand_board_moves(c, nibling);
    }
    NodeOwner* expand_board_moves(NodeOwner* c, const Node** const nibling)
    {
        for (; *nibling; *nibling = (*nibling)->get_sibling()) {
            if ((*nibling)->get_action().is_drop())
                break;
            *c = make_unique_from_nibling(**nibling);
            c = &((*c)->m_sibling);
        }
        return c;
    }
    NodeOwner* expand_drop_moves(
        NodeOwner* c, const State<P>& s, const Node** const nibling)
    {
        if ((*nibling) == nullptr) {
            if (m_offence)
                return expand_by_generator<DropMoveGenerator<P, true>>(c, s);
            else
                return expand_by_generator<DropMoveGenerator<P, false>>(c, s);
        }
        return expand_drop_moves(c, nibling, s.get_stand(s.get_turn()));
    }
    NodeOwner* expand_drop_moves(
        NodeOwner* c, const Node** const nibling, const Stand<P>& s)
    {
        for (; *nibling; *nibling = (*nibling)->get_sibling()) {
            assert((*nibling)->get_action().is_drop());
            if (!s.exist((*nibling)->get_action().source_piece()))
                continue;
            *c = make_unique_from_nibling(**nibling);
            c = &((*c)->m_sibling);
        }
        return c;
    }
    static auto make_unique_from_nibling(const Node& nibling)
    {
        return std::make_unique<Node>(
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
        return m_child.get();
    }
    Node* sibling()
    {
        return m_sibling.get();
    }

public: // utility
    Node()
        : m_offence(true), m_action(), m_phi(inf), m_delta(inf),
          m_proved_by_repetition(false), m_sibling(nullptr), m_child(nullptr),
          m_fully_expanded(false), m_child_1st(nullptr), m_child_2nd(nullptr)

    {
    }
    Node(const bool offence, const Move<P>& action)
        : m_offence(offence), m_action(action), m_phi(unit), m_delta(unit),
          m_proved_by_repetition(false), m_sibling(nullptr), m_child(nullptr),
          m_fully_expanded(false), m_child_1st(nullptr), m_child_2nd(nullptr)
    {
    }
    Node(
        const bool offence,
        const Move<P>& action,
        const uint phi,
        const uint delta)
        : m_offence(offence), m_action(action), m_phi(phi), m_delta(delta),
          m_proved_by_repetition(false), m_sibling(nullptr), m_child(nullptr),
          m_fully_expanded(false), m_child_1st(nullptr), m_child_2nd(nullptr)
    {
    }

    // Rules of 5
    ~Node() = default; // 1/5 destructor
    Node(const Node& other) = delete; // 2/5 copy constructor
    Node& operator=(const Node& other) = delete; // 3/5 copy assignment
    Node(Node&& other) = default; // 4/5 move constructor
    Node& operator=(Node&& other) = default; // 5/5 move assignment

    void init()
    {
        m_phi = inf;
        m_delta = inf;
        m_proved_by_repetition = false;
        m_child.reset();
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
        return m_child.get();
    }
    const Node* get_sibling() const
    {
        return m_sibling.get();
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

} // namespace vshogi::engine::dfpn3

#endif // VSHOGI_ENGINE_DFPN3_NODE_HPP
