#ifndef VSHOGI_ENGINE_DFPN_NODE_HPP
#define VSHOGI_ENGINE_DFPN_NODE_HPP

#include <memory>

#include "vshogi/common/game.hpp"

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
namespace vshogi::engine::dfpn
{

static constexpr uint zero = 0u;
static constexpr uint unit = 100u;
static constexpr uint cent = 1u;
static constexpr uint kilo = 1000u * unit;
static constexpr uint max_number = std::numeric_limits<uint>::max();

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

template <class Parameters>
class Node
{
private:
    using C = Configuration<Parameters>;
    using GameType = Game<Parameters>;
    using StateType = State<Parameters>;
    using MoveType = Move<Parameters>;
    using Square = typename C::Square;
    using PHelper = Pieces<Parameters>;
    using SHelper = Squares<Parameters>;

private:
    /**
     * @brief If true, `m_action` = defence move, turn of `m_game` = offence.
     */
    const bool m_offence;
    /**
     * @brief If `m_offence` is true, this should be a defence move.
     * If `m_offence` is false, this should be a check move.
     */
    const MoveType m_action;

    /**
     * @brief Pointer to sibling node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<Node> m_sibling;

    /**
     * @brief Pointer to first child node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<Node> m_child;

    Node* m_child_1st;
    Node* m_child_2nd;

    /**
     * @brief Number of searches required to prove there is a mate.
     *
     */
    uint m_pn;

