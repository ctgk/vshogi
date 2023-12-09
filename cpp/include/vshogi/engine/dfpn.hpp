#ifndef VSHOGI_ENGINE_DFPN_HPP
#define VSHOGI_ENGINE_DFPN_HPP

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "vshogi/color.hpp"
#include "vshogi/result.hpp"

namespace vshogi::engine::dfpn
{

template <class Game, class Move, bool Attacker = true>
class Node
{
private:
    using NodeAlly = Node<Game, Move, Attacker>;
    using NodeEnemy = Node<Game, Move, !Attacker>;

    NodeEnemy* m_parent;

    /**
     * @brief Pointer to sibling node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<NodeAlly> m_sibling;

    /**
     * @brief Pointer to first child node.
     * @ref https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
     */
    std::unique_ptr<NodeEnemy> m_child;

    Move m_action;
    std::uint32_t m_pn;
    std::uint32_t m_dn;
    static constexpr std::uint32_t max_number
        = std::numeric_limits<std::uint32_t>::max();
    friend class Node<Game, Move, !Attacker>;

public:
    Node()
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr), m_action(0),
          m_pn(1), m_dn(1)
    {
    }
    Node(const Move& action)
        : m_parent(nullptr), m_sibling(nullptr), m_child(nullptr),
          m_action(action), m_pn(1), m_dn(1)
    {
    }

    // Rules of 5
    ~Node() = default; // 1/5 destructor
    Node(const Node<Game, Move, Attacker>& other)
        = default; // 2/5 copy constructor
    Node<Game, Move, Attacker>&
    operator=(const Node<Game, Move, Attacker>& other)
        = default; // 3/5 copy assignment
    Node(Node<Game, Move, Attacker>&& other) = default; // 4/5 move constructor
    Node<Game, Move, Attacker>& operator=(Node<Game, Move, Attacker>&& other)
        = default; // 5/5 move assignment

    std::uint32_t get_pn() const
    {
        return m_pn;
    }
    std::uint32_t get_dn() const
    {
        return m_dn;
    }
    std::vector<Move> get_mate_moves() const
    {
        std::vector<Move> out;
        if (!found_mate())
            return out;

        const NodeAlly* node = this;
        while (true) {
            if (node->m_child == nullptr) {
                // Length of mate moves can be even number
                // by king entering declaration.
                return out;
            }
            const NodeEnemy* node_enemy = node->select();
            out.emplace_back(node_enemy->m_action); // Attack move
            if (node_enemy->m_child == nullptr) {
                return out;
            }
            node = node_enemy->select();
            out.emplace_back(node->m_action); // Defence move
        }
    }
    bool found_mate() const
    {
        return (m_pn == 0) && (m_dn == max_number);
    }
    bool found_no_mate() const
    {
        return (m_pn == max_number) && (m_dn == 0);
    }
    bool found_conclusion() const
    {
        return (found_mate() || found_no_mate());
    }

