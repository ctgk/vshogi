#ifndef VSHOGI_ENGINE_DFPN_HPP
#define VSHOGI_ENGINE_DFPN_HPP

#include <cstdint>
#include <limits>
#include <memory>
#include <unordered_map>
#include <vector>

#include "vshogi/common/color.hpp"
#include "vshogi/common/generator.hpp"
#include "vshogi/common/result.hpp"
#include "vshogi/common/utils.hpp"
#include "vshogi/variants/animal_shogi.hpp"

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
static constexpr uint max_number = std::numeric_limits<uint>::max();

template <class Config>
class Searcher;

template <class Config>
class Node
{
    static_assert(!std::is_same<Config, animal_shogi::Config>::value);

private:
    using GameType = Game<Config>;
    using MoveType = Move<Config>;
    using Square = typename Config::Square;
    using SHelper = Squares<Config>;
    friend Searcher<Config>;

private:
    /**
     * @brief If true, `m_action` = defence move, turn of `m_game` = attacker.
     */
    const bool m_attacker;
    /**
     * @brief If `m_attacker` is true, this should be a defence move.
     * If `m_attacker` is false, this should be a check move.
     */
    const MoveType m_action;

    Node* const m_parent;

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
    Node(const GameType& g)
        : m_attacker(true), m_action(), m_parent(nullptr), m_sibling(nullptr),
          m_child(nullptr), m_child_1st(nullptr), m_child_2nd(nullptr),
          m_pn(unit), m_dn(unit)
    {
        simulate_or_expand(g);
    }
    Node(Node* const parent, const MoveType& action)
        : m_attacker(!parent->m_attacker), m_action(action), m_parent(parent),
          m_sibling(nullptr), m_child(nullptr), m_child_1st(nullptr),
          m_child_2nd(nullptr), m_pn(unit), m_dn(unit)
    {
    }

    // Rules of 5
    ~Node() = default; // 1/5 destructor
    Node(const Node& other) = default; // 2/5 copy constructor
    Node& operator=(const Node& other) = default; // 3/5 copy assignment
    Node(Node&& other) = default; // 4/5 move constructor
    Node& operator=(Node&& other) = default; // 5/5 move assignment