    /**
     * @brief Number of searches required to prove there is no mate.
     *
     */
    uint m_dn;

public:
    /**
     * @brief Simulate the current game position.
     *
     * - {winner: BLACK, turn: BLACK, node: Attacker}
     *
     * - Typical checkmate (winner!=turn && node!=Attacker): `set_pndn_mate`
     *
     * - King entering by opponent (winner==turn && node!=Attacker) `set_pndn_no_mate`
     *
     * @param game
     */
    bool simulate(
        const GameType& game,
        const Node<Parameters>* const node_l = nullptr,
        const Node<Parameters>* const node_e = nullptr,
        const Node<Parameters>* const node_g = nullptr)
    {
        if (simulate_using_cousins(node_l, node_e, node_g))
            return true;
        return simulate_using_game(game);
    }
    /**
     * @brief Expand child nodes.
     *
     * @param game
     * @param cousin_ge_stand
     * @param cousin_le_stand
     * @return true Full expansion.
     * @return false Partial expansion, so that the node should not be added to
     * a transposition table.
     */
    bool expand(
        const GameType& game,
        const Node<Parameters>* const cousin_ge_stand = nullptr,
        const Node<Parameters>* const cousin_le_stand = nullptr)
    {
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
        if (m_offence)
            return expand_at_offence(game, cousin_ge_stand, cousin_le_stand);
        else
            return expand_at_defence(game, cousin_ge_stand, cousin_le_stand);
    }
    uint compute_thpn_for_child(const uint thpn) const
    {
        if (m_offence) {
            return std::min(
                thpn,
                (m_child_2nd && (m_child_2nd->m_pn != max_number))
                    ? m_child_2nd->m_pn + 1u
                    : max_number);
        } else {
            return thpn;
        }
    }
    uint compute_thdn_for_child(const uint thdn) const
    {
        if (m_offence) {
            return thdn;
        } else {
            return std::min(
                thdn,
                (m_child_2nd && (m_child_2nd->m_dn != max_number))
                    ? m_child_2nd->m_dn + 1u
                    : max_number);
        }
    }
    void backprop()
    {
        // - Offence: #P = min(#P of children), #D = sum(#D of children)
        // - Defence: #P = sum(#P of children), #D = min(#D of children)
        if (proved())
            return;
        if (m_offence) {
            if (m_child_1st->proved_mate()) {
                set_pndn_mate();
            } else {
                m_child_1st = nullptr;
                m_child_2nd = nullptr;
                m_dn = 0u;
                for (Node* ch = m_child.get(); ch; ch = ch->get_sibling())
                    update_offence_dn_ch1st_ch2nd(ch);
                m_pn = m_child_1st->m_pn;
            }
        } else {
            backprop_at_defence();
        }
        assert((m_pn == 0u) ? (m_dn == max_number) : (m_dn != max_number));
        assert((m_dn == 0u) ? (m_pn == max_number) : (m_pn != max_number));
    }

private:
    bool simulate_using_cousins(
        const Node* const node_l,
        const Node* const node_e,
        const Node* const node_g)
    {
        if (node_l && m_offence && node_l->proved_mate()) {
            m_pn = zero;
            m_dn = max_number;
            return true;
        } else if (node_l && (!m_offence) && node_l->proved_no_mate()) {
            m_pn = max_number;
            m_dn = zero;
            return true;
        } else if (node_e && node_e->proved()) {
            m_pn = node_e->m_pn;
            m_dn = node_e->m_dn;
            return true;
        } else if (node_g && m_offence && node_g->proved_no_mate()) {
            m_pn = max_number;
            m_dn = zero;
            return true;
        } else if (node_g && (!m_offence) && node_g->proved_mate()) {
            m_pn = zero;
            m_dn = max_number;
            return true;
        }
        return false;
    }
    bool simulate_using_game(const GameType& game)
    {
        const auto r = game.get_result();
        if (r == ONGOING)
            return false;
        if (r == DRAW) {
            set_pndn_no_mate();
            return true;
        }

        const auto winner = (r == BLACK_WIN) ? BLACK : WHITE;
        const auto turn = game.get_turn();
        if ((winner == turn) == m_offence)
            set_pndn_mate();
        else
            set_pndn_no_mate();
        return true;
    }

public: // utility
    Node()
        : m_offence(true), m_action(), m_sibling(nullptr), m_child(nullptr),
          m_child_1st(nullptr), m_child_2nd(nullptr), m_pn(unit), m_dn(unit)
    {
    }
    Node(const bool offence, const MoveType& action)
        : m_offence(offence), m_action(action), m_sibling(nullptr),
          m_child(nullptr), m_child_1st(nullptr), m_child_2nd(nullptr),
          m_pn(unit), m_dn(unit)
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
        m_sibling.reset();
        m_child.reset();
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
        m_pn = unit;
        m_dn = unit;
    }
    bool offence() const
    {
        return m_offence;
    }
    uint pn() const
    {
        return m_pn;
    }
    uint dn() const
    {
        return m_dn;
    }
    MoveType get_action() const
    {
        return m_action;
    }
    bool has_child() const
    {
        return static_cast<bool>(m_child);
    }
    const Node* get_child() const
    {
        return m_child ? m_child.get() : nullptr;
    }
    Node* get_sibling()
    {
        return m_sibling ? m_sibling.get() : nullptr;
    }
    const Node* get_sibling() const
    {
        return m_sibling ? m_sibling.get() : nullptr;
    }
    Node* get_child_1st()
    {
        return m_child_1st;
    }
    const Node* get_child_1st() const
    {
        return m_child_1st;
    }
    bool proved_mate() const
    {
        return (m_pn == zero) && (m_dn == max_number);
    }
    bool proved_no_mate() const
    {
        return (m_pn == max_number) && (m_dn == zero);
    }
    bool proved() const
    {
        return (proved_mate() || proved_no_mate());
    }

private:
    bool expand_at_offence(
        const GameType& g,
        const Node<Parameters>* const cousin_ge_stand,
        const Node<Parameters>* const cousin_le_stand)
    {
        const State<Parameters>& s = g.get_state();
        assert(s.get_board().get_king_location(~s.get_turn()) != C::SQ_NA);
        assert(
            s.get_board()[s.get_board().get_king_location(~s.get_turn())]
            == PHelper::to_board_piece(~s.get_turn(), C::OU));
        m_dn = zero;
        if (cousin_ge_stand) {
            assert(!cousin_ge_stand->proved());
            const Node<Parameters>* nibling = cousin_ge_stand->get_child();
            const auto next_child = expand_drop_moves_at_offence(
                s.get_stand(s.get_turn()), &nibling);
            expand_board_moves_at_offence(next_child, nibling);
        } else if (cousin_le_stand) {
            assert(!cousin_le_stand->proved());
            const Node<Parameters>* nibling = cousin_le_stand->get_child();
            for (; nibling; nibling = nibling->get_sibling()) {
                if (!nibling->get_action().is_drop())
                    break;
            }
            const auto next_child = expand_drop_moves_at_offence(s);
            expand_board_moves_at_offence(next_child, nibling);
        } else {
            const auto next_child = expand_drop_moves_at_offence(s);
            expand_board_moves_at_offence(next_child, s);
        }
        if (m_child_1st == nullptr)
            set_pndn_no_mate();
        else
            m_pn = m_child_1st->m_pn;
        assert((pn() == 0u) ? (dn() == max_number) : (dn() != max_number));
        assert((dn() == 0u) ? (pn() == max_number) : (pn() != max_number));
        return true;
    }
    bool expand_at_defence(
        const GameType& g,
        const Node<Parameters>* const cousin_ge_stand,
        const Node<Parameters>* const cousin_le_stand)
    {
        m_pn = zero;
        const auto& s = g.get_state();
        bool fully_expanded = false;
        if (cousin_ge_stand) {
            assert(!cousin_ge_stand->proved());
            const Node<Parameters>* nibling = cousin_ge_stand->get_child();
            const auto next_child = expand_board_moves_at_defence(&nibling);
            if (!had_two_consecutive_sacrifice_drops(g)) {
                expand_drop_moves_at_defence(next_child, s, nibling);
                fully_expanded = true;
            }
        } else if (cousin_le_stand) {
            assert(!cousin_le_stand->proved());
            const Node<Parameters>* nibling = cousin_le_stand->get_child();
            const auto next_child = expand_board_moves_at_defence(&nibling);
            if (!had_two_consecutive_sacrifice_drops(g)) {
                expand_drop_moves_at_defence(next_child, s);
                fully_expanded = true;
            }
        } else {
            const auto next_child = expand_board_moves_at_defence(s);
            if (!had_two_consecutive_sacrifice_drops(g)) {
                expand_drop_moves_at_defence(next_child, s);
                fully_expanded = true;
            }
        }
        if (m_child_1st == nullptr)
            set_pndn_mate();
        else
            m_dn = m_child_1st->m_dn;
        return fully_expanded;
    }
    void expand_board_moves_at_offence(
        std::unique_ptr<Node<Parameters>>* holder,
        const Node<Parameters>* nibling)
    {
        for (; nibling; nibling = nibling->get_sibling()) {
            const auto m = nibling->get_action();
            assert(!m.is_drop());
            *holder = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = holder->get();
            ch->m_pn = std::clamp(nibling->pn(), cent, kilo);
            ch->m_dn = std::clamp(nibling->dn(), cent, kilo);
            update_offence_dn_ch1st_ch2nd(ch);
            holder = &(ch->m_sibling);
        }
    }
    void expand_board_moves_at_offence(
        std::unique_ptr<Node<Parameters>>* holder, const State<Parameters>& s)
    {
        for (MoveType m : CheckBoardMoveGenerator<Parameters>(s)) {
            *holder = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = holder->get();
            if (s.is_declined_promotion(m)) {
                ch->m_pn = kilo;
                ch->m_dn = cent;
            }
            update_offence_dn_ch1st_ch2nd(ch);
            holder = &(ch->m_sibling);
        }
    }
    std::unique_ptr<Node<Parameters>>*
    expand_drop_moves_at_offence(const State<Parameters>& state)
    {
        std::unique_ptr<Node<Parameters>>* holder = &m_child;
        for (Move<Parameters> m : DropMoveGenerator<Parameters, true>(state)) {
            *holder = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = holder->get();
            update_offence_dn_ch1st_ch2nd(ch);
            holder = &(ch->m_sibling);
        }
        return holder;
    }
    std::unique_ptr<Node<Parameters>>* expand_drop_moves_at_offence(
        const Stand<Parameters>& stand, const Node<Parameters>** const nibling)
    {
        std::unique_ptr<Node<Parameters>>* holder = &m_child;
        for (; (*nibling); (*nibling) = (*nibling)->get_sibling()) {
            const auto m = (*nibling)->get_action();
            if (!m.is_drop())
                break;
            if (!stand.exist(m.source_piece()))
                continue;
            *holder = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = holder->get();
            ch->m_pn = std::clamp((*nibling)->pn(), cent, kilo);
            ch->m_dn = std::clamp((*nibling)->dn(), cent, kilo);
            update_offence_dn_ch1st_ch2nd(ch);
            holder = &(ch->m_sibling);
        }
        return holder;
    }
    std::unique_ptr<Node<Parameters>>*
    expand_board_moves_at_defence(const Node<Parameters>** const nibling)
    {
        std::unique_ptr<Node<Parameters>>* holder = &m_child;
        for (; (*nibling); *nibling = (*nibling)->get_sibling()) {
            const auto m = (*nibling)->get_action();
            if (m.is_drop())
                break;
            *holder = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = holder->get();
            ch->m_pn = std::clamp((*nibling)->pn(), cent, kilo);
            ch->m_dn = std::clamp((*nibling)->dn(), cent, kilo);
            update_defence_pn_ch1st_ch2nd(ch);
            holder = &(ch->m_sibling);
        }
        return holder;
    }
    std::unique_ptr<Node<Parameters>>*
    expand_board_moves_at_defence(const StateType& state)
    {
        std::unique_ptr<Node<Parameters>>* holder = &m_child;
        for (Move<Parameters> m : KingMoveGenerator<Parameters>(state)) {
            *holder = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = holder->get();
            update_defence_pn_ch1st_ch2nd(ch);
            holder = &(ch->m_sibling);
        }
        for (Move<Parameters> m : BlockMoveGenerator<Parameters>(state)) {
            *holder = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = holder->get();
            update_defence_pn_ch1st_ch2nd(ch);
            holder = &(ch->m_sibling);
        }
        return holder;
    }
    void expand_drop_moves_at_defence(
        std::unique_ptr<Node<Parameters>>* next, const StateType& state)
    {
        // https://komorinfo.com/blog/proof-number-double-count/
        uint pn_max[C::num_squares] = {0u};
        const auto dst = m_action.destination();
        for (Move<Parameters> m : DropMoveGenerator<Parameters>(state)) {
            *next = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = next->get();
            if (ch->is_better_dn_choice_than(m_child_1st, dst)) {
                m_child_2nd = m_child_1st;
                m_child_1st = ch;
            } else if (ch->is_better_dn_choice_than(m_child_2nd, dst)) {
                m_child_2nd = ch;
            }
            const auto d = ch->m_action.destination();
            if (pn_max[d] < ch->m_pn)
                pn_max[d] = ch->m_pn;
            next = &(ch->m_sibling);
        }
        for (uint ii = C::num_squares; ii--;) {
            assert(pn_max[ii] != max_number);
            m_pn += pn_max[ii];
        }
    }
    void expand_drop_moves_at_defence(
        std::unique_ptr<Node<Parameters>>* next,
        const StateType& state,
        const Node<Parameters>* nibling)
    {
        // https://komorinfo.com/blog/proof-number-double-count/
        uint pn_max[C::num_squares] = {0u};
        const Stand<Parameters>& stand = state.get_stand(state.get_turn());
        const auto dst = m_action.destination();
        for (; nibling; nibling = nibling->get_sibling()) {
            const auto m = nibling->get_action();
            assert(m.is_drop());
            if (!stand.exist(m.source_piece()))
                continue;
            *next = std::make_unique<Node<Parameters>>(!m_offence, m);
            Node<Parameters>* const ch = next->get();
            ch->m_pn = std::clamp(nibling->pn(), cent, kilo);
            ch->m_dn = std::clamp(nibling->dn(), cent, kilo);
            if (ch->is_better_dn_choice_than(m_child_1st, dst)) {
                m_child_2nd = m_child_1st;
                m_child_1st = ch;
            } else if (ch->is_better_dn_choice_than(m_child_2nd, dst)) {
                m_child_2nd = ch;
            }
            const auto d = ch->m_action.destination();
            if (pn_max[d] < ch->m_pn)
                pn_max[d] = ch->m_pn;
            next = &(ch->m_sibling);
        }
        for (uint ii = C::num_squares; ii--;) {
            assert(pn_max[ii] != max_number);
            m_pn += pn_max[ii];
        }
    }

private:
    void backprop_at_defence()
    {
        if (m_child_1st->proved_no_mate()) {
            set_pndn_no_mate();
        } else {
            m_child_1st = nullptr;
            m_child_2nd = nullptr;
            m_pn = 0u;
            Node* const ch = backprop_at_defence_board_moves();
            backprop_at_defence_drop_moves(ch);
            m_dn = m_child_1st->m_dn;
        }
    }
    Node* backprop_at_defence_board_moves()
    {
        Node* ch = m_child.get();
        for (; ch && !ch->m_action.is_drop(); ch = ch->get_sibling()) {
            update_defence_pn_ch1st_ch2nd(ch);
        }
        return ch;
    }
    void backprop_at_defence_drop_moves(Node* ch)
    {
        // https://komorinfo.com/blog/proof-number-double-count/
        uint pn_max[C::num_squares] = {0u};
        const auto dst = m_action.destination();
        for (; ch; ch = ch->get_sibling()) {
            assert(ch->m_action.is_drop());
            if (ch->is_better_dn_choice_than(m_child_1st, dst)) {
                m_child_2nd = m_child_1st;
                m_child_1st = ch;
            } else if (ch->is_better_dn_choice_than(m_child_2nd, dst)) {
                m_child_2nd = ch;
            }
            const auto d = ch->m_action.destination();
            if (pn_max[d] < ch->m_pn)
                pn_max[d] = ch->m_pn;
        }
        for (uint ii = C::num_squares; ii--;) {
            assert(pn_max[ii] != max_number);
            m_pn += pn_max[ii];
        }
    }

private:
    void set_pndn_mate()
    {
        m_pn = zero;
        m_dn = max_number;
    }
    void set_pndn_no_mate()
    {
        m_pn = max_number;
        m_dn = zero;
    }
    void update_offence_dn_ch1st_ch2nd(Node* const ch)
    {
        if (ch->is_better_pn_choice_than(m_child_1st)) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if (ch->is_better_pn_choice_than(m_child_2nd)) {
            m_child_2nd = ch;
        }
        assert(ch->m_dn != max_number);
        m_dn += ch->m_dn;
    }
    bool is_better_pn_choice_than(const Node* const other) const
    {
        if (other == nullptr)
            return true;
        return m_pn < other->m_pn;
    }
    void update_defence_pn_ch1st_ch2nd(Node* const ch)
    {
        const auto dst = m_action.destination();
        if (ch->is_better_dn_choice_than(m_child_1st, dst)) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if (ch->is_better_dn_choice_than(m_child_2nd, dst)) {
            m_child_2nd = ch;
        }
        assert(ch->m_pn != max_number);
        m_pn += ch->m_pn;
    }
    bool is_better_dn_choice_than(
        const Node* const other, const Square& dst_prev) const
    {
        if (other == nullptr)
            return true;
        if (m_dn < other->m_dn)
            return true;
        if (m_dn > other->m_dn)
            return false;
        return (m_action.destination() == dst_prev)
               && (other->m_action.destination() != dst_prev);
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_NODE_HPP