    void select_simulate_expand_backprop(Game& game)
    {
        if (m_child == nullptr) {
            // pn and dn should not be infinity when there are no children.
            simulate_expand_backprop(game);
        } else {
            NodeEnemy* child = select();
            child->m_parent = this;
            child->select_simulate_expand_backprop(game.apply(child->m_action));
        }
    }

private:
    NodeEnemy* select()
    {
        if (Attacker) {
            std::uint32_t min_pn = max_number;
            NodeEnemy* out = m_child.get();
            for (NodeEnemy* ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_pn > ch->m_pn) {
                    min_pn = ch->m_pn;
                    out = ch;
                }
            }
            return out;
        } else {
            std::uint32_t min_dn = max_number;
            NodeEnemy* out = m_child.get();
            for (NodeEnemy* ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_dn > ch->m_dn) {
                    min_dn = ch->m_dn;
                    out = ch;
                }
            }
            return out;
        }
    }
    const NodeEnemy* select() const
    {
        if (Attacker) {
            std::uint32_t min_pn = max_number;
            const NodeEnemy* out = m_child.get();
            for (auto ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_pn > ch->m_pn) {
                    min_pn = ch->m_pn;
                    out = ch;
                }
            }
            return out;
        } else {
            std::uint32_t min_dn = max_number;
            const NodeEnemy* out = m_child.get();
            for (auto ch = out; ch != nullptr; ch = ch->m_sibling.get()) {
                if (min_dn > ch->m_dn) {
                    min_dn = ch->m_dn;
                    out = ch;
                }
            }
            return out;
        }
    }
    void simulate_expand_backprop(const Game& game)
    {
        const std::uint32_t pn_original = m_pn;
        const std::uint32_t dn_original = m_dn;
        const auto r = game.get_result();
        if (r == ONGOING) {
            expand(game);
        } else {
            simulate(game);
        }
        if (m_parent != nullptr)
            m_parent->backprop(m_pn, m_dn, pn_original, dn_original);
    }
    void simulate(const Game& game)
    {
        const auto r = game.get_result();
        if (r == DRAW) {
            set_pndn_no_mate();
        } else {
            const auto winner = (r == BLACK_WIN) ? BLACK : WHITE;
            const auto turn = game.get_turn();
            if ((winner == turn) == Attacker)
                set_pndn_mate();
            else
                set_pndn_no_mate();
        }
    }
    void expand(const Game& game)
    {
        const std::vector<Move>& legal_moves = game.get_legal_moves();
        std::unique_ptr<NodeEnemy>* ch = &m_child;
        int num_child = 0;
        for (auto&& m : legal_moves) {
            if ((!Attacker) || game.in_check_after_move(m)) {
                *ch = std::make_unique<NodeEnemy>(m);
                ch = &ch->get()->m_sibling;
                ++num_child;
            }
        }
        if (m_child == nullptr)
            set_pndn_no_mate(); // game is ongoing but no check move.
        else {
            if (Attacker) {
                m_pn = 1;
                m_dn = static_cast<std::uint32_t>(num_child);
            } else {
                m_pn = static_cast<std::uint32_t>(num_child);
                m_dn = 1;
            }
        }
    }
    void backprop(
        const std::uint32_t& pn,
        const std::uint32_t& dn,
        const std::uint32_t& pn_prev,
        const std::uint32_t& dn_prev)
    {
        const std::uint32_t pn_original = m_pn;
        const std::uint32_t dn_original = m_dn;
        if (Attacker) {
            m_pn = max_number;
            const NodeEnemy* ch = m_child.get();
            for (; ch != nullptr; ch = ch->m_sibling.get()) {
                if (m_pn > ch->m_pn)
                    m_pn = ch->m_pn;
            }
            if ((m_dn == max_number) || (dn == max_number)) {
                m_dn = max_number;
            } else {
                m_dn = static_cast<std::uint32_t>(
                    static_cast<int>(m_dn) + static_cast<int>(dn)
                    - static_cast<int>(dn_prev));
            }
        } else {
            if ((m_pn == max_number) || (pn == max_number)) {
                m_pn = max_number;
            } else {
                m_pn = static_cast<std::uint32_t>(
                    static_cast<int>(m_pn) + static_cast<int>(pn)
                    - static_cast<int>(pn_prev));
            }
            m_dn = max_number;
            const NodeEnemy* ch = m_child.get();
            for (; ch != nullptr; ch = ch->m_sibling.get()) {
                if (m_dn > ch->m_dn)
                    m_dn = ch->m_dn;
            }
        }
        if (found_no_mate()) {
            m_child.reset();
        }
        if (m_parent != nullptr)
            m_parent->backprop(m_pn, m_dn, pn_original, dn_original);
    }
    void set_pndn_mate()
    {
        m_pn = 0;
        m_dn = max_number;
    }
    void set_pndn_no_mate()
    {
        m_pn = max_number;
        m_dn = 0;
    }
};

template <class Game, class Move>
class Searcher
{
private:
    const int m_default_num_nodes;
    Node<Game, Move, true> m_root;
    std::unique_ptr<Game> m_game_ptr;

public:
    Searcher(const int default_num_nodes = 1000)
        : m_default_num_nodes(default_num_nodes), m_root(), m_game_ptr(nullptr)
    {
    }
    void set_root(const Game& g)
    {
        m_root = Node<Game, Move, true>();
        m_game_ptr = std::make_unique<Game>(Game(g));
    }
    bool explore()
    {
        return explore(m_default_num_nodes);
    }
    bool explore(std::size_t n)
    {
        const Game& g = *m_game_ptr;
        for (; n--;) {
            if (m_root.found_conclusion())
                break;
            auto g_tmp = Game(g);
            m_root.select_simulate_expand_backprop(g_tmp);
        }
        return m_root.found_mate();
    }
    bool found_mate() const
    {
        return m_root.found_mate();
    }
    bool found_no_mate() const
    {
        return m_root.found_no_mate();
    }
    bool found_conclusion() const
    {
        return m_root.found_conclusion();
    }
    std::vector<Move> get_mate_moves()
    {
        return m_root.get_mate_moves();
    }
};

} // namespace vshogi::engine::dfpn

#endif // VSHOGI_ENGINE_DFPN_HPP
