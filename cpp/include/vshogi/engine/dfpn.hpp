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
    Node()
        : m_attacker(true), m_action(), m_parent(nullptr), m_sibling(nullptr),
          m_child(nullptr), m_child_1st(nullptr), m_child_2nd(nullptr),
          m_pn(unit), m_dn(unit)
    {
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

    void init()
    {
        m_sibling.reset();
        m_child.reset();
        m_child_1st = nullptr;
        m_child_2nd = nullptr;
        m_pn = unit;
        m_dn = unit;
    }
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

    void backprop_one(const GameType& g)
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
                update_defence_pn_ch1st_ch2nd(ch, g);
            m_dn = m_child_1st->m_dn;
        }
        assert((m_pn == 0u) ? (m_dn == max_number) : (m_dn != max_number));
        assert((m_dn == 0u) ? (m_pn == max_number) : (m_pn != max_number));
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
    void update_offence_dn_ch1st_ch2nd(Node* const ch)
    {
        if (ch->is_better_pn_choice_than(m_child_1st)) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if (ch->is_better_pn_choice_than(m_child_2nd)) {
            m_child_2nd = ch;
        }
        increment_with_guard(m_dn, ch->m_dn);
    }
    bool is_better_pn_choice_than(const Node* const other) const
    {
        if (other == nullptr)
            return true;
        if (m_pn < other->m_pn)
            return true;
        if (m_pn > other->m_pn)
            return false;
        return (m_action.is_drop() && (!other->m_action.is_drop()));
    }
    void update_defence_pn_ch1st_ch2nd(Node* const ch, const GameType& g)
    {
        if (ch->is_better_dn_choice_than(m_child_1st, g)) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if (ch->is_better_dn_choice_than(m_child_2nd, g)) {
            m_child_2nd = ch;
        }
        increment_with_guard(m_pn, ch->m_pn);
    }
    bool
    is_better_dn_choice_than(const Node* const other, const GameType& g) const
    {
        if (other == nullptr)
            return true;
        if (m_dn < other->m_dn)
            return true;
        if (m_dn > other->m_dn)
            return false;
        const State<Config>& s = g.get_state();
        return s.is_checker_location(m_action.destination())
               && !s.is_checker_location(other->m_action.destination());
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
class TranspositionTable
{
private:
    using BaseTypeStand = typename Config::BaseTypeStand;
    using StandNodeTable = std::unordered_map<BaseTypeStand, Node<Config>*>;
    using StandType = Stand<Config>;
    using GameType = Game<Config>;
    using MoveType = Move<Config>;
    using NodeType = Node<Config>;

private:
    std::unordered_map<std::uint64_t, StandNodeTable> m_table;
    NodeType m_root;

public:
    TranspositionTable() : m_table{}, m_root{}
    {
    }
    void clear()
    {
        m_table.clear();
        m_root.init();
    }
    NodeType* get_root()
    {
        return &m_root;
    }
    const NodeType* get_root() const
    {
        return &m_root;
    }

    void add(NodeType* const n, const GameType& g)
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        const auto t = g.get_turn();
        const auto s = g.get_stand(t).value();
        auto it = m_table.find(bt_hash);
        if (it == m_table.end()) {
            m_table.emplace(bt_hash, StandNodeTable());
            m_table[bt_hash].emplace(s, n);
        } else {
            it->second.emplace(s, n);
        }
    }

    NodeType* look_up_fuzzy(const bool attacker, const GameType& g)
    {
        const std::uint64_t bt_hash = g.get_board_turn_hash();
        auto it = m_table.find(bt_hash);
        if (it == m_table.end())
            return nullptr;
        return look_up_fuzzy(attacker, g, it->second);
    }

private:
    NodeType*
    look_up_fuzzy(const bool attacker, const GameType& g, StandNodeTable& table)
    {
        const auto t = g.get_turn();
        const auto s = g.get_stand(t);
        NodeType* n_weaker = nullptr; // weaker for offence
        Stand<Config> s_weaker = Stand<Config>();
        for (auto& it : table) {
            const auto s_iter = Stand<Config>(it.first);
            if (s_iter == s) {
                n_weaker = it.second;
                s_weaker = s_iter;
            } else if (attacker ? (s_iter < s) : (s < s_iter)) {
                // return a node if there is one with weaker stand.
                if (attacker ? it.second->found_mate()
                             : it.second->found_no_mate())
                    return it.second;
                if ((n_weaker == nullptr)
                    || (attacker ? (s_weaker < s_iter) : (s_iter < s_weaker))) {
                    // s_weaker < s_iter < s
                    s_weaker = it.first;
                    n_weaker = it.second;
                }
            }
        }
        return n_weaker;
    }
};

template <class Config>
class Searcher
{
private:
    using GameType = Game<Config>;
    using MoveType = Move<Config>;

private:
    TranspositionTable<Config> m_table;
    std::unique_ptr<GameType> m_game;
    uint m_num_searched;

public:
    Searcher() : m_table(), m_game(nullptr), m_num_searched(0u)
    {
    }