    bool is_attacker() const
    {
        return m_attacker;
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
    bool found_mate() const
    {
        return (m_pn == zero) && (m_dn == max_number);
    }
    bool found_no_mate() const
    {
        return (m_pn == max_number) && (m_dn == zero);
    }
    bool found_conclusion() const
    {
        return (found_mate() || found_no_mate());
    }
    void search(
        GameType& game,
        uint& num_nodes,
        const uint thpn,
        const uint thdn,
        const bool collect_garbage = true)
    {
        if (m_parent)
            game.apply_dfpn(m_action);

        if (!has_child()) {
            simulate_or_expand(game);
            --num_nodes;
        }
        while (num_nodes) {
            if ((m_pn < thpn) && (m_dn < thdn)) {
                const uint thpn_ch = compute_thpn_for_child(thpn);
                const uint thdn_ch = compute_thdn_for_child(thdn);
                m_child_1st->search(game, num_nodes, thpn_ch, thdn_ch);
            } else {
                break;
            }
            backprop_one(collect_garbage);
        }
        if (m_parent)
            game.undo(false);
    }

private:
    uint compute_thpn_for_child(const uint thpn) const
    {
        if (m_attacker) {
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
        if (m_attacker) {
            return thdn;
        } else {
            return std::min(
                thdn,
                (m_child_2nd && (m_child_2nd->m_dn != max_number))
                    ? m_child_2nd->m_dn + 1u
                    : max_number);
        }
    }
    void simulate_or_expand(const GameType& game)
    {
        if (!simulate(game))
            expand(game);
    }

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
    bool simulate(const GameType& game)
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
        if ((winner == turn) == m_attacker)
            set_pndn_mate();
        else
            set_pndn_no_mate();
        return true;
    }

    /**
     * @brief Expand child nodes and compute #P(Proof) and #D(Disproof).
     *
     * - Offence: #P = min(#P of children), #D = sum(#D of children)
     * - Defence: #P = sum(#P of children), #D = min(#D of children)
     *
     * @param game
     */
    void expand(const GameType& game)
    {
        if (m_attacker) {
            expand_offence_moves(game);
        } else {
            expand_defence_moves(game);
        }
    }
    void expand_offence_moves(const GameType& game)
    {
        std::unique_ptr<Node>* ch = &m_child;
        const State<Config>& s = game.get_state();
        m_dn = zero;
        for (Move<Config> atk_move : CheckMoveGenerator<Config>(s)) {
            *ch = std::make_unique<Node>(this, atk_move);
            Node* const p = ch->get();
            if (atk_move.is_drop())
                p->m_pn = cent;
            update_offence_dn_ch1st_ch2nd(p);
            ch = &(p->m_sibling);
        }
        m_pn = m_child_1st ? m_child_1st->m_pn : max_number;
    }
    void expand_defence_moves(const GameType& game)
    {
        std::unique_ptr<Node>* ch = &m_child;
        const State<Config>& s = game.get_state();
        m_pn = zero;
        uint num_ch = 0u;
        const bool include_drop = !had_two_consecutive_sacrifice_drops();
        for (Move<Config> def_move :
             LegalMoveGenerator<Config>(s, include_drop)) {
            *ch = std::make_unique<Node>(this, def_move);
            ++num_ch;
            Node* const p = ch->get();
            if (s.is_checker_location(def_move.destination()))
                p->m_dn = cent;
            update_defence_pn_ch1st_ch2nd(p);
            ch = &(p->m_sibling);
        }
        if (num_ch == 0u) {
            set_pndn_mate();
        } else {
            m_dn = m_child_1st->m_dn;
        }
    }
    bool had_two_consecutive_sacrifice_drops() const
    {
        // `get_action()`: capture second sacrifice drop
        // `m_parent->get_action()`: second sacrifice drop
        // `m_parent->m_parent->get_action()`: capture first sacrifice drop
        // `m_parent->m_parent->m_parent->get_action()`: first sacrifice drop

        if ((m_parent == nullptr) || (m_parent->m_parent == nullptr)
            || (m_parent->m_parent->m_parent == nullptr))
            return false;
        const Move<Config> drop1st = m_parent->m_parent->m_parent->get_action();
        const Move<Config> capt1st = m_parent->m_parent->get_action();
        const Move<Config> drop2nd = m_parent->get_action();
        const Move<Config> capt2nd = get_action();
        if (!drop1st.is_drop())
            return false;
        if (drop1st.destination() != capt1st.destination())
            return false;
        if (!drop2nd.is_drop())
            return false;
        if (drop2nd.destination() != capt2nd.destination())
            return false;
        return true;
    }

    void backprop_one(const bool& collect_garbage = true)
    {
        update_pn_dn_ch1st_ch2nd();
        if (collect_garbage && found_no_mate())
            m_child.reset();
    }
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
    void update_pn_dn_ch1st_ch2nd()
    {
        // - Offence: #P = min(#P of children), #D = sum(#D of children)
        // - Defence: #P = sum(#P of children), #D = min(#D of children)
        if (found_conclusion())
            return;
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
        if (m_attacker) {
            m_dn = 0u;
            for (Node* ch = m_child.get(); ch; ch = ch->get_sibling())
                update_offence_dn_ch1st_ch2nd(ch);
            m_pn = m_child_1st->m_pn;
        } else {
            m_pn = 0u;
            for (Node* ch = m_child.get(); ch; ch = ch->get_sibling())
                update_defence_pn_ch1st_ch2nd(ch);
            m_dn = m_child_1st->m_dn;
        }
    }
    void update_offence_dn_ch1st_ch2nd(Node* const ch)
    {
        if ((m_child_1st == nullptr) || (m_child_1st->m_pn > ch->m_pn)) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if ((m_child_2nd == nullptr) || (m_child_2nd->m_pn > ch->m_pn)) {
            m_child_2nd = ch;
        }
        increment_with_guard(m_dn, ch->m_dn);
    }
    void update_defence_pn_ch1st_ch2nd(Node* const ch)
    {
        if ((m_child_1st == nullptr) || (m_child_1st->m_dn > ch->m_dn)) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if ((m_child_2nd == nullptr) || (m_child_2nd->m_dn > ch->m_dn)) {
            m_child_2nd = ch;
        }
        increment_with_guard(m_pn, ch->m_pn);
    }
    static void increment_with_guard(uint& n, uint other)
    {
        if ((n == max_number) || (other == max_number))
            n = max_number;
        else
            n += other;
    }
};

template <class Config>
class Searcher
{
private:
    using GameType = Game<Config>;
    using MoveType = Move<Config>;

private:
    std::unique_ptr<GameType> m_game;
    std::unique_ptr<Node<Config>> m_root;
    const bool m_collect_garbage;
    uint m_num_searched;

public:
    Searcher(const bool& collect_garbage = true)
        : m_game(nullptr), m_root(nullptr), m_collect_garbage(collect_garbage),
          m_num_searched(0u)
    {
    }

    bool is_ready() const
    {
        return (m_root != nullptr);
    }
    void set_game(const GameType& g)
    {
        m_game = std::make_unique<GameType>(g);
        m_game->clear_records_for_dfpn();
        m_root = std::make_unique<Node<Config>>(*m_game);
        m_num_searched = 0u;
    }

    /**
     * @brief Search for mate moves at given game state.
     *
     * @param n Number of nodes to explore.
     * @return true Found mate moves.
     * @return false No mate moves found but further searches may find ones.
     */
    bool search(const uint n)
    {
        Node<Config>* const root = m_root.get();
        GameType& game = *m_game;
        uint num = n;
        root->search(game, num, max_number, max_number, m_collect_garbage);
        m_num_searched += n - num;
        return root->found_mate();
    }
    bool found_mate() const
    {
        return m_root->found_mate();
    }
    bool found_no_mate() const
    {
        return m_root->found_no_mate();
    }
    bool found_conclusion() const
    {
        return m_root->found_conclusion();
    }
    uint get_search_count() const
    {
        return m_num_searched;
    }
    MoveType get_mate_move() const
    {
        return m_root->get_child_1st()->get_action();
    }
    std::vector<MoveType> get_mate_moves() const
    {
        std::vector<MoveType> out{};
        Node<Config>* n = m_root.get();
        while (n->has_child()) {
            n = n->get_child_1st();
            out.emplace_back(n->get_action());
        }
        return out;
    }
    const Node<Config>* get_root() const
    {
        return m_root ? m_root.get() : nullptr;
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_HPP
