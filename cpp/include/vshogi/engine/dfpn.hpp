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
    static constexpr uint zero = 0u;
    static constexpr uint unit = 100u;
    static constexpr uint cent = 1u;
    static constexpr uint max_number = std::numeric_limits<uint>::max();

    Node(const GameType& g)
        : m_attacker(true), m_action(), m_parent(nullptr), m_sibling(nullptr),
          m_child(nullptr), m_child_1st(nullptr), m_child_2nd(nullptr),
          m_pn(unit), m_dn(unit)
    {
        simulate(g) || expand(g);
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
    /**
     * @brief Select a pre-leaf node by selecting the best path.
     *
     * - Offence: select a node with smallest proof number.
     *
     * - Defence: select a node with smallest dis-proof number.
     *
     * @return Node*
     */
    Node* select_leaf(GameType& g)
    {
        Node* n = this;
        while (n->has_child()) {
            n = n->m_child_1st;
            g.apply_dfpn(n->m_action);
        }
        return n;
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
    bool expand(const GameType& game)
    {
        std::unique_ptr<Node>* ch = &m_child;
        const State<Config>& s = game.get_state();
        const Board<Config>& b = s.get_board();
        const ColorEnum t = s.get_turn();
        if (m_attacker) {
            m_pn = unit;
            m_dn = zero;
            for (Move<Config> m : CheckMoveGenerator<Config>(s)) {
                ch = append(m, ch);
                // if (!m_child) {
                //     m_child = std::make_unique<Node>(this, m);
                //     ch = &(m_child.get()->m_sibling);
                // } else if (b.is_square_attacked(
                //         t,
                //         m.destination(),
                //         m.is_drop() ? SHelper::SQ_NA : m.source_square())) {
                //     prepend(m);
                // } else {
                //     ch = append(m, ch);
                // }
                m_dn += unit;
            }
        } else {
            m_pn = zero;
            m_dn = unit;
            for (Move<Config> m : LegalMoveGenerator<Config>(s)) {
                if (!m_child) {
                    ch = append(m, ch);
                } else if (s.is_checker_location(m.destination())) {
                    prepend(m);
                } else {
                    ch = append(m, ch);
                }
                m_pn += unit;
            }
        }
        if (has_child()) {
            m_child_1st = m_child.get();
            m_child_2nd = m_child_1st->get_sibling();
        } else {
            set_pndn_no_mate();
            return true;
        }

        if (m_parent)
            return m_parent->is_child2nd_better_than_child1st();
        else
            return false;
    }
    void prepend(const Move<Config>& m)
    {
        if (m_child) {
            auto tmp = std::move(m_child);
            m_child = std::make_unique<Node>(this, m);
            m_child.get()->m_sibling = std::move(tmp);
        } else {
            m_child = std::make_unique<Node>(this, m);
        }
    }
    std::unique_ptr<Node>*
    append(const Move<Config>& m, std::unique_ptr<Node>* const end)
    {
        *end = std::make_unique<Node>(this, m);
        return &(end->get()->m_sibling);
    }

    /**
     * @brief Backprop #P and #D from child.
     *
     * - Offence: #P = min(#P of children), #D = sum(#D of children)
     * - Defence: #P = sum(#P of children), #D = min(#D of children)
     *
     */
    Node* backprop(GameType& g)
    {
        update_pn_dn_ch1st_ch2nd();
        if (found_no_mate())
            m_child.reset();
        if (m_parent == nullptr)
            return this;
        if (found_conclusion() || m_parent->is_child2nd_better_than_child1st())
            return m_parent->backprop(g.undo());
        return this;
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
            for (Node* ch = m_child.get(); ch; ch = ch->get_sibling()) {
                update_offence_ch1st_ch2nd(ch);
                increment_with_guard(m_dn, ch->m_dn);
            }
            m_pn = m_child_1st->m_pn;
        } else {
            m_pn = 0u;
            for (Node* ch = m_child.get(); ch; ch = ch->get_sibling()) {
                update_defence_ch1st_ch2nd(ch);
                increment_with_guard(m_pn, ch->m_pn);
            }
            m_dn = m_child_1st->m_dn;
        }
    }
    void update_offence_ch1st_ch2nd(Node* const ch)
    {
        if ((m_child_1st == nullptr) || (m_child_1st->m_pn > ch->m_pn)) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if ((m_child_2nd == nullptr) || (m_child_2nd->m_pn > ch->m_pn)) {
            m_child_2nd = ch;
        }
    }
    void update_defence_ch1st_ch2nd(Node* const ch)
    {
        if ((m_child_1st == nullptr) || (m_child_1st->m_dn > ch->m_dn)) {
            m_child_2nd = m_child_1st;
            m_child_1st = ch;
        } else if ((m_child_2nd == nullptr) || (m_child_2nd->m_dn > ch->m_dn)) {
            m_child_2nd = ch;
        }
    }
    static void increment_with_guard(uint& n, uint other)
    {
        if ((n == max_number) || (other == max_number))
            n = max_number;
        else
            n += other;
    }
    bool is_child2nd_better_than_child1st() const
    {
        if (m_child_2nd == nullptr)
            return false;
        if (m_attacker)
            return m_child_1st->m_pn > m_child_2nd->m_pn;
        else
            return m_child_1st->m_dn > m_child_2nd->m_dn;
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
    Node<Config>* m_curr;

    /**
     * @brief True if there is a mate, false if there is no mate for sure.
     *
     */
    std::unordered_map<std::uint64_t, bool> m_mate_cache_for_black;

    /**
     * @brief True if there is a mate, false if there is no mate for sure.
     *
     */
    std::unordered_map<std::uint64_t, bool> m_mate_cache_for_white;

public:
    Searcher()
        : m_game(nullptr), m_root(nullptr),
          m_curr(nullptr), m_mate_cache_for_black{}, m_mate_cache_for_white{}
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
        m_curr = m_root.get();
    }

    /**
     * @brief Search for mate moves at given game state.
     *
     * @param n Number of nodes to explore.
     * @return true Found mate moves.
     * @return false No mate moves found but further searches may find ones.
     */
    bool search(uint n)
    {
        Node<Config>* const root = m_root.get();
        GameType& game = *m_game;
        if (root->found_conclusion())
            return root->found_mate();
        for (; n--;) {
            m_curr = m_curr->select_leaf(game);
            if (m_curr->simulate(game) || m_curr->expand(game)) {
                m_curr = m_curr->backprop(game);
                if (root->found_conclusion())
                    break;
            }
        }
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
    MoveType get_mate_move()
    {
        return m_root->get_child_1st()->get_action();
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_HPP