    bool is_ready() const
    {
        return static_cast<bool>(m_game);
    }
    void set_game(const GameType& g)
    {
        m_table.clear();
        m_game = std::make_unique<GameType>(g);
        m_game->clear_records_for_dfpn();
        const GameType& game = *m_game;
        Node<Config>* const root = m_table.get_root();
        if (!root->simulate(game))
            expand_at(*root, game);
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
        Node<Config>* const root = m_table.get_root();
        GameType& game = *m_game;
        uint num = n;
        while (num) {
            if (root->found_conclusion())
                break;
            const uint thpn_ch = root->compute_thpn_for_child(max_number);
            const uint thdn_ch = root->compute_thdn_for_child(max_number);
            search_inner(*root->get_child_1st(), game, num, thpn_ch, thdn_ch);
            root->backprop_one(game);
        }
        m_num_searched += n - num;
        return root->found_mate();
    }
    bool found_mate() const
    {
        return m_table.get_root()->found_mate();
    }
    bool found_no_mate() const
    {
        return m_table.get_root()->found_no_mate();
    }
    bool found_conclusion() const
    {
        return m_table.get_root()->found_conclusion();
    }
    uint get_search_count() const
    {
        return m_num_searched;
    }
    MoveType get_mate_move() const
    {
        return m_table.get_root()->get_child_1st()->get_action();
    }
    std::vector<MoveType> get_mate_moves() const
    {
        std::vector<MoveType> out{};
        const Node<Config>* n = m_table.get_root();
        while (n->has_child()) {
            n = n->get_child_1st();
            out.emplace_back(n->get_action());
        }
        return out;
    }
    const Node<Config>* get_root() const
    {
        return m_table.get_root();
    }

private:
    void search_inner(
        Node<Config>& n,
        GameType& game,
        uint& searches,
        const uint thpn,
        const uint thdn)
    {
        game.apply_dfpn(n.get_action());
        {
            if (Node<Config>* p
                = m_table.look_up_fuzzy(n.is_attacker(), game)) {
                if (p->found_conclusion()) {
                    n.m_pn = p->pn();
                    n.m_dn = p->dn();
                    n.m_child_1st = p->m_child_1st;
                }
                game.undo();
                return;
            }
        }
        if (!n.has_child()) {
            if (!n.simulate(game))
                expand_at(n, game);
            --searches;
        }
        while (searches) {
            if ((n.pn() >= thpn) || (n.dn() >= thdn))
                break;
            const uint thpn_ch = n.compute_thpn_for_child(thpn);
            const uint thdn_ch = n.compute_thdn_for_child(thdn);
            Node<Config>* const ch1st = n.get_child_1st();
            search_inner(*ch1st, game, searches, thpn_ch, thdn_ch);
            n.backprop_one(game);
        }
        if (n.found_conclusion()) {
            m_table.add(&n, game);
        }
        game.undo();
    }
    void expand_at(Node<Config>& n, const GameType& g)
    {
        if (n.is_attacker())
            expand_at_offence(n, g);
        else
            expand_at_defence(n, g);
        assert(
            (n.pn() == 0u) ? (n.dn() == max_number) : (n.dn() != max_number));
        assert(
            (n.dn() == 0u) ? (n.pn() == max_number) : (n.pn() != max_number));
    }
    void expand_at_offence(Node<Config>& n, const GameType& g)
    {
        std::unique_ptr<Node<Config>>* ch = &n.m_child;
        const State<Config>& s = g.get_state();
        Node<Config>* candidate = nullptr;
        n.m_dn = zero;
        for (Move<Config> atk_move : CheckMoveGenerator<Config>(s)) {
            *ch = std::make_unique<Node<Config>>(&n, atk_move);
            Node<Config>* const p = ch->get();
            n.update_offence_dn_ch1st_ch2nd(p);
            ch = &(p->m_sibling);
        }
        n.m_pn = n.m_child_1st ? n.m_child_1st->m_pn : max_number;
    }
    void expand_at_defence(Node<Config>& n, const GameType& g)
    {
        std::unique_ptr<Node<Config>>* ch = &n.m_child;
        const State<Config>& s = g.get_state();
        Node<Config>* candidate = nullptr;
        n.m_pn = zero;
        const bool include_drop = !n.had_two_consecutive_sacrifice_drops();
        for (Move<Config> def_move :
             LegalMoveGenerator<Config>(s, include_drop)) {
            *ch = std::make_unique<Node<Config>>(&n, def_move);
            Node<Config>* const p = ch->get();
            n.update_defence_pn_ch1st_ch2nd(p, g);
            ch = &(p->m_sibling);
        }
        if (n.m_child_1st == nullptr) {
            n.set_pndn_mate();
        } else {
            n.m_dn = n.m_child_1st->m_dn;
        }
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_HPP
